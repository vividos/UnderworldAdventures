/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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

   Uint8 npc_goal;      //!< goal of the npc
   Uint8 npc_gtarg;     //!< goal target
   Uint8 npc_level;     //!< experience level
   bool  npc_talkedto;  //!< is true when player talked to npc
   Uint8 npc_attitude;  //!< attitude

   Uint8 npc_xhome;     //!< home tile x coordinate
   Uint8 npc_yhome;     //!< home tile y coordinate

   Uint8 npc_hunger;    //!< hungriness of the npc
   Uint8 npc_whoami;    //!< conversation slot of npc, or 0 for generic

   //! animation state (used in ua_critter)
   Uint8 animstate;

   //! current animation frame (used in ua_critter)
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

   //! returns object info
   const ua_object_info &get_object_info() const { return info; }

   //! returns extended object info
   ua_object_info_ext &get_ext_object_info(){ return extinfo; }

   //! returns extended object info
   const ua_object_info_ext &get_ext_object_info() const { return extinfo; }

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
   Uint16 get_tile_list_start(unsigned int xpos, unsigned int ypos) const;

   //! returns an object at a specific list pos
   ua_object &get_object(Uint16 at){ return master_obj_list[at]; }

   //! returns an object at a specific list pos
   const ua_object &get_object(Uint16 at) const { return master_obj_list[at]; }

   //! returns master object list
   std::vector<ua_object>& get_master_obj_list(){ return master_obj_list; }

   //! returns master object list
   const std::vector<ua_object>& get_master_obj_list() const { return master_obj_list; }

   //! returns tile index map
   std::vector<Uint16>& get_tile_index(){ return tile_index; }

   // todo: object insert, remove, etc.


   //! deletes object at given position
   void delete_object(Uint16 pos);


   // loading / saving

   //! loads a savegame
   void load_game(ua_savegame &sg);

   //! saves to a savegame
   void save_game(ua_savegame &sg);

protected:
   //! indices for each tile into master object list
   std::vector<Uint16> tile_index;

   //! master object list
   std::vector<ua_object> master_obj_list;

   //! object tilemap position xref table
   std::vector<std::pair<Uint8,Uint8> > xref_tilepos;
};


#endif
//@}
