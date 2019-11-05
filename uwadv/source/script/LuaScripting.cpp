//
// Underworld Adventures - an Ultima Underworld remake project
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
#include "pch.hpp"
#include "LuaScripting.hpp"
#include "GameInterface.hpp"
#include "IUserInterface.hpp"
#include "GameStrings.hpp"
#include "uwadv/DebugServer.hpp"
#include "ResourceManager.hpp"

extern "C"
{
#include "lua.h"
}

const char* LuaScripting::s_selfName = "_scripting_self";

LuaScripting::LuaScripting()
   :m_game(NULL)
{
}

void LuaScripting::Init(IBasicGame* game)
{
   m_game = game;

   // set "self" pointer userdata
   lua_pushlightuserdata(L, this);
   lua_setglobal(L, s_selfName);

   RegisterFunctions();

   LuaCodeDebugger::Init(m_game->GetDebugger());
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

   int ret = LuaCodeDebugger::LoadScript(script, complete_scriptname.c_str());

   UaTrace("loaded Lua %sscript \"%s\"\n",
      compiled ? "compiled " : "", scriptname.c_str());

   return ret == 0;
}

void LuaScripting::Done()
{
   // notify debugger of end of Lua script code debugger
   m_game->GetDebugger().EndCodeDebugger(this);
}

void LuaScripting::InitNewGame()
{
   lua_getglobal(L, "game_init_new");
   CheckedCall(0, 0);
}

void LuaScripting::EvalCritter(Uint16 pos)
{
   lua_getglobal(L, "critter_eval");
   lua_pushinteger(L, pos);
   CheckedCall(1, 0);
}

void LuaScripting::TriggerSetOff(Uint16 pos)
{
   lua_getglobal(L, "trigger_set_off");
   lua_pushinteger(L, pos);
   CheckedCall(1, 0);
}

void LuaScripting::UserAction(UserInterfaceUserAction action,
   unsigned int param)
{
   switch (action)
   {
   case userActionLookObject:
      lua_getglobal(L, "object_look");
      lua_pushinteger(L, param);
      CheckedCall(1, 0);
      break;

   case userActionLookObjectInventory:
      lua_getglobal(L, "inventory_look");
      lua_pushinteger(L, param);
      CheckedCall(1, 0);
      break;

   case userActionLookWall:
      lua_getglobal(L, "look_at_wall");
      lua_pushinteger(L, param);
      CheckedCall(1, 0);
      break;

   case userActionUseObject:
      lua_getglobal(L, "object_use");
      lua_pushinteger(L, param);
      CheckedCall(1, 0);
      break;

   case userActionUseObjectInventory:
      lua_getglobal(L, "inventory_use");
      lua_pushinteger(L, param);
      CheckedCall(1, 0);
      break;

   case userActionUseWall:
      lua_getglobal(L, "wall_use");
      lua_pushinteger(L, param);
      CheckedCall(1, 0);
      break;

   case userActionGetObject:
      lua_getglobal(L, "object_get");
      lua_pushinteger(L, param);
      CheckedCall(1, 0);
      break;

   case userActionTalkObject:
      lua_getglobal(L, "object_talk");
      lua_pushinteger(L, param);
      CheckedCall(1, 0);
      break;

   case userActionClickedActiveSpell:
      lua_getglobal(L, "spell_cast");
      CheckedCall(0, 0);
      break;

   case userActionClickedRuneshelf:
      lua_getglobal(L, "spell_cancel");
      lua_pushinteger(L, param);
      CheckedCall(1, 0);
      break;

   case userActionClickedCompass:
      lua_getglobal(L, "ui_clicked_compass");
      CheckedCall(0, 0);
      break;

   case userActionClickedVitalityFlash:
      lua_getglobal(L, "ui_clicked_vitality_flask");
      CheckedCall(0, 0);
      break;

   case userActionClickedManaFlask:
      lua_getglobal(L, "ui_clicked_mana_flask");
      CheckedCall(0, 0);
      break;

   case userActionClickedGargoyle:
      lua_getglobal(L, "ui_clicked_gargoyle");
      CheckedCall(0, 0);
      break;

   case userActionClickedDragons:
      lua_getglobal(L, "ui_clicked_dragons");
      CheckedCall(0, 0);
      break;

      // TODO userActionCombatEnter
      // userActionCombatDrawBack
      // userActionCombatLeave
      // userActionCombatRelease

   case userActionTrackCreatures:
      lua_getglobal(L, "track_creatures");
      CheckedCall(0, 0);
      break;

   case userActionSleep:
      lua_getglobal(L, "sleep");
      lua_pushboolean(L, param != 0);
      CheckedCall(1, 1);
      break;

   case userActionTargetSelected:
      lua_getglobal(L, "target_selected");
      lua_pushinteger(L, param);
      CheckedCall(1, 0);
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

IScripting* IScripting::CreateScripting(ScriptingLanguage)
{
   return new LuaScripting;
}

LuaScripting& LuaScripting::GetScriptingFromSelf(lua_State* L)
{
   LuaScripting* self = NULL;

   lua_getglobal(L, s_selfName);
   if (!lua_islightuserdata(L, -1))
      throw Base::Exception("'self' parameter wasn't set by LuaScripting");

   // get pointer to underworld
   self = reinterpret_cast<LuaScripting*>(lua_touserdata(L, -1));

   if (self == NULL || self->L != L)
      throw Base::Exception("wrong 'self' parameter in LuaScripting");

   lua_pop(L, 1);

   return *self;
}

/// registers a C function inside a table
#define lua_register_table(L, n, f) { \
   lua_pushstring(L, n); lua_pushcfunction(L, f); lua_settable(L,-3); }

/// Functions that are callable via Lua are organized in global tables; this
/// way we simulate objects that can be accessed, e.g. in uw.print("text")
void LuaScripting::RegisterFunctions()
{
   // uw object
   lua_newtable(L);
   lua_register_table(L, "print", uw_print);
   lua_register_table(L, "get_string", uw_get_string);
   lua_register_table(L, "change_level", uw_change_level);
   lua_register_table(L, "start_conv", uw_start_conv);
   lua_register_table(L, "show_cutscene", uw_show_cutscene);
   lua_setglobal(L, "uw");

   // player object
   lua_newtable(L);
   lua_register_table(L, "get_info", player_get_info);
   lua_register_table(L, "set_info", player_set_info);
   lua_register_table(L, "get_attr", player_get_attr);
   lua_register_table(L, "set_attr", player_set_attr);
   lua_register_table(L, "get_skill", player_get_skill);
   lua_register_table(L, "set_skill", player_set_skill);
   lua_setglobal(L, "player");

   // objectList object
   lua_newtable(L);
   lua_register_table(L, "get_info", objectlist_get_info);
   lua_register_table(L, "set_info", objectlist_set_info);
   lua_register_table(L, "delete", objectlist_delete);
   lua_register_table(L, "insert", objectlist_insert);
   lua_setglobal(L, "objectlist");

   // inventory object
   lua_newtable(L);
   lua_register_table(L, "get_info", inventory_get_info);
   lua_register_table(L, "get_floating_item", inventory_get_floating_item);
   lua_register_table(L, "float_add_item", inventory_float_add_item);
   lua_setglobal(L, "inventory");

   // tilemap object
   lua_newtable(L);
   lua_register_table(L, "get_info", tilemap_get_info);
   lua_register_table(L, "set_info", tilemap_set_info);
   lua_register_table(L, "get_floor_height", tilemap_get_floor_height);
   lua_register_table(L, "get_objlist_link", tilemap_get_object_list_link);
   lua_setglobal(L, "tilemap");

   // runes object
   lua_newtable(L);
   lua_register_table(L, "set", runebag_set);
   lua_register_table(L, "test", runebag_test);
   lua_setglobal(L, "runebag");

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


int LuaScripting::uw_print(lua_State* L)
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

   Uint16 block = static_cast<Uint16>(lua_tointeger(L, -2));
   size_t number = static_cast<size_t>(lua_tointeger(L, -1));

   // retrieve game string
   std::string text{ self.m_game->GetGameStrings().GetString(block, number) };
   lua_pushstring(L, text.c_str());

   return 1;
}

int LuaScripting::uw_change_level(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);

   size_t level = static_cast<size_t>(lua_tointeger(L, -1));

   self.m_game->GetGameLogic().ChangeLevel(level);

   return 0;
}

int LuaScripting::uw_start_conv(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);

   IUserInterface* callback = self.m_game->GetUserInterface();
   if (callback != NULL)
   {
      Uint16 conversationObjectPos = static_cast<Uint16>(lua_tointeger(L, -1));
      callback->StartConversation(conversationObjectPos);
   }

   return 0;
}

int LuaScripting::uw_show_cutscene(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);

   IUserInterface* callback = self.m_game->GetUserInterface();
   if (callback != NULL)
   {
      size_t cutsceneNumber = static_cast<size_t>(lua_tointeger(L, -1));
      // TODO
      //callback->ShowCutscene(cutsceneNumber);
   }

   return 0;
}

int LuaScripting::uw_show_ingame_anim(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);

   IUserInterface* callback = self.m_game->GetUserInterface();
   if (callback != NULL)
   {
      unsigned int ingameAnimationNumber = static_cast<unsigned int>(lua_tointeger(L, -1));
      callback->Notify(notifyUpdateShowIngameAnimation, ingameAnimationNumber);
   }

   return 0;
}

int LuaScripting::uw_cursor_use_item(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);

   IUserInterface* callback = self.m_game->GetUserInterface();
   if (callback != NULL)
   {
      unsigned int ingameAnimationNumber = static_cast<unsigned int>(lua_tointeger(L, -1));
      // TODO implement
   }

   return 0;
}

int LuaScripting::uw_cursor_target(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);

   IUserInterface* callback = self.m_game->GetUserInterface();
   if (callback != NULL)
   {
      // TODO implement
   }

   return 0;
}

int LuaScripting::player_get_info(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);
   const Underworld::Player& player = self.m_game->GetUnderworld().GetPlayer();

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
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::Player& player = self.m_game->GetUnderworld().GetPlayer();

   if (!lua_istable(L, -1))
   {
      UaTrace("player.set_info: wrong number of parameters\n");
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
   LuaScripting& self = GetScriptingFromSelf(L);
   const Underworld::Player& player = self.m_game->GetUnderworld().GetPlayer();

   Underworld::PlayerAttribute attr =
      static_cast<Underworld::PlayerAttribute>(lua_tointeger(L, -1));

   lua_pushinteger(L, static_cast<lua_Integer>(player.GetAttribute(attr)));
   return 1;
}

int LuaScripting::player_set_attr(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::Player& player = self.m_game->GetUnderworld().GetPlayer();

   Underworld::PlayerAttribute attrType =
      static_cast<Underworld::PlayerAttribute>(lua_tointeger(L, -2));
   Uint16 attrValue = static_cast<Uint16>(lua_tointeger(L, -1));

   player.SetAttribute(attrType, attrValue);
   return 0;
}

int LuaScripting::player_get_skill(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);
   const Underworld::Player& player = self.m_game->GetUnderworld().GetPlayer();

   Underworld::PlayerSkill skillType =
      static_cast<Underworld::PlayerSkill>(lua_tointeger(L, -1));

   lua_pushinteger(L, player.GetSkill(skillType));
   return 1;
}

int LuaScripting::player_set_skill(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::Player& player = self.m_game->GetUnderworld().GetPlayer();

   Underworld::PlayerSkill skillType =
      static_cast<Underworld::PlayerSkill>(lua_tointeger(L, -2));
   Uint16 skillValue = static_cast<Uint16>(lua_tointeger(L, -1));

   player.SetSkill(skillType, skillValue);
   return 0;
}

int LuaScripting::objectlist_get_info(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);
   const Underworld::ObjectList& objectList =
      self.m_game->GetGameLogic().GetCurrentLevel().GetObjectList();

   // get object pos and object
   Uint16 objectPos = static_cast<Uint16>(lua_tointeger(L, -1));

   const Underworld::ObjectPtr obj = objectList.GetObject(objectPos);
   const Underworld::ObjectInfo& info = obj->GetObjectInfo();
   const Underworld::ObjectPositionInfo& posInfo = obj->GetPosInfo();

   // create new table and fill it with infos
   lua_newtable(L);

   lua_pushstring(L, "objectlist_pos");
   lua_pushinteger(L, objectPos);
   lua_settable(L, -3);

   AddObjectInfoTableFields(L, info);

   lua_pushstring(L, "xpos");
   lua_pushinteger(L, posInfo.m_xpos);
   lua_settable(L, -3);

   lua_pushstring(L, "ypos");
   lua_pushinteger(L, posInfo.m_ypos);
   lua_settable(L, -3);

   lua_pushstring(L, "zpos");
   lua_pushinteger(L, posInfo.m_zpos);
   lua_settable(L, -3);

   lua_pushstring(L, "heading");
   lua_pushinteger(L, posInfo.m_heading);
   lua_settable(L, -3);

   lua_pushstring(L, "tilex");
   lua_pushinteger(L, posInfo.m_tileX);
   lua_settable(L, -3);

   lua_pushstring(L, "tiley");
   lua_pushinteger(L, posInfo.m_tileY);
   lua_settable(L, -3);

   bool isNpc = obj->IsNpcObject();

   lua_pushstring(L, "is_npc");
   lua_pushboolean(L, isNpc);
   lua_settable(L, -3);

   // add npc infos
   if (isNpc)
   {
      const Underworld::NpcObject& npc = obj->GetNpcObject();
      const Underworld::NpcInfo& npcInfo = npc.GetNpcInfo();

      lua_pushstring(L, "npc_hp");
      lua_pushinteger(L, npcInfo.m_npc_hp);
      lua_settable(L, -3);

      lua_pushstring(L, "npc_goal");
      lua_pushinteger(L, npcInfo.m_npc_goal);
      lua_settable(L, -3);

      lua_pushstring(L, "npc_gtarg");
      lua_pushinteger(L, npcInfo.m_npc_gtarg);
      lua_settable(L, -3);

      lua_pushstring(L, "npc_level");
      lua_pushinteger(L, npcInfo.m_npc_level);
      lua_settable(L, -3);

      lua_pushstring(L, "npc_talkedto");
      lua_pushboolean(L, npcInfo.m_npc_talkedto);
      lua_settable(L, -3);

      lua_pushstring(L, "npc_attitude");
      lua_pushinteger(L, npcInfo.m_npc_attitude);
      lua_settable(L, -3);

      lua_pushstring(L, "npc_xhome");
      lua_pushinteger(L, npcInfo.m_npc_xhome);
      lua_settable(L, -3);

      lua_pushstring(L, "npc_yhome");
      lua_pushinteger(L, npcInfo.m_npc_yhome);
      lua_settable(L, -3);

      lua_pushstring(L, "npc_hunger");
      lua_pushinteger(L, npcInfo.m_npc_hunger);
      lua_settable(L, -3);

      lua_pushstring(L, "npc_whoami");
      lua_pushinteger(L, npcInfo.m_npc_whoami);
      lua_settable(L, -3);
   }

   // leave table on stack and return it
   return 1;
}

void LuaScripting::AddObjectInfoTableFields(lua_State* L, const Underworld::ObjectInfo& info)
{
   lua_pushstring(L, "item_id");
   lua_pushinteger(L, info.m_itemID);
   lua_settable(L, -3);

   lua_pushstring(L, "link");
   lua_pushinteger(L, info.m_link);
   lua_settable(L, -3);

   lua_pushstring(L, "quality");
   lua_pushinteger(L, info.m_quality);
   lua_settable(L, -3);

   lua_pushstring(L, "owner");
   lua_pushinteger(L, info.m_owner);
   lua_settable(L, -3);

   lua_pushstring(L, "quantity");
   lua_pushinteger(L, info.m_quantity);
   lua_settable(L, -3);

   lua_pushstring(L, "is_enchanted");
   lua_pushboolean(L, info.m_isEnchanted);
   lua_settable(L, -3);

   lua_pushstring(L, "is_quantity");
   lua_pushboolean(L, info.m_isQuantity);
   lua_settable(L, -3);

   lua_pushstring(L, "is_hidden");
   lua_pushboolean(L, info.m_isHidden);
   lua_settable(L, -3);

   lua_pushstring(L, "flags");
   lua_pushinteger(L, info.m_flags);
   lua_settable(L, -3);
}

/// \todo implement
int LuaScripting::objectlist_set_info(lua_State* L)
{
   UNUSED(L);
   return 0;
}

int LuaScripting::objectlist_delete(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::ObjectList& objectList =
      self.m_game->GetGameLogic().GetCurrentLevel().GetObjectList();

   Uint16 objectPos = static_cast<Uint16>(lua_tonumber(L, -1));

   objectList.Free(objectPos);

   return 0;
}

/*! \todo implement */
int LuaScripting::objectlist_insert(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::ObjectList& objectList =
      self.m_game->GetGameLogic().GetCurrentLevel().GetObjectList();
   UNUSED(objectList);

   return 0;//1;
}

int LuaScripting::inventory_get_info(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);
   const Underworld::Inventory& inventory =
      self.m_game->GetGameLogic().GetUnderworld().GetPlayer().GetInventory();

   Uint16 inventoryPos = static_cast<Uint16>(lua_tointeger(L, -1));

   if (inventoryPos != Underworld::c_inventorySlotNoItem)
   {
      const Underworld::ObjectInfo& info = inventory.GetObjectInfo(inventoryPos);

      if (info.m_itemID != Underworld::c_itemIDNone)
      {
         // create new table and fill it with infos
         lua_newtable(L);

         lua_pushstring(L, "inventory_pos");
         lua_pushinteger(L, inventoryPos);
         lua_settable(L, -3);

         AddObjectInfoTableFields(L, info);

         // leave table on stack and return it
         return 1;
      }
   }

   lua_pushnil(L);
   return 1;
}

int LuaScripting::inventory_get_floating_item(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);
   const Underworld::Inventory& inventory =
      self.m_game->GetGameLogic().GetUnderworld().GetPlayer().GetInventory();

   Uint16 inventoryPos = inventory.GetFloatingObjectPos();

   lua_pushinteger(L, inventoryPos);

   return 1;
}

int LuaScripting::inventory_float_add_item(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::Inventory& inventory =
      self.m_game->GetGameLogic().GetUnderworld().GetPlayer().GetInventory();

   Underworld::ObjectList& objectList =
      self.m_game->GetGameLogic().GetCurrentLevel().GetObjectList();

   Uint16 objectListPos = static_cast<Uint16>(lua_tointeger(L, -1));

   Underworld::ObjectPtr obj = objectList.GetObject(objectListPos);
   const Underworld::ObjectInfo& info = obj->GetObjectInfo();
   const Underworld::ObjectPositionInfo& posInfo = obj->GetPosInfo();

   Uint16 inventoryPos = inventory.InsertItem(info);
   inventory.FloatObject(inventoryPos);

   objectList.RemoveObjectFromTileList(objectListPos, posInfo.m_tileX, posInfo.m_tileY);
   objectList.Free(objectListPos);

   lua_pushinteger(L, static_cast<double>(inventoryPos));

   // param is the item ID
   self.m_game->GetUserInterface()->Notify(notifySelectTarget, info.m_itemID);

   return 1;
}

int LuaScripting::tilemap_get_info(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);
   const Underworld::Level& level = self.m_game->GetGameLogic().GetCurrentLevel();

   unsigned int xpos = static_cast<unsigned int>(lua_tointeger(L, -2));
   unsigned int ypos = static_cast<unsigned int>(lua_tointeger(L, -1));

   const Underworld::TileInfo& tileinfo = level.GetTilemap().GetTileInfo(xpos, ypos);

   // create new table and fill it with infos
   lua_newtable(L);

   lua_pushstring(L, "xpos");
   lua_pushinteger(L, xpos);
   lua_settable(L, -3);

   lua_pushstring(L, "ypos");
   lua_pushinteger(L, ypos);
   lua_settable(L, -3);

   lua_pushstring(L, "type");
   lua_pushinteger(L, static_cast<double>(tileinfo.m_type));
   lua_settable(L, -3);

   lua_pushstring(L, "floor");
   lua_pushinteger(L, static_cast<double>(tileinfo.m_floor));
   lua_settable(L, -3);

   lua_pushstring(L, "ceiling");
   lua_pushinteger(L, static_cast<double>(tileinfo.m_ceiling));
   lua_settable(L, -3);

   lua_pushstring(L, "slope");
   lua_pushinteger(L, static_cast<double>(tileinfo.m_slope));
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
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::Level& level = self.m_game->GetGameLogic().GetCurrentLevel();
   UNUSED(level);

   return 0;
}

int LuaScripting::tilemap_get_floor_height(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);
   const Underworld::Level& level = self.m_game->GetGameLogic().GetCurrentLevel();

   double xpos = lua_tonumber(L, -2);
   double ypos = lua_tonumber(L, -1);

   lua_pushnumber(L, level.GetTilemap().GetFloorHeight(xpos, ypos));
   return 1;
}

int LuaScripting::tilemap_get_object_list_link(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);
   const Underworld::Level& level = self.m_game->GetGameLogic().GetCurrentLevel();

   Uint8 xpos = static_cast<Uint8>(lua_tointeger(L, -2));
   Uint8 ypos = static_cast<Uint8>(lua_tointeger(L, -1));

   Uint16 objectPos = level.GetObjectList().GetListStart(xpos, ypos);

   lua_pushinteger(L, objectPos);
   return 1;
}

/// allowed values are lowercase and uppercase characters of runes, or 1-based indices of runes
Underworld::RuneType LuaScripting::GetRuneTypeFromInteger(unsigned int rune)
{
   if (rune >= 'a' && rune < 'x')
      rune -= 'a' - 1;
   if (rune == 'y')
      rune -= 'a' - 2;
   if (rune >= 'A' && rune < 'X')
      rune -= 'A' - 1;
   if (rune == 'Y')
      rune -= 'a' - 2;

   return static_cast<Underworld::RuneType>(rune - 1);
}

int LuaScripting::runebag_set(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::Runebag& runeBag = self.m_game->GetUnderworld().GetPlayer().GetRunebag();

   unsigned int rune = static_cast<unsigned int>(lua_tointeger(L, -2));
   Underworld::RuneType runeType = GetRuneTypeFromInteger(rune);

   if (runeType > Underworld::runeLast)
   {
      UaTrace("runebag.set: invalid rune number %i\n", rune);
      return 0;
   }

   bool isSet = lua_toboolean(L, -1);

   runeBag.SetRune(runeType, isSet);

   return 0;
}

int LuaScripting::runebag_test(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);
   const Underworld::Runebag& runeBag = self.m_game->GetUnderworld().GetPlayer().GetRunebag();

   unsigned int rune = static_cast<unsigned int>(lua_tointeger(L, -1));
   Underworld::RuneType runeType = GetRuneTypeFromInteger(rune);

   if (runeType > Underworld::runeLast)
   {
      UaTrace("runebag.test: invalid rune number %i\n", rune);
      return 0;
   }

   lua_pushboolean(L, runeBag.IsInBag(runeType));

   return 1;
}

int LuaScripting::conv_is_avail(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);
   const GameStrings& gameStrings = self.m_game->GetGameStrings();

   Uint16 conversationSlot = static_cast<Uint16>(lua_tointeger(L, -1));

   lua_pushboolean(L, gameStrings.IsBlockAvail(conversationSlot + 0x0e00));

   return 1;
}

int LuaScripting::conv_get_global(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);

   Uint16 conversationSlot = static_cast<Uint16>(lua_tointeger(L, -2));
   unsigned int globalsPos = static_cast<unsigned int>(lua_tointeger(L, -1));

   const Underworld::ConvGlobals::ConvSlotGlobals& globals =
      self.m_game->GetUnderworld().GetPlayer().GetConvGlobals().GetSlotGlobals(conversationSlot);

   if (globalsPos >= globals.size())
   {
      UaTrace("conv.get_global: globals_pos out of range!\n");
      return 0;
   }

   lua_pushinteger(L, static_cast<double>(globals[globalsPos]));
   return 1;
}

int LuaScripting::conv_set_global(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);

   Uint16 conversationSlot = static_cast<Uint16>(lua_tointeger(L, -3));
   unsigned int globalsPos = static_cast<unsigned int>(lua_tointeger(L, -2));
   Uint16 value = static_cast<Uint16>(lua_tointeger(L, -1));

   Underworld::ConvGlobals::ConvSlotGlobals& globals =
      self.m_game->GetUnderworld().GetPlayer().GetConvGlobals().GetSlotGlobals(conversationSlot);

   if (globalsPos >= globals.size())
      UaTrace("conv.set_global: globals_pos out of range!\n");
   else
      globals[globalsPos] = value;

   return 0;
}

int LuaScripting::quest_get_flag(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);
   const Underworld::QuestFlags& questFlags =
      self.m_game->GetUnderworld().GetPlayer().GetQuestFlags();

   Uint16 flagNumber = static_cast<Uint16>(lua_tointeger(L, -1));

   if (flagNumber >= questFlags.GetFlagCount())
   {
      UaTrace("quest.get_flag: flag_number out of range!\n");
      return 0;
   }

   lua_pushinteger(L, questFlags.GetFlag(flagNumber));

   return 1;
}

int LuaScripting::quest_set_flag(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);
   Underworld::QuestFlags& questFlags =
      self.m_game->GetUnderworld().GetPlayer().GetQuestFlags();

   size_t flagNumber = static_cast<size_t>(lua_tointeger(L, -2));
   Uint16 flagValue = static_cast<Uint16>(lua_tointeger(L, -1));

   if (flagNumber >= questFlags.GetFlagCount())
      UaTrace("quest.set_flag: flag_number out of range!\n");
   else
      questFlags.SetFlag(flagNumber, flagValue);

   return 0;
}

int LuaScripting::prop_get_common(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);

   Uint16 item_id = static_cast<Uint16>(lua_tointeger(L, -1));

   const Underworld::CommonObjectProperty& prop = self.m_game->GetGameLogic().
      GetObjectProperties().GetCommonProperty(item_id);

   // create new table and fill it with infos
   lua_newtable(L);

   lua_pushstring(L, "item_id");
   lua_pushinteger(L, item_id);
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
   lua_pushinteger(L, prop.m_qualityType);
   lua_settable(L, -3);

   lua_pushstring(L, "can_have_owner");
   lua_pushboolean(L, prop.m_canHaveOwner);
   lua_settable(L, -3);

   lua_pushstring(L, "can_be_looked_at");
   lua_pushboolean(L, prop.m_canBeLookedAt);
   lua_settable(L, -3);

   lua_pushstring(L, "can_be_picked_up");
   lua_pushboolean(L, prop.m_canBePickedUp);
   lua_settable(L, -3);

   lua_pushstring(L, "is_container");
   lua_pushboolean(L, prop.m_isContainer);
   lua_settable(L, -3);

   return 1;
}

int LuaScripting::prop_get_special(lua_State* L)
{
   LuaScripting& self = GetScriptingFromSelf(L);
   const Underworld::ObjectProperties& prop =
      self.m_game->GetGameLogic().GetObjectProperties();
   UNUSED(prop);

   // TODO implement

   return 0;//1;
}
