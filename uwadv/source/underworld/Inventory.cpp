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

Underworld::ObjectProperties g_emptyObjectProperties;

Inventory::Inventory()
   :m_floatingObjectPos(c_inventorySlotNoItem),
   m_objectProperties(g_emptyObjectProperties)
{
   Create();

#ifdef HAVE_DEBUG
   // test item inventory
   Uint16 testInventory[] =
   {
      0x0080, 0x0082, 0x008f, 0x0088, // 8 top slots
      0x0095, 0x0116, 0x00b4, 0x0080,
      0x000e, 0x0037, // hands
      0xffff, 0x0094, // shoulder
      0x0038, 0xffff, // finger
      0x0024, 0x0021, 0x0027, 0x002f, 0x002d, // paperdoll
      0x00b6, 0x0130, 0x0138, 0x0031 // 4 first objects
   };

   // init with some stuff
   for (unsigned int index = 0; index < slotMax + 4; index++)
   {
      m_objectList[index].m_itemID = testInventory[index];
      m_objectList[index].m_link = 0;
   }

   // set some container links
   GetObjectInfo(0).m_quantity = slotMax;
   GetObjectInfo(1).m_quantity = slotMax + 1;
   GetObjectInfo(3).m_quantity = slotMax + 2;
   GetObjectInfo(7).m_quantity = slotMax + 3;

   GetObjectInfo(0).m_isQuantity = false;
   GetObjectInfo(1).m_isQuantity = false;
   GetObjectInfo(3).m_isQuantity = false;
   GetObjectInfo(7).m_isQuantity = false;

   BuildSlotList(c_inventorySlotNoItem);
#endif
}

void Inventory::AddItemCombineRule(Uint16 firstItemID, Uint16 secondItemID,
   Uint16 resultItemID, unsigned int itemDestroyedMask)
{
   UaAssert(itemDestroyedMask >= 1 && itemDestroyedMask <= 3);

   ItemCombineRule rule;
   rule.m_itemID1 = firstItemID;
   rule.m_itemID2 = secondItemID;
   rule.m_resultItemID = resultItemID;
   rule.m_itemDestroyedMask = itemDestroyedMask;

   m_itemCombineRuleList.push_back(rule);
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

void Inventory::OpenContainer(Uint16 pos)
{
   UaAssert(true == IsContainer(GetObjectInfo(pos).m_itemID));
   UaAssert(GetObjectInfo(pos).m_isQuantity == false);
   UaAssert(GetContainerPos() != pos); // container must not already be open

   // check if user wants to open container from paperdoll
   if (pos >= slotPlayerObjectsStart && pos < slotMax)
   {
      // clear container stack; container to open becomes topmost
      m_containerStack.clear();
   }

   // quantity field is used as pointer to content of containers
   UaAssert(GetObjectInfo(pos).m_isQuantity == false);
   Uint16 link = GetObjectInfo(pos).m_quantity;

   BuildSlotList(link);

   // set current container as top item
   m_containerStack.push_back(pos);
}

void Inventory::CloseContainer()
{
   UaAssert(!m_containerStack.empty());
   UaAssert(IsContainer(GetObjectInfo(m_containerStack.back()).m_itemID));

   // remove current container link from stack
   if (!m_containerStack.empty())
      m_containerStack.pop_back();

   // get previous topmost item index
   Uint16 pos = c_inventorySlotNoItem; // in case of topmost container
   if (!m_containerStack.empty())
   {
      pos = m_containerStack.back();

      // quantity is used as pointer to content of container
      UaAssert(GetObjectInfo(pos).m_isQuantity == false);
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
///         isn't changed.
bool Inventory::AddToContainer(Uint16& pos, Uint16 containerPos)
{
   UaAssert(GetObjectInfo(pos).m_link == 0); // object must not have linked objects

   if (containerPos == c_inventorySlotNoItem)
   {
      // adding to topmost container

      // try to find an unused space
      Uint16 topmostPos;
      for (topmostPos = 0; topmostPos < 8; topmostPos++)
      {
         if (GetObjectInfo(topmostPos).m_itemID == c_itemIDNone)
         {
            // copy object to that slot
            GetObjectInfo(topmostPos) = GetObjectInfo(pos);

            // free object
            Free(pos);
            pos = topmostPos;
            break;
         }
      }

      if (topmostPos == 8)
      {
         UaTrace("no space in topmost container\n");
         return false; // found no space to drop
      }

      UaTrace("adding to topmost container pos\n");
   }
   else
   {
      // adding to container at containerPos
      UaAssert(true == IsContainer(GetObjectInfo(containerPos).m_itemID));
      UaAssert(GetObjectInfo(containerPos).m_isQuantity == false);

      if (GetObjectInfo(containerPos).m_quantity == 0)
      {
         // no object in container yet; add it as the first
         GetObjectInfo(containerPos).m_quantity = pos;

         UaTrace("adding to empty container\n");
      }
      else
      {
         // find last object in list
         Uint16 link = GetObjectInfo(containerPos).m_quantity;
         Uint16 lastLink = link;
         while (link != 0)
         {
            lastLink = link;
            link = GetObjectInfo(link).m_link;
         }

         GetObjectInfo(lastLink).m_link = pos;

         UaTrace("adding to container, after pos %04x\n", lastLink);
      }
   }

   // adding to current container?
   if (containerPos == GetContainerPos())
   {
      // refresh slot list
      Uint16 pos1stObj = containerPos == c_inventorySlotNoItem
         ? c_inventorySlotNoItem
         : GetObjectInfo(containerPos).m_quantity;

      BuildSlotList(pos1stObj);
   }

   return true;
}

void Inventory::RemoveFromContainer(Uint16 pos, Uint16 containerPos)
{
   // if the object to remove is a container, it must not have items in it
   if (IsContainer(GetObjectInfo(pos).m_itemID))
   {
      UaAssert(GetObjectInfo(pos).m_isQuantity == false);
      UaAssert(GetObjectInfo(pos).m_quantity == 0);
   }

   if (containerPos == c_inventorySlotNoItem)
   {
      // removing from topmost container
      UaAssert(pos < slotPlayerObjectsStart); // must be in range
      UaAssert(0 == GetObjectInfo(pos).m_link); // must not have a link

      // as topmost container items don't have m_link set, there is no need
      // to remove linking; just free object
      Free(pos);
   }
   else
   {
      // removing from non-topmost container
      UaAssert(GetObjectInfo(containerPos).m_isQuantity == false);

      // search container for object
      Uint16 link = GetObjectInfo(containerPos).m_quantity;
      UaAssert(link != 0); // no objects in container? should not happen

      if (link == pos)
      {
         // first object in list; link next one as first
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

      // unlink object to remove
      GetObjectInfo(pos).m_link = 0;
   }

   // adding to current container?
   if (containerPos == GetContainerPos())
   {
      // refresh slot list
      Uint16 pos1stObj = containerPos == c_inventorySlotNoItem
         ? c_inventorySlotNoItem
         : GetObjectInfo(containerPos).m_quantity;

      BuildSlotList(pos1stObj);
   }
}

/// \param pos position of object to convert to the floating object
void Inventory::FloatObject(Uint16 pos)
{
   UaAssert(m_floatingObjectPos == c_inventorySlotNoItem); // must have no floating object yet

   if (pos < slotMax)
   {
      // in topmost slot or on paperdoll
      m_floatingObjectPos = InsertItem(GetObjectInfo(pos));
      GetObjectInfo(pos).m_itemID = c_itemIDNone;
   }
   else
   {
      // in a container, or a newly added object
      m_floatingObjectPos = pos;

      // which container is currently open?
      Uint16 containerPos = GetContainerPos();
      if (containerPos == c_inventorySlotNoItem)
      {
         // topmost container; do nothing
      }
      else
      {
         // non-topmost container
         RemoveFromContainer(pos, containerPos);
      }
   }
}

/// Dropping floating objects must support four different scenarios:
/// 1. Dropping object to the symbol of the currently open container (the parent)
///    (both containerPos and objectPos are 0xffff)
/// 2. Dropping object to an empty place in the container, basically adding
///    it to the end of the slot list (only objectPos is 0xffff)
/// 3. Dropping objects to the topmost slots or paperdoll slots (containerPos
///    is 0xffff, objectPos is in the paperdoll pos range)
/// 4. Dropping object to an item in the container (both containerPos and
///    objectPos are set); this generates three further possibilities:
/// 4a. When the object is a container, drop item in this container (if possible)
/// 4b. When the objects can be combined, the new object is dropped (there still
///     may be a floating object afterwards, e.g. lighting incense with a torch)
/// 4c. When the objects can't be combined, the objects just swap
/// \param containerPos position of container item which should be used to drop item
/// \param objectPos object position of object in container to drop onto
/// \return returns if dropping floating item succeeded; reasons for failure are
///         when the container to drop to is full, or the object type cannot be
///         dropped in that container.
bool Inventory::DropFloatingObject(Uint16 containerPos, Uint16 objectPos)
{
   if (!CanDropItemOnPos(GetObjectInfo(m_floatingObjectPos), objectPos))
      return false;

   UaTrace("dropping inventory object at %04x in container %04x...", containerPos, objectPos);

   // add to topmost container / paperdoll?
   if (containerPos == c_inventorySlotNoItem)
   {
      // must be a topmost slot, or a paperdoll pos, or the "no item" pos
      UaAssert(objectPos < slotMax || objectPos == c_inventorySlotNoItem);

      // dropping to topmost container?
      if (objectPos == c_inventorySlotNoItem)
      {
         // scenario 1: add to topmost container, at the end
         bool ret = AddToContainer(m_floatingObjectPos, containerPos);

         if (ret)
            m_floatingObjectPos = c_inventorySlotNoItem;

         return ret;
      }
      else
      {
         // scenario 3: drop on topmost object (0..7) or paperdoll

         // just drop on (maybe existing) object
         return DropOnObject(containerPos, objectPos);
      }
   }
   else
   {
      // no, adding to specified container
      // TODO combine this with above code

      // add to end of container?
      if (objectPos == c_inventorySlotNoItem)
      {
         // scenario 2: adding to end of container
         bool ret = AddToContainer(m_floatingObjectPos, containerPos);

         if (ret)
            m_floatingObjectPos = c_inventorySlotNoItem;

         return ret;
      }
      else
      {
         // scenario 4: drop op (maybe existing) object
         return DropOnObject(containerPos, objectPos);
      }
   }
}

Uint16 Inventory::InsertItem(const ::Underworld::ObjectInfo& info)
{
   Uint16 pos = Allocate();
   if (pos == c_inventorySlotNoItem)
      return pos;

   ObjectInfo& newobj = GetObjectInfo(pos);
   newobj = info;
   newobj.m_link = 0;

   return pos;
}

/// \param pos object position to drop to; may be empty
bool Inventory::DropOnObject(Uint16 containerPos, Uint16 pos)
{
   // is target pos empty?
   if (m_objectList[pos].m_itemID == c_itemIDNone)
   {
      UaTrace("in empty pos %04x\n", pos);

      // empty, then just drop it there
      GetObjectInfo(pos) = GetObjectInfo(m_floatingObjectPos);
      Free(m_floatingObjectPos);
      m_floatingObjectPos = c_inventorySlotNoItem;

      Uint16 pos1stObj = containerPos == c_inventorySlotNoItem
         ? c_inventorySlotNoItem
         : GetObjectInfo(containerPos).m_quantity;

      BuildSlotList(pos1stObj);
      return true;
   }

   // scenario 4a: is it a container object?
   if (IsContainer(m_objectList[pos].m_itemID))
   {
      UaTrace("dropping into container...");

      UaAssert(GetObjectInfo(pos).m_isQuantity == false);

      // add to container, checking restrictions
      bool ret = AddToContainer(m_floatingObjectPos, pos);

      if (ret)
         m_floatingObjectPos = c_inventorySlotNoItem;

      return ret;
   }

   // scenario 4b: dropping on existing object; try to combine it
   if (TryCombineItems(pos, m_floatingObjectPos))
      return true;

   // scenario 4c: no combining; just swap the objects
   SwapObjectWithFloating(containerPos, pos);

   return true;
}

bool Inventory::CanDropItemOnPos(const ::Underworld::ObjectInfo& objectInfo, Uint16 pos) const
{
   // dropping on parent container?
   if (pos == c_inventorySlotNoItem)
      return true; // always allow

   // check if container allows object type and has capacity
   if (IsContainer(m_objectList[pos].m_itemID))
   {
      UaAssert(GetObjectInfo(pos).m_isQuantity == false);

      // check if container limit is reached
      const ContainerProperty& containerProperty =
         m_objectProperties.get().GetContainerProperty(m_objectList[pos].m_itemID);

      if (containerProperty.m_capacity != 0)
      {
         unsigned int weightInTenths = GetObjectWeight(pos);
         if (weightInTenths > containerProperty.m_capacity)
            return false;
      }

      if (!IsContainerAcceptedObjectType(m_objectList[pos].m_itemID, objectInfo.m_itemID))
         return false;
   }

   return CheckObjectTypeSlotType(objectInfo.m_itemID, pos);
}

bool Inventory::CheckObjectTypeSlotType(Uint16 itemID, Uint16 pos) const
{
   if (pos < slotPaperdollStart || pos >= slotMax)
      return true;

   // get inventory object category
   ArmourCategory category = armourNone;

   if (itemID >= 0x0020 && itemID < 0x0040)
   {
      const ArmourAndWearableProperty& property =
         m_objectProperties.get().GetArmourAndWearableProperty(itemID);

      category = property.m_category;
   }

   // check for rings
   if ((pos == slotRightFinger || pos == slotLeftFinger) &&
      category != armourRing)
      return false;

   // check for paperdoll items
   if ((pos == slotPaperdollHead && category != armourHat) ||
      (pos == slotPaperdollChest && category != armourBodyArmour) ||
      (pos == slotPaperdollHands && category != armourGloves) ||
      (pos == slotPaperdollLegs && category != armourLeggings) ||
      (pos == slotPaperdollFeet && category != armourBoots))
      return false;

   return true;
}

bool Inventory::IsContainerAcceptedObjectType(Uint16 containerItemId, Uint16 objectItemId) const
{
   const ContainerProperty& containerProperty =
      m_objectProperties.get().GetContainerProperty(containerItemId);

   // check object type
   switch (containerProperty.m_acceptedObjectClass)
   {
   case -1:
      return true; // any type is accepted

   case 0: // runes
      if (objectItemId >= 0x00e8 && objectItemId <= 0x00ff)
         return true;
      break;

   case 1: // arrows, crossbow bolts
      if (objectItemId == 0x0151 || objectItemId == 0x0152)
         return true;
      break;

   case 2: // scrolls, maps
      if (objectItemId >= 0x0138 && objectItemId <= 0x013f)
         return true;
      break;

   case 3: // edibles
      if (objectItemId >= 0x00b0 && objectItemId <= 0x00bf)
         return true;
      break;

   default:
      UaAssert(false); // invalid accepted object class
      break;
   }

   return false;
}

bool Inventory::TryCombineItems(Uint16 pos1, Uint16 pos2)
{
   Uint16 resultID = c_itemIDNone;
   unsigned int itemDestroyedMask = 0;

   for (const auto& rule : m_itemCombineRuleList)
   {
      if (m_objectList[pos1].m_itemID == rule.m_itemID1 &&
         m_objectList[pos2].m_itemID == rule.m_itemID2)
      {
         resultID = rule.m_resultItemID;
         itemDestroyedMask = rule.m_itemDestroyedMask;
      }

      if (m_objectList[pos1].m_itemID == rule.m_itemID2 &&
         m_objectList[pos2].m_itemID == rule.m_itemID1)
      {
         resultID = rule.m_resultItemID;
         itemDestroyedMask =
            ((rule.m_itemDestroyedMask & 1) << 1) |
            ((rule.m_itemDestroyedMask & 2) >> 1);
      }
   }

   if (resultID == c_itemIDNone || itemDestroyedMask == 0)
      return false;

   UaTrace("combining items %04x and %04x to %04x, mask %i\n",
      m_objectList[pos1].m_itemID,
      m_objectList[pos2].m_itemID,
      resultID,
      itemDestroyedMask);

   Uint16 containerPos = GetContainerPos();

   switch (itemDestroyedMask)
   {
   case 1: // replace first item
      m_objectList[pos1].m_itemID = resultID;
      break;

   case 2: // replace second item
      m_objectList[pos2].m_itemID = resultID;
      break;

   case 3: // replace first and delete second item
      m_objectList[pos1].m_itemID = resultID;
      RemoveFromContainer(pos2, containerPos);
      if (pos2 == m_floatingObjectPos)
         m_floatingObjectPos = c_inventorySlotNoItem;
      break;

   default:
      UaAssert(false); // invalid mask
      break;
   }

   Uint16 pos1stObj = containerPos == c_inventorySlotNoItem
      ? c_inventorySlotNoItem
      : GetObjectInfo(containerPos).m_quantity;

   BuildSlotList(pos1stObj);

   return true;
}

void Inventory::SwapObjectWithFloating(Uint16 containerPos, Uint16 pos)
{
   if (containerPos == c_inventorySlotNoItem)
   {
      // swap with topmost item
      UaTrace("swapping with topmost item\n");
      std::swap(m_objectList[pos], m_objectList[m_floatingObjectPos]);
   }
   else
   {
      // swap floating object in chain
      Uint16 link = GetObjectInfo(containerPos).m_quantity;
      UaAssert(link != 0); // no objects in container? should not happen

      if (link == pos)
      {
         UaTrace("swapping with first item in chain\n");

         // first object in list; change link of next one
         GetObjectInfo(containerPos).m_quantity = m_floatingObjectPos;
         GetObjectInfo(m_floatingObjectPos).m_link = GetObjectInfo(pos).m_link;
      }
      else
      {
         // search for object in list
         while (link != 0)
         {
            if (GetObjectInfo(link).m_link == pos)
            {
               ObjectInfo& dropTargetObj = GetObjectInfo(link);
               ObjectInfo& floatingObj = GetObjectInfo(m_floatingObjectPos);

               // chain previous object to the floating object
               Uint16 nextLink = dropTargetObj.m_link;
               dropTargetObj.m_link = floatingObj.m_link;
               floatingObj.m_link = nextLink;

               UaTrace("swapping with item in chain, in pos %04x\n", link);
               break;
            }

            link = GetObjectInfo(link).m_link;
         }
      }

      m_floatingObjectPos = pos;
      GetObjectInfo(m_floatingObjectPos).m_link = 0;
   }

   Uint16 pos1stObj = containerPos == c_inventorySlotNoItem
      ? c_inventorySlotNoItem
      : GetObjectInfo(containerPos).m_quantity;

   BuildSlotList(pos1stObj);
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
/// is equal to c_inventorySlotNoItem, the "top" slot list is built.
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

unsigned int Inventory::GetInventoryWeight() const
{
   // weight in 1/10 stones
   unsigned int weightInTenth = 0;

   // calculate weight from 8 topmost items and the paperdoll items
   for (Uint16 pos = slotTopmostFirstItem; pos < slotMax; pos++)
      weightInTenth += GetObjectWeight(pos);

   return weightInTenth;
}

unsigned int Inventory::GetObjectWeight(Uint16 pos) const
{
   if (pos == c_inventorySlotNoItem ||
      pos == GetFloatingObjectPos())
      return 0;

   const ObjectInfo& objectInfo = GetObjectInfo(pos);
   if (objectInfo.m_itemID == c_itemIDNone)
      return 0; // unallocated

   const CommonObjectProperty& commonProperty = m_objectProperties.get().GetCommonProperty(objectInfo.m_itemID);

   unsigned int weightInTenth = commonProperty.m_mass;

   if (commonProperty.m_isContainer &&
      objectInfo.m_isQuantity == false &&
      objectInfo.m_quantity != 0)
   {
      Uint16 link = objectInfo.m_quantity;
      while (link != 0)
      {
         weightInTenth += GetObjectWeight(link);

         const ObjectInfo& nextObjectInfo = GetObjectInfo(link);
         link = nextObjectInfo.m_link;
      }
   }

   return weightInTenth;
}
