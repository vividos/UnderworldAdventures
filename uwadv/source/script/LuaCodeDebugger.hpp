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
/// \file LuaCodeDebugger.hpp
/// \brief Lua code debugger
//
#pragma once

#include "LuaState.hpp"
#include "IDebugServer.hpp"
#include <vector>
#include <map>
#include <set>
#include <optional>
#include <mutex>

struct lua_Debug;

/// Lua state with a code debugger
class LuaCodeDebugger : public LuaState, public ICodeDebugger
{
public:
   /// ctor
   LuaCodeDebugger();

   /// initializes code debugger and notifies debug server
   void Init(IDebugServer& debugServer);

   /// loads a script from rwops
   virtual int LoadScript(Base::SDL_RWopsPtr rwops, const char* sourceFilename) override;

   /// lua function call, with debugging
   virtual void CheckedCall(int numArgs, int numResults) override;

private:
   /// returns code debugger class from Lua state
   static LuaCodeDebugger& GetCodeDebuggerFromState(lua_State* L);

   /// debug hook called from Lua
   static void DebugHook(lua_State* L, lua_Debug* ar);

   /// translate Lua debug hook event codes to readable text
   static const char* LuaDebugEventNameFromInt(int event);

   /// debug hook called from Lua; now with actual scripting object
   void DebugHook(lua_Debug* ar);

   /// checks if a breakpoint was reached
   void CheckBreakpoints();

   /// waits for the debugger to continue debugging
   void WaitDebuggerContinue();

   // virtual methods from ICodeDebugger
   virtual DebugServerCodeDebuggerType GetDebuggerType() const override;
   virtual void PrepareDebugInfo() override;
   virtual DebugServerCodeDebuggerState GetDebuggerState() const override;
   virtual void SetDebuggerState(DebugServerCodeDebuggerState state) override;
   virtual DebugServerCodeDebuggerCommand GetDebuggerCommand() const override;
   virtual void SetDebuggerCommand(DebugServerCodeDebuggerCommand command) override;
   virtual void GetCurrentPos(size_t& sourcefileIndex, size_t& sourcefileLine,
      size_t& codePosition, bool& isSourcefileValid) override;
   virtual size_t GetNumSourcefiles() const override;
   virtual size_t GetSourcefileName(size_t index, char* buffer, size_t length) override;
   virtual size_t GetNumBreakpoints() const override;
   virtual void GetBreakpointInfo(size_t breakpointIndex,
      size_t& sourcefileIndex, size_t& sourcefileLine,
      size_t& codePosition, bool& visible) const override;

private:
   /// name for 'self' global in Lua
   static const char* s_selfDebuggerName;

   /// reference to debug server; only valid when Init() was called
   std::optional<std::reference_wrapper<IDebugServer>> m_debugServer;

   /// array with all loaded script files
   std::vector<std::string> m_loadedScriptFiles;

   /// map with all source files and line numbers
   std::map<std::string, std::vector<size_t>> m_allLineNumbers;

   /// mutex to protect access to all following member variables
   mutable std::mutex m_debuggerMutex;

   /// current debugger state
   DebugServerCodeDebuggerState m_debuggerState;

   /// current debugger command
   DebugServerCodeDebuggerCommand m_debuggerCommand;

   /// current position sourcefile index
   size_t m_currentPositionSourcefileIndex;

   /// current position sourcefile line
   size_t m_currentPositionSourcefileLine;

   /// function call depth when doing "step over"
   size_t m_stepOverFunctionCallDepth;

   /// list with all breakpoints
   std::vector<DebugCodeBreakpointInfo> m_breakpointsList;
};
