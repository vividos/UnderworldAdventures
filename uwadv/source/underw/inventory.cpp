/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2002,2003,2004,2005,2006 Underworld Adventures Team

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file inventory.cpp

   \brief inventory implementation

*/

// needed includes
#include "underw.hpp"
#include "inventory.hpp"
#include "savegame.hpp"

using Underworld::Inventory;

// Inventory methods

Inventory::Inventory()
:m_uiFloatingObjectPos(c_uiInventorySlotNoItem)
{
}

void Inventory::Create()
{
   m_vecObjectList.resize(0x0100);

   m_uiFloatingObjectPos = c_uiInventorySlotNoItem;
}

void Inventory::Destroy()
{
   m_vecObjectList.clear();
   m_vecSlotList.clear();
   m_vecContainerStack.clear();

   m_uiFloatingObjectPos = c_uiInventorySlotNoItem;
}

Uint16 Inventory::Allocate()
{
   // start after paperdoll objects
   Uint16 uiPos = slotMax;
   Uint16 uiSize = static_cast<Uint16>(m_vecObjectList.size());
   while (uiPos < uiSize && m_vecObjectList[uiPos].m_uiItemID != c_uiItemIDNone)
      uiPos++;

   // hit end of list?
   if (uiPos >= m_vecObjectList.size())
   {
      // already at maximum size?
      if (m_vecObjectList.size() >= 0x10000)
         throw Base::RuntimeException("Error while enlarging inventory list; already at maximum size");

      // new pos is start of enlarged list
      uiPos = static_cast<Uint16>(m_vecObjectList.size());

      // enlarge list by factor 1,25
      unsigned int uiNewSize = m_vecObjectList.size();
      uiNewSize += uiNewSize >> 2;

      // limit to Uint16 range
      if (uiNewSize >= 0x10000)
         uiNewSize = 0x10000;

      m_vecObjectList.resize(uiNewSize);
   }

   return uiPos;
}

void Inventory::Free(Uint16 uiPos)
{
   UaAssert(uiPos != c_uiInventorySlotNoItem);
   UaAssert(uiPos < m_vecObjectList.size()); // object must not be part of a tile list

   m_vecObjectList[uiPos].m_uiItemID = c_uiItemIDNone;
}

Uint16 Inventory::GetSlotListPos(unsigned int uiIndex) const
{
   UaAssert(uiIndex < GetNumSlots());

   return m_vecSlotList[uiIndex];
}

/*! \todo use ObjectProperties to check */
bool Inventory::IsContainer(Uint16 uiItemID) const
{
   return uiItemID >= 0x0080 && uiItemID < 0x008f;
}

Uint16 Inventory::GetContainerPos() const
{
   if (m_vecContainerStack.size() == 0)
      return c_uiInventorySlotNoItem; // topmost

   unsigned int uiTopPos = m_vecContainerStack.size()-1;

   UaAssert(true == IsContainer(GetObject(m_vecContainerStack[uiTopPos]).m_uiItemID));
   return m_vecContainerStack[uiTopPos];
}

Uint16 Inventory::GetParentContainerPos() const
{
   if (m_vecContainerStack.size() <= 1)
      return c_uiInventorySlotNoItem; // topmost

   unsigned int uiParentPos = m_vecContainerStack.size()-2;

   UaAssert(true == IsContainer(GetObject(m_vecContainerStack[uiParentPos]).m_uiItemID));
   return m_vecContainerStack[uiParentPos];
}

/*! \todo check if object is in current container */
void Inventory::OpenContainer(Uint16 uiPos)
{
   UaAssert(true == IsContainer(GetObject(uiPos).m_uiItemID));
   UaAssert(false == GetObject(uiPos).m_bIsQuantity);
   UaAssert(GetContainerPos() != uiPos); // container already open

   // check if user wants to open container from paperdoll
   if (uiPos >= slotPlayerObjectsStart && uiPos < slotMax)
   {
      // clear container stack; container to open becomes topmost
      m_vecContainerStack.clear();
   }

   // quantity field is used as pointer to content of containers
   Uint16 uiLink = GetObject(uiPos).m_uiQuantity;

   BuildSlotList(uiLink);

   // set current container as top item
   m_vecContainerStack.push_back(uiPos);
}

void Inventory::CloseContainer()
{
   UaAssert(m_vecContainerStack.size() > 0);
   UaAssert(true == IsContainer(GetObject(m_vecContainerStack.back()).m_uiItemID));

   Uint16 uiPos = c_uiInventorySlotNoItem; // start of topmost item list at list pos 0

   // remove current container link
   if (m_vecContainerStack.size() > 0)
      m_vecContainerStack.pop_back();

   // get previous topmost item index
   if (m_vecContainerStack.size() > 0)
   {
      uiPos = m_vecContainerStack.back();

      UaAssert(false == GetObject(uiPos).m_bIsQuantity);

      // quantity is used as pointer to content of container
      uiPos = GetObject(uiPos).m_uiQuantity;
   }

   BuildSlotList(uiPos);
}

/*! Object must not be member of a container yet.
    \param[in,out] uiPos position of item to add; if an item is moved, the new
                   position is stored in this parameter.
    \param uiContainerPos position of container to add item to; when
           c_uiInventorySlotNoItem is given, it is added to topmost container
    \return true if object could be added, or false when not (e.g. when
            container is full; when false is returned, the value of uiPos
            isn't change.
*/
bool Inventory::AddToContainer(Uint16& uiPos, Uint16 uiContainerPos)
{
   UaAssert(GetObject(uiPos).m_uiLink == 0);

   if (uiContainerPos == c_uiInventorySlotNoItem)
   {
      // adding to topmost container

      // try to find an unused space
      Uint16 ui;
      for (ui=0; ui<8; ui++)
      {
         if (GetObject(ui).m_uiItemID == c_uiItemIDNone)
         {
            // copy object to that slot
            GetObject(ui) = GetObject(uiPos);

            // free object
            Free(uiPos);
            uiPos = ui;
            break;
         }
      }

      if (ui == 8)
         return false; // no space to drop
   }
   else
   {
      UaAssert(true == IsContainer(GetObject(uiContainerPos).m_uiItemID));
      UaAssert(false == GetObject(uiContainerPos).m_bIsQuantity);

      // add to container
      if (GetObject(uiContainerPos).m_uiQuantity == 0)
      {
         // no object in container
         GetObject(uiContainerPos).m_uiQuantity = uiPos;
      }
      else
      {
         // search last object in list
         Uint16 uiLink = GetObject(uiContainerPos).m_uiQuantity;
         Uint16 uiLastLink = uiLink;
         while (uiLink != 0)
         {
            uiLastLink = uiLink;
            uiLink = GetObject(uiLink).m_uiLink;
         }

         GetObject(uiLastLink).m_uiLink = uiPos;
      }
   }

   if (uiContainerPos == GetContainerPos())
   {
      Uint16 uiPos1stObj = uiContainerPos == c_uiInventorySlotNoItem ? c_uiInventorySlotNoItem : GetObject(uiContainerPos).m_uiQuantity;
      BuildSlotList(uiPos1stObj);
   }

   return true;
}

void Inventory::RemoveFromContainer(Uint16 uiPos, Uint16 uiContainerPos)
{
   // if the object to remove is a container, it must not have items in it
   if (IsContainer(GetObject(uiPos).m_uiItemID))
   {
      UaAssert(false == GetObject(uiPos).m_bIsQuantity);
      UaAssert(GetObject(uiPos).m_uiQuantity == 0);
   }

   if (uiContainerPos == c_uiInventorySlotNoItem)
   {
      // remove from topmost container
      UaAssert(uiPos < slotPlayerObjectsStart); // must be in range
      UaAssert(0 == GetObject(uiPos).m_uiLink); // must not have a link

      // as topmost container items don't have m_uiLink set, there is no need
      // to remove linking; just free object
      Free(uiPos);
   }
   else
   {
      UaAssert(false == GetObject(uiContainerPos).m_bIsQuantity);

      // search container for object
      Uint16 uiLink = GetObject(uiContainerPos).m_uiQuantity;
      UaAssert(uiLink != 0); // no objects in container?

      if (uiLink == uiPos)
      {
         // first object in list
         GetObject(uiContainerPos).m_uiQuantity = GetObject(uiLink).m_uiLink;
      }
      else
      {
         // search for object in list
         while (uiLink != 0)
         {
            if (GetObject(uiLink).m_uiLink == uiPos)
            {
               // chain to object after the one to remove
               GetObject(uiLink).m_uiLink = GetObject(uiPos).m_uiLink;
               break;
            }

            uiLink = GetObject(uiLink).m_uiLink;
         }
      }

      GetObject(uiPos).m_uiLink = 0;
   }

   if (uiContainerPos == GetContainerPos())
   {
      Uint16 uiPos1stObj = uiContainerPos == c_uiInventorySlotNoItem ? c_uiInventorySlotNoItem : GetObject(uiContainerPos).m_uiQuantity;
      BuildSlotList(uiPos1stObj);
   }
}

/*! \param uiPos position of object to convert to the floating object
    \return if the given object can be made floated; reasons for not be able
    \note check weight constraints before making an object floating; an object
          is considered part of the inventory as soon as it is added as
          floating object.
    \todo check if bool return value is needed; not needed, remove it!
*/
bool Inventory::FloatObject(Uint16 uiPos)
{
   UaAssert(m_uiFloatingObjectPos == c_uiInventorySlotNoItem); // must have no floating object
   UaAssert(GetObject(uiPos).m_uiLink == 0); // must not be in a container

   m_uiFloatingObjectPos = uiPos;

   return true;
}

/*! Dropping floating objects must support four different scenarios:
    - dropping objects to the symbol of the currently open container (the parent)
    - dropping objects to an empty place in the container, basically adding
      it to the end of the slot list
    - dropping objects to the paperdoll or paperdoll slots
    - dropping object to an item in the container; this generates three further
      possibilities:
      - when the object is a container, drop item in this container (if possible)
      - when the objects can be combined, the new object is dropped (there still
        may be a floating object afterwards, e.g. lighting incense with a torch)
      - when the objects can't be combined, the objects just swap
    \param uiContainerPos container pos which should be used to drop item
    \param uiObjectIndex object position in container
    \return returns if dropping floating item succeeded; reasons for failure is
            when the container to drop to is full, the object type cannot be
            dropped in that container, ... TODO
*/
bool Inventory::DropFloatingObject(Uint16 uiContainerPos, Uint16 uiObjectPos)
{
   // add to topmost container / paperdoll?
   if (uiContainerPos == c_uiInventorySlotNoItem)
   {
      // must be a topmost slot or paperdoll pos, or "no item" pos
      UaAssert(uiObjectPos < Underworld::slotMax || uiObjectPos == c_uiInventorySlotNoItem);

      // dropping to topmost slot or paperdoll?
      if (uiObjectPos < Underworld::slotMax)
      {
         // just drop on existing object
         return DropOnObject(uiContainerPos, uiObjectPos);
      }
      else
      {
         // add to topmost container, at end
         bool bRet = AddToContainer(m_uiFloatingObjectPos, uiContainerPos);

         if (bRet)
            m_uiFloatingObjectPos = c_uiInventorySlotNoItem;
         return bRet;
      }
   }
   else
   {
      // no, adding to specified container

      // add to end of container?
      // TODO move together with above code
      if (uiObjectPos == c_uiInventorySlotNoItem)
      {
         bool bRet = AddToContainer(m_uiFloatingObjectPos, uiContainerPos);

         if (bRet)
            m_uiFloatingObjectPos = c_uiInventorySlotNoItem;

         return bRet;
      }
      else
      {
         // swap at specific pos in container
         DropOnObject(uiContainerPos, uiObjectPos);
      }
   }

   return false;
}

bool Inventory::DropOnObject(Uint16 uiContainerPos, Uint16 uiPos)
{
   UaAssert(false);
   return false;
}

void Inventory::Load(Base::Savegame& sg)
{
   Destroy();
   Create();

   sg.BeginSection("inventory");

   // load inventory object list
   Uint16 uiMax = sg.Read16();
   m_vecObjectList.resize(uiMax);

   for (Uint16 ui=0; ui<uiMax; ui++)
      m_vecObjectList[ui].Load(sg);

   // load container stack
   uiMax = sg.Read16();
   m_vecContainerStack.resize(uiMax);

   for (Uint16 ui2=0; ui2<uiMax; ui2++)
      m_vecContainerStack[ui2] = sg.Read16();

   m_uiFloatingObjectPos = sg.Read16();

   sg.EndSection();

   // rebuild current container list
   BuildSlotList(GetContainerPos());
}

void Inventory::Save(Base::Savegame& sg) const
{
   sg.BeginSection("inventory");

   // save inventory object list
   Uint16 uiMax = static_cast<Uint16>(m_vecObjectList.size());
   sg.Write16(uiMax);

   for (Uint16 ui=0; ui<uiMax; ui++)
      m_vecObjectList[ui].Save(sg);

   // save container stack
   uiMax = static_cast<Uint16>(m_vecContainerStack.size());
   sg.Write16(uiMax);

   for (Uint16 ui2=0; ui2<uiMax; ui2++)
      sg.Write16(m_vecContainerStack[ui2]);

   sg.Write16(m_uiFloatingObjectPos);

   sg.EndSection();
}

/*! \param uiPos position of first object to build slot list with. when uiPos
           is equal to c_uiInventorySlotNoItem, the "top" slot list is built.
*/
void Inventory::BuildSlotList(Uint16 uiPos)
{
   // rebuild slot list
   m_vecSlotList.clear();

   if (uiPos == c_uiInventorySlotNoItem)
   {
      // build list with first 8 items
      for (Uint16 i=0; i<8; i++)
         m_vecSlotList.push_back(i);
   }
   else
   {
      // build normal list
      while (uiPos != 0)
      {
         // add to slot links
         m_vecSlotList.push_back(uiPos);

         // get next object
         uiPos = GetObject(uiPos).m_uiLink;
      }
   }
}

#if 0

// ua_inventory methods

void ua_inventory::init(ua_underworld* theunderw)
{
   underw = theunderw;

   itemlist.resize(256);
   floating_object = ua_slot_no_item;

#ifdef HAVE_DEBUG
   // test item inventory
   Uint16 test_inv[] =
   {
      0x0080, 0x0082, 0x008f, 0x0088, // 8 top slots
      0x0095, 0x0116, 0x00b4, 0x0080,
      0x000e, 0x0037, // hands
      0xffff, 0x0094, // shoulder
      0x0038, 0xffff, // finger
      0x0024, 0x0021, 0x0027, 0x002f, 0x002d, // paperdoll
      0x00b6, 0x0130, 0x0138, 0x0031
   };

   // init with some stuff
   for(unsigned int i=0; i<ua_slot_max+4; i++)
   {
      itemlist[i].item_id = test_inv[i];
      itemlist[i].link = 0;
   }

   // set some container links
   get_item(0).quantity = ua_slot_max;
   get_item(1).quantity = ua_slot_max+1;
   get_item(3).quantity = ua_slot_max+2;
   get_item(7).quantity = ua_slot_max+3;
#endif

   // build link list
   build_slot_link_list(ua_slot_no_item);
}

double ua_inventory::get_inventory_weight()
{
   ua_object_properties& prop = underw->get_obj_properties();

   unsigned int weight = 0;

   for(unsigned int i=0; i<256; i++)
   if (itemlist[i].item_id != ua_slot_no_item && i != floating_object)
   {
      ua_common_obj_property& cprop = prop.get_common_property(itemlist[i].item_id);
      weight += cprop.mass;
   }

   return weight / 10.0;
}

double ua_inventory::get_container_weight(Uint16 cont_pos)
{
   // check if we got a container item pos
   if (!is_container(get_item(cont_pos).item_id))
      return 0.0;

   ua_object_properties& prop = underw->get_obj_properties();

   unsigned int weight = 0;
   Uint16 link = get_item(cont_pos).quantity;

   // build normal list
   while (link != 0)
   {
      // get object
      ua_object_info& obj = get_item(link);

      // add weight
      weight += prop.get_common_property(obj.item_id).mass;

      // next object
      link = obj.link;
   }

   return weight / 10.0;
}

Uint16 ua_inventory::get_floating_item()
{
   return floating_object;
}

bool ua_inventory::float_item_slot(Uint16 slot_index)
{
   // don't do anything when we already have a floating object
   if (floating_object != ua_slot_no_item) return false;

   if (container_stack.size()==0 && slot_index>=8)
      return false;

   if (slot_index>=slot_links.size())
      return false;

   return float_item(slot_links[slot_index]);
}

bool ua_inventory::float_item(Uint16 index)
{
   // don't do anything when we already have a floating object
   if (floating_object != ua_slot_no_item) return false;

   // check if user wants to "float" current container
   if (get_container_item()==index)
      return false; // don't allow that

   // remove object from current container
   if (get_container_item() == ua_slot_no_item || index<ua_slot_max)
   {
      // topmost inventory or paperdoll slot; allocate a new object position

      floating_object = allocate_item();
      if (floating_object == ua_slot_no_item)
         return false; // no more space in inventory

      // copy item
      get_item(floating_object) = get_item(index);

      // remove old item
      get_item(index).item_id = ua_slot_no_item;
   }
   else
   {
      floating_object = index;

      unlink_object(index);
   }

   return true;
}

/*! drops the currently floating object onto the given slot index.
    if an object already is at the slot position, the object is
    swapped and we have a new floating object.
*/
bool ua_inventory::drop_floating_item_slot(Uint16 slot_index)
{
   if (container_stack.size()==0 && slot_index>=8)
      return false;

   if (slot_index>slot_links.size()/* || slot_index>=get_container_max_size()*/)
      return false;

   Uint16 item = slot_links[slot_index];

   if (slot_index==slot_links.size())
   {
      // allocate new item space in itemlist and drop it in that place
      item = allocate_item();
      if (item == ua_slot_no_item)
         return false; // no more space in inventory
   }

   Uint16 link = get_item(item).link;

   // drop on item list
   bool ret = drop_floating_item(item);

   if (ret)
   {
      // fix links
      if (container_stack.size()!=0)
      {
         if (slot_index==0)
         {
            // fix the container's link2
            get_item(get_container_item()).quantity = item;
         }
         else
         {
            // fix the previous item's link
            get_item(slot_index-1).link = item;
         }
      }

      // fix object's link to next
      get_item(item).link = link;

      // reset floating item's link
      if (floating_object!=0xffff)
         get_item(floating_object).link = 0;
   }

   return ret;
}

/*! drops the currently floating object onto the given itemlist position.
    an enum value from ua_inv_paperdoll_slots can also be passed.

    if an object already is at the itemlist position, the object is
    swapped and we have a new floating object.

    if the object is dropped onto a container, it is added to the container's
    contents.

    if the function is called with ua_slot_no_item as index, a new item is
    allocated.
*/
bool ua_inventory::drop_floating_item(Uint16 index)
{
   // dropping into empty container list?
   if (index == ua_slot_no_item)
   {
      // append the floating item to the current slot list
      append_item(get_container_item(),floating_object);
      floating_object = ua_slot_no_item;

      // rebuild list
      build_slot_link_list(get_item(get_container_item()).quantity);

      return true;
   }

   ua_object_info& obj = get_item(index);

   // check if object can be placed in that slot
   {
      // get inventory object category
      Uint16 item_id = get_item(floating_object).item_id;

      ua_armour_category category = ua_armour_none;

      if (item_id>=0x0020 && item_id<0x0040)
      {
         ua_armour_wearable_property& prop =
            underw->get_obj_properties().get_armour_property(item_id);

         category = prop.category;
      }

      // check for rings
      if ((index == ua_slot_rightfinger || index == ua_slot_leftfinger ) &&
          category != ua_armour_ring)
         return false;

      // check for paperdoll items
      if ( (index == ua_slot_paperdoll_head && category != ua_armour_hat) ||
           (index == ua_slot_paperdoll_chest && category != ua_armour_body_armour) ||
           (index == ua_slot_paperdoll_hands && category != ua_armour_gloves) ||
           (index == ua_slot_paperdoll_legs &&  category != ua_armour_leggings) ||
           (index == ua_slot_paperdoll_feet &&  category != ua_armour_boots) )
         return false;
   }

   if (obj.item_id == ua_slot_no_item)
   {
      // object place isn't used; just drop the floating item to that spot
      obj = get_item(floating_object);

      // invalidate object's prior position
      get_item(floating_object).item_id = ua_slot_no_item;

      floating_object = ua_slot_no_item;
   }
   else
   {
      // object place is used

      // check if item was dropped onto a container
      if (is_container(get_item(index).item_id))
      {
         // yes, a container

         //! \todo check if container likes that type of object

         // add it to the container's list
         append_item(index,floating_object);
         floating_object = ua_slot_no_item;

         // dropping into current container?
         if (index==get_container_item())
         {
            // then rebuild slot list
            build_slot_link_list(get_item(get_container_item()).quantity);
         }
      }
      else
      {
         // no, simple object

         // check if objects can be combined
         {
            Uint16 item_id2 = get_item(floating_object).item_id;
            Uint16 result_id = ua_slot_no_item;
            ua_item_combine_status status = ua_item_combine_failed;

            // try to combine objects
            ua_scripting* scripting = underw->get_scripting();
            if (scripting != NULL)
                status = scripting->item_combine(obj.item_id, item_id2,
                result_id);

            if (status != ua_item_combine_failed)
            {
               switch(status)
               {
               case ua_item_combine_destroy_first:
                  // existing item is to be erased; just replace item id
                  obj.item_id = result_id;
                  break;

               case ua_item_combine_destroy_second:
                  // dropped item is to be erased; just replace item id
                  get_item(floating_object).item_id = result_id;
                  break;

               case ua_item_combine_destroy_both:
                  // all two items are erased
                  obj.item_id = result_id;
                  get_item(floating_object).item_id = ua_slot_no_item;
                  floating_object = ua_slot_no_item;
                  break;

               default:
                  break;
               }

               return true; // combining was successful
            }
         }

         // make copy of item on that place
         ua_object_info temp = get_item(index);

         // put floating item
         get_item(index) = get_item(floating_object);

         // correct link list
         get_item(index).link = temp.link;
         temp.link = 0;

         // put new floating item
         get_item(floating_object) = temp;
      }
   }

   return true; // dropping was successful
}

Uint16 ua_inventory::insert_floating_item(ua_object_info& objinfo)
{
   // already have a floating object?
   if (floating_object != ua_slot_no_item)
      return ua_slot_no_item;

   Uint16 pos = allocate_item();
   if (pos == ua_slot_no_item)
      return pos;

   ua_object_info& newobj = get_item(pos);
   newobj = objinfo;
   newobj.link = 0;

   // make floating
   floating_object = pos;

   return pos;
}

#endif
