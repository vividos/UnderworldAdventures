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
//#include "underworld.hpp"


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

   // Lua script functions

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

   //! returns underworld object reference from lua stack variable "self"
   static ua_underworld& lua_get_underworld(lua_State* L, int params);

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

protected:
   //! lua script state
   lua_State *L;
};

#endif
