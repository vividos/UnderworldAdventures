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
/*! \file uwscript.hpp

   \brief underworld Lua script bindings

*/

// include guard
#ifndef __uwadv_uwscript_hpp_
#define __uwadv_uwscript_hpp_

// needed includes
#include "savegame.hpp"


// enums

//! inventory item category
typedef enum
{
   ua_inv_cat_normal = 0,   // normal item
   ua_inv_cat_ring,
   ua_inv_cat_legs,
   ua_inv_cat_chest,
   ua_inv_cat_hands,
   ua_inv_cat_feet,
   ua_inv_cat_head,
} ua_inv_item_category;

//! result value when combining items
typedef enum
{
   ua_obj_cmb_failed = 0,
   ua_obj_cmb_dstr_first,
   ua_obj_cmb_dstr_second,
   ua_obj_cmb_dstr_both
} ua_obj_combine_result;


// forward declarations
class ua_underworld;
struct ua_levelmap_tile;


// classes

//! underworld script bindings
class ua_underworld_script_bindings
{
public:
   //! ctor
   ua_underworld_script_bindings(){}

   //! initialize underworld scripting
   void init(ua_underworld *uw);

   //! cleans up underworld scripting
   void done();

   //! loads a savegame
   void load_game(ua_savegame &sg);

   //! saves to a savegame
   void save_game(ua_savegame &sg);

   // Lua script functions

   //! game functions called every tick
   void lua_game_tick(double curtime);

   //! returns true when a item_id is a container
   bool lua_inventory_is_container(Uint16 item_id);

   //! categorizes item by item id
   ua_inv_item_category lua_inventory_categorize_item(Uint16 item_id);

   //! tries to combine items
   ua_obj_combine_result lua_obj_combine(Uint16 item_id1, Uint16 item_id2,
      Uint16 &result_id);

protected:
   //! registers all ua_underworld functions
   void register_functions();

   //! does a "lua_call" and checks the result value
   void checked_lua_call(int params, int retvals);

   //! returns a lua constant value
   double get_lua_constant(const char *name);

protected:

   //! returns underworld object reference from lua state
   static ua_underworld& get_underworld_from_self(lua_State* L);

   //! returns levelmap tile from given tile_handle
   static ua_levelmap_tile& get_tile_per_handle(ua_underworld& uw,
      unsigned int tile_handle);

   // registered C functions callable from Lua
   // prototype: static int xyz(lua_State* L);

   static int player_get_attr(lua_State* L);
   static int player_set_attr(lua_State* L);
   static int player_get_skill(lua_State* L);
   static int player_set_skill(lua_State* L);
   static int player_get_pos(lua_State* L);
   static int player_set_pos(lua_State* L);
   static int player_get_angle(lua_State* L);
   static int player_set_angle(lua_State* L);

   static int tilemap_get_tile(lua_State* L);
   static int tilemap_get_type(lua_State* L);
   static int tilemap_set_type(lua_State* L);
   static int tilemap_get_floor(lua_State* L);
   static int tilemap_set_floor(lua_State* L);
   static int tilemap_get_automap_visible(lua_State* L);
   static int tilemap_set_automap_visible(lua_State* L);
   static int tilemap_get_objlist_start(lua_State* L);


protected:
   //! lua script state
   lua_State *L;
};

#endif
