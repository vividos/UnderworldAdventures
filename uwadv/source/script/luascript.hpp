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
/*! \file luascript.hpp

   \brief Lua scripting support

*/
//! \ingroup scripting

//@{

// include guard
#ifndef uwadv_luascript_hpp_
#define uwadv_luascript_hpp_

// needed includes
#include "scripting.hpp"
extern "C"
{
#include "lua/include/lua.h"
#include "lua/include/lualib.h"
}


// forward references


// classes

//! lua scripting class
class ua_lua_scripting: public ua_scripting
{
public:
   //! ctor
   ua_lua_scripting():L(NULL), game(NULL){}
   virtual ~ua_lua_scripting(){}

   //! loads a script from uadata
   int load_script(ua_game_interface& game, const char* basename);

   //! returns lua state info struct
   lua_State* get_lua_State(){ return L; }

   //! lua function call
   void checked_call(int nargs, int nresults);

   // virtual methods from ua_scripting
   virtual void init(ua_game_interface* game);
   virtual bool load_script(const char* basename);
   virtual void done();
   virtual void init_new_game();
   virtual void eval_critter(Uint16 pos);
   virtual void trigger_set_off(Uint16 pos);
   virtual void user_action(ua_underworld_user_action action,
      unsigned int param);
   virtual void on_changing_level();
   virtual ua_item_combine_status item_combine(Uint16 item_id1,
      Uint16 item_id2, Uint16& result_id);

protected:
   //! loads a script
   int load_script(SDL_RWops* rwops, const char* chunkname);

   //! returns scripting class from Lua state
   static ua_lua_scripting& get_scripting_from_self(lua_State* L);

   //! registers all callable functions
   void register_functions();

protected:
   //! lua state information
   lua_State* L;

   //! ptr to game interface
   ua_game_interface* game;

   //! name for 'self' global in Lua
   static const char* self_name;

protected:
   // registered C functions callable from Lua
   // prototype: static int xyz(lua_State* L);
   static int uw_print(lua_State* L);        //!< Lua function uw.print
   static int uw_get_string(lua_State* L);   //!< uw.get_string
   static int uw_change_level(lua_State* L); //!< uw.change_level
   static int uw_start_conv(lua_State* L);   //!< uw.start_conv

   static int player_get_info(lua_State* L); //!< player.get_info
   static int player_set_info(lua_State* L); //!< player.set_info
   static int player_get_attr(lua_State* L); //!< player.get_attr
   static int player_get_skill(lua_State* L);//!< player.get_skill
   static int player_set_attr(lua_State* L); //!< player.set_attr
   static int player_set_skill(lua_State* L);//!< player.set_skill

   static int objlist_get_info(lua_State* L);//!< objlist.get_info
   static int objlist_set_info(lua_State* L);//!< objlist.set_info
   static int objlist_delete(lua_State* L);  //!< objlist.delete
   static int objlist_insert(lua_State* L);  //!< objlist.insert

   static int tilemap_get_info(lua_State* L);//!< tilemap.get_info
   static int tilemap_set_info(lua_State* L);//!< tilemap.set_info
   static int tilemap_get_floor_height(lua_State* L); //!< tilemap.get_floor_height
   static int tilemap_get_objlist_link(lua_State* L); //!< tilemap.get_objlist_link

   static int runes_set(lua_State* L);       //!< runes.set
   static int runes_test(lua_State* L);      //!< runes.test

   static int conv_is_avail(lua_State* L);   //!< conv.is_avail
   static int conv_get_global(lua_State* L); //!< conv.get_global
   static int conv_set_global(lua_State* L); //!< conv.set_global

   static int quest_get_flag(lua_State* L);  //!< quest.get_flag
   static int quest_set_flag(lua_State* L);  //!< quest.set_flag

   static int prop_get_common(lua_State* L); //!< prop.get_common
   static int prop_get_special(lua_State* L);//!< prop.get_special
};


#endif
//@}
