/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Michael Fink

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
/*! \file objects.hpp

   \brief objects management

*/

// include guard
#ifndef uwadv_objects_hpp_
#define uwadv_objects_hpp_

// needed includes
#include <vector>
#include "settings.hpp"
#include "savegame.hpp"


// constants

//! item id for unused object
const Uint16 ua_item_none = 0xffff;


// structs

//! basic object info
struct ua_object_info
{
   //! struct ctor
   ua_object_info();

   //! object item id
   Uint16 item_id;

   //! true when object is enchanted
   bool enchanted;

   //! true when "quantity" is a special link
   bool is_link;


   //! item quality
   Uint16 quality;

   //! object chain link
   Uint16 link;

   //! object owner
   Uint16 owner;

   //! item quantity / special
   Uint16 quantity;


   //! custom object data
   std::vector<Uint16> data;
};


//! extended infos for object
struct ua_object_info_ext
{
   //! struct ctor
   ua_object_info_ext();

   //! fractional x and y positions in tile
   double xpos, ypos;

   //! z position
   double zpos;

   //! direction (0..7)
   Uint16 dir;
};


// classes

//! object in levelmap
class ua_object
{
public:
   //! ctor
   ua_object(){}
   //! dtor
   virtual ~ua_object(){}

   //! returns object info
   ua_object_info &get_object_info(){ return info; }

   //! returns extended object info
   ua_object_info_ext &get_ext_object_info(){ return extinfo; }

   //! loads object from savegame
   void load_object(ua_savegame &sg);

   //! saves object to savegame
   void save_object(ua_savegame &sg);

protected:
   //! basic object info
   ua_object_info info;

   //! extended object info
   ua_object_info_ext extinfo;
};


//! object list class
class ua_object_list
{
public:
   //! ctor
   ua_object_list();
   //! dtor
   ~ua_object_list();

   // master object list access

   //! returns list pos of first object in element
   Uint16 get_tile_list_start(unsigned int xpos, unsigned int ypos);

   //! returns an object at a specific list pos
   ua_object &get_object(Uint16 at){ return master_obj_list[at]; }

   // todo: object insert, remove, etc.


   // loading / saving / importing

   //! loads a savegame
   void load_game(ua_savegame &sg);

   //! saves to a savegame
   void save_game(ua_savegame &sg);

   //! imports an object list
   void import_objs(FILE *fd);

private:
   //! adds object to master object list and follows link1 and link2 objs
   void addobj_follow(Uint32 objprop[0x400*2], Uint8 npcinfo[0x100*19],
      Uint16 objpos);

protected:
   //! indices for each tile into master object list
   std::vector<Uint16> tile_index;

   //! master object list
   std::vector<ua_object> master_obj_list;
};

#endif
