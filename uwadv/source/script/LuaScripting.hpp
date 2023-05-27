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
/// \file LuaScripting.hpp
/// \brief Lua scripting support
//
#pragma once

#include "Base.hpp"
#include "LuaCodeDebugger.hpp"
#include "IScripting.hpp"

class IGame;

namespace Base
{
   class Settings;
   class ResourceManager;
}

/// lua scripting class
class LuaScripting : public LuaCodeDebugger, public IScripting
{
public:
   /// ctor
   LuaScripting();
   virtual ~LuaScripting() {}

   /// loads a script from resource manager
   static bool LoadScript(LuaState& lua, const Base::Settings& settings, const Base::ResourceManager& resourceManager, const char* basename);

   // virtual methods from IScripting
   virtual void Init(IGameInstance* game) override;
   virtual bool LoadScript(const char* basename) override;
   virtual void Done() override;
   virtual void InitNewGame() override;
   virtual void EvalCritter(Uint16 pos) override;
   virtual void TriggerSetOff(Uint16 pos) override;
   virtual void UserAction(UserInterfaceUserAction action,
      unsigned int param) override;
   virtual void OnChangingLevel() override;

private:
   /// returns scripting class from Lua state
   static LuaScripting& GetScriptingFromSelf(lua_State* L);

   /// registers all callable functions
   void RegisterFunctions();

private:
   /// game instance
   IGameInstance* m_game;

   /// name for 'self' global in Lua
   static const char* s_selfName;

private:
   // registered C functions callable from Lua
   // prototype: static int xyz(lua_State* L);
   static int uw_print(lua_State* L);        ///< Lua function uw.print
   static int uw_get_string(lua_State* L);   ///< uw.get_string
   static int uw_change_level(lua_State* L); ///< uw.change_level
   static int uw_start_conv(lua_State* L);   ///< uw.start_conv
   static int uw_show_cutscene(lua_State* L);      ///< uw.show_cutscene
   static int uw_show_map(lua_State* L);           ///< uw.show_map
   static int uw_show_ingame_anim(lua_State* L);   ///< uw.show_ingame_anim
   static int uw_cursor_use_item(lua_State* L);    ///< uw.cursor_use_item
   static int uw_cursor_target(lua_State* L);      ///< uw.cursor_target

   static int player_get_info(lua_State* L); ///< player.get_info
   static int player_set_info(lua_State* L); ///< player.set_info
   static int player_get_attr(lua_State* L); ///< player.get_attr
   static int player_get_skill(lua_State* L);///< player.get_skill
   static int player_set_attr(lua_State* L); ///< player.SetAttribute
   static int player_set_skill(lua_State* L);///< player.SetSkill

   static int objectlist_get_info(lua_State* L);///< objectlist.get_info
   static int objectlist_set_info(lua_State* L);///< objectlist.set_info
   static int objectlist_delete(lua_State* L);  ///< objectlist.delete
   static int objectlist_insert(lua_State* L);  ///< objectlist.insert

   static int inventory_get_info(lua_State* L);  ///< inventory.get_info
   static int inventory_get_floating_item(lua_State* L);  ///< inventory.get_floating_item
   static int inventory_float_add_item(lua_State* L);  ///< inventory.float_add_item

   static int tilemap_get_info(lua_State* L);///< tilemap.get_info
   static int tilemap_set_info(lua_State* L);///< tilemap.set_info
   static int tilemap_get_floor_height(lua_State* L); ///< tilemap.get_floor_height
   static int tilemap_get_object_list_link(lua_State* L); ///< tilemap.get_object_list_link

   static int runebag_set(lua_State* L);       ///< runebag.set
   static int runebag_test(lua_State* L);      ///< runebag.test

   static int conv_is_avail(lua_State* L);   ///< conv.is_avail
   static int conv_get_global(lua_State* L); ///< conv.GetGlobal
   static int conv_set_global(lua_State* L); ///< conv.SetGlobal

   static int quest_get_flag(lua_State* L);  ///< quest.get_flag
   static int quest_set_flag(lua_State* L);  ///< quest.set_flag

   static int prop_get_common(lua_State* L); ///< prop.get_common
   static int prop_get_special(lua_State* L);///< prop.get_special

   /// translates from rune type Integer value to RuneType
   static Underworld::RuneType GetRuneTypeFromInteger(unsigned int rune);

   /// adds ObjectInfo to the table currently on top of the stack
   static void AddObjectInfoTableFields(lua_State* L, const Underworld::ObjectInfo& info);
};
