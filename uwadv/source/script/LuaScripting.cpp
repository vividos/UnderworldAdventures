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
/// \file LuaScripting.cpp
/// \brief Lua scripting implementation
//
#include "common.hpp"
#include "LuaScripting.hpp"
#include "GameInterface.hpp"
#include "IUserInterface.hpp"
#include "GameStrings.hpp"
#include "uwadv/DebugServer.hpp"
#include "ResourceManager.hpp"

extern "C"
{
#include "lua/include/lua.h"
#include "lua/include/lualib.h"
#include "lua/include/luadebug.h"

#include "lua/src/lstate.h"
#include "lua/src/lobject.h"
}

const char* LuaScripting::s_selfName = "_scripting_self";

LuaScripting::LuaScripting()
   :L(NULL), m_game(NULL), m_debuggerState(codeDebuggerStateInactive)
{
}

void LuaScripting::Init(IBasicGame* game)
{
   m_game = game;

   // init lua state
   L = lua_open(256);

   // open lualib libraries
   lua_baselibopen(L);
   lua_strlibopen(L);
   lua_mathlibopen(L);

   // set "self" pointer userdata
   lua_pushuserdata(L, this);
   lua_setglobal(L, s_selfName);

   RegisterFunctions();

   // notify debugger of start of Lua script code debugger
   m_game->GetDebugger().StartCodeDebugger(this);

   m_debuggerState = codeDebuggerStateRunning;
   //   m_debuggerCommand = codeDebuggerCommandStepInto; // ensures that we break at the first line
   m_debuggerCommand = codeDebuggerCommandRun;
   m_stepOverFunctionCallDepth = 0;
   m_currentPositionSourcefileIndex = m_currentPositionSourcefileLine = unsigned(-1);
}

bool LuaScripting::LoadScript(const char* basename)
{
   std::string scriptname(basename);
   scriptname.append(".lua");

   bool compiled = false;

   // load lua script
   Base::SDL_RWopsPtr script =
      m_game->GetResourceManager().GetResourceFile(scriptname.c_str());

   // not found? try binary one
   if (script == NULL)
   {
      scriptname.assign(basename);
      scriptname.append(".lob");
      compiled = true;

      script = m_game->GetResourceManager().GetResourceFile(scriptname.c_str());
   }

   // still not found? exception
   if (script == NULL)
   {
      std::string text("could not load Lua script from uadata: ");
      text.append(scriptname.c_str());
      throw Base::Exception(text.c_str());
   }

   std::string complete_scriptname = m_game->GetSettings().GetString(Base::settingUadataPath);
   complete_scriptname += scriptname.c_str();

   int ret = LoadScript(script, complete_scriptname.c_str());

   UaTrace("loaded Lua %sscript \"%s\"\n",
      compiled ? "compiled " : "", scriptname.c_str());

   // put script name in list, if it's not a compiled one
   if (!compiled)
      m_loadedScriptFiles.push_back(complete_scriptname);

   return ret == 0;
}

void LuaScripting::Done()
{
   // notify debugger of end of Lua script code debugger
   m_game->GetDebugger().EndCodeDebugger(this);

   lua_close(L);
}

void LuaScripting::CheckedCall(int nargs, int nresults)
{
   lua_setcallhook(L, DebugHookCall);
   lua_setlinehook(L, DebugHookLine);

   lua_call(L, nargs, nresults);

   lua_setcallhook(L, NULL);
   lua_setlinehook(L, NULL);
}

void LuaScripting::InitNewGame()
{
   lua_getglobal(L, "game_init_new");
   CheckedCall(0, 0);
}

void LuaScripting::EvalCritter(Uint16 pos)
{
   lua_getglobal(L, "critter_eval");
   lua_pushnumber(L, static_cast<double>(pos));
   CheckedCall(1, 0);
}

void LuaScripting::TriggerSetOff(Uint16 pos)
{
   lua_getglobal(L, "TriggerSetOff");
   lua_pushnumber(L, static_cast<double>(pos));
   CheckedCall(1, 0);
}

void LuaScripting::UserAction(UserInterfaceUserAction action,
   unsigned int param)
{
   switch (action)
   {
   case userActionLookObject:
      lua_getglobal(L, "object_look");
      lua_pushnumber(L, static_cast<double>(param));
      CheckedCall(1, 0);
      break;

   case userActionLookObjectInventory:
      lua_getglobal(L, "inventory_look");
      lua_pushnumber(L, static_cast<double>(param));
      CheckedCall(1, 0);
      break;

   case userActionLookWall:
      lua_getglobal(L, "look_at_wall");
      lua_pushnumber(L, static_cast<double>(param));
      CheckedCall(1, 0);
      break;

   case userActionUseObject:
      lua_getglobal(L, "object_use");
      lua_pushnumber(L, static_cast<double>(param));
      CheckedCall(1, 0);
      break;

   case userActionUseObjectInventory:
      lua_getglobal(L, "inventory_use");
      lua_pushnumber(L, static_cast<double>(param));
      CheckedCall(1, 0);
      break;

   case userActionUseWall:
      lua_getglobal(L, "wall_use");
      lua_pushnumber(L, static_cast<double>(param));
      CheckedCall(1, 0);
      break;

   case userActionGetObject:
      lua_getglobal(L, "object_get");
      lua_pushnumber(L, static_cast<double>(param));
      CheckedCall(1, 0);
      break;

   case userActionTalkObject:
      lua_getglobal(L, "object_talk");
      lua_pushnumber(L, static_cast<double>(param));
      CheckedCall(1, 0);
      break;

   case userActionClickedActiveSpell:
      lua_getglobal(L, "spell_cast");
      CheckedCall(0, 0);
      break;

   case userActionClickedRuneshelf:
      lua_getglobal(L, "spell_cancel");
      lua_pushnumber(L, static_cast<double>(param));
      CheckedCall(1, 0);
      break;

   case userActionClickedCompass:
      lua_getglobal(L, "ui_clicked_compass");
      lua_pushnumber(L, static_cast<double>(param));
      CheckedCall(1, 0);
      break;

   case userActionClickedVitalityFlash:
      lua_getglobal(L, "ui_clicked_vitality_flask");
      lua_pushnumber(L, static_cast<double>(param));
      CheckedCall(1, 0);
      break;

   case userActionClickedManaFlask:
      lua_getglobal(L, "ui_clicked_mana_flask");
      lua_pushnumber(L, static_cast<double>(param));
      CheckedCall(1, 0);
      break;

   case userActionClickedGargoyle:
      lua_getglobal(L, "ui_clicked_gargoyle");
      lua_pushnumber(L, static_cast<double>(param));
      CheckedCall(1, 0);
      break;

   case userActionClickedDragons:
      lua_getglobal(L, "ui_clicked_dragons");
      lua_pushnumber(L, static_cast<double>(param));
      CheckedCall(1, 0);
      break;

      // userActionCombatEnter
      // userActionCombatDrawBack
      // userActionCombatLeave
      // userActionCombatRelease

   case userActionTrackCreatures:
      lua_getglobal(L, "track_creatures");
      CheckedCall(0, 0);
      break;

   case userActionSleep:
      lua_getglobal(L, "sleep");
      CheckedCall(0, 0);
      break;

   case userActionTargetSelected:
      break;

   default:
      break;
   }
}

void LuaScripting::OnChangingLevel()
{
   lua_getglobal(L, "on_change_level");
   CheckedCall(0, 0);
}

ItemCombineStatus LuaScripting::ItemCombine(Uint16 itemId1,
   Uint16 itemId2, Uint16& resultId)
{
   lua_getglobal(L, "item_combine");
   lua_pushnumber(L, static_cast<double>(itemId1));
   lua_pushnumber(L, static_cast<double>(itemId2));
   CheckedCall(2, 2);

   resultId = Underworld::c_itemIDNone;
   return itemCombineFailed;
}

int LuaScripting::LoadScript(Base::SDL_RWopsPtr rwops, const char* chunkname)
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

IScripting* IScripting::CreateScripting(ScriptingLanguage)
{
   return new LuaScripting;
}

LuaScripting& LuaScripting::GetScriptingFromSelf(lua_State* L)
{
   LuaScripting* self = NULL;

   lua_getglobal(L, s_selfName);
   if (!lua_isuserdata(L, -1))
      throw Base::Exception("'self' parameter wasn't set by LuaScripting");

   // get pointer to underworld
   self = reinterpret_cast<LuaScripting*>(lua_touserdata(L, -1));

   if (self == NULL || self->L != L)
      throw Base::Exception("wrong 'self' parameter in LuaScripting");

   lua_pop(L, 1);

   return *self;
}

void LuaScripting::DebugHookCall(lua_State* L, lua_Debug* ar)
{
   lua_getstack(L, 0, ar);
   lua_getinfo(L, "Snlu", ar);

   // don't report C functions
   if (0 == strcmp(ar->source, "=C"))
      return;

   LuaScripting& script = GetScriptingFromSelf(L);
   UaAssert(script.GetLuaState() == L);

   script.DebugHook(ar);
}

void LuaScripting::DebugHookLine(lua_State* L, lua_Debug* ar)
{
   lua_getstack(L, 0, ar);
   lua_getinfo(L, "Snlu", ar);

   LuaScripting& script = GetScriptingFromSelf(L);
   UaAssert(script.GetLuaState() == L);

   script.DebugHook(ar);
}

void LuaScripting::DebugHook(lua_Debug* ar)
{
   UaTrace("debug: event=%s, name=%s, start=%d, line=%d, in=%s\n",
      ar->event, ar->name, ar->linedefined, ar->currentline, ar->source);

   if (ar->source != NULL)
   {
      m_currentPositionSourcefileIndex = unsigned(-1);

      // find out current index
      unsigned int max = m_loadedScriptFiles.size();
      for (unsigned int n = 0; n < max; n++)
         if (m_loadedScriptFiles[n] == ar->source)
         {
            m_currentPositionSourcefileIndex = n;
            break;
         }
   }

   if (ar->currentline > 0)
      m_currentPositionSourcefileLine = static_cast<unsigned int>(ar->currentline);

   DebugServerCodeDebuggerState old_state = GetDebuggerState();

   // check commands
//   if (GetDebuggerState() == codeDebuggerStateRunning)
   {
      switch (GetDebuggerCommand())
      {
      case codeDebuggerCommandRun:
         SetDebuggerState(codeDebuggerStateRunning);
         break;

      case codeDebuggerCommandStepOver:
         if (0 == strcmp(ar->event, "line"))
         {
            if (m_stepOverFunctionCallDepth == 0)
               SetDebuggerState(codeDebuggerStateBreak);
         }
         else
            if (0 == strcmp(ar->event, "call"))
            {
               if (GetDebuggerState() == codeDebuggerStateRunning)
                  m_stepOverFunctionCallDepth++; //
               else
                  m_stepOverFunctionCallDepth = 0; // start of stepping over a function
            }
            else
               if (0 == strcmp(ar->event, "return"))
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
         if (0 == strcmp(ar->event, "line"))
         {
            SetDebuggerState(codeDebuggerStateBreak);
         }
         break;

      case codeDebuggerCommandStepOut:
         if (0 == strcmp(ar->event, "return"))
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
   if (old_state != GetDebuggerState())
      m_game->GetDebugger().SendCodeDebuggerStatusUpdate(m_debuggerId);

   // wait for state to change to "running" again
   WaitDebuggerContinue();
}

void LuaScripting::CheckBreakpoints()
{
}

void LuaScripting::WaitDebuggerContinue()
{
   while (GetDebuggerState() == codeDebuggerStateBreak)
      SDL_Delay(10); // TODO
}

/// registers a C function inside a table
#define lua_register_table(L, n, f) { \
   lua_pushstring(L, n); lua_pushcfunction(L, f); lua_settable(L,-3); }

/*! Functions that are callable via Lua are organized in global tables; this
    way we simulate objects that can be accessed, e.g. in uw.print("text")
*/
void LuaScripting::RegisterFunctions()
{
   // uw object
   lua_newtable(L);
   lua_register_table(L, "print", PrintScroll);
   lua_register_table(L, "get_string", uw_get_string);
   lua_register_table(L, "change_level", uw_change_level);
   lua_register_table(L, "start_conv", uw_start_conv);
   lua_setglobal(L, "uw");

   // player object
   lua_newtable(L);
   lua_register_table(L, "get_info", player_get_info);
   lua_register_table(L, "set_info", player_set_info);
   lua_register_table(L, "get_attr", player_get_attr);
   lua_register_table(L, "set_attribute", player_set_attr);
   lua_register_table(L, "get_skill", player_get_skill);
   lua_register_table(L, "set_skill", player_set_skill);
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
   lua_register_table(L, "get_global", conv_get_global);
   lua_register_table(L, "set_global", conv_set_global);
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

DebugServerCodeDebuggerType LuaScripting::GetDebuggerType()
{
   return codeDebuggerTypeLuaScript;
}

/// Prepares debugging info.
/// The Lua struct Proto contains all informations about a function prototype.
/// It also contains line number info when a source file was loaded. The
/// format of the "lineinfo" array is as follows:
/// The lineinfo array contains "nlineinfo" items, which is always an odd
/// number. The last item contains MAX_INT to signal an end. The list contains
/// pairs of int values. The first one describes a line number and is coded to
/// save space. If the value is negative, it is a relative value to the
/// previous line number. The first line number is 1 (one). If the value is
/// positive, it is not a line number offset and the line number stays the
/// same. The second value is a program counter into the compiled bytecode.
/// It points to the start of the code that is generated from the line
/// described in the first value.
void LuaScripting::PrepareDebugInfo()
{
   // retrieve debug info from lua struct
   // go through list of all function prototype structures
   Proto* proto = L->rootproto;
   while (proto != NULL)
   {
      std::string filename(proto->source->str);

      // find line numbers set
      std::map<std::string, std::set<unsigned int> >::iterator iter =
         m_allLineNumbers.find(filename);
      if (iter == m_allLineNumbers.end())
      {
         // insert new set; we have a new filename
         std::set<unsigned int> emptySet;
         m_allLineNumbers[filename] = emptySet;
         iter = m_allLineNumbers.find(filename);
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

DebugServerCodeDebuggerState LuaScripting::GetDebuggerState() const
{
   // TODO protect access
   return m_debuggerState;
}

void LuaScripting::SetDebuggerState(DebugServerCodeDebuggerState new_state)
{
   // TODO protect access
   m_debuggerState = new_state;
}

DebugServerCodeDebuggerCommand LuaScripting::GetDebuggerCommand() const
{
   // TODO protect access
   return m_debuggerCommand;
}

void LuaScripting::SetDebuggerCommand(DebugServerCodeDebuggerCommand debuggerCommand)
{
   //   if (GetDebuggerState() == codeDebuggerStateRunning)
   //      return; // don't set command when still running

      // TODO protect access
   m_debuggerCommand = debuggerCommand;
   //   m_debuggerState = codeDebuggerStateRunning;
}

void LuaScripting::GetCurrentPos(unsigned int& sourcefileIndex,
   unsigned int& sourcefileLine, unsigned int& codePosition,
   bool& isSourcefileValid)
{
   UNUSED(codePosition);
   sourcefileIndex = m_currentPositionSourcefileIndex;
   sourcefileLine = m_currentPositionSourcefileLine;
   isSourcefileValid = true;
}

unsigned int LuaScripting::GetNumSourcefiles() const
{
   return static_cast<unsigned int>(m_loadedScriptFiles.size());
}

unsigned int LuaScripting::GetSourcefileName(unsigned int index, char* buffer, unsigned int len)
{
   std::string filename(m_loadedScriptFiles[index]);
   if (filename.size() > 0 && filename[0] == '@')
      filename.erase(0, 1); // remove @ char

   size_t size = filename.size();

   if (buffer == NULL || len == 0 || len < size + 1)
      return size + 1;

   strncpy(buffer, filename.c_str(), size);
   buffer[size] = 0;

   return filename.size() + 1;
}

unsigned int LuaScripting::GetNumBreakpoints()
{
   return m_breakpointsList.size();
}

void LuaScripting::GetBreakpointInfo(unsigned int breakpointIndex,
   unsigned int& sourcefileIndex, unsigned int& sourcefileLine,
   unsigned int& codePosition, bool& visible)
{
   UaAssert(breakpointIndex < GetNumBreakpoints());
   if (breakpointIndex >= GetNumBreakpoints())
      return;

   DebugCodeBreakpointInfo& info = m_breakpointsList[breakpointIndex];

   visible = info.visible;
   sourcefileIndex = info.pos.sourcefileIndex;
   sourcefileLine = info.pos.sourcefileLine;
   codePosition = info.pos.codePosition;
}

int LuaScripting::PrintScroll(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);

   IUserInterface* callback = self.m_game->GetUserInterface();
   if (callback != NULL)
   {
      const char* text = lua_tostring(L, -1);
      callback->PrintScroll(text);
   }
   return 0;
}

int LuaScripting::uw_get_string(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);

   unsigned int block = static_cast<unsigned int>(lua_tonumber(L, -2));
   unsigned int num = static_cast<unsigned int>(lua_tonumber(L, -1));

   // retrieve game string
   std::string str(self.m_game->GetGameStrings().GetString(block, num));
   lua_pushstring(L, str.c_str());

   return 1;
}

int LuaScripting::uw_change_level(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);

   unsigned int level = static_cast<unsigned int>(lua_tonumber(L, -1));

   self.m_game->GetGameLogic().ChangeLevel(level);

   return 0;
}

int LuaScripting::uw_start_conv(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);

   IUserInterface* callback = self.m_game->GetUserInterface();
   if (callback != NULL)
   {
      Uint16 conv_obj = static_cast<Uint16>(lua_tonumber(L, -1));
      callback->StartConversation(conv_obj);
   }
   return 0;
}

int LuaScripting::player_get_info(lua_State* L)
{
   // retrieve player object
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::Player& player = self.m_game->GetUnderworld().GetPlayer();

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
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::Player& player = self.m_game->GetUnderworld().GetPlayer();

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
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::Player& player = self.m_game->GetUnderworld().GetPlayer();

   Underworld::PlayerAttribute attr =
      static_cast<Underworld::PlayerAttribute>(static_cast<int>(lua_tonumber(L, -1)));

   lua_pushnumber(L, static_cast<double>(player.GetAttribute(attr)));
   return 1;
}

int LuaScripting::player_set_attr(lua_State* L)
{
   // retrieve player object
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::Player& player = self.m_game->GetUnderworld().GetPlayer();

   Underworld::PlayerAttribute attrType =
      static_cast<Underworld::PlayerAttribute>(static_cast<int>(lua_tonumber(L, -2)));
   unsigned int attrValue = static_cast<unsigned int>(lua_tonumber(L, -1));

   player.SetAttribute(attrType, attrValue);
   return 0;
}

int LuaScripting::player_get_skill(lua_State* L)
{
   // retrieve player object
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::Player& player = self.m_game->GetUnderworld().GetPlayer();

   Underworld::PlayerSkill skillType =
      static_cast<Underworld::PlayerSkill>(static_cast<int>(lua_tonumber(L, -1)));

   lua_pushnumber(L, static_cast<double>(player.GetSkill(skillType)));
   return 1;
}

int LuaScripting::player_set_skill(lua_State* L)
{
   // retrieve player object
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::Player& player = self.m_game->GetUnderworld().GetPlayer();

   Underworld::PlayerSkill skillType =
      static_cast<Underworld::PlayerSkill>(static_cast<int>(lua_tonumber(L, -2)));
   unsigned int skill_value = static_cast<unsigned int>(lua_tonumber(L, -1));

   player.SetSkill(skillType, skill_value);
   return 0;
}

int LuaScripting::objlist_get_info(lua_State* L)
{
   // retrieve object list
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::ObjectList& objectList =
      self.m_game->GetGameLogic().GetCurrentLevel().GetObjectList();

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
      lua_pushnumber(L, static_cast<double>(npcInfo.m_npc_hp));
      lua_settable(L, -3);

      lua_pushstring(L, "npc_goal");
      lua_pushnumber(L, static_cast<double>(npcInfo.m_npc_goal));
      lua_settable(L, -3);

      lua_pushstring(L, "npc_gtarg");
      lua_pushnumber(L, static_cast<double>(npcInfo.m_npc_gtarg));
      lua_settable(L, -3);

      lua_pushstring(L, "npc_level");
      lua_pushnumber(L, static_cast<double>(npcInfo.m_npc_level));
      lua_settable(L, -3);

      lua_pushstring(L, "npc_talkedto");
      lua_pushnumber(L, npcInfo.m_npc_talkedto ? 1.0 : 0.0);
      lua_settable(L, -3);

      lua_pushstring(L, "npc_attitude");
      lua_pushnumber(L, static_cast<double>(npcInfo.m_npc_attitude));
      lua_settable(L, -3);

      lua_pushstring(L, "npc_xhome");
      lua_pushnumber(L, static_cast<double>(npcInfo.m_npc_xhome));
      lua_settable(L, -3);

      lua_pushstring(L, "npc_yhome");
      lua_pushnumber(L, static_cast<double>(npcInfo.m_npc_yhome));
      lua_settable(L, -3);

      lua_pushstring(L, "npc_hunger");
      lua_pushnumber(L, static_cast<double>(npcInfo.m_npc_hunger));
      lua_settable(L, -3);

      lua_pushstring(L, "npc_whoami");
      lua_pushnumber(L, static_cast<double>(npcInfo.m_npc_whoami));
      lua_settable(L, -3);
   }

   // leave table on stack and return it
   return 1;
}

/// \todo implement
int LuaScripting::objlist_set_info(lua_State* L)
{
   UNUSED(L);
   return 0;
}

int LuaScripting::objlist_delete(lua_State* L)
{
   // retrieve object list
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::ObjectList& objectList =
      self.m_game->GetGameLogic().GetCurrentLevel().GetObjectList();

   // get object pos and object
   Uint16 objectPos = static_cast<Uint16>(lua_tonumber(L, -1));

   objectList.Free(objectPos);

   return 0;
}

/*! \todo implement */
int LuaScripting::objlist_insert(lua_State* L)
{
   // retrieve object list
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::ObjectList& objectList =
      self.m_game->GetGameLogic().GetCurrentLevel().GetObjectList();
   objectList;

   return 0;//1;
}

int LuaScripting::tilemap_get_info(lua_State* L)
{
   // retrieve current level
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::Level& level = self.m_game->GetGameLogic().GetCurrentLevel();

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
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::Level& level = self.m_game->GetGameLogic().GetCurrentLevel();
   level;

   return 0;
}

int LuaScripting::tilemap_get_floor_height(lua_State* L)
{
   // retrieve current level
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::Level& level = self.m_game->GetGameLogic().GetCurrentLevel();

   double xpos = lua_tonumber(L, -2);
   double ypos = lua_tonumber(L, -1);

   lua_pushnumber(L, level.GetTilemap().GetFloorHeight(xpos, ypos));
   return 1;
}

int LuaScripting::tilemap_get_objlist_link(lua_State* L)
{
   // retrieve current level
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::Level& level = self.m_game->GetGameLogic().GetCurrentLevel();

   double xpos = lua_tonumber(L, -2);
   double ypos = lua_tonumber(L, -1);
   Uint16 objpos = level.GetObjectList().GetListStart(unsigned(xpos), unsigned(ypos));

   lua_pushnumber(L, static_cast<double>(objpos));
   return 1;
}

int LuaScripting::runes_set(lua_State* L)
{
   // retrieve runes object
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::Runebag& runeBag = self.m_game->GetUnderworld().GetPlayer().GetRunebag();

   unsigned int rune = static_cast<unsigned int>(lua_tonumber(L, -2));
   bool flag = lua_tonumber(L, -1) > 0.0;

   runeBag.SetRune(static_cast<Underworld::RuneType>(rune), flag);

   return 0;
}

int LuaScripting::runes_test(lua_State* L)
{
   // retrieve runes object
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::Runebag& runeBag = self.m_game->GetUnderworld().GetPlayer().GetRunebag();

   unsigned int rune = static_cast<unsigned int>(lua_tonumber(L, -1));

   lua_pushnumber(L, runeBag.IsInBag(static_cast<Underworld::RuneType>(rune)) ? 1.0 : 0.0);

   return 1;
}

int LuaScripting::conv_is_avail(lua_State* L)
{
   // retrieve game strings object
   LuaScripting& self = GetScriptingFromSelf(L);
   GameStrings& gameStrings = self.m_game->GetGameStrings();

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
   LuaScripting& self = GetScriptingFromSelf(L);

   Uint16 slot = static_cast<Uint16>(lua_tonumber(L, -1));
   unsigned int pos = static_cast<unsigned int>(lua_tonumber(L, -2));

   std::vector<Uint16>& globals =
      self.m_game->GetUnderworld().GetPlayer().GetConvGlobals().GetSlotGlobals(slot);

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
   LuaScripting& self = GetScriptingFromSelf(L);

   Uint16 slot = static_cast<Uint16>(lua_tonumber(L, -1));
   unsigned int pos = static_cast<unsigned int>(lua_tonumber(L, -2));
   Uint16 value = static_cast<Uint16>(lua_tonumber(L, -2));

   std::vector<Uint16>& globals =
      self.m_game->GetUnderworld().GetPlayer().GetConvGlobals().GetSlotGlobals(slot);

   if (pos >= globals.size())
      UaTrace("conv.SetGlobal: globals pos out of range!\n");
   else
      globals[pos] = value;

   return 0;
}

int LuaScripting::quest_get_flag(lua_State* L)
{
   // retrieve quest flags object
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::QuestFlags& questFlags =
      self.m_game->GetUnderworld().GetPlayer().GetQuestFlags();

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
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::QuestFlags& questFlags =
      self.m_game->GetUnderworld().GetPlayer().GetQuestFlags();

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
   LuaScripting& self = GetScriptingFromSelf(L);

   Uint16 item_id = static_cast<Uint16>(lua_tonumber(L, -1));

   const Underworld::CommonObjectProperty& prop = self.m_game->GetGameLogic().
      GetObjectProperties().GetCommonProperty(item_id);

   // create new table and fill it with infos
   lua_newtable(L);

   lua_pushstring(L, "item_id");
   lua_pushnumber(L, static_cast<double>(item_id));
   lua_settable(L, -3);

   lua_pushstring(L, "height");
   lua_pushnumber(L, static_cast<double>(prop.m_height));
   lua_settable(L, -3);

   lua_pushstring(L, "mass");
   lua_pushnumber(L, static_cast<double>(prop.m_mass));
   lua_settable(L, -3);

   lua_pushstring(L, "radius");
   lua_pushnumber(L, static_cast<double>(prop.m_radius));
   lua_settable(L, -3);

   lua_pushstring(L, "quality_class");
   lua_pushnumber(L, static_cast<double>(prop.m_qualityClass));
   lua_settable(L, -3);

   lua_pushstring(L, "quality_type");
   lua_pushnumber(L, static_cast<double>(prop.m_qualityType));
   lua_settable(L, -3);

   lua_pushstring(L, "can_have_owner");
   lua_pushnumber(L, prop.m_canHaveOwner ? 1.0 : 0.0);
   lua_settable(L, -3);

   lua_pushstring(L, "can_be_looked_at");
   lua_pushnumber(L, prop.m_canBeLookedAt ? 1.0 : 0.0);
   lua_settable(L, -3);

   lua_pushstring(L, "can_be_picked_up");
   lua_pushnumber(L, prop.m_canBePickedUp ? 1.0 : 0.0);
   lua_settable(L, -3);

   lua_pushstring(L, "is_container");
   lua_pushnumber(L, prop.m_isContainer ? 1.0 : 0.0);
   lua_settable(L, -3);

   return 1;
}

/*! \todo implement */
int LuaScripting::prop_get_special(lua_State* L)
{
   // retrieve common object properties
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::ObjectProperties& prop =
      self.m_game->GetGameLogic().GetObjectProperties();
   prop;

   return 0;//1;
}
