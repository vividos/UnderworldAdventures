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
/*! \file inventory.hpp

   \brief inventory representation

*/
//! \ingroup underworld

//@{

// include guard
#ifndef uwadv_inventory_hpp_
#define uwadv_inventory_hpp_

// needed includes
#include <vector>
#include <bitset>
#include "objects.hpp"


// forward declarations
class ua_underworld;


// constants

//! item position or item id that indicates "no item"
const Uint16 ua_slot_no_item = 0xffff;


// enums

//! inventory slot enum
enum ua_inv_paperdoll_slots
{
   ua_slot_topmost_first_item=0, // topmost inventory; first item list pos

   ua_slot_lefthand=8,
   ua_slot_righthand,
   ua_slot_leftshoulder,
   ua_slot_rightshoulder,

   ua_slot_leftfinger,
   ua_slot_rightfinger,

   ua_slot_paperdoll_legs,
   ua_slot_paperdoll_chest,
   ua_slot_paperdoll_hands,
   ua_slot_paperdoll_feet,
   ua_slot_paperdoll_head,

   ua_slot_max,            // from here on indices in itemlist are free

   ua_slot_paperdoll_start=ua_slot_paperdoll_legs
};


// classes

//! inventory class
class ua_inventory
{
public:
   //! ctor
   ua_inventory(){}

   //! initializes inventory
   void init(ua_underworld *underw);

   // common functionality

   //! returns runebag as bitset
   std::bitset<24>& get_runebag(){ return runebag; }

   //! retrieves an item from an object
   ua_object_info &get_item(Uint16 index);

   //! calculates weight of inventory in stones
   double get_inventory_weight();

   // container functionality

   //! returns number of slots
   unsigned int get_num_slots();

   //! returns position of one of the item slots
   Uint16 get_slot_item(unsigned int index);

   //! returns current container item pos, or ua_slot_no_item when topmost
   Uint16 get_container_item();

   //! returns current container item id, or ua_slot_no_item when topmost
   Uint16 get_container_item_id();

   //! returns true if the given item id is a container
   bool is_container(Uint16 item_id);

   //! opens a container and sets it as current
   void open_container(Uint16 index);

   //! closes current container and enables the previous one
   void close_container();

   //! calculates weight of items in given container in stones
   double get_container_weight(Uint16 cont_pos);

   // floating object functionality

   //! returns currently floating item, or ua_slot_no_item when none floats
   Uint16 get_floating_item();

   //! makes an item "floating", by slot index
   bool float_item_slot(Uint16 slot_index);

   //! makes an item in the itemlist "floating"
   bool float_item(Uint16 index);

   //! drops a floating item into the given slot
   bool drop_floating_item_slot(Uint16 slot_index);

   //! drops a floating item onto the given itemlist index
   bool drop_floating_item(Uint16 index);

   //! drops a floating item into the parent's list
   bool drop_floating_item_parent();

   //! inserts new item and makes it floating; returns itemlist pos
   Uint16 insert_floating_item(ua_object_info& objinfo);

   // loading / saving / importing

   //! loads a savegame
   void load_game(ua_savegame &sg);

   //! saves to a savegame
   void save_game(ua_savegame &sg);

protected:
   //! builds new slot link list
   void build_slot_link_list(Uint16 link);

   //! allocates a new item; returns ua_slot_no_item when no itemlist position is free
   Uint16 allocate_item();

   //! removes an object from current inventory by unlinking it
   void unlink_object(Uint16 item);

   //! appends item to link1'ed container list
   void append_item(Uint16 cont, Uint16 item);

protected:
   //! underworld object
   ua_underworld* underw;

   //! rune bag
   std::bitset<24> runebag;

   //! item list
   std::vector<ua_object_info> itemlist;

   //! slot links of current items
   std::vector<Uint16> slot_links;

   //! stack with currently opened container
   std::vector<Uint16> container_stack;

   //! link to floating object
   Uint16 floating_object;
};


#endif
//@}
