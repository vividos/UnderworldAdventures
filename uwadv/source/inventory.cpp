/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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
#include "common.hpp"
#include "inventory.hpp"


// ua_inventory methods

void ua_inventory::init()
{
   itemlist.resize(256);
   runebag.reset();
   floating_object = 0xffff;

#ifdef _DEBUG

   Uint16 test_inv[] =
   {
      0x0080, 0x0082, 0x008f, 0x0088, // 8 top slots
      0xffff, 0x012b, 0x013b, 0x0080,
      0x000e, 0x0037, // hands
      0xffff, 0x0094, // shoulder
      0x0038, 0xffff, // finger
      0x0024, 0x0021, 0x0027, 0x002f, 0x002d // paperdoll
   };

   // init with some stuff
   for(unsigned int i=0; i<ua_slot_max; i++)
      itemlist.at(i).item_id = test_inv[i];

   build_slot_link_list(0xffff);
#endif
}

Uint16 ua_inventory::get_slot_item(unsigned int index)
{
   if (index>=slot_links.size())
      throw ua_exception("inventory: illegal slot item access");

   return slot_links.at(index);
}

ua_object_info &ua_inventory::get_item(Uint16 index)
{
   if (index>=itemlist.size())
      throw ua_exception("inventory: illegal item list access");

   return itemlist.at(index);
}

unsigned int ua_inventory::get_num_slots()
{
   return slot_links.size();
}

Uint16 ua_inventory::get_container_item_id()
{
   if (container_stack.size()==0)
      return 0xffff; // topmost

   unsigned int top = container_stack.size()-1;
   Uint16 id = container_stack.at(top);

   return get_item(id).item_id;
}

void ua_inventory::open_container(Uint16 index)
{
   // quantity is used as pointer to content of containers
   Uint16 link1 = get_item(index).quantity;
   build_slot_link_list(link1);

   // set current container as top item
   container_stack.push_back(index);
}

void ua_inventory::close_container()
{
   Uint16 index = 0xffff; // start of topmost item list at list pos 0

   // remove current container link
   if (container_stack.size()!=0)
      container_stack.pop_back();

   // get previous topmost item index
   if (container_stack.size()>0)
      index = container_stack.back();

   // quantity is used as pointer to content of container
   Uint16 link1 = get_item(index).quantity;
   build_slot_link_list(link1);
}

Uint16 ua_inventory::get_floating_item()
{
   return floating_object;
}

bool ua_inventory::float_item_slot(Uint16 slot_index)
{
   // don't do anything when we already have a floating object
   if (floating_object!=0xffff) return false;

   if (container_stack.size()==0 && slot_index>=8)
      return false;

   if (slot_index>=slot_links.size())
      return false;

   return float_item(slot_links.at(slot_index));
}

bool ua_inventory::float_item(Uint16 index)
{
   // don't do anything when we already have a floating object
   if (floating_object!=0xffff) return false;

   floating_object = index;

   // remove object from current container
   if (get_container_item_id()==0xffff)
   {
      // topmost inventory, just remove object
      get_item(index).item_id = 0xffff;
   }
   else
   {
      // remove object by unlinking it
      //    remove_object_link(index);
      unsigned int max = slot_links.size();
      for(unsigned int i=0; i<max; i++)
      {
         if (slot_links.at(i)==index)
         {
            if (i==0)
            {
               // was first object; fix link2 in container
               ua_object_info &cont = get_item(get_container_item_id());
               cont.quantity = get_item(slot_links.at(i)).link1;
            }
            else
            {
               // fix link1 in previous object
               ua_object_info &obj = get_item(slot_links.at(i-1));
               obj.link1 = get_item(slot_links.at(i)).link1;
            }
         }
      }
   }

   // rebuild slot link list
   Uint16 link1 = get_item(get_container_item_id()).link1;
   build_slot_link_list(link1);

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

   Uint16 item = slot_links.at(slot_index);

   if (slot_index==slot_links.size())
   {
      // allocate new item space in itemlist and drop it in that place
      item = allocate_item();
   }

   Uint16 link1 = get_item(item).link1;

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
            get_item(get_container_item_id()).quantity = item;
         }
         else
         {
            // fix the previous item's link1
            get_item(slot_index-1).link1 = item;
         }
      }

      // fix object's link to next
      get_item(item).link1 = link1;

      // reset floating item's link1
      if (floating_object!=0xffff)
         get_item(floating_object).link1 = 0;
   }

   return ret;
}

/*! drops the currently floating object onto the given itemlist position.
    an enum value from ua_inv_paperdoll_slots can also be passed.
    if an object already is at the itemlist position, the object is
    swapped and we have a new floating object.
*/
bool ua_inventory::drop_floating_item(Uint16 index)
{
   ua_object_info &obj = get_item(index);

   if (obj.item_id == 0xffff)
   {
      // object place isn't used; just drop the floating item to that spot
      obj = get_item(floating_object);

      // invalidate object's prior position
      get_item(floating_object).item_id = 0xffff;
   }
   else
   {
      // object place is used

      // make copy of item on that place
      ua_object_info temp = get_item(index);

      // put floating item
      get_item(index) = get_item(floating_object);

      // put new floating item
      get_item(floating_object) = temp;
   }

   return true; // dropping was successfully
}

void ua_inventory::build_slot_link_list(Uint16 link1)
{
   // rebuild slot_links list
   slot_links.clear();

   if (link1==0xffff)
   {
      // build list with first 8 items
      for(unsigned int i=0; i<8; i++)
         slot_links.push_back(i);
   }
   else
   {
      // build normal list
      while (link1 != 0)
      {
         // add to slot links
         slot_links.push_back(link1);

         // get next object
         ua_object_info &obj = get_item(link1);
         link1 = obj.link1;
      }
   }
}

Uint16 ua_inventory::allocate_item()
{
   Uint16 item = 0xffff;

   // search for free item position
   unsigned int max = itemlist.size();
   for(unsigned int i = ua_slot_max; i<max; i++)
   {
      if (itemlist.at(i).item_id == 0xffff)
      {
         // found one
         item = i;
         break;
      }
   }

   return item;
}
