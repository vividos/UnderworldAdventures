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
/*! \file objects.hpp

   \brief objects management

*/

// include guard
#ifndef __uwadv_objects_hpp_
#define __uwadv_objects_hpp_

// needed includes
#include <vector>
#include "settings.hpp"
#include "texture.hpp"
#include "quadtree.hpp"
#include "savegame.hpp"


// constants

const Uint16 ua_item_none = 0xffff;


// enums

enum ua_obj_type
{
   //! no object
   ua_obj_none=0,

   //! item object, can be picked up
   ua_obj_item,

   //! NPC object
   ua_obj_npc,

   //! decal objects
   ua_obj_decal,

   //! 3d objects
   ua_obj_3d,
   //! invisible, e.g. traps, etc.
   ua_obj_invisible
};


// structs

//! basic object info
struct ua_object_info
{
   //! struct ctor
   ua_object_info()
      :type(ua_obj_none),item_id(ua_item_none),quantity(0),
      quality(0),link1(0){}

   //! object type
   ua_obj_type type;

   //! object item id
   Uint16 item_id;

   //! item quantity
   Uint16 quantity;

   //! item quality
   Uint16 quality;

   //! object chain link
   Uint16 link1;

   //! custom object data
   std::vector<Uint16> data;
};


// forward declarations
class ua_level;


// classes

//! object in levelmap
class ua_object
{
public:
   //! ctor
   ua_object():xpos(0.0),ypos(0.0){}
   //! ctor
   ua_object(double objxpos, double objypos):xpos(objxpos),ypos(objypos){}

   //! returns object xpos
   double get_xpos(){ return xpos; }

   //! returns object ypos
   double get_ypos(){ return ypos; }

   //! returns object info
   ua_object_info &get_object_info(){ return info; }

   //! loads object from savegame
   void load_object(ua_savegame &sg);

   //! saves object to savegame
   void save_object(ua_savegame &sg);

   //! renders object to OpenGL
   virtual void render(unsigned int x, unsigned int y,
      ua_texture_manager &texmgr, ua_frustum &fr, ua_level &lev);

protected:
   //! basic object info
   ua_object_info info;

   //! object coordinates
   double xpos,ypos;
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

   //! returns first object in a specific tile
   bool get_first_tile_object(unsigned int xpos, unsigned int ypos, ua_object &obj);

   //! returns next object in object chain
   bool get_next_tile_object(ua_object &obj);

   //! returns an object at a specific list pos
   ua_object &get_object(Uint16 at){ return master_obj_list[at]; }

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
