//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Underworld Adventures Team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file Inventory.cpp
/// \brief inventory implementation
//
#include "pch.hpp"
#include "Inventory.hpp"
#include "Object.hpp"
#include "Properties.hpp"
#include "Savegame.hpp"

using Underworld::Inventory;

Inventory::Inventory()
   :m_floatingObjectPos(c_inventorySlotNoItem)
{
   Create();
}

void Inventory::Create()
{
   m_objectList.resize(0x0100);

   m_floatingObjectPos = c_inventorySlotNoItem;
}

void Inventory::Destroy()
{
   m_objectList.clear();
   m_slotList.clear();
   m_containerStack.clear();

   m_floatingObjectPos = c_inventorySlotNoItem;
}

Uint16 Inventory::Allocate()
{
   // start after paperdoll objects
   Uint16 pos = slotMax;
   Uint16 size = static_cast<Uint16>(m_objectList.size());
   while (pos < size && m_objectList[pos].m_itemID != c_itemIDNone)
      pos++;

   // hit end of list?
   if (pos >= m_objectList.size())
   {
      // already at maximum size?
      if (m_objectList.size() >= 0x10000)
         throw Base::RuntimeException("Error while enlarging inventory list; already at maximum size");

      // new pos is start of enlarged list
      pos = static_cast<Uint16>(m_objectList.size());

      // enlarge list by factor 1,25
      size_t newSize = m_objectList.size();
      newSize += newSize >> 2;

      // limit to Uint16 range
      if (newSize >= 0x10000)
         newSize = 0x10000;

      m_objectList.resize(newSize);
   }

   return pos;
}

void Inventory::Free(Uint16 pos)
{
   UaAssert(pos != c_inventorySlotNoItem);
   UaAssert(pos < m_objectList.size()); // object must not be part of a tile list

   m_objectList[pos].m_itemID = c_itemIDNone;
}

Uint16 Inventory::GetSlotListPos(size_t index) const
{
   UaAssert(index < GetNumSlots());

   return m_slotList[index];
}

bool Inventory::IsContainer(Uint16 itemID) const
{
   // note: this is hardcoded, as both games have the same item ID range
   return itemID >= 0x0080 && itemID < 0x008f;
}

Uint16 Inventory::GetContainerPos() const
{
   if (m_containerStack.empty())
      return c_inventorySlotNoItem; // topmost

   size_t topPos = m_containerStack.size() - 1;

   UaAssert(true == IsContainer(GetObjectInfo(m_containerStack[topPos]).m_itemID));
   return m_containerStack[topPos];
}

Uint16 Inventory::GetParentContainerPos() const
{
   if (m_containerStack.size() <= 1)
      return c_inventorySlotNoItem; // topmost

   size_t parentPos = m_containerStack.size() - 2;

   UaAssert(true == IsContainer(GetObjectInfo(m_containerStack[parentPos]).m_itemID));
   return m_containerStack[parentPos];
}

/// \todo check if object is in current container
void Inventory::OpenContainer(Uint16 pos)
{
   UaAssert(true == IsContainer(GetObjectInfo(pos).m_itemID));
   UaAssert(false == GetObjectInfo(pos).m_isQuantity);
   UaAssert(GetContainerPos() != pos); // container already open

   // check if user wants to open container from paperdoll
   if (pos >= slotPlayerObjectsStart && pos < slotMax)
   {
      // clear container stack; container to open becomes topmost
      m_containerStack.clear();
   }

   // quantity field is used as pointer to content of containers
   Uint16 link = GetObjectInfo(pos).m_quantity;

   BuildSlotList(link);

   // set current container as top item
   m_containerStack.push_back(pos);
}

void Inventory::CloseContainer()
{
   UaAssert(!m_containerStack.empty());
   UaAssert(IsContainer(GetObjectInfo(m_containerStack.back()).m_itemID));

   Uint16 pos = c_inventorySlotNoItem; // start of topmost item list at list pos 0

   // remove current container link
   if (!m_containerStack.empty())
      m_containerStack.pop_back();

   // get previous topmost item index
   if (!m_containerStack.empty())
   {
      pos = m_containerStack.back();

      UaAssert(false == GetObjectInfo(pos).m_isQuantity);

      // quantity is used as pointer to content of container
      pos = GetObjectInfo(pos).m_quantity;
   }

   BuildSlotList(pos);
}

/// Object must not be member of a container yet.
/// \param[in,out] pos position of item to add; if an item is moved, the new
///                position is stored in this parameter.
/// \param containerPos position of container to add item to; when
///        c_inventorySlotNoItem is given, it is added to topmost container
/// \return true if object could be added, or false when not (e.g. when
///         container is full; when false is returned, the value of pos
///         isn't change.
bool Inventory::AddToContainer(Uint16& pos, Uint16 containerPos)
{
   UaAssert(GetObjectInfo(pos).m_link == 0);

   if (containerPos == c_inventorySlotNoItem)
   {
      // adding to topmost container

      // try to find an unused space
      Uint16 ui;
      for (ui = 0; ui < 8; ui++)
      {
         if (GetObjectInfo(ui).m_itemID == c_itemIDNone)
         {
            // copy object to that slot
            GetObjectInfo(ui) = GetObjectInfo(pos);

            // free object
            Free(pos);
            pos = ui;
            break;
         }
      }

      if (ui == 8)
         return false; // no space to drop
   }
   else
   {
      UaAssert(true == IsContainer(GetObjectInfo(containerPos).m_itemID));
      UaAssert(false == GetObjectInfo(containerPos).m_isQuantity);

      // add to container
      if (GetObjectInfo(containerPos).m_quantity == 0)
      {
         // no object in container
         GetObjectInfo(containerPos).m_quantity = pos;
      }
      else
      {
         // search last object in list
         Uint16 link = GetObjectInfo(containerPos).m_quantity;
         Uint16 lastLink = link;
         while (link != 0)
         {
            lastLink = link;
            link = GetObjectInfo(link).m_link;
         }

         GetObjectInfo(lastLink).m_link = pos;
      }
   }

   if (containerPos == GetContainerPos())
   {
      Uint16 pos1stObj = containerPos == c_inventorySlotNoItem ? c_inventorySlotNoItem : GetObjectInfo(containerPos).m_quantity;
      BuildSlotList(pos1stObj);
   }

   return true;
}

void Inventory::RemoveFromContainer(Uint16 pos, Uint16 containerPos)
{
   // if the object to remove is a container, it must not have items in it
   if (IsContainer(GetObjectInfo(pos).m_itemID))
   {
      UaAssert(false == GetObjectInfo(pos).m_isQuantity);
      UaAssert(GetObjectInfo(pos).m_quantity == 0);
   }

   if (containerPos == c_inventorySlotNoItem)
   {
      // remove from topmost container
      UaAssert(pos < slotPlayerObjectsStart); // must be in range
      UaAssert(0 == GetObjectInfo(pos).m_link); // must not have a link

      // as topmost container items don't have m_link set, there is no need
      // to remove linking; just free object
      Free(pos);
   }
   else
   {
      UaAssert(false == GetObjectInfo(containerPos).m_isQuantity);

      // search container for object
      Uint16 link = GetObjectInfo(containerPos).m_quantity;
      UaAssert(link != 0); // no objects in container?

      if (link == pos)
      {
         // first object in list
         GetObjectInfo(containerPos).m_quantity = GetObjectInfo(link).m_link;
      }
      else
      {
         // search for object in list
         while (link != 0)
         {
            if (GetObjectInfo(link).m_link == pos)
            {
               // chain to object after the one to remove
               GetObjectInfo(link).m_link = GetObjectInfo(pos).m_link;
               break;
            }

            link = GetObjectInfo(link).m_link;
         }
      }

      GetObjectInfo(pos).m_link = 0;
   }

   if (containerPos == GetContainerPos())
   {
      Uint16 pos1stObj = containerPos == c_inventorySlotNoItem ? c_inventorySlotNoItem : GetObjectInfo(containerPos).m_quantity;
      BuildSlotList(pos1stObj);
   }
}

/// \param pos position of object to convert to the floating object
/// \return if the given object can be made floated; reasons for not be able
/// \note check weight constraints before making an object floating; an object
///       is considered part of the inventory as soon as it is added as
///       floating object.
/// \todo check if bool return value is needed; not needed, remove it!
bool Inventory::FloatObject(Uint16 pos)
{
   UaAssert(m_floatingObjectPos == c_inventorySlotNoItem); // must have no floating object
   UaAssert(GetObjectInfo(pos).m_link == 0); // must not be in a container

   m_floatingObjectPos = pos;

   return true;
}

/// Dropping floating objects must support four different scenarios:
/// - dropping objects to the symbol of the currently open container (the parent)
/// - dropping objects to an empty place in the container, basically adding
///   it to the end of the slot list
/// - dropping objects to the paperdoll or paperdoll slots
/// - dropping object to an item in the container; this generates three further
///   possibilities:
///   - when the object is a container, drop item in this container (if possible)
///   - when the objects can be combined, the new object is dropped (there still
///     may be a floating object afterwards, e.g. lighting incense with a torch)
///   - when the objects can't be combined, the objects just swap
/// \param containerPos container pos which should be used to drop item
/// \param objectPos object position in container
/// \return returns if dropping floating item succeeded; reasons for failure is
///         when the container to drop to is full, the object type cannot be
///         dropped in that container, ... TODO
bool Inventory::DropFloatingObject(Uint16 containerPos, Uint16 objectPos)
{
   // add to topmost container / paperdoll?
   if (containerPos == c_inventorySlotNoItem)
   {
      // must be a topmost slot or paperdoll pos, or "no item" pos
      UaAssert(objectPos < ::Underworld::slotMax || objectPos == c_inventorySlotNoItem);

      // dropping to topmost slot or paperdoll?
      if (objectPos < ::Underworld::slotMax)
      {
         // just drop on existing object
         return DropOnObject(containerPos, objectPos);
      }
      else
      {
         // add to topmost container, at end
         bool ret = AddToContainer(m_floatingObjectPos, containerPos);

         if (ret)
            m_floatingObjectPos = c_inventorySlotNoItem;
         return ret;
      }
   }
   else
   {
      // no, adding to specified container

      // add to end of container?
      // TODO move together with above code
      if (objectPos == c_inventorySlotNoItem)
      {
         bool ret = AddToContainer(m_floatingObjectPos, containerPos);

         if (ret)
            m_floatingObjectPos = c_inventorySlotNoItem;

         return ret;
      }
      else
      {
         // swap at specific pos in container
         DropOnObject(containerPos, objectPos);
      }
   }

   return false;
}

Uint16 Inventory::InsertFloatingItem(const ::Underworld::ObjectInfo& info)
{
   // already have a floating object?
   if (m_floatingObjectPos != c_inventorySlotNoItem)
      return c_inventorySlotNoItem;

   Uint16 pos = Allocate();
   if (pos == c_inventorySlotNoItem)
      return pos;

   ::Underworld::ObjectInfo& newobj = GetObjectInfo(pos);
   newobj = info;
   newobj.m_link = 0;

   FloatObject(pos);

   return pos;
}

bool Inventory::DropOnObject(Uint16 /*containerPos*/, Uint16 /*pos*/)
{
   // TODO implement
   UaAssert(false);
   return false;
}

void Inventory::Load(Base::Savegame& sg)
{
   Destroy();
   Create();

   sg.BeginSection("inventory");

   // load inventory object list
   Uint16 max = sg.Read16();
   m_objectList.resize(max);

   for (Uint16 ui = 0; ui < max; ui++)
      m_objectList[ui].Load(sg);

   // load container stack
   max = sg.Read16();
   m_containerStack.resize(max);

   for (Uint16 ui2 = 0; ui2 < max; ui2++)
      m_containerStack[ui2] = sg.Read16();

   m_floatingObjectPos = sg.Read16();

   sg.EndSection();

   // rebuild current container list
   BuildSlotList(GetContainerPos());
}

void Inventory::Save(Base::Savegame& sg) const
{
   sg.BeginSection("inventory");

   // save inventory object list
   Uint16 max = static_cast<Uint16>(m_objectList.size());
   sg.Write16(max);

   for (Uint16 index = 0; index < max; index++)
      m_objectList[index].Save(sg);

   // save container stack
   max = static_cast<Uint16>(m_containerStack.size());
   sg.Write16(max);

   for (Uint16 index2 = 0; index2 < max; index2++)
      sg.Write16(m_containerStack[index2]);

   sg.Write16(m_floatingObjectPos);

   sg.EndSection();
}

/// \param pos position of first object to build slot list with. when pos
///        is equal to c_inventorySlotNoItem, the "top" slot list is built.
void Inventory::BuildSlotList(Uint16 pos)
{
   // rebuild slot list
   m_slotList.clear();

   if (pos == c_inventorySlotNoItem)
   {
      // build list with first 8 items
      for (Uint16 index = 0; index < 8; index++)
         m_slotList.push_back(index);
   }
   else
   {
      // build normal list
      while (pos != 0)
      {
         // add to slot links
         m_slotList.push_back(pos);

         // get next object
         pos = GetObjectInfo(pos).m_link;
      }
   }
}

unsigned int Inventory::GetInventoryWeight(const Underworld::ObjectProperties& properties) const
{
   // weight in 1/10 stones
   unsigned int weightInTenth = 0;

   // calculate weight from 8 topmost items and the paperdoll items
   for (Uint16 pos = slotTopmostFirstItem; pos < slotMax; pos++)
      weightInTenth += GetObjectWeight(pos, properties);

   return weightInTenth;
}

unsigned int Inventory::GetObjectWeight(Uint16 pos,
   const Underworld::ObjectProperties& properties) const
{
   if (pos == c_inventorySlotNoItem ||
      pos == GetFloatingObjectPos())
      return 0;

   const ObjectInfo& objectInfo = GetObjectInfo(pos);
   if (objectInfo.m_itemID == c_itemIDNone)
      return 0;

   const CommonObjectProperty& commonProperty = properties.GetCommonProperty(objectInfo.m_itemID);

   unsigned int weightInTenth = commonProperty.m_mass;

   if (commonProperty.m_isContainer &&
      !objectInfo.m_isQuantity &&
      objectInfo.m_quantity != 0)
   {
      Uint16 link = objectInfo.m_quantity;
      while (link != 0)
      {
         weightInTenth += GetObjectWeight(link, properties);

         const ObjectInfo& nextObjectInfo = GetObjectInfo(link);
         link = nextObjectInfo.m_link;
      }
   }

   // follow inventory link chain
   if (objectInfo.m_link != 0)
   {
      weightInTenth += GetObjectWeight(objectInfo.m_link, properties);
   }

   return weightInTenth;
}
