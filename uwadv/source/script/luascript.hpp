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
/*! \file luascript.hpp

   \brief Lua scripting support

*/

// include guard
#ifndef uwadv_script_hpp_
#define uwadv_script_hpp_

// needed includes
extern "C"
{
#include "lua/include/lua.h"
#include "lua/include/lualib.h"
}

// forward references
class ua_game_interface;


// classes

//! lua scripting class
class ua_lua_scripting
{
public:
   //! ctor
   ua_lua_scripting(){}

   //! initializes scripting
   void init();

   //! cleans up scripting
   void done();

   //! loads a script from uadata
   int load_script(ua_game_interface& game, const char* basename);

   //! returns lua state info struct
   lua_State* get_lua_State(){ return L; }

protected:
   //! loads a script
   int load_script(SDL_RWops* rwops, const char* chunkname);

   //! lua state information
   lua_State* L;
};


#endif
