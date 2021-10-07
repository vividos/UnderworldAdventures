//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2021 Underworld Adventures Team
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
/// \file LuaState.hpp
/// \brief Lua state
//
#pragma once

struct lua_State;

/// \brief Lua state
/// RAII class for lua_State
class LuaState
{
public:
   /// ctor
   LuaState();
   /// dtor
   virtual ~LuaState() noexcept;

   /// returns lua state info struct
   lua_State* GetLuaState() noexcept { return L; }

   /// loads a script from rwops
   virtual int LoadScript(Base::SDL_RWopsPtr rwops, const char* chunkname);

   /// lua function call
   virtual void CheckedCall(int numArgs, int numResults);

   /// checks Lua code for syntax errors
   static bool CheckSyntax(const std::string& luaSource, std::vector<std::string>& errorMessages);

   /// parses Lua code and returns line numbers of active lines
   static void GetActiveLines(const std::string& luaSource, std::vector<size_t>& lineNumbers);

   // diagnostic methods

   /// dumps value stack to debug output
   void DumpStack() const;

   /// dumps all upvalues for the current function
   void DumpUpValues() const;

   /// enumerates all table indices in the table at given stack index
   std::string EnumTableIndices(int stackIndex) const;

   /// dumps single stack value
   void DumpStackValue(int stackIndex, bool isUpValue) const;

   /// dumps call stack to debug output
   void DumpCallStack() const;

protected:
   /// lua state information
   lua_State* L;
};
