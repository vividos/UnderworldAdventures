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

//! abstract script callback interface class
class ua_underworld_script_callback
{
public:
   //! ctor
   ua_underworld_script_callback(){}
   //! dtor
   virtual ~ua_underworld_script_callback(){}

   //! called when current level has changed
   virtual void ui_changed_level(unsigned int level)=0;

   //! starts conversation with NPC
   virtual void ui_start_conv(unsigned int level, unsigned int objpos)=0;

   //! starts viewing a cutscene
   virtual void ui_show_cutscene(unsigned int cutscene)=0;

   //! prints string to text scroll
   virtual void ui_print_string(const char* str)=0;

   //! shows ingame animation
   virtual void ui_show_ingame_anim(unsigned int anim)=0;

   //! uses item as cursor
   virtual void ui_cursor_use_item(Uint16 item_id)=0;

   //! changes cursor to target cursor
   virtual void ui_cursor_target()=0;
};


//! underworld script bindings
class ua_underworld_script_bindings
{
public:
   //! ctor
   ua_underworld_script_bindings();

   //! initialize underworld scripting
   void init(ua_underworld* uw);

   //! registers script callback interface
   void register_callback(ua_underworld_script_callback* cback=NULL);

   //! returns lua state
   lua_State* get_lua_State(){ return L; }

   //! cleans up underworld scripting
   void done();

   //! loads a savegame
   void load_game(ua_savegame& sg);

   //! saves to a savegame
   void save_game(ua_savegame& sg);

   // Lua script functions


   // general functions

   //! inits scripts after loading all underworld scripts
   void lua_init_script();

   //! called when starting a new game
   void lua_started_newgame();

   //! game functions called every tick
   void lua_game_tick(double curtime);

   //! performs "track" skill
   void lua_track();

   //! starts (or ends, when start==false) sleeping; returns true when starting sleeping
   bool lua_sleep(bool start);

   //! called when changing to a new level
   void lua_change_level(unsigned int level);


   // objlist functions

   //! prints "look" text for object in objlist
   void lua_objlist_look(Uint32 level, Uint32 objpos);

   //! does "talk" action to object
   void lua_objlist_talk(Uint32 level, Uint32 objpos);


   // inventory functions

   //! returns true when a item_id is a container
   bool lua_inventory_is_container(Uint16 item_id);

   //! categorizes item by item id
   ua_inv_item_category lua_inventory_categorize_item(Uint16 item_id);

   //! prints "look" text of an inventory item
   void lua_inventory_look(Uint16 item_pos);

   //! uses inventory item
   void lua_inventory_use(Uint16 item_pos);

   //! tries to combine items
   ua_obj_combine_result lua_inventory_combine_obj(Uint16 item_id1, Uint16 item_id2,
      Uint16& result_id);


protected:
   //! registers all ua_underworld functions
   void register_functions();

   //! does a "lua_call" and checks the result value
   void checked_lua_call(int params, int retvals);

   //! returns a lua constant value
   double get_lua_constant(const char* name);

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
   static int player_get_height(lua_State* L);
   static int player_set_height(lua_State* L);
   static int player_get_angle(lua_State* L);
   static int player_set_angle(lua_State* L);

   static int objlist_get_obj_info(lua_State* L);
   static int objlist_set_obj_info(lua_State* L);
   static int objlist_remove_obj(lua_State* L);
   static int objlist_obj_is_npc(lua_State* L);
   static int objlist_insert_obj(lua_State* L);

   static int tilemap_get_tile(lua_State* L);
   static int tilemap_get_type(lua_State* L);
   static int tilemap_set_type(lua_State* L);
   static int tilemap_get_floor(lua_State* L);
   static int tilemap_set_floor(lua_State* L);
   static int tilemap_get_ceiling(lua_State* L);
   static int tilemap_set_ceiling(lua_State* L);
   static int tilemap_get_floor_height(lua_State* L);
   static int tilemap_get_objlist_start(lua_State* L);

   static int inventory_rune_avail(lua_State* L);
   static int inventory_rune_add(lua_State* L);

   static int quest_get_flag(lua_State* L);
   static int quest_set_flag(lua_State* L);

   static int conv_is_avail(lua_State* L);
   static int conv_get_global(lua_State* L);
   static int conv_set_global(lua_State* L);

   static int ui_start_conv(lua_State* L);
   static int ui_show_cutscene(lua_State* L);
   static int ui_print_string(lua_State* L);
   static int ui_get_gamestring(lua_State* L);
   static int ui_show_ingame_anim(lua_State* L);
   static int ui_cursor_use_item(lua_State* L);
   static int ui_cursor_target(lua_State* L);

   static int savegame_store_value(lua_State* L);
   static int savegame_restore_value(lua_State* L);

protected:
   //! lua script state
   lua_State* L;

   //! script callback interface
   ua_underworld_script_callback* callback;
};

#endif
