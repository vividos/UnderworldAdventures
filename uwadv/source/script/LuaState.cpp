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
/// \file LuaState.cpp
/// \brief Lua state
//
#include "pch.hpp"
#include "LuaState.hpp"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

int OnLuaPanic(lua_State* L)
{
   const char* errorText = lua_tostring(L, -1);
   lua_pop(L, 1);
   UaTrace("Lua panic handler reported error: %s", errorText);

   for (int stackIndex = 0;; stackIndex++)
   {
      lua_Debug debug = { 0 };

      int ret = lua_getstack(L, stackIndex, &debug);
      if (ret == 0)
         break;

      try
      {
         lua_getinfo(L, "Sl", &debug);
      }
      catch (...)
      {
         // may throw exception of type  struct lua_longjmp
         UaTrace("exception during lua_getinfo()\n");
      }

      try
      {
         UaTrace("%hs:%u\n",
            debug.short_src,
            debug.currentline);
      }
      catch (...)
      {
         UaTrace("exception during formatting debug info()\n");
      }
   }

   return 0;
}

LuaState::LuaState()
   :L(NULL)
{
   // init lua state
   L = luaL_newstate();

   lua_atpanic(L, OnLuaPanic);

   // add Lua libraries
   luaL_requiref(L, "_G", luaopen_base, 1);
   lua_pop(L, 1);

   luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1);
   lua_pop(L, 1);

   luaL_requiref(L, LUA_MATHLIBNAME, luaopen_math, 1);
   lua_pop(L, 1);
}

LuaState::~LuaState() noexcept
{
   lua_close(L);
}

int LuaState::LoadScript(Base::SDL_RWopsPtr rwops, const char* chunkname)
{
   // load script into buffer
   std::vector<char> buffer;
   unsigned int len = 0;
   {
      SDL_RWseek(rwops.get(), 0, SEEK_END);
      len = SDL_RWtell(rwops.get());
      SDL_RWseek(rwops.get(), 0, SEEK_SET);

      buffer.resize(len + 1, 0);

      SDL_RWread(rwops.get(), &buffer[0], len, 1);
   }

   // execute script
   int ret = luaL_loadbuffer(L, buffer.data(), len, chunkname);

   if (ret != LUA_OK)
      UaTrace("Lua script loading ended with error code %u\n", ret);

   ret = lua_pcall(L, 0, LUA_MULTRET, 0);
   if (ret != LUA_OK)
      UaTrace("Lua script calling ended with error code %u\n", ret);

   return ret;
}

bool LuaState::CheckedCall(int numArgs, int numResults)
{
   int ret = lua_pcall(L, numArgs, numResults, 0);
   if (ret != 0)
   {
      const char* errorText = lua_tostring(L, -1);
      UaTrace("Error in Lua function call; error code %u: %s\n", ret, errorText);
      return false;
   }

   return true;
}

bool LuaState::CheckSyntax(const std::string& luaSource, std::vector<std::string>& errorMessages)
{
   if (luaSource.empty())
      return true;

   try
   {
      LuaState state;
      lua_State* L = state.GetLuaState();

      int ret = luaL_loadstring(L, luaSource.c_str());
      switch (ret)
      {
      case 0:
         return true;

      case LUA_ERRSYNTAX:
         errorMessages.push_back(std::string(lua_tostring(L, -1)));
         lua_pop(L, 1);
         break;

      case LUA_ERRMEM:
         errorMessages.push_back("memory error during checking syntax");
         break;

      default:
         UaAssert(false); // unknown return value
         errorMessages.push_back("unknown error during checking syntax");
         break;
      }
   }
   catch (const std::exception& ex)
   {
      errorMessages.push_back(std::string("std exception during checking syntax: ") + ex.what());
   }
   catch (...)
   {
      errorMessages.push_back("C++ exception during checking syntax: ");
   }

   return false;
}

void LuaState::GetActiveLines(const std::string& luaSource, std::vector<size_t>& lineNumbers)
{
   lineNumbers.clear();

   if (luaSource.empty())
      return;

   LuaState state;
   lua_State* L = state.GetLuaState();

   int ret = luaL_loadstring(L, luaSource.c_str());
   if (ret != 0)
      return;

   // the '>' char uses the top value on the stack, examines it and pops it
   // L then pushes an array of active lines onto the stack
   lua_getinfo(L, ">L", nullptr);

   lua_pushnil(L); // first key
   while (lua_next(L, -2))
   {
      if (lua_isinteger(L, -2))
      {
         size_t line = (size_t)lua_tointeger(L, -2);
         lineNumbers.push_back(line);
      }

      lua_pop(L, 1); // pop value, but leave key for next pair
   }

   lua_pop(L, 1);
}

void LuaState::DumpStack() const
{
   int stackDepth = lua_gettop(L);

   UaTrace("dumping current value stack, stack depth %i ...\n", stackDepth);

   for (int stackIndex = 1; stackIndex <= stackDepth; stackIndex++)
   {
      DumpStackValue(stackIndex, false);
   }

   UaTrace("done dumping value stack.\n");
}

void LuaState::DumpUpValues() const
{
   UaTrace("dumping upvalues:\n");

   for (int upvalueIndex = 1; upvalueIndex < 256; upvalueIndex++)
   {
      int index = lua_upvalueindex(upvalueIndex);

      if (lua_type(L, index) == LUA_TNONE)
         continue;

      DumpStackValue(index, true);
   }

   UaTrace("end dumping upvalues.\n");
}

std::string LuaState::EnumTableIndices(int stackIndex) const
{
   std::string text;

   if (stackIndex < 0)
      stackIndex = lua_absindex(L, stackIndex);

   lua_pushnil(L);
   while (lua_next(L, stackIndex) != 0)
   {
      if (lua_type(L, -2) == LUA_TSTRING)
         text += std::string{ " \"" } + lua_tostring(L, -2) + "\"";
      else if (lua_type(L, -2) == LUA_TNUMBER)
         text += " \"" + std::to_string(lua_tonumber(L, -2)) + "\"";
      else
         text += std::string{ " (type " } + lua_typename(L, lua_type(L, -2)) + ")";

      lua_pop(L, 1);
   }

   return text;
}

void LuaState::DumpStackValue(int stackIndex, bool isUpValue) const
{
   const char* typeName = lua_typename(L, lua_type(L, stackIndex));

   std::string content;

   switch (lua_type(L, stackIndex))
   {
   case LUA_TNONE: content = "[none]"; break;
   case LUA_TNIL:  content = "nil"; break;
   case LUA_TNUMBER: content = std::to_string(lua_tonumber(L, stackIndex)); break;
   case LUA_TBOOLEAN: content = lua_toboolean(L, stackIndex) != 0 ? "true" : "false"; break;
   case LUA_TSTRING: content = std::string("[") + lua_tostring(L, stackIndex) + "]"; break;
   case LUA_TTABLE:
      content = "[table" + EnumTableIndices(stackIndex) + "]";
      break;

   case LUA_TFUNCTION:
      content = lua_iscfunction(L, stackIndex)
         ? "(C) 0x" + std::to_string((size_t)lua_tocfunction(L, stackIndex))
         : "(Lua)";
      break;

   case LUA_TLIGHTUSERDATA:
      content = "0x" + std::to_string((size_t)lua_touserdata(L, stackIndex));
      break;

   case LUA_TUSERDATA:
      content = "0x" + std::to_string((size_t)lua_touserdata(L, stackIndex)) +
         ", size=" + std::to_string(lua_rawlen(L, stackIndex));
      break;

   case LUA_TTHREAD:
      content = "[thread]";
      break;

   default:
      UaAssert(false); // unhandled Lua type
      break;
   }

   std::string metatableInfo;
   int hasMetatable = lua_getmetatable(L, stackIndex);
   if (hasMetatable != 0)
   {
      metatableInfo = " +[metatable";
      metatableInfo += EnumTableIndices(-1);
      metatableInfo += "]";
      lua_pop(L, 1);
   }

   int displayIndex = isUpValue ? lua_upvalueindex(stackIndex) : stackIndex;

   UaTrace("[%i] (%s) %s%s\n", displayIndex, typeName, content.c_str(), metatableInfo.c_str());
}

void LuaState::DumpCallStack() const
{
   UaTrace("dumping current call stack...\n");

   for (int stack = 0;; stack++)
   {
      lua_Debug debug = { 0 };

      int ret = lua_getstack(L, stack, &debug);
      if (ret == 0)
         break;

      try
      {
         lua_getinfo(L, "Sl", &debug);
      }
      catch (...)
      {
         // may throw exception of type  struct lua_longjmp
         UaTrace("exception during lua_getinfo()\n");
      }

      UaTrace("%s:%u\n", debug.short_src, debug.currentline);
   }

   UaTrace("done dumping call stack.\n");
}
