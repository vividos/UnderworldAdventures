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
/// \file LuaCodeDebugger.cpp
/// \brief Lua code debugger
//
#include "pch.hpp"
#include "LuaCodeDebugger.hpp"

extern "C"
{
#include "lua.h"
}

const char* LuaCodeDebugger::s_selfDebuggerName = "_debugger_self";

LuaCodeDebugger::LuaCodeDebugger()
   :m_debuggerState(codeDebuggerStateInactive),
   m_debuggerCommand(codeDebuggerCommandRun)
{
   m_stepOverFunctionCallDepth = 0;
   m_currentPositionSourcefileIndex = m_currentPositionSourcefileLine = unsigned(-1);
}

void LuaCodeDebugger::Init(IDebugServer& debugServer)
{
   // set "selfDebugger" pointer userdata
   lua_pushlightuserdata(L, this);
   lua_setglobal(L, s_selfDebuggerName);

   m_debugServer = std::ref(debugServer);

   m_debuggerState = codeDebuggerStateRunning;
   m_debuggerCommand = codeDebuggerCommandRun;

   // notify debugger of start of Lua script code debugger
   debugServer.StartCodeDebugger(this);
}

int LuaCodeDebugger::LoadScript(Base::SDL_RWopsPtr rwops, const char* sourceFilename)
{
   int ret = LuaState::LoadScript(rwops, sourceFilename);

   // put script name in list, if it's not a compiled script
   bool compiled = std::string(sourceFilename).find(".lob") != std::string::npos;
   if (!compiled)
   {
      m_loadedScriptFiles.push_back(sourceFilename);

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

      // store active lines
      std::string luaSource{ buffer.begin(), buffer.end() };

      std::vector<size_t> lineNumbers;
      LuaState::GetActiveLines(luaSource, lineNumbers);

      m_allLineNumbers[sourceFilename] = lineNumbers;
   }

   return ret;
}

bool LuaCodeDebugger::CheckedCall(int numArgs, int numResults)
{
   lua_sethook(L, DebugHook, LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE, 0);

   bool result = LuaState::CheckedCall(numArgs, numResults);

   lua_sethook(L, NULL, 0, 0);

   return result;
}

LuaCodeDebugger& LuaCodeDebugger::GetCodeDebuggerFromState(lua_State* L)
{
   LuaCodeDebugger* self = nullptr;

   lua_getglobal(L, s_selfDebuggerName);
   if (!lua_islightuserdata(L, -1))
      throw Base::Exception("'selfDebugger' parameter wasn't set by LuaCodeDebugger");

   self = reinterpret_cast<LuaCodeDebugger*>(lua_touserdata(L, -1));

   if (self == nullptr || self->L != L)
      throw Base::Exception("wrong 'selfDebugger' parameter in LuaScripting");

   lua_pop(L, 1);

   return *self;
}

void LuaCodeDebugger::DebugHook(lua_State* L, lua_Debug* ar)
{
   lua_getstack(L, 0, ar);
   lua_getinfo(L, "Snlu", ar);

   // don't report C functions
   if (0 == strcmp(ar->source, "=C"))
      return;

   LuaCodeDebugger& debugger = GetCodeDebuggerFromState(L);
   debugger.DebugHook(ar);
}

const char* LuaCodeDebugger::LuaDebugEventNameFromInt(int event)
{
   switch (event)
   {
   case LUA_HOOKCALL: return "call";
   case LUA_HOOKRET: return "return";
   case LUA_HOOKTAILCALL: return "tailcall";
   case LUA_HOOKLINE: return "line";
   case LUA_HOOKCOUNT: return "count";
   default:
      return "???";
   }
}

void LuaCodeDebugger::DebugHook(lua_Debug* ar)
{
   //UaTrace("debug: event=%s, name=%s, start=%d, line=%d, in=%s\n",
   //   LuaDebugEventNameFromInt(ar->event), ar->name, ar->linedefined, ar->currentline, ar->source);

   if (ar->source != NULL)
   {
      m_currentPositionSourcefileIndex = size_t(-1);

      // find out current index
      size_t max = m_loadedScriptFiles.size();
      for (size_t n = 0; n < max; n++)
         if (m_loadedScriptFiles[n] == ar->source)
         {
            m_currentPositionSourcefileIndex = n;
            break;
         }
   }

   if (ar->currentline > 0)
      m_currentPositionSourcefileLine = static_cast<unsigned int>(ar->currentline);

   DebugServerCodeDebuggerState oldDebuggerState = GetDebuggerState();

   // check commands
   if (GetDebuggerState() == codeDebuggerStateRunning)
   {
      switch (GetDebuggerCommand())
      {
      case codeDebuggerCommandRun:
         SetDebuggerState(codeDebuggerStateRunning);
         break;

      case codeDebuggerCommandStepOver:
         if (ar->event == LUA_HOOKLINE)
         {
            if (m_stepOverFunctionCallDepth == 0)
               SetDebuggerState(codeDebuggerStateBreak);
         }
         else if (ar->event == LUA_HOOKCALL)
         {
            if (GetDebuggerState() == codeDebuggerStateRunning)
               m_stepOverFunctionCallDepth++; //
            else
               m_stepOverFunctionCallDepth = 0; // start of stepping over a function
         }
         else if (ar->event == LUA_HOOKRET)
         {
            --m_stepOverFunctionCallDepth; // returning from function
            if (m_stepOverFunctionCallDepth == 0)
            {
               // returned from stepped over function
               SetDebuggerState(codeDebuggerStateRunning);
               SetDebuggerCommand(codeDebuggerCommandStepInto);
            }
         }
         break;

      case codeDebuggerCommandStepInto:
         if (ar->event == LUA_HOOKLINE ||
            ar->event == LUA_HOOKCALL)
         {
            SetDebuggerState(codeDebuggerStateBreak);
         }
         break;

      case codeDebuggerCommandStepOut:
         if (ar->event == LUA_HOOKRET)
         {
            // when returning set "step over" command to stop at the next line
            SetDebuggerState(codeDebuggerStateRunning);
            SetDebuggerCommand(codeDebuggerCommandStepOver);
         }
         break;
      }
   }

   // check for breakpoints
   CheckBreakpoints();

   // check if state changed
   if (oldDebuggerState != GetDebuggerState())
   {
      UaAssert(m_debugServer.has_value());
      m_debugServer.value().get().SendCodeDebuggerStatusUpdate(m_debuggerId);
   }

   // wait for state to change to "running" again
   WaitDebuggerContinue();
}

void LuaCodeDebugger::CheckBreakpoints()
{
   // TODO implement
}

void LuaCodeDebugger::WaitDebuggerContinue()
{
   while (GetDebuggerState() == codeDebuggerStateBreak)
      SDL_Delay(10); // TODO
}

DebugServerCodeDebuggerType LuaCodeDebugger::GetDebuggerType() const
{
   return codeDebuggerTypeLuaScript;
}

void LuaCodeDebugger::PrepareDebugInfo()
{
   // nothing to do for Lua
}

DebugServerCodeDebuggerState LuaCodeDebugger::GetDebuggerState() const
{
   std::unique_lock lock{ m_debuggerMutex };
   return m_debuggerState;
}

void LuaCodeDebugger::SetDebuggerState(DebugServerCodeDebuggerState newState)
{
   std::unique_lock lock{ m_debuggerMutex };
   m_debuggerState = newState;
}

DebugServerCodeDebuggerCommand LuaCodeDebugger::GetDebuggerCommand() const
{
   std::unique_lock lock{ m_debuggerMutex };
   return m_debuggerCommand;
}

void LuaCodeDebugger::SetDebuggerCommand(DebugServerCodeDebuggerCommand debuggerCommand)
{
   std::unique_lock lock{ m_debuggerMutex };
   m_debuggerCommand = debuggerCommand;
}

void LuaCodeDebugger::GetCurrentPos(size_t& sourcefileIndex,
   size_t& sourcefileLine, size_t& codePosition,
   bool& isSourcefileValid)
{
   std::unique_lock lock{ m_debuggerMutex };

   UNUSED(codePosition);
   sourcefileIndex = m_currentPositionSourcefileIndex;
   sourcefileLine = m_currentPositionSourcefileLine;
   isSourcefileValid = true;
}

size_t LuaCodeDebugger::GetNumSourcefiles() const
{
   return m_loadedScriptFiles.size();
}

size_t LuaCodeDebugger::GetSourcefileName(size_t index, char* buffer, size_t length)
{
   std::string filename(m_loadedScriptFiles[index]);
   if (!filename.empty() && filename[0] == '@')
      filename.erase(0, 1); // remove @ char

   size_t size = filename.size();

   if (buffer == NULL || length == 0 || length < size + 1)
      return size + 1;

   strncpy(buffer, filename.c_str(), size);
   buffer[size] = 0;

   return filename.size() + 1;
}

size_t LuaCodeDebugger::GetNumBreakpoints() const
{
   std::unique_lock lock{ m_debuggerMutex };
   return m_breakpointsList.size();
}

void LuaCodeDebugger::GetBreakpointInfo(size_t breakpointIndex,
   size_t& sourcefileIndex, size_t& sourcefileLine,
   size_t& codePosition, bool& visible) const
{
   UaAssert(breakpointIndex < GetNumBreakpoints());
   if (breakpointIndex >= GetNumBreakpoints())
      return;

   std::unique_lock lock{ m_debuggerMutex };

   const DebugCodeBreakpointInfo& info = m_breakpointsList[breakpointIndex];

   visible = info.visible;
   sourcefileIndex = info.pos.sourcefileIndex;
   sourcefileLine = info.pos.sourcefileLine;
   codePosition = info.pos.codePosition;
}
