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

// include guard
#ifndef uwadv_luascript_hpp_
#define uwadv_luascript_hpp_

// needed includes
#include "script.hpp"
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
   virtual void eval_critter(unsigned int pos);
   virtual void do_trigger(unsigned int pos);
   virtual void user_action(ua_underworld_user_action action,
      unsigned int param);
   virtual void on_changing_level();
   virtual void object_look(unsigned int pos);
   virtual void object_use(unsigned int pos);
   virtual void inventory_look(unsigned int pos);
   virtual void inventory_use(unsigned int pos);
   virtual ua_item_combine_status item_combine(Uint16 item_id1, Uint16 item_id2,
      Uint16& result_id);

protected:
   //! loads a script
   int load_script(SDL_RWops* rwops, const char* chunkname);

   //! lua state information
   lua_State* L;

   //! ptr to game interface
   ua_game_interface* game;
};


#endif
