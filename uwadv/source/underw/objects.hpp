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
/*! \file objects.hpp

   \brief objects management

*/
//! \ingroup underworld

//@{

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

   //! reads struct from savegame
   void load_info(ua_savegame& sg);

   //! writes struct to savegame
   void save_info(ua_savegame& sg);


   // item properties

   //! object item id
   Uint16 item_id;

   //! object chain link
   Uint16 link;

   //! quality
   Uint16 quality;

   //! owner / special field
   Uint16 owner;

   //! quantity / special / special link
   Uint16 quantity;

   //! object flags
   Uint16 flags;


   // flags

   //! true when object is enchanted
   bool enchanted;

   //! indicates if object is hidden or visible
   bool is_hidden;

   //! true when "quantity" field is a quantity/special property
   bool is_quantity;
};


//! extended infos for object
struct ua_object_info_ext
{
   //! struct ctor
   ua_object_info_ext();

   //! reads struct from savegame
   void load_extinfo(ua_savegame& sg);

   //! writes struct to savegame
   void save_extinfo(ua_savegame& sg);


   // position info

   //! fractional x and y positions in tile (0..7)
   Uint8 xpos, ypos;

   //! z position (0..127)
   Uint8 zpos;

   //! direction (0..7)
   Uint8 heading;

   //! tilemap coordinates
   Uint8 tilex, tiley;

   // npc infos

   //! indicates if npc infos are used
   bool npc_used;

   // npc infos

   //! hit points
   Uint8 npc_hp;

   Uint8 npc_goal;
   Uint8 npc_gtarg;
   Uint8 npc_level;
   bool  npc_talkedto;
   Uint8 npc_attitude;

   Uint8 npc_xhome;
   Uint8 npc_yhome;

   Uint8 npc_hunger;
   Uint8 npc_whoami;

   // extra (unknown) values
   //Uint8 extra1,extra2,extra5,extra7,extra8,extra9,extra10;
   //Uint8 extra11,extra12,extra13,extra14,extra15,extra16,extra17;

   //! npc state
   Uint8 state;

   //! current animation frame
   Uint8 animframe;
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


   //! deletes object at given position
   void delete_object(Uint16 pos);


   // loading / saving / importing

   //! loads a savegame
   void load_game(ua_savegame &sg);

   //! saves to a savegame
   void save_game(ua_savegame &sg);

   //! imports an object list
   void import_objs(SDL_RWops* rwops, Uint16 texmap[64]);

private:
   //! adds object to master object list and follows link1 and link2 objs
   void addobj_follow(Uint32 objprop[0x400*2], Uint8 npcinfo[0x100*19],
      Uint16 objpos, Uint16 texmap[64], Uint8 tilex, Uint8 tiley);

protected:
   //! indices for each tile into master object list
   std::vector<Uint16> tile_index;

   //! master object list
   std::vector<ua_object> master_obj_list;

   //! object tilemap position xref table
   std::vector<std::pair<Uint8,Uint8> > xref_tilepos;
};


//}@

#endif
