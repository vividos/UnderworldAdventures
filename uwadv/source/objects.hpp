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


// enums

enum ua_obj_type
{
   //! npc object
   ua_obj_npc,
   //! normal object, can be picked up
   ua_obj_object,
   //! unmovable object, decoration, doors etc.
   ua_obj_immobile,
   //! invisible, e.g. traps etc.
   ua_obj_invisible
};


// forward references
class ua_level;


// classes

struct ua_object_info
{
   //! struct ctor
   ua_object_info():item_id(0xffff){}

   //! object item id
   Uint16 item_id;
   //! link to next object in chain
   Uint16 link1;
   //! quantity of object
   Uint16 quantity;
};


//! object
class ua_object
{
public:
   //! ctor
   ua_object(unsigned int xpos,unsigned int ypos,Uint16 link1,Uint16 id);
   //! dtor
   virtual ~ua_object(){}

   //! renders object to OpenGL
   virtual void render(unsigned int x, unsigned int y,
      ua_texture_manager &texmgr, ua_frustum &fr, ua_level &lev);

   //! sets object type
   void set_type(ua_obj_type tp){ type=tp; };

   //! returns object type
   ua_obj_type get_type(){ return type; }

   //! returns object xpos
   unsigned int get_xpos(){ return xpos; }
   //! returns object ypos
   unsigned int get_ypos(){ return ypos; }

   //! returns object info
   ua_object_info &get_object_info(){ return info; }

protected:
   //! object info
   ua_object_info info;

   //! object type
   ua_obj_type type;

   //! object position within tile
   unsigned int xpos,ypos;
};


//! npc object
class ua_npc_object: public ua_object
{
public:
   //! ctor
   ua_npc_object(unsigned int xpos,unsigned int ypos,Uint16 link1,Uint16 id);
};


typedef ua_smart_ptr<ua_object> ua_object_ptr;


//! object list class
class ua_object_list
{
public:
   //! ctor
   ua_object_list(){}
   //! dtor
   ~ua_object_list();

   //! returns list of objects on a tile
   void get_object_list(unsigned int xpos, unsigned int ypos,
      std::vector<ua_object_ptr> &objlist);

   //! returns an object at a specific list pos
   ua_object_ptr get_object(Uint16 at){ return master_obj_list[at]; }

   // loading / saving / importing

   //! loads a savegame
   void load_game(ua_savegame &sg);

   //! saves to a savegame
   void save_game(ua_savegame &sg);

   //! imports an object list
   void import_objs(FILE *fd);

private:
   //! adds object to master object list and follows link1 and link2 objs
   void addobj_follow(std::vector<Uint32> &objprop,Uint16 objpos);

protected:
   //! indices for each tile into master object list
   std::vector<unsigned int> tile_index;

   //! master object list
   std::vector<ua_object_ptr> master_obj_list;
};

#endif
