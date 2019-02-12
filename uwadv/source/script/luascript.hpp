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
/// \file luascript.hpp
/// \brief Lua scripting support
//
#pragma once

#include "base.hpp"
#include "scripting.hpp"
#include "dbgserver.hpp"
#include <vector>
#include <map>
#include <set>

extern "C"
{
#include "lua/include/lua.h"
#include "lua/include/lualib.h"
#include "lua/include/luadebug.h"
}


// forward references
class IGame;

// classes

/// lua scripting class
class LuaScripting : public IScripting, public ua_debug_code_interface
{
public:
   /// ctor
   LuaScripting();
   virtual ~LuaScripting() {}

   /// loads a script from uadata
   int load_script(IGame& game, const char* basename);

   /// returns lua state info struct
   lua_State* get_lua_State() { return L; }

   /// lua function call
   void checked_call(int numArgs, int numResults);

   // virtual methods from IScripting
   virtual void init(IBasicGame* game) override;
   virtual bool load_script(const char* basename) override;
   virtual void done() override;
   virtual void init_new_game() override;
   virtual void eval_critter(Uint16 pos) override;
   virtual void trigger_set_off(Uint16 pos) override;
   virtual void UserAction(EUserInterfaceUserAction action,
      unsigned int param) override;
   virtual void on_changing_level() override;
   virtual EItemCombineStatus item_combine(Uint16 item_id1,
      Uint16 item_id2, Uint16& result_id) override;

protected:
   /// loads a script
   int load_script(Base::SDL_RWopsPtr rwops, const char* chunkname);

   /// returns scripting class from Lua state
   static LuaScripting& get_scripting_from_self(lua_State* L);

   static void debug_hook_call(lua_State* L, lua_Debug* ar);
   static void debug_hook_line(lua_State* L, lua_Debug* ar);

   void debug_hook(lua_Debug* ar);

   void check_breakpoints();

   /// waits for the debugger to continue debugging
   void wait_debugger_continue();

   /// sends status update to debugger client
   void send_debug_status_update();

   /// registers all callable functions
   void register_functions();

   // virtual methods from ua_debug_code_interface
   virtual ua_debug_code_debugger_type get_debugger_type();
   virtual void prepare_debug_info();
   virtual ua_debug_code_debugger_state get_debugger_state() const;
   virtual void set_debugger_state(ua_debug_code_debugger_state state);
   virtual ua_debug_code_debugger_command get_debugger_command() const;
   virtual void set_debugger_command(ua_debug_code_debugger_command command);
   virtual void get_current_pos(unsigned int& sourcefile_index, unsigned int& sourcefile_line,
      unsigned int& code_pos, bool& sourcefile_is_valid);
   virtual unsigned int get_num_sourcefiles() const;
   virtual unsigned int get_sourcefile_name(unsigned int index, char* buffer, unsigned int len);
   virtual unsigned int get_num_breakpoints();
   virtual void get_breakpoint_info(unsigned int breakpoint_index,
      unsigned int& sourcefile_index, unsigned int& sourcefile_line,
      unsigned int& code_pos, bool& visible);

protected:
   /// lua state information
   lua_State* L;

   /// ptr to basic game interface
   IBasicGame* game;

   /// name for 'self' global in Lua
   static const char* self_name;

   /// array with all loaded script files
   std::vector<std::string> loaded_script_files;

   /// map with all source files and line numbers
   std::map<std::string, std::set<unsigned int> > all_line_numbers;

   /// current debugger state
   ua_debug_code_debugger_state debugger_state;

   /// current debugger command
   ua_debug_code_debugger_command command;

   /// current position sourcefile index
   unsigned int curpos_srcfile;

   /// current position sourcefile line
   unsigned int curpos_line;

   /// function call depth when doing "step over"
   unsigned int step_over_func_depth;

   /// list with all breakpoints
   std::vector<ua_debug_code_breakpoint_info> breakpoints;

protected:
   // registered C functions callable from Lua
   // prototype: static int xyz(lua_State* L);
   static int uw_print(lua_State* L);        ///< Lua function uw.print
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
