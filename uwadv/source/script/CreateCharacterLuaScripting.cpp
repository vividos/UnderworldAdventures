//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2022 Underworld Adventures Team
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
#include <ctime>

extern "C"
{
#include "lua.h"
}

/// storage name of DoAction function pointer
static const char* s_doActionFuncName = "_do_action_func";

CreateCharacterLuaScripting::~CreateCharacterLuaScripting() noexcept
{
   lua_getglobal(L, s_doActionFuncName);
   T_actionFunc* func = reinterpret_cast<T_actionFunc*>(lua_touserdata(L, -1));
   delete func;
}

void CreateCharacterLuaScripting::SetActionHandler(T_actionFunc doActionFunc)
{
   lua_register(L, "cchar_do_action", cchar_do_action);

   lua_pushlightuserdata(L, new T_actionFunc(doActionFunc));
   lua_setglobal(L, s_doActionFuncName);
}

bool CreateCharacterLuaScripting::Start()
{
   lua_getglobal(L, "cchar_start");
   lua_pushnumber(L, clock());
   return CheckedCall(1, 0);
}

void CreateCharacterLuaScripting::Cancel()
{
   lua_getglobal(L, "cchar_cancel");
   CheckedCall(0, 0);
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
