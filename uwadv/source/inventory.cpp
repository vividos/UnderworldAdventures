/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

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
#include "underworld.hpp"


// ua_inventory methods

void ua_inventory::init(ua_underworld *theunderw)
{
   underw = theunderw;

   itemlist.resize(256);
   runebag.reset();
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

Uint16 ua_inventory::get_slot_item(unsigned int index)
{
   if (index>=slot_links.size())
      return ua_slot_no_item;

   return slot_links[index];
}

ua_object_info &ua_inventory::get_item(Uint16 index)
{
   if (index>=itemlist.size())
      throw ua_exception("inventory: illegal item list access");

   return itemlist[index];
}

unsigned int ua_inventory::get_num_slots()
{
   return slot_links.size();
}

Uint16 ua_inventory::get_container_item()
{
   if (container_stack.size()==0)
      return ua_slot_no_item; // topmost

   unsigned int top = container_stack.size()-1;
   return container_stack[top];
}

Uint16 ua_inventory::get_container_item_id()
{
   Uint16 id = get_container_item();
   return id==ua_slot_no_item ? ua_slot_no_item : get_item(id).item_id;
}

bool ua_inventory::is_container(Uint16 item_id)
{
   return underw->get_scripts().lua_inventory_is_container(item_id);
}

void ua_inventory::open_container(Uint16 index)
{
   // check if user wants to open container that already is open
   if (get_container_item()==index)
      return; // don't do anything

   // check if user wants to open container from paperdoll
   if (index>=8 && index<ua_slot_max)
   {
      // clear container stack; container to open becomes topmost
      container_stack.clear();
   }

   // quantity is used as pointer to content of containers
   Uint16 link = get_item(index).quantity;
   build_slot_link_list(link);

   // set current container as top item
   container_stack.push_back(index);
}

void ua_inventory::close_container()
{
   Uint16 index = ua_slot_no_item; // start of topmost item list at list pos 0

   // remove current container link
   if (container_stack.size()!=0)
      container_stack.pop_back();

   // get previous topmost item index
   if (container_stack.size()>0)
   {
      index = container_stack.back();

      // quantity is used as pointer to content of container
      index = get_item(index).quantity;
   }

   build_slot_link_list(index);
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

   ua_object_info &obj = get_item(index);

   // check if object can be placed in that slot
   {
      // get inventory object category
      ua_inv_item_category cat =
         underw->get_scripts().lua_inventory_categorize_item(
            get_item(floating_object).item_id);

      // check for rings
      if ( (index == ua_slot_rightfinger || index == ua_slot_leftfinger ) &&
         cat != ua_inv_cat_ring)
            return false;

      // check for paperdoll items
      if ( (index==ua_slot_paperdoll_head && cat != ua_inv_cat_head) ||
           (index==ua_slot_paperdoll_chest && cat != ua_inv_cat_chest) ||
           (index==ua_slot_paperdoll_hands && cat != ua_inv_cat_hands) ||
           (index==ua_slot_paperdoll_legs &&  cat != ua_inv_cat_legs) ||
           (index==ua_slot_paperdoll_feet &&  cat != ua_inv_cat_feet) )
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

            // try to combine objects
            ua_obj_combine_result ret =
               underw->get_scripts().lua_inventory_combine_obj(obj.item_id,item_id2,result_id);

            if (ret!=ua_obj_cmb_failed)
            {
               switch(ret)
               {
               case ua_obj_cmb_dstr_first:
                  // existing item is to be erased; just replace item id
                  obj.item_id = result_id;
                  break;

               case ua_obj_cmb_dstr_second:
                  // dropped item is to be erased; just replace item id
                  get_item(floating_object).item_id = result_id;
                  break;

               case ua_obj_cmb_dstr_both:
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

bool ua_inventory::drop_floating_item_parent()
{
   if (get_container_item() == ua_slot_no_item)
      return false; // no dropping to parent in topmost

   // get parent container item
   if (container_stack.size()>1)
   {
      // dropping to parent container that is not the topmost one
      unsigned int parent = container_stack[container_stack.size()-2];

      // follow link list to last object
      Uint16 link = get_item(parent).quantity;
      if (link==0)
         return false; // impossible, since at least the container we are in
                       // must be in the parent container

      append_item(parent,floating_object);
      floating_object = ua_slot_no_item;
   }
   else
   {
      // dropping to topmost container

      // try to find a unused space
      for(unsigned int i=0; i<8; i++)
      {
         if (get_item(i).item_id == ua_slot_no_item)
         {
            // copy floating object to that slot
            get_item(i) = get_item(floating_object);

            // unlink object
            //unlink_object(floating_object);

            floating_object = ua_slot_no_item;
            return true;
         }
      }
      return false; // no space to drop
   }

   return true; // dropping was successful
}

void ua_inventory::build_slot_link_list(Uint16 link)
{
   // rebuild slot_links list
   slot_links.clear();

   if (link==ua_slot_no_item)
   {
      // build list with first 8 items
      for(unsigned int i=0; i<8; i++)
         slot_links.push_back(i);
   }
   else
   {
      // build normal list
      while (link != 0)
      {
         // add to slot links
         slot_links.push_back(link);

         // get next object
         ua_object_info &obj = get_item(link);
         link = obj.link;
      }
   }
}

Uint16 ua_inventory::allocate_item()
{
   Uint16 item = ua_slot_no_item;

   // search for free item position
   unsigned int max = itemlist.size();
   for(unsigned int i = ua_slot_max; i<max; i++)
   {
      if (itemlist[i].item_id == ua_slot_no_item)
      {
         // found one
         item = i;
         break;
      }
   }

   return item;
}

void ua_inventory::unlink_object(Uint16 item)
{
   // remove object by unlinking it
   unsigned int max = slot_links.size();
   for(unsigned int i=0; i<max; i++)
   {
      if (slot_links[i]==item)
      {
         if (i==0)
         {
            // was first object; fix link2 in container
            ua_object_info &cont = get_item(get_container_item());
            cont.quantity = get_item(slot_links[i]).link;
         }
         else
         {
            // fix link in previous object
            ua_object_info &obj = get_item(slot_links[i-1]);
            obj.link = get_item(slot_links[i]).link;
         }
         break;
      }
   }

   // rebuild slot link list
   Uint16 link = get_item(get_container_item()).quantity;
   if (link==0)
      slot_links.clear();
   else
      build_slot_link_list(link);
}

void ua_inventory::append_item(Uint16 cont, Uint16 item)
{
   Uint16 link = get_item(cont).quantity;

   // check if container already has a link'ed list
   if (link==0)
   {
      // add new item
      get_item(cont).quantity = item;
   }
   else
   {
      // find last object
      while (get_item(link).link != 0)
      {
         // get next object
         link = get_item(link).link;
      }
      // link is last object

      // append to list
      get_item(link).link = item;
      get_item(item).link = 0;
   }
}

double ua_inventory::get_weight_avail()
{
   return 42.0;
}

void ua_inventory::load_game(ua_savegame& sg)
{
   sg.begin_section("inventory");

   // load runebag
   for(unsigned int r=0; r<26; r++)
      runebag.set(r, sg.read8()!=0 );

   // load itemlist
   for(unsigned int i=0; i<256; i++)
      itemlist[i].load_info(sg);

   // TODO restore slot_links, container_stack (?)

   sg.end_section();
}

void ua_inventory::save_game(ua_savegame& sg)
{
   sg.begin_section("inventory");

   // save runebag
   for(unsigned int r=0; r<26; r++) {
      // Hope this doesn't break anything but bitset::at() is not available in gcc/mingw.. 'test' should be the 'official' function to use (telemachos) 
      // sg.write8(runebag.at(r) ? 1 : 0);
      sg.write8(runebag.test(r) ? 1 : 0);
   }  
   
   
   // save itemlist
   for(unsigned int i=0; i<256; i++)
      itemlist[i].save_info(sg);

   // TODO store slot_links, container_stack (?)

   sg.end_section();
}
