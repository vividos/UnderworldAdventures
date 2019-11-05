//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file CreateCharacterLuaScripting.cpp
/// \brief Lua scripting for character creation
//
#include "pch.hpp"
#include "CreateCharacterLuaScripting.hpp"

extern "C"
{
#include "lua.h"
}

/// storage name of DoAction function pointer
static const char* s_doActionFuncName = "_do_action_func";

CreateCharacterLuaScripting::~CreateCharacterLuaScripting()
{
   lua_getglobal(L, s_doActionFuncName);
   T_actionFunc* func = reinterpret_cast<T_actionFunc*>(lua_touserdata(L, -1));
   delete func;
}

void CreateCharacterLuaScripting::Init(T_actionFunc doActionFunc)
{
   lua_register(L, "cchar_do_action", cchar_do_action);

   lua_pushlightuserdata(L, new T_actionFunc(doActionFunc));
   lua_setglobal(L, s_doActionFuncName);
}

bool CreateCharacterLuaScripting::CallGlobal(CreateCharGlobalAction globalAction, int seed)
{
   lua_getglobal(L, "cchar_global");
   lua_pushnumber(L, globalAction);
   lua_pushnumber(L, seed);
   int ret = lua_pcall(L, 2, 0, 0);
   if (ret != 0)
   {
      const char* errorText = lua_tostring(L, -1);
      UaTrace("Lua function call cchar_global ended with error code %u: %s\n", ret, errorText);
      return false;
   }

   return true;
}

int CreateCharacterLuaScripting::cchar_do_action(lua_State* L)
{
   lua_getglobal(L, s_doActionFuncName);
   UaAssert(lua_islightuserdata(L, -1));

   T_actionFunc& func = *reinterpret_cast<T_actionFunc*>(lua_touserdata(L, -1));

   lua_pop(L, 1);

   func();

   return 0;
}
