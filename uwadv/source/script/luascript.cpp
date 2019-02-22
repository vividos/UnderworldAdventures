//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2005,2019 Underworld Adventures Team
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
/// \file luascript.cpp
/// \brief Lua scripting implementation
//
#include "common.hpp"
#include "luascript.hpp"
#include "game_interface.hpp"
#include "user_interface.hpp"
#include "gamestrings.hpp"
#include "uwadv/debug.hpp"
#include "resourcemanager.hpp"

extern "C"
{
#include "lua/include/lua.h"
#include "lua/include/lualib.h"
#include "lua/include/luadebug.h"

#include "lua/src/lstate.h"
#include "lua/src/lobject.h"
}

const char* LuaScripting::self_name = "_scripting_self";

LuaScripting::LuaScripting()
   :L(NULL), game(NULL), debugger_state(ua_debugger_state_inactive)
{
}

void LuaScripting::init(IBasicGame* the_game)
{
   game = the_game;

   // init lua state
   L = lua_open(256);

   // open lualib libraries
   lua_baselibopen(L);
   lua_strlibopen(L);
   lua_mathlibopen(L);

   // set "self" pointer userdata
   lua_pushuserdata(L, this);
   lua_setglobal(L, self_name);

   register_functions();

   // notify debugger of start of Lua script code debugger
   game->get_debugger().start_code_debugger(this);

   debugger_state = ua_debugger_state_running;
   //   command = ua_debugger_command_step_into; // ensures that we break at the first line
   command = ua_debugger_command_run;
   step_over_func_depth = 0;
   curpos_srcfile = curpos_line = unsigned(-1);
}

bool LuaScripting::load_script(const char* basename)
{
   std::string scriptname(basename);
   scriptname.append(".lua");

   bool compiled = false;

   // load lua script
   Base::SDL_RWopsPtr script =
      game->GetResourceManager().GetResourceFile(scriptname.c_str());

   // not found? try binary one
   if (script == NULL)
   {
      scriptname.assign(basename);
      scriptname.append(".lob");
      compiled = true;

      script = game->GetResourceManager().GetResourceFile(scriptname.c_str());
   }

   // still not found? exception
   if (script == NULL)
   {
      std::string text("could not load Lua script from uadata: ");
      text.append(scriptname.c_str());
      throw Base::Exception(text.c_str());
   }

   std::string complete_scriptname = game->get_settings().GetString(Base::settingUadataPath);
   complete_scriptname += scriptname.c_str();

   int ret = load_script(script, complete_scriptname.c_str());

   UaTrace("loaded Lua %sscript \"%s\"\n",
      compiled ? "compiled " : "", scriptname.c_str());

   // put script name in list, if it's not a compiled one
   if (!compiled)
      loaded_script_files.push_back(complete_scriptname);

   return ret == 0;
}

void LuaScripting::done()
{
   // notify debugger of end of Lua script code debugger
   game->get_debugger().end_code_debugger(this);

   lua_close(L);
}

void LuaScripting::checked_call(int nargs, int nresults)
{
   lua_setcallhook(L, debug_hook_call);
   lua_setlinehook(L, debug_hook_line);

   lua_call(L, nargs, nresults);

   lua_setcallhook(L, NULL);
   lua_setlinehook(L, NULL);
}

void LuaScripting::init_new_game()
{
   lua_getglobal(L, "game_init_new");
   checked_call(0, 0);
}

void LuaScripting::eval_critter(Uint16 pos)
{
   lua_getglobal(L, "critter_eval");
   lua_pushnumber(L, static_cast<double>(pos));
   checked_call(1, 0);
}

void LuaScripting::trigger_set_off(Uint16 pos)
{
   lua_getglobal(L, "trigger_set_off");
   lua_pushnumber(L, static_cast<double>(pos));
   checked_call(1, 0);
}

void LuaScripting::UserAction(EUserInterfaceUserAction action,
   unsigned int param)
{
   switch (action)
   {
   case ua_action_look_object:
      lua_getglobal(L, "object_look");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1, 0);
      break;

   case ua_action_look_object_inventory:
      lua_getglobal(L, "inventory_look");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1, 0);
      break;

   case ua_action_look_wall:
      lua_getglobal(L, "look_at_wall");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1, 0);
      break;

   case ua_action_use_object:
      lua_getglobal(L, "object_use");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1, 0);
      break;

   case ua_action_use_object_inventory:
      lua_getglobal(L, "inventory_use");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1, 0);
      break;

   case ua_action_use_wall:
      lua_getglobal(L, "wall_use");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1, 0);
      break;

   case ua_action_get_object:
      lua_getglobal(L, "object_get");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1, 0);
      break;

   case ua_action_talk_object:
      lua_getglobal(L, "object_talk");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1, 0);
      break;

   case ua_action_clicked_spells:
      lua_getglobal(L, "spell_cast");
      checked_call(0, 0);
      break;

   case ua_action_clicked_runeshelf:
      lua_getglobal(L, "spell_cancel");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1, 0);
      break;

   case ua_action_clicked_compass:
      lua_getglobal(L, "ui_clicked_compass");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1, 0);
      break;

   case ua_action_clicked_vitality_flask:
      lua_getglobal(L, "ui_clicked_vitality_flask");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1, 0);
      break;

   case ua_action_clicked_mana_flask:
      lua_getglobal(L, "ui_clicked_mana_flask");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1, 0);
      break;

   case ua_action_clicked_gargoyle:
      lua_getglobal(L, "ui_clicked_gargoyle");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1, 0);
      break;

   case ua_action_clicked_dragons:
      lua_getglobal(L, "ui_clicked_dragons");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1, 0);
      break;

      // ua_action_combat_enter
      // ua_action_combat_draw
      // ua_action_combat_leave
      // ua_action_combat_release

   case ua_action_track_creatures:
      lua_getglobal(L, "track_creatures");
      checked_call(0, 0);
      break;

   case ua_action_sleep:
      lua_getglobal(L, "sleep");
      checked_call(0, 0);
      break;

   case ua_action_target_selected:
      break;

   default:
      break;
   }
}

void LuaScripting::on_changing_level()
{
   lua_getglobal(L, "on_change_level");
   checked_call(0, 0);
}

EItemCombineStatus LuaScripting::item_combine(Uint16 item_id1,
   Uint16 item_id2, Uint16& result_id)
{
   lua_getglobal(L, "item_combine");
   lua_pushnumber(L, static_cast<double>(item_id1));
   lua_pushnumber(L, static_cast<double>(item_id2));
   checked_call(2, 2);

   result_id = Underworld::c_itemIDNone;
   return ua_item_combine_failed;
}

int LuaScripting::load_script(Base::SDL_RWopsPtr rwops, const char* chunkname)
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
      buffer[len] = 0;
   }

   // execute script
   int ret = lua_dobuffer(L, &buffer[0], len, chunkname);

   if (ret != 0)
      UaTrace("Lua script loading ended with error code %u\n", ret);

   return ret;
}

IScripting* IScripting::create_scripting(EScriptingLanguage)
{
   return new LuaScripting;
}

LuaScripting& LuaScripting::get_scripting_from_self(lua_State* L)
{
   LuaScripting* self = NULL;

   lua_getglobal(L, self_name);
   if (!lua_isuserdata(L, -1))
      throw Base::Exception("'self' parameter wasn't set by LuaScripting");

   // get pointer to underworld
   self = reinterpret_cast<LuaScripting*>(lua_touserdata(L, -1));

   if (self == NULL || self->L != L)
      throw Base::Exception("wrong 'self' parameter in LuaScripting");

   lua_pop(L, 1);

   return *self;
}

void LuaScripting::debug_hook_call(lua_State* L, lua_Debug* ar)
{
   lua_getstack(L, 0, ar);
   lua_getinfo(L, "Snlu", ar);

   // don't report C functions
   if (0 == strcmp(ar->source, "=C"))
      return;

   LuaScripting& script = get_scripting_from_self(L);
   UaAssert(script.get_lua_State() == L);

   script.debug_hook(ar);
}

void LuaScripting::debug_hook_line(lua_State* L, lua_Debug* ar)
{
   lua_getstack(L, 0, ar);
   lua_getinfo(L, "Snlu", ar);

   LuaScripting& script = get_scripting_from_self(L);
   UaAssert(script.get_lua_State() == L);

   script.debug_hook(ar);
}

void LuaScripting::debug_hook(lua_Debug* ar)
{
   UaTrace("debug: event=%s, name=%s, start=%d, line=%d, in=%s\n",
      ar->event, ar->name, ar->linedefined, ar->currentline, ar->source);

   if (ar->source != NULL)
   {
      curpos_srcfile = unsigned(-1);

      // find out current index
      unsigned int max = loaded_script_files.size();
      for (unsigned int n = 0; n < max; n++)
         if (loaded_script_files[n] == ar->source)
         {
            curpos_srcfile = n;
            break;
         }
   }

   if (ar->currentline > 0)
      curpos_line = static_cast<unsigned int>(ar->currentline);

   ua_debug_code_debugger_state old_state = get_debugger_state();

   // check commands
//   if (get_debugger_state() == ua_debugger_state_running)
   {
      switch (get_debugger_command())
      {
      case ua_debugger_command_run:
         set_debugger_state(ua_debugger_state_running);
         break;

      case ua_debugger_command_step_over:
         if (0 == strcmp(ar->event, "line"))
         {
            if (step_over_func_depth == 0)
               set_debugger_state(ua_debugger_state_break);
         }
         else
            if (0 == strcmp(ar->event, "call"))
            {
               if (get_debugger_state() == ua_debugger_state_running)
                  step_over_func_depth++; //
               else
                  step_over_func_depth = 0; // start of stepping over a function
            }
            else
               if (0 == strcmp(ar->event, "return"))
               {
                  --step_over_func_depth; // returning from function
                  if (step_over_func_depth == 0)
                  {
                     // returned from stepped over function
                     set_debugger_state(ua_debugger_state_running);
                     set_debugger_command(ua_debugger_command_step_into);
                  }
               }
         break;

      case ua_debugger_command_step_into:
         if (0 == strcmp(ar->event, "line"))
         {
            set_debugger_state(ua_debugger_state_break);
         }
         break;

      case ua_debugger_command_step_out:
         if (0 == strcmp(ar->event, "return"))
         {
            // when returning set "step over" command to stop at the next line
            set_debugger_state(ua_debugger_state_running);
            set_debugger_command(ua_debugger_command_step_over);
         }
         break;
      }
   }

   // check for breakpoints
   check_breakpoints();

   // check if state changed
   if (old_state != get_debugger_state())
      game->get_debugger().send_code_debugger_status_update(debugger_id);

   // wait for state to change to "running" again
   wait_debugger_continue();
}

void LuaScripting::check_breakpoints()
{
}

void LuaScripting::wait_debugger_continue()
{
   while (get_debugger_state() == ua_debugger_state_break)
      SDL_Delay(10); // TODO
}

//! registers a C function inside a table
#define lua_register_table(L, n, f) { \
   lua_pushstring(L, n); lua_pushcfunction(L, f); lua_settable(L,-3); }

/*! Functions that are callable via Lua are organized in global tables; this
    way we simulate objects that can be accessed, e.g. in uw.print("text")
*/
void LuaScripting::register_functions()
{
   // uw object
   lua_newtable(L);
   lua_register_table(L, "print", uw_print);
   lua_register_table(L, "get_string", uw_get_string);
   lua_register_table(L, "ChangeLevel", uw_change_level);
   lua_register_table(L, "start_conv", uw_start_conv);
   lua_setglobal(L, "uw");

   // player object
   lua_newtable(L);
   lua_register_table(L, "get_info", player_get_info);
   lua_register_table(L, "set_info", player_set_info);
   lua_register_table(L, "get_attr", player_get_attr);
   lua_register_table(L, "SetAttribute", player_set_attr);
   lua_register_table(L, "get_skill", player_get_skill);
   lua_register_table(L, "SetSkill", player_set_skill);
   lua_setglobal(L, "player");

   // objectList object
   lua_newtable(L);
   lua_register_table(L, "get_info", objlist_get_info);
   lua_register_table(L, "set_info", objlist_set_info);
   lua_register_table(L, "delete", objlist_delete);
   lua_register_table(L, "insert", objlist_insert);
   lua_setglobal(L, "objectList");

   // tilemap object
   lua_newtable(L);
   lua_register_table(L, "get_info", tilemap_get_info);
   lua_register_table(L, "set_info", tilemap_set_info);
   lua_register_table(L, "get_floor_height", tilemap_get_floor_height);
   lua_register_table(L, "get_objlist_link", tilemap_get_objlist_link);
   lua_setglobal(L, "tilemap");

   // runes object
   lua_newtable(L);
   lua_register_table(L, "set", runes_set);
   lua_register_table(L, "test", runes_test);
   lua_setglobal(L, "runes");

   // conv object
   lua_newtable(L);
   lua_register_table(L, "is_avail", conv_is_avail);
   lua_register_table(L, "GetGlobal", conv_get_global);
   lua_register_table(L, "SetGlobal", conv_set_global);
   lua_setglobal(L, "conv");

   // quest object
   lua_newtable(L);
   lua_register_table(L, "get_flag", quest_get_flag);
   lua_register_table(L, "set_flag", quest_set_flag);
   lua_setglobal(L, "quest");

   // prop object
   lua_newtable(L);
   lua_register_table(L, "get_common", prop_get_common);
   lua_register_table(L, "get_special", prop_get_special);
   lua_setglobal(L, "prop");
}

ua_debug_code_debugger_type LuaScripting::get_debugger_type()
{
   return ua_code_debugger_lua;
}

/*! Prepares debugging info.
    The Lua struct Proto contains all informations about a function prototype.
    It also contains line number info when a source file was loaded. The
    format of the "lineinfo" array is as follows:
    The lineinfo array contains "nlineinfo" items, which is always an odd
    number. The last item contains MAX_INT to signal an end. The list contains
    pairs of int values. The first one describes a line number and is coded to
    save space. If the value is negative, it is a relative value to the
    previous line number. The first line number is 1 (one). If the value is
    positive, it is not a line number offset and the line number stays the
    same. The second value is a program counter into the compiled bytecode.
    It points to the start of the code that is generated from the line
    described in the first value.
*/
void LuaScripting::prepare_debug_info()
{
   // retrieve debug info from lua struct
   // go through list of all function prototype structures
   Proto* proto = L->rootproto;
   while (proto != NULL)
   {
      std::string filename(proto->source->str);

      // find line numbers set
      std::map<std::string, std::set<unsigned int> >::iterator iter =
         all_line_numbers.find(filename);
      if (iter == all_line_numbers.end())
      {
         // insert new set; we have a new filename
         std::set<unsigned int> empty_set;
         all_line_numbers[filename] = empty_set;
         iter = all_line_numbers.find(filename);
      }

      // go through all lineinfo value pairs
      int* lineinfo = proto->lineinfo;
      int curline = 0;
      for (int n = 0; n < proto->nlineinfo && lineinfo[n] != MAX_INT; n += 2)
      {
         // value negative? then add offset
         if (lineinfo[n] < 0)
            curline += -lineinfo[n] + 1;

         iter->second.insert(static_cast<unsigned int>(curline));
      }

      proto = proto->next;
   }
}

ua_debug_code_debugger_state LuaScripting::get_debugger_state() const
{
   // TODO protect access
   return debugger_state;
}

void LuaScripting::set_debugger_state(ua_debug_code_debugger_state new_state)
{
   // TODO protect access
   debugger_state = new_state;
}

ua_debug_code_debugger_command LuaScripting::get_debugger_command() const
{
   // TODO protect access
   return command;
}

void LuaScripting::set_debugger_command(ua_debug_code_debugger_command the_command)
{
   //   if (get_debugger_state() == ua_debugger_state_running)
   //      return; // don't set command when still running

      // TODO protect access
   command = the_command;
   //   debugger_state = ua_debugger_state_running;
}

void LuaScripting::get_current_pos(unsigned int& sourcefile_index,
   unsigned int& sourcefile_line, unsigned int& code_pos,
   bool& sourcefile_is_valid)
{
   sourcefile_index = curpos_srcfile;
   sourcefile_line = curpos_line;
   sourcefile_is_valid = true;
}

unsigned int LuaScripting::get_num_sourcefiles() const
{
   return static_cast<unsigned int>(loaded_script_files.size());
}

unsigned int LuaScripting::get_sourcefile_name(unsigned int index, char* buffer, unsigned int len)
{
   std::string filename(loaded_script_files[index]);
   if (filename.size() > 0 && filename[0] == '@')
      filename.erase(0, 1); // remove @ char

   size_t size = filename.size();

   if (buffer == NULL || len == 0 || len < size + 1)
      return size + 1;

   strncpy(buffer, filename.c_str(), size);
   buffer[size] = 0;

   return filename.size() + 1;
}

unsigned int LuaScripting::get_num_breakpoints()
{
   return breakpoints.size();
}

void LuaScripting::get_breakpoint_info(unsigned int breakpoint_index,
   unsigned int& sourcefile_index, unsigned int& sourcefile_line,
   unsigned int& code_pos, bool& visible)
{
   UaAssert(breakpoint_index < get_num_breakpoints());
   if (breakpoint_index >= get_num_breakpoints())
      return;

   ua_debug_code_breakpoint_info& info = breakpoints[breakpoint_index];

   visible = info.visible;
   sourcefile_index = info.pos.sourcefile_index;
   sourcefile_line = info.pos.sourcefile_line;
   code_pos = info.pos.code_pos;
}

int LuaScripting::uw_print(lua_State* L)
{
   LuaScripting& self = get_scripting_from_self(L);

   IUserInterface* callback = self.game->GetUserInterface();
   if (callback != NULL)
   {
      const char* text = lua_tostring(L, -1);
      callback->uw_print(text);
   }
   return 0;
}

int LuaScripting::uw_get_string(lua_State* L)
{
   LuaScripting& self = get_scripting_from_self(L);

   unsigned int block = static_cast<unsigned int>(lua_tonumber(L, -2));
   unsigned int num = static_cast<unsigned int>(lua_tonumber(L, -1));

   // retrieve game string
   std::string str(self.game->GetGameStrings().GetString(block, num));
   lua_pushstring(L, str.c_str());

   return 1;
}

int LuaScripting::uw_change_level(lua_State* L)
{
   LuaScripting& self = get_scripting_from_self(L);

   unsigned int level = static_cast<unsigned int>(lua_tonumber(L, -1));

   self.game->GetGameLogic().ChangeLevel(level);

   return 0;
}

int LuaScripting::uw_start_conv(lua_State* L)
{
   LuaScripting& self = get_scripting_from_self(L);

   IUserInterface* callback = self.game->GetUserInterface();
   if (callback != NULL)
   {
      Uint16 conv_obj = static_cast<Uint16>(lua_tonumber(L, -1));
      callback->uw_start_conversation(conv_obj);
   }
   return 0;
}

int LuaScripting::player_get_info(lua_State* L)
{
   // retrieve player object
   LuaScripting& self = get_scripting_from_self(L);
   Underworld::Player& player = self.game->GetUnderworld().GetPlayer();

   // create new table and fill it with infos
   lua_newtable(L);

   lua_pushstring(L, "name");
   lua_pushstring(L, player.GetName().c_str());
   lua_settable(L, -3);

   lua_pushstring(L, "xpos");
   lua_pushnumber(L, player.GetXPos());
   lua_settable(L, -3);

   lua_pushstring(L, "ypos");
   lua_pushnumber(L, player.GetYPos());
   lua_settable(L, -3);

   lua_pushstring(L, "height");
   lua_pushnumber(L, player.GetHeight());
   lua_settable(L, -3);

   lua_pushstring(L, "angle");
   lua_pushnumber(L, player.GetRotateAngle());
   lua_settable(L, -3);

   // leave table on stack and return it
   return 1;
}

int LuaScripting::player_set_info(lua_State* L)
{
   // retrieve player object
   LuaScripting& self = get_scripting_from_self(L);
   Underworld::Player& player = self.game->GetUnderworld().GetPlayer();

   if (!lua_istable(L, -1))
   {
      UaTrace("player_set_info: got wrong parameter\n");
      return 0;
   }

   // table is on stack, at -1
   lua_pushstring(L, "name");
   lua_gettable(L, -2);
   std::string name = lua_tostring(L, -1);
   player.SetName(name);
   lua_pop(L, 1);

   lua_pushstring(L, "xpos");
   lua_gettable(L, -2);
   lua_pushstring(L, "ypos");
   lua_gettable(L, -3);
   player.SetPos(lua_tonumber(L, -2), lua_tonumber(L, -1));
   lua_pop(L, 2);

   lua_pushstring(L, "height");
   lua_gettable(L, -2);
   player.SetHeight(lua_tonumber(L, -1));
   lua_pop(L, 1);

   lua_pushstring(L, "angle");
   lua_gettable(L, -2);
   player.SetRotateAngle(lua_tonumber(L, -1));
   lua_pop(L, 1);

   return 0;
}

int LuaScripting::player_get_attr(lua_State* L)
{
   // retrieve player object
   LuaScripting& self = get_scripting_from_self(L);
   Underworld::Player& player = self.game->GetUnderworld().GetPlayer();

   Underworld::EPlayerAttribute attr =
      static_cast<Underworld::EPlayerAttribute>(static_cast<int>(lua_tonumber(L, -1)));

   lua_pushnumber(L, static_cast<double>(player.GetAttribute(attr)));
   return 1;
}

int LuaScripting::player_set_attr(lua_State* L)
{
   // retrieve player object
   LuaScripting& self = get_scripting_from_self(L);
   Underworld::Player& player = self.game->GetUnderworld().GetPlayer();

   Underworld::EPlayerAttribute attrType =
      static_cast<Underworld::EPlayerAttribute>(static_cast<int>(lua_tonumber(L, -2)));
   unsigned int attrValue = static_cast<unsigned int>(lua_tonumber(L, -1));

   player.SetAttribute(attrType, attrValue);
   return 0;
}

int LuaScripting::player_get_skill(lua_State* L)
{
   // retrieve player object
   LuaScripting& self = get_scripting_from_self(L);
   Underworld::Player& player = self.game->GetUnderworld().GetPlayer();

   Underworld::EPlayerSkill skillType =
      static_cast<Underworld::EPlayerSkill>(static_cast<int>(lua_tonumber(L, -1)));

   lua_pushnumber(L, static_cast<double>(player.GetSkill(skillType)));
   return 1;
}

int LuaScripting::player_set_skill(lua_State* L)
{
   // retrieve player object
   LuaScripting& self = get_scripting_from_self(L);
   Underworld::Player& player = self.game->GetUnderworld().GetPlayer();

   Underworld::EPlayerSkill skillType =
      static_cast<Underworld::EPlayerSkill>(static_cast<int>(lua_tonumber(L, -2)));
   unsigned int skill_value = static_cast<unsigned int>(lua_tonumber(L, -1));

   player.SetSkill(skillType, skill_value);
   return 0;
}

int LuaScripting::objlist_get_info(lua_State* L)
{
   // retrieve object list
   LuaScripting& self = get_scripting_from_self(L);
   Underworld::ObjectList& objectList =
      self.game->GetGameLogic().GetCurrentLevel().GetObjectList();

   // get object pos and object
   Uint16 objectPos = static_cast<Uint16>(lua_tonumber(L, -1));

   Underworld::ObjectPtr obj = objectList.GetObject(objectPos);
   Underworld::ObjectInfo& info = obj->GetObjectInfo();
   Underworld::ObjectPositionInfo& posInfo = obj->GetPosInfo();

   // create new table and fill it with infos
   lua_newtable(L);

   lua_pushstring(L, "objectPos");
   lua_pushnumber(L, static_cast<double>(objectPos));
   lua_settable(L, -3);

   lua_pushstring(L, "item_id");
   lua_pushnumber(L, static_cast<double>(info.m_itemID));
   lua_settable(L, -3);

   lua_pushstring(L, "enchanted");
   lua_pushnumber(L, info.m_isEnchanted ? 1.0 : 0.0);
   lua_settable(L, -3);

   lua_pushstring(L, "is_quantity");
   lua_pushnumber(L, info.m_isQuantity ? 1.0 : 0.0);
   lua_settable(L, -3);

   lua_pushstring(L, "is_hidden");
   lua_pushnumber(L, info.m_isHidden ? 1.0 : 0.0);
   lua_settable(L, -3);

   lua_pushstring(L, "flags");
   lua_pushnumber(L, static_cast<double>(info.m_flags));
   lua_settable(L, -3);


   lua_pushstring(L, "xpos");
   lua_pushnumber(L, static_cast<double>(posInfo.m_xpos));
   lua_settable(L, -3);

   lua_pushstring(L, "ypos");
   lua_pushnumber(L, static_cast<double>(posInfo.m_ypos));
   lua_settable(L, -3);

   lua_pushstring(L, "zpos");
   lua_pushnumber(L, static_cast<double>(posInfo.m_zpos));
   lua_settable(L, -3);

   lua_pushstring(L, "heading");
   lua_pushnumber(L, static_cast<double>(posInfo.m_heading));
   lua_settable(L, -3);

   lua_pushstring(L, "tilex");
   lua_pushnumber(L, static_cast<double>(posInfo.m_tileX));
   lua_settable(L, -3);

   lua_pushstring(L, "tiley");
   lua_pushnumber(L, static_cast<double>(posInfo.m_tileY));
   lua_settable(L, -3);


   lua_pushstring(L, "quality");
   lua_pushnumber(L, static_cast<double>(info.m_quality));
   lua_settable(L, -3);

   lua_pushstring(L, "link_next");
   lua_pushnumber(L, static_cast<double>(info.m_link));
   lua_settable(L, -3);

   lua_pushstring(L, "owner");
   lua_pushnumber(L, static_cast<double>(info.m_owner));
   lua_settable(L, -3);

   lua_pushstring(L, "quantity");
   lua_pushnumber(L, static_cast<double>(info.m_quantity));
   lua_settable(L, -3);

   bool isNpc = obj->IsNpcObject();

   lua_pushstring(L, "npc_used");
   lua_pushnumber(L, isNpc ? 1.0 : 0.0);
   lua_settable(L, -3);

   // add npc infos
   if (isNpc)
   {
      Underworld::NpcObject& npc = obj->GetNpcObject();
      Underworld::NpcInfo& npcInfo = npc.GetNpcInfo();

      lua_pushstring(L, "npc_hp");
      lua_pushnumber(L, static_cast<double>(npcInfo.npc_hp));
      lua_settable(L, -3);

      lua_pushstring(L, "npc_goal");
      lua_pushnumber(L, static_cast<double>(npcInfo.npc_goal));
      lua_settable(L, -3);

      lua_pushstring(L, "npc_gtarg");
      lua_pushnumber(L, static_cast<double>(npcInfo.npc_gtarg));
      lua_settable(L, -3);

      lua_pushstring(L, "npc_level");
      lua_pushnumber(L, static_cast<double>(npcInfo.npc_level));
      lua_settable(L, -3);

      lua_pushstring(L, "npc_talkedto");
      lua_pushnumber(L, npcInfo.npc_talkedto ? 1.0 : 0.0);
      lua_settable(L, -3);

      lua_pushstring(L, "npc_attitude");
      lua_pushnumber(L, static_cast<double>(npcInfo.npc_attitude));
      lua_settable(L, -3);

      lua_pushstring(L, "npc_xhome");
      lua_pushnumber(L, static_cast<double>(npcInfo.npc_xhome));
      lua_settable(L, -3);

      lua_pushstring(L, "npc_yhome");
      lua_pushnumber(L, static_cast<double>(npcInfo.npc_yhome));
      lua_settable(L, -3);

      lua_pushstring(L, "npc_hunger");
      lua_pushnumber(L, static_cast<double>(npcInfo.npc_hunger));
      lua_settable(L, -3);

      lua_pushstring(L, "npc_whoami");
      lua_pushnumber(L, static_cast<double>(npcInfo.npc_whoami));
      lua_settable(L, -3);
   }

   // leave table on stack and return it
   return 1;
}

/// \todo implement
int LuaScripting::objlist_set_info(lua_State* L)
{
   return 0;
}

int LuaScripting::objlist_delete(lua_State* L)
{
   // retrieve object list
   LuaScripting& self = get_scripting_from_self(L);
   Underworld::ObjectList& objectList =
      self.game->GetGameLogic().GetCurrentLevel().GetObjectList();

   // get object pos and object
   Uint16 objectPos = static_cast<Uint16>(lua_tonumber(L, -1));

   objectList.Free(objectPos);

   return 0;
}

/*! \todo implement */
int LuaScripting::objlist_insert(lua_State* L)
{
   // retrieve object list
   LuaScripting& self = get_scripting_from_self(L);
   Underworld::ObjectList& objectList =
      self.game->GetGameLogic().GetCurrentLevel().GetObjectList();
   objectList;

   return 0;//1;
}

int LuaScripting::tilemap_get_info(lua_State* L)
{
   // retrieve current level
   LuaScripting& self = get_scripting_from_self(L);
   Underworld::Level& level = self.game->GetGameLogic().GetCurrentLevel();

   // get tilemap info
   double xpos = lua_tonumber(L, -2);
   double ypos = lua_tonumber(L, -1);

   const Underworld::TileInfo& tileinfo = level.GetTilemap().GetTileInfo(unsigned(xpos), unsigned(ypos));

   // create new table and fill it with infos
   lua_newtable(L);

   lua_pushstring(L, "xpos");
   lua_pushnumber(L, static_cast<double>(xpos));
   lua_settable(L, -3);

   lua_pushstring(L, "ypos");
   lua_pushnumber(L, static_cast<double>(ypos));
   lua_settable(L, -3);

   lua_pushstring(L, "type");
   lua_pushnumber(L, static_cast<double>(tileinfo.m_type));
   lua_settable(L, -3);

   lua_pushstring(L, "floor");
   lua_pushnumber(L, static_cast<double>(tileinfo.m_floor));
   lua_settable(L, -3);

   lua_pushstring(L, "ceiling");
   lua_pushnumber(L, static_cast<double>(tileinfo.m_ceiling));
   lua_settable(L, -3);

   lua_pushstring(L, "slope");
   lua_pushnumber(L, static_cast<double>(tileinfo.m_slope));
   lua_settable(L, -3);

   // TODO tileinfo.texture_floor
   //tileinfo.texture_ceiling
   //tileinfo.texture_wall;

   // leave table on stack and return it
   return 1;
}

/*! \todo implement */
int LuaScripting::tilemap_set_info(lua_State* L)
{
   // retrieve current level
   LuaScripting& self = get_scripting_from_self(L);
   Underworld::Level& level = self.game->GetGameLogic().GetCurrentLevel();
   level;

   return 0;
}

int LuaScripting::tilemap_get_floor_height(lua_State* L)
{
   // retrieve current level
   LuaScripting& self = get_scripting_from_self(L);
   Underworld::Level& level = self.game->GetGameLogic().GetCurrentLevel();

   double xpos = lua_tonumber(L, -2);
   double ypos = lua_tonumber(L, -1);

   lua_pushnumber(L, level.GetTilemap().GetFloorHeight(xpos, ypos));
   return 1;
}

int LuaScripting::tilemap_get_objlist_link(lua_State* L)
{
   // retrieve current level
   LuaScripting& self = get_scripting_from_self(L);
   Underworld::Level& level = self.game->GetGameLogic().GetCurrentLevel();

   double xpos = lua_tonumber(L, -2);
   double ypos = lua_tonumber(L, -1);
   Uint16 objpos = level.GetObjectList().GetListStart(unsigned(xpos), unsigned(ypos));

   lua_pushnumber(L, static_cast<double>(objpos));
   return 1;
}

int LuaScripting::runes_set(lua_State* L)
{
   // retrieve runes object
   LuaScripting& self = get_scripting_from_self(L);
   Underworld::Runebag& runeBag = self.game->GetUnderworld().GetPlayer().GetRunebag();

   unsigned int rune = static_cast<unsigned int>(lua_tonumber(L, -2));
   bool flag = lua_tonumber(L, -1) > 0.0;

   runeBag.SetRune(static_cast<Underworld::ERuneType>(rune), flag);

   return 0;
}

int LuaScripting::runes_test(lua_State* L)
{
   // retrieve runes object
   LuaScripting& self = get_scripting_from_self(L);
   Underworld::Runebag& runeBag = self.game->GetUnderworld().GetPlayer().GetRunebag();

   unsigned int rune = static_cast<unsigned int>(lua_tonumber(L, -1));

   lua_pushnumber(L, runeBag.IsInBag(static_cast<Underworld::ERuneType>(rune)) ? 1.0 : 0.0);

   return 1;
}

int LuaScripting::conv_is_avail(lua_State* L)
{
   // retrieve game strings object
   LuaScripting& self = get_scripting_from_self(L);
   GameStrings& gameStrings = self.game->GetGameStrings();

   Uint16 slot = static_cast<Uint16>(lua_tonumber(L, -1));

   if (!gameStrings.IsBlockAvail(slot + 0x0e00))
      lua_pushnil(L);
   else
      lua_pushnumber(L, static_cast<double>(slot));

   return 1;
}

int LuaScripting::conv_get_global(lua_State* L)
{
   // retrieve conv globals object
   LuaScripting& self = get_scripting_from_self(L);

   Uint16 slot = static_cast<Uint16>(lua_tonumber(L, -1));
   unsigned int pos = static_cast<unsigned int>(lua_tonumber(L, -2));

   std::vector<Uint16>& globals =
      self.game->GetUnderworld().GetPlayer().GetConvGlobals().GetSlotGlobals(slot);

   if (pos >= globals.size())
   {
      UaTrace("conv.GetGlobal: globals pos out of range!\n");
      return 0;
   }

   lua_pushnumber(L, static_cast<double>(globals[pos]));
   return 1;
}

int LuaScripting::conv_set_global(lua_State* L)
{
   // retrieve conv globals object
   LuaScripting& self = get_scripting_from_self(L);

   Uint16 slot = static_cast<Uint16>(lua_tonumber(L, -1));
   unsigned int pos = static_cast<unsigned int>(lua_tonumber(L, -2));
   Uint16 value = static_cast<Uint16>(lua_tonumber(L, -2));

   std::vector<Uint16>& globals =
      self.game->GetUnderworld().GetPlayer().GetConvGlobals().GetSlotGlobals(slot);

   if (pos >= globals.size())
      UaTrace("conv.SetGlobal: globals pos out of range!\n");
   else
      globals[pos] = value;

   return 0;
}

int LuaScripting::quest_get_flag(lua_State* L)
{
   // retrieve quest flags object
   LuaScripting& self = get_scripting_from_self(L);
   Underworld::QuestFlags& questFlags =
      self.game->GetUnderworld().GetPlayer().GetQuestFlags();

   Uint16 flagNumber = static_cast<Uint16>(lua_tonumber(L, -1));

   if (flagNumber >= questFlags.GetFlagCount())
   {
      UaTrace("quest.get_flag: flagNumber out of range!\n");
      return 0;
   }

   lua_pushnumber(L, static_cast<double>(questFlags.GetFlag(flagNumber)));

   return 1;
}

int LuaScripting::quest_set_flag(lua_State* L)
{
   // retrieve quest flags object
   LuaScripting& self = get_scripting_from_self(L);
   Underworld::QuestFlags& questFlags =
      self.game->GetUnderworld().GetPlayer().GetQuestFlags();

   Uint16 flagNumber = static_cast<Uint16>(lua_tonumber(L, -1));
   Uint16 flagValue = static_cast<Uint16>(lua_tonumber(L, -2));

   if (flagNumber >= questFlags.GetFlagCount())
      UaTrace("quest.set_flag: flagNumber out of range!\n");
   else
      questFlags.SetFlag(flagNumber, flagValue);

   return 0;
}

int LuaScripting::prop_get_common(lua_State* L)
{
   // retrieve common object properties
   LuaScripting& self = get_scripting_from_self(L);

   Uint16 item_id = static_cast<Uint16>(lua_tonumber(L, -1));

   const Underworld::CommonObjectProperty& prop = self.game->GetGameLogic().
      GetObjectProperties().GetCommonProperty(item_id);

   // create new table and fill it with infos
   lua_newtable(L);

   lua_pushstring(L, "item_id");
   lua_pushnumber(L, static_cast<double>(item_id));
   lua_settable(L, -3);

   lua_pushstring(L, "height");
   lua_pushnumber(L, static_cast<double>(prop.uiHeight));
   lua_settable(L, -3);

   lua_pushstring(L, "mass");
   lua_pushnumber(L, static_cast<double>(prop.uiMass));
   lua_settable(L, -3);

   lua_pushstring(L, "radius");
   lua_pushnumber(L, static_cast<double>(prop.uiRadius));
   lua_settable(L, -3);

   lua_pushstring(L, "quality_class");
   lua_pushnumber(L, static_cast<double>(prop.uiQualityClass));
   lua_settable(L, -3);

   lua_pushstring(L, "quality_type");
   lua_pushnumber(L, static_cast<double>(prop.uiQualityType));
   lua_settable(L, -3);

   lua_pushstring(L, "can_have_owner");
   lua_pushnumber(L, prop.bCanHaveOwner ? 1.0 : 0.0);
   lua_settable(L, -3);

   lua_pushstring(L, "can_be_looked_at");
   lua_pushnumber(L, prop.bCanBeLookedAt ? 1.0 : 0.0);
   lua_settable(L, -3);

   lua_pushstring(L, "can_be_picked_up");
   lua_pushnumber(L, prop.bCanBePickedUp ? 1.0 : 0.0);
   lua_settable(L, -3);

   lua_pushstring(L, "is_container");
   lua_pushnumber(L, prop.bIsContainer ? 1.0 : 0.0);
   lua_settable(L, -3);

   return 1;
}

/*! \todo implement */
int LuaScripting::prop_get_special(lua_State* L)
{
   // retrieve common object properties
   LuaScripting& self = get_scripting_from_self(L);
   Underworld::ObjectProperties& prop =
      self.game->GetGameLogic().GetObjectProperties();
   prop;

   return 0;//1;
}
