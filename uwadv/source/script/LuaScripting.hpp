//
// Underworld Adventures - an Ultima Underworld hacking project
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
/// \file LuaScripting.hpp
/// \brief Lua scripting support
//
#pragma once

#include "Base.hpp"
#include "IScripting.hpp"
#include "IDebugServer.hpp"
#include <vector>
#include <map>
#include <set>

class IGame;
struct lua_State;
struct lua_Debug;

/// lua scripting class
class LuaScripting : public IScripting, public ICodeDebugger
{
public:
   /// ctor
   LuaScripting();
   virtual ~LuaScripting() {}

   /// loads a script from uadata
   int LoadScript(IGame& game, const char* basename);

   /// returns lua state info struct
   lua_State* GetLuaState() { return L; }

   /// lua function call
   void CheckedCall(int numArgs, int numResults);

   // virtual methods from IScripting
   virtual void Init(IBasicGame* game) override;
   virtual bool LoadScript(const char* basename) override;
   virtual void Done() override;
   virtual void InitNewGame() override;
   virtual void EvalCritter(Uint16 pos) override;
   virtual void TriggerSetOff(Uint16 pos) override;
   virtual void UserAction(UserInterfaceUserAction action,
      unsigned int param) override;
   virtual void OnChangingLevel() override;
   virtual ItemCombineStatus ItemCombine(Uint16 itemId1,
      Uint16 itemId2, Uint16& resultId) override;

protected:
   /// loads a script
   int LoadScript(Base::SDL_RWopsPtr rwops, const char* chunkname);

   /// returns scripting class from Lua state
   static LuaScripting& GetScriptingFromSelf(lua_State* L);

   static void DebugHookCall(lua_State* L, lua_Debug* ar);
   static void DebugHookLine(lua_State* L, lua_Debug* ar);

   void DebugHook(lua_Debug* ar);

   void CheckBreakpoints();

   /// waits for the debugger to continue debugging
   void WaitDebuggerContinue();

   /// sends status update to debugger client
   void SendDebugStatusUpdate();

   /// registers all callable functions
   void RegisterFunctions();

   // virtual methods from ICodeDebugger
   virtual DebugServerCodeDebuggerType GetDebuggerType() override;
   virtual void PrepareDebugInfo() override;
   virtual DebugServerCodeDebuggerState GetDebuggerState() const override;
   virtual void SetDebuggerState(DebugServerCodeDebuggerState state) override;
   virtual DebugServerCodeDebuggerCommand GetDebuggerCommand() const override;
   virtual void SetDebuggerCommand(DebugServerCodeDebuggerCommand command) override;
   virtual void GetCurrentPos(unsigned int& sourcefileIndex, unsigned int& sourcefileLine,
      unsigned int& codePosition, bool& isSourcefileValid) override;
   virtual unsigned int GetNumSourcefiles() const override;
   virtual unsigned int GetSourcefileName(unsigned int index, char* buffer, unsigned int len) override;
   virtual unsigned int GetNumBreakpoints() override;
   virtual void GetBreakpointInfo(unsigned int breakpointIndex,
      unsigned int& sourcefileIndex, unsigned int& sourcefileLine,
      unsigned int& codePosition, bool& visible) override;

protected:
   /// lua state information
   lua_State* L;

   /// ptr to basic game interface
   IBasicGame* m_game;

   /// name for 'self' global in Lua
   static const char* s_selfName;

   /// array with all loaded script files
   std::vector<std::string> m_loadedScriptFiles;

   /// map with all source files and line numbers
   std::map<std::string, std::set<unsigned int> > m_allLineNumbers;

   /// current debugger state
   DebugServerCodeDebuggerState m_debuggerState;

   /// current debugger command
   DebugServerCodeDebuggerCommand m_debuggerCommand;

   /// current position sourcefile index
   unsigned int m_currentPositionSourcefileIndex;

   /// current position sourcefile line
   unsigned int m_currentPositionSourcefileLine;

   /// function call depth when doing "step over"
   unsigned int m_stepOverFunctionCallDepth;

   /// list with all breakpoints
   std::vector<DebugCodeBreakpointInfo> m_breakpointsList;

protected:
   // registered C functions callable from Lua
   // prototype: static int xyz(lua_State* L);
   static int PrintScroll(lua_State* L);        ///< Lua function uw.print
   static int uw_get_string(lua_State* L);   ///< uw.get_string
   static int uw_change_level(lua_State* L); ///< uw.ChangeLevel
   static int uw_start_conv(lua_State* L);   ///< uw.start_conv

   static int player_get_info(lua_State* L); ///< player.get_info
   static int player_set_info(lua_State* L); ///< player.set_info
   static int player_get_attr(lua_State* L); ///< player.get_attr
   static int player_get_skill(lua_State* L);///< player.get_skill
   static int player_set_attr(lua_State* L); ///< player.SetAttribute
   static int player_set_skill(lua_State* L);///< player.SetSkill

   static int objlist_get_info(lua_State* L);///< objlist.get_info
   static int objlist_set_info(lua_State* L);///< objlist.set_info
   static int objlist_delete(lua_State* L);  ///< objlist.delete
   static int objlist_insert(lua_State* L);  ///< objlist.insert

   static int tilemap_get_info(lua_State* L);///< tilemap.get_info
   static int tilemap_set_info(lua_State* L);///< tilemap.set_info
   static int tilemap_get_floor_height(lua_State* L); ///< tilemap.get_floor_height
   static int tilemap_get_objlist_link(lua_State* L); ///< tilemap.get_objlist_link

   static int runes_set(lua_State* L);       ///< runes.set
   static int runes_test(lua_State* L);      ///< runes.test

   static int conv_is_avail(lua_State* L);   ///< conv.is_avail
   static int conv_get_global(lua_State* L); ///< conv.GetGlobal
   static int conv_set_global(lua_State* L); ///< conv.SetGlobal

   static int quest_get_flag(lua_State* L);  ///< quest.get_flag
   static int quest_set_flag(lua_State* L);  ///< quest.set_flag

   static int prop_get_common(lua_State* L); ///< prop.get_common
   static int prop_get_special(lua_State* L);///< prop.get_special
};
