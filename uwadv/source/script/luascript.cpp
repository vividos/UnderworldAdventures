/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file luascript.cpp

   \brief Lua scripting implementation

*/

// needed includes
#include "common.hpp"
#include "luascript.hpp"
#include "game_interface.hpp"
#include "files.hpp"
#include "gamestrings.hpp"


// constants

const char* ua_lua_scripting::self_name = "_scripting_self";


// ua_lua_scripting methods

void ua_lua_scripting::init(ua_basic_game_interface* the_game)
{
   game = the_game;

   // init lua state
   L = lua_open(256);

   // open lualib libraries
   lua_baselibopen(L);
   lua_strlibopen(L);
   lua_mathlibopen(L);

   // set "self" pointer userdata
   lua_pushuserdata(L,this);
   lua_setglobal(L,self_name);

   register_functions();
}

bool ua_lua_scripting::load_script(const char* basename)
{
   std::string scriptname(basename);
   scriptname.append(".lua");

   bool compiled = false;

   // load lua script
   SDL_RWops* script =
      game->get_files_manager().get_uadata_file(scriptname.c_str());

   // not found? try binary one
   if (script==NULL)
   {
      scriptname.assign(basename);
      scriptname.append(".lob");
      compiled = true;

      script = game->get_files_manager().get_uadata_file(scriptname.c_str());
   }

   // still not found? exception
   if (script==NULL)
   {
      std::string text("could not load Lua script from uadata: ");
      text.append(scriptname.c_str());
      throw ua_exception(text.c_str());
   }

   int ret = load_script(script, basename);

   ua_trace("loaded Lua %sscript \"%s\"\n",
      compiled ? "compiled " : "", scriptname.c_str());

   SDL_RWclose(script);

   return ret == 0;
}

void ua_lua_scripting::done()
{
   lua_close(L);
}

void ua_lua_scripting::checked_call(int nargs, int nresults)
{
   lua_call(L,nargs,nresults);
}

void ua_lua_scripting::init_new_game()
{
   lua_getglobal(L,"game_init_new");
   checked_call(0,0);
}

void ua_lua_scripting::eval_critter(Uint16 pos)
{
   lua_getglobal(L,"critter_eval");
   lua_pushnumber(L, static_cast<double>(pos));
   checked_call(1,0);
}

void ua_lua_scripting::trigger_set_off(Uint16 pos)
{
   lua_getglobal(L,"trigger_set_off");
   lua_pushnumber(L, static_cast<double>(pos));
   checked_call(1,0);
}

void ua_lua_scripting::user_action(ua_underworld_user_action action,
   unsigned int param)
{
   switch(action)
   {
   case ua_action_look_object:
      lua_getglobal(L,"object_look");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1,0);
      break;

   case ua_action_look_object_inventory:
      lua_getglobal(L,"inventory_look");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1,0);
      break;

   case ua_action_look_wall:
      lua_getglobal(L,"look_at_wall");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1,0);
      break;

   case ua_action_use_object:
      lua_getglobal(L,"object_use");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1,0);
      break;

   case ua_action_use_object_inventory:
      lua_getglobal(L,"inventory_use");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1,0);
      break;

   case ua_action_use_wall:
      lua_getglobal(L,"wall_use");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1,0);
      break;

   case ua_action_get_object:
      lua_getglobal(L,"object_get");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1,0);
      break;

   case ua_action_talk_object:
      lua_getglobal(L,"object_talk");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1,0);
      break;

   case ua_action_clicked_spells:
      lua_getglobal(L,"spell_cast");
      checked_call(0,0);
      break;

   case ua_action_clicked_runeshelf:
      lua_getglobal(L,"spell_cancel");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1,0);
      break;

   case ua_action_clicked_compass:
      lua_getglobal(L,"ui_clicked_compass");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1,0);
      break;

   case ua_action_clicked_vitality_flask:
      lua_getglobal(L,"ui_clicked_vitality_flask");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1,0);
      break;

   case ua_action_clicked_mana_flask:
      lua_getglobal(L,"ui_clicked_mana_flask");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1,0);
      break;

   case ua_action_clicked_gargoyle:
      lua_getglobal(L,"ui_clicked_gargoyle");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1,0);
      break;

   case ua_action_clicked_dragons:
      lua_getglobal(L,"ui_clicked_dragons");
      lua_pushnumber(L, static_cast<double>(param));
      checked_call(1,0);
      break;

      // ua_action_combat_enter
      // ua_action_combat_draw
      // ua_action_combat_leave
      // ua_action_combat_release

   case ua_action_track_creatures:
      lua_getglobal(L,"track_creatures");
      checked_call(0,0);
      break;

   case ua_action_sleep:
      lua_getglobal(L,"sleep");
      checked_call(0,0);
      break;

   case ua_action_target_selected:
      break;

   default:
      break;
   }
}

void ua_lua_scripting::on_changing_level()
{
   lua_getglobal(L,"on_change_level");
   checked_call(0,0);
}

ua_item_combine_status ua_lua_scripting::item_combine(Uint16 item_id1,
   Uint16 item_id2, Uint16& result_id)
{
   lua_getglobal(L,"item_combine");
   lua_pushnumber(L, static_cast<double>(item_id1));
   lua_pushnumber(L, static_cast<double>(item_id2));
   checked_call(2,2);

   result_id = ua_item_none;
   return ua_item_combine_failed;
}

int ua_lua_scripting::load_script(SDL_RWops* rwops, const char* chunkname)
{
   // load script into buffer
   std::vector<char> buffer;
   unsigned int len=0;
   {
      SDL_RWseek(rwops,0,SEEK_END);
      len = SDL_RWtell(rwops);
      SDL_RWseek(rwops,0,SEEK_SET);

      buffer.resize(len+1,0);

      SDL_RWread(rwops,&buffer[0],len,1);
      buffer[len]=0;
   }

   // execute script
   int ret = lua_dobuffer(L,&buffer[0],len,chunkname);

   if (ret!=0)
      ua_trace("Lua script loading ended with error code %u\n",ret);

   return ret;
}

ua_scripting* ua_scripting::create_scripting(ua_scripting_language)
{
   return new ua_lua_scripting;
}

ua_lua_scripting& ua_lua_scripting::get_scripting_from_self(lua_State* L)
{
   ua_lua_scripting* self = NULL;

   lua_getglobal(L,self_name);
   if (!lua_isuserdata(L,-1))
      throw ua_exception("'self' parameter wasn't set by ua_lua_scripting");

   // get pointer to underworld
   self = reinterpret_cast<ua_lua_scripting*>(lua_touserdata(L,-1));

   if (self == NULL || self->L != L)
      throw ua_exception("wrong 'self' parameter in ua_lua_scripting");

   lua_pop(L,1);

   return *self;
}

//! registers a C function inside a table
#define lua_register_table(L, n, f) { \
   lua_pushstring(L, n); lua_pushcfunction(L, f); lua_settable(L,-3); }

/*! Functions that are callable via Lua are organized in global tables; this
    way we simulate objects that can be accessed, e.g. in uw.print("text")
*/
void ua_lua_scripting::register_functions()
{
   // uw object
   lua_newtable(L);
   lua_register_table(L, "print", uw_print);
   lua_register_table(L, "get_string", uw_get_string);
   lua_register_table(L, "change_level", uw_change_level);
   lua_register_table(L, "start_conv", uw_start_conv);
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

   // objlist object
   lua_newtable(L);
   lua_register_table(L, "get_info", objlist_get_info);
   lua_register_table(L, "set_info", objlist_set_info);
   lua_register_table(L, "delete", objlist_delete);
   lua_register_table(L, "insert", objlist_insert);
   lua_setglobal(L, "objlist");

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

int ua_lua_scripting::uw_print(lua_State* L)
{
   ua_lua_scripting& self = get_scripting_from_self(L);

   ua_underworld_callback* callback = self.game->get_underworld().get_callback();
   if (callback != NULL)
   {
      const char* text = lua_tostring(L,-1);
      callback->uw_print(text);
   }
   return 0;
}

int ua_lua_scripting::uw_get_string(lua_State* L)
{
   ua_lua_scripting& self = get_scripting_from_self(L);

   unsigned int block = static_cast<unsigned int>(lua_tonumber(L,-2));
   unsigned int num = static_cast<unsigned int>(lua_tonumber(L,-1));

   // retrieve game string
   std::string str(self.game->get_gamestrings().get_string(block,num));
   lua_pushstring(L,str.c_str());

   return 1;
}

int ua_lua_scripting::uw_change_level(lua_State* L)
{
   ua_lua_scripting& self = get_scripting_from_self(L);

   unsigned int level = static_cast<unsigned int >(lua_tonumber(L, -1));

   self.game->get_underworld().change_level(level);

   return 0;
}

int ua_lua_scripting::uw_start_conv(lua_State* L)
{
   ua_lua_scripting& self = get_scripting_from_self(L);

   ua_underworld_callback* callback = self.game->get_underworld().get_callback();
   if (callback != NULL)
   {
      Uint16 conv_obj = static_cast<Uint16>(lua_tonumber(L, -1));
      callback->uw_start_conversation(conv_obj);
   }
   return 0;
}

int ua_lua_scripting::player_get_info(lua_State* L)
{
   // retrieve player object
   ua_lua_scripting& self = get_scripting_from_self(L);
   ua_player& player = self.game->get_underworld().get_player();

   // create new table and fill it with infos
   lua_newtable(L);

   lua_pushstring(L,"name");
   lua_pushstring(L, player.get_name().c_str());
   lua_settable(L,-3);

   lua_pushstring(L,"xpos");
   lua_pushnumber(L,player.get_xpos());
   lua_settable(L,-3);

   lua_pushstring(L,"ypos");
   lua_pushnumber(L,player.get_ypos());
   lua_settable(L,-3);

   lua_pushstring(L,"height");
   lua_pushnumber(L,player.get_height());
   lua_settable(L,-3);

   lua_pushstring(L,"angle");
   lua_pushnumber(L,player.get_angle_rot());
   lua_settable(L,-3);

   // leave table on stack and return it
   return 1;
}

int ua_lua_scripting::player_set_info(lua_State* L)
{
   // retrieve player object
   ua_lua_scripting& self = get_scripting_from_self(L);
   ua_player& player = self.game->get_underworld().get_player();

   if (!lua_istable(L,-1))
   {
      ua_trace("player_set_info: got wrong parameter\n");
      return 0;
   }

   // table is on stack, at -1
   lua_pushstring(L,"name");
   lua_gettable(L, -2);
   std::string name = lua_tostring(L,-1);
   player.set_name(name);
   lua_pop(L,1);

   lua_pushstring(L,"xpos");
   lua_gettable(L, -2);
   lua_pushstring(L,"ypos");
   lua_gettable(L, -3);
   player.set_pos(lua_tonumber(L,-2), lua_tonumber(L,-1));
   lua_pop(L,2);
   
   lua_pushstring(L,"height");
   lua_gettable(L, -2);
   player.set_height(lua_tonumber(L,-1));
   lua_pop(L,1);

   lua_pushstring(L,"angle");
   lua_gettable(L, -2);
   player.set_angle_rot(lua_tonumber(L,-1));
   lua_pop(L,1);

   return 0;
}

int ua_lua_scripting::player_get_attr(lua_State* L)
{
   // retrieve player object
   ua_lua_scripting& self = get_scripting_from_self(L);
   ua_player& player = self.game->get_underworld().get_player();

   ua_player_attributes attr =
      static_cast<ua_player_attributes>(static_cast<int>(lua_tonumber(L,-1)));

   lua_pushnumber(L,static_cast<double>(player.get_attr(attr)));
   return 1;
}

int ua_lua_scripting::player_set_attr(lua_State* L)
{
   // retrieve player object
   ua_lua_scripting& self = get_scripting_from_self(L);
   ua_player& player = self.game->get_underworld().get_player();

   ua_player_attributes attr_type =
      static_cast<ua_player_attributes>(static_cast<int>(lua_tonumber(L,-2)));
   unsigned int attr_value = static_cast<unsigned int>(lua_tonumber(L,-1));

   player.set_attr(attr_type,attr_value);
   return 0;
}

int ua_lua_scripting::player_get_skill(lua_State* L)
{
   // retrieve player object
   ua_lua_scripting& self = get_scripting_from_self(L);
   ua_player& player = self.game->get_underworld().get_player();

   ua_player_skills skill_type =
      static_cast<ua_player_skills>(static_cast<int>(lua_tonumber(L,-1)));

   lua_pushnumber(L,static_cast<double>(player.get_skill(skill_type)));
   return 1;
}

int ua_lua_scripting::player_set_skill(lua_State* L)
{
   // retrieve player object
   ua_lua_scripting& self = get_scripting_from_self(L);
   ua_player& player = self.game->get_underworld().get_player();

   ua_player_skills skill_type =
      static_cast<ua_player_skills>(static_cast<int>(lua_tonumber(L,-2)));
   unsigned int skill_value = static_cast<unsigned int>(lua_tonumber(L,-1));

   player.set_skill(skill_type,skill_value);
   return 0;
}

int ua_lua_scripting::objlist_get_info(lua_State* L)
{
   // retrieve object list
   ua_lua_scripting& self = get_scripting_from_self(L);
   ua_object_list& objlist =
      self.game->get_underworld().get_current_level().get_mapobjects();

   // get object pos and object
   Uint16 obj_pos = static_cast<Uint16>(lua_tonumber(L,-1));

   ua_object& obj = objlist.get_object(obj_pos);
   ua_object_info& info = obj.get_object_info();
   ua_object_info_ext& extinfo = obj.get_ext_object_info();

   // create new table and fill it with infos
   lua_newtable(L);

   lua_pushstring(L,"obj_pos");
   lua_pushnumber(L,static_cast<double>(obj_pos));
   lua_settable(L,-3);

   lua_pushstring(L,"item_id");
   lua_pushnumber(L,static_cast<double>(info.item_id));
   lua_settable(L,-3);

   lua_pushstring(L,"enchanted");
   lua_pushnumber(L,info.enchanted ? 1.0 : 0.0);
   lua_settable(L,-3);

   lua_pushstring(L,"is_quantity");
   lua_pushnumber(L,info.is_quantity ? 1.0 : 0.0);
   lua_settable(L,-3);

   lua_pushstring(L,"is_hidden");
   lua_pushnumber(L,info.is_hidden ? 1.0 : 0.0);
   lua_settable(L,-3);

   lua_pushstring(L,"flags");
   lua_pushnumber(L,static_cast<double>(info.flags));
   lua_settable(L,-3);


   lua_pushstring(L,"xpos");
   lua_pushnumber(L,static_cast<double>(extinfo.xpos));
   lua_settable(L,-3);

   lua_pushstring(L,"ypos");
   lua_pushnumber(L,static_cast<double>(extinfo.ypos));
   lua_settable(L,-3);

   lua_pushstring(L,"zpos");
   lua_pushnumber(L,static_cast<double>(extinfo.zpos));
   lua_settable(L,-3);

   lua_pushstring(L,"heading");
   lua_pushnumber(L,static_cast<double>(extinfo.heading));
   lua_settable(L,-3);

   lua_pushstring(L,"tilex");
   lua_pushnumber(L,static_cast<double>(extinfo.tilex));
   lua_settable(L,-3);

   lua_pushstring(L,"tiley");
   lua_pushnumber(L,static_cast<double>(extinfo.tiley));
   lua_settable(L,-3);


   lua_pushstring(L,"quality");
   lua_pushnumber(L,static_cast<double>(info.quality));
   lua_settable(L,-3);

   lua_pushstring(L,"link_next");
   lua_pushnumber(L,static_cast<double>(info.link));
   lua_settable(L,-3);

   lua_pushstring(L,"owner");
   lua_pushnumber(L,static_cast<double>(info.owner));
   lua_settable(L,-3);

   lua_pushstring(L,"quantity");
   lua_pushnumber(L,static_cast<double>(info.quantity));
   lua_settable(L,-3);

   lua_pushstring(L,"npc_used");
   lua_pushnumber(L,extinfo.npc_used? 1.0 : 0.0);
   lua_settable(L,-3);

   // add npc infos
   if (extinfo.npc_used)
   {
      lua_pushstring(L,"npc_hp");
      lua_pushnumber(L,static_cast<double>(extinfo.npc_hp));
      lua_settable(L,-3);

      lua_pushstring(L,"npc_goal");
      lua_pushnumber(L,static_cast<double>(extinfo.npc_goal));
      lua_settable(L,-3);

      lua_pushstring(L,"npc_gtarg");
      lua_pushnumber(L,static_cast<double>(extinfo.npc_gtarg));
      lua_settable(L,-3);

      lua_pushstring(L,"npc_level");
      lua_pushnumber(L,static_cast<double>(extinfo.npc_level));
      lua_settable(L,-3);

      lua_pushstring(L,"npc_talkedto");
      lua_pushnumber(L,extinfo.npc_talkedto? 1.0 : 0.0);
      lua_settable(L,-3);

      lua_pushstring(L,"npc_attitude");
      lua_pushnumber(L,static_cast<double>(extinfo.npc_attitude));
      lua_settable(L,-3);

      lua_pushstring(L,"npc_xhome");
      lua_pushnumber(L,static_cast<double>(extinfo.npc_xhome));
      lua_settable(L,-3);

      lua_pushstring(L,"npc_yhome");
      lua_pushnumber(L,static_cast<double>(extinfo.npc_yhome));
      lua_settable(L,-3);

      lua_pushstring(L,"npc_hunger");
      lua_pushnumber(L,static_cast<double>(extinfo.npc_hunger));
      lua_settable(L,-3);

      lua_pushstring(L,"npc_whoami");
      lua_pushnumber(L,static_cast<double>(extinfo.npc_whoami));
      lua_settable(L,-3);
   }

   // leave table on stack and return it
   return 1;
}

/*! \todo implement */
int ua_lua_scripting::objlist_set_info(lua_State* L)
{
   return 0;
}

int ua_lua_scripting::objlist_delete(lua_State* L)
{
   // retrieve object list
   ua_lua_scripting& self = get_scripting_from_self(L);
   ua_object_list& objlist =
      self.game->get_underworld().get_current_level().get_mapobjects();

   // get object pos and object
   Uint16 obj_pos = static_cast<Uint16>(lua_tonumber(L,-1));

   objlist.delete_object(obj_pos);

   return 0;
}

/*! \todo implement */
int ua_lua_scripting::objlist_insert(lua_State* L)
{
   // retrieve object list
   ua_lua_scripting& self = get_scripting_from_self(L);
   ua_object_list& objlist =
      self.game->get_underworld().get_current_level().get_mapobjects();

   return 0;//1;
}

int ua_lua_scripting::tilemap_get_info(lua_State* L)
{
   // retrieve current level
   ua_lua_scripting& self = get_scripting_from_self(L);
   ua_level& level = self.game->get_underworld().get_current_level();

   // get tilemap info
   double xpos = lua_tonumber(L,-2);
   double ypos = lua_tonumber(L,-1);

   ua_levelmap_tile& tileinfo = level.get_tile(unsigned(xpos),unsigned(ypos));

   // create new table and fill it with infos
   lua_newtable(L);

   lua_pushstring(L,"xpos");
   lua_pushnumber(L,static_cast<double>(xpos));
   lua_settable(L,-3);

   lua_pushstring(L,"ypos");
   lua_pushnumber(L,static_cast<double>(ypos));
   lua_settable(L,-3);

   lua_pushstring(L,"type");
   lua_pushnumber(L,static_cast<double>(tileinfo.type));
   lua_settable(L,-3);

   lua_pushstring(L,"floor");
   lua_pushnumber(L,static_cast<double>(tileinfo.floor));
   lua_settable(L,-3);

   lua_pushstring(L,"ceiling");
   lua_pushnumber(L,static_cast<double>(tileinfo.ceiling));
   lua_settable(L,-3);

   lua_pushstring(L,"slope");
   lua_pushnumber(L,static_cast<double>(tileinfo.slope));
   lua_settable(L,-3);

//   tileinfo.texture_floor
//   tileinfo.texture_ceiling
//   tileinfo.texture_wall;

   // leave table on stack and return it
   return 1;
}

/*! \todo implement */
int ua_lua_scripting::tilemap_set_info(lua_State* L)
{
   // retrieve current level
   ua_lua_scripting& self = get_scripting_from_self(L);
   ua_level& level = self.game->get_underworld().get_current_level();

   return 0;
}

int ua_lua_scripting::tilemap_get_floor_height(lua_State* L)
{
   // retrieve current level
   ua_lua_scripting& self = get_scripting_from_self(L);
   ua_level& level = self.game->get_underworld().get_current_level();

   double xpos = lua_tonumber(L,-2);
   double ypos = lua_tonumber(L,-1);

   lua_pushnumber(L,level.get_floor_height(xpos,ypos));
   return 1;
}

int ua_lua_scripting::tilemap_get_objlist_link(lua_State* L)
{
   // retrieve current level
   ua_lua_scripting& self = get_scripting_from_self(L);
   ua_level& level = self.game->get_underworld().get_current_level();

   double xpos = lua_tonumber(L,-2);
   double ypos = lua_tonumber(L,-1);
   Uint16 objpos = level.get_mapobjects().get_tile_list_start(unsigned(xpos),unsigned(ypos));

   lua_pushnumber(L,static_cast<double>(objpos));
   return 1;
}

int ua_lua_scripting::runes_set(lua_State* L)
{
   // retrieve runes object
   ua_lua_scripting& self = get_scripting_from_self(L);
   ua_runes& runes = self.game->get_underworld().get_player().get_runes();

   unsigned int rune = static_cast<unsigned int>(lua_tonumber(L,-2));
   bool flag = lua_tonumber(L,-1) > 0.0;

   runes.get_runebag().set(rune, flag);
   return 0;
}

int ua_lua_scripting::runes_test(lua_State* L)
{
   // retrieve runes object
   ua_lua_scripting& self = get_scripting_from_self(L);
   ua_runes& runes = self.game->get_underworld().get_player().get_runes();

   unsigned int rune = static_cast<unsigned int>(lua_tonumber(L,-1));

   lua_pushnumber(L,runes.get_runebag().test(rune) ? 1.0 : 0.0);
   return 1;
}

int ua_lua_scripting::conv_is_avail(lua_State* L)
{
   // retrieve game strings object
   ua_lua_scripting& self = get_scripting_from_self(L);
   ua_gamestrings& gstr = self.game->get_gamestrings();

   Uint16 slot = static_cast<Uint16>(lua_tonumber(L,-1));

   if (!gstr.is_avail(slot+0x0e00))
      lua_pushnil(L);
   else
      lua_pushnumber(L,static_cast<double>(slot));

   return 1;
}

int ua_lua_scripting::conv_get_global(lua_State* L)
{
   // retrieve conv globals object
   ua_lua_scripting& self = get_scripting_from_self(L);

   Uint16 slot = static_cast<Uint16>(lua_tonumber(L,-1));
   unsigned int pos = static_cast<unsigned int>(lua_tonumber(L,-2));

   std::vector<Uint16>& globals =
      self.game->get_underworld().get_conv_globals().get_globals(slot);

   if (pos >= globals.size())
   {
      ua_trace("conv.get_global: globals pos out of range!\n");
      return 0;
   }

   lua_pushnumber(L,static_cast<double>(globals[pos]));
   return 1;
}

int ua_lua_scripting::conv_set_global(lua_State* L)
{
   // retrieve conv globals object
   ua_lua_scripting& self = get_scripting_from_self(L);

   Uint16 slot = static_cast<Uint16>(lua_tonumber(L,-1));
   unsigned int pos = static_cast<unsigned int>(lua_tonumber(L,-2));
   Uint16 value = static_cast<Uint16>(lua_tonumber(L,-2));

   std::vector<Uint16>& globals =
      self.game->get_underworld().get_conv_globals().get_globals(slot);

   if (pos >= globals.size())
      ua_trace("conv.set_global: globals pos out of range!\n");
   else
      globals[pos] = value;

   return 0;
}

int ua_lua_scripting::quest_get_flag(lua_State* L)
{
   // retrieve quest flags object
   ua_lua_scripting& self = get_scripting_from_self(L);
   std::vector<Uint16>& questflags =
      self.game->get_underworld().get_questflags();

   Uint16 flag_nr = static_cast<Uint16>(lua_tonumber(L,-1));

   if(flag_nr >= questflags.size())
   {
      ua_trace("quest.get_flag: flag_nr out of range!\n");
      return 0;
   }

   lua_pushnumber(L,static_cast<double>(questflags[flag_nr]));

   return 1;
}

int ua_lua_scripting::quest_set_flag(lua_State* L)
{
   // retrieve quest flags object
   ua_lua_scripting& self = get_scripting_from_self(L);
   std::vector<Uint16>& questflags =
      self.game->get_underworld().get_questflags();

   Uint16 flag_nr = static_cast<Uint16>(lua_tonumber(L,-1));
   Uint16 flag_val = static_cast<Uint16>(lua_tonumber(L,-2));

   if(flag_nr >= questflags.size())
      ua_trace("quest.set_flag: flag_nr out of range!\n");
   else
      questflags[flag_nr] = flag_val;

   return 0;
}

int ua_lua_scripting::prop_get_common(lua_State* L)
{
   // retrieve common object properties
   ua_lua_scripting& self = get_scripting_from_self(L);

   Uint16 item_id = static_cast<Uint16>(lua_tonumber(L,-1));

   ua_common_obj_property& prop = self.game->get_underworld().
      get_obj_properties().get_common_property(item_id);

   // create new table and fill it with infos
   lua_newtable(L);

   lua_pushstring(L,"item_id");
   lua_pushnumber(L,static_cast<double>(item_id));
   lua_settable(L,-3);

   lua_pushstring(L,"height");
   lua_pushnumber(L,static_cast<double>(prop.height));
   lua_settable(L,-3);

   lua_pushstring(L,"mass");
   lua_pushnumber(L,static_cast<double>(prop.mass));
   lua_settable(L,-3);

   lua_pushstring(L,"radius");
   lua_pushnumber(L,static_cast<double>(prop.radius));
   lua_settable(L,-3);

   lua_pushstring(L,"quality_class");
   lua_pushnumber(L,static_cast<double>(prop.quality_class));
   lua_settable(L,-3);

   lua_pushstring(L,"quality_type");
   lua_pushnumber(L,static_cast<double>(prop.quality_type));
   lua_settable(L,-3);

   lua_pushstring(L,"can_have_owner");
   lua_pushnumber(L,prop.can_have_owner ? 1.0 : 0.0);
   lua_settable(L,-3);

   lua_pushstring(L,"can_be_looked_at");
   lua_pushnumber(L,prop.can_be_looked_at ? 1.0 : 0.0);
   lua_settable(L,-3);

   lua_pushstring(L,"can_be_picked_up");
   lua_pushnumber(L,prop.can_be_picked_up ? 1.0 : 0.0);
   lua_settable(L,-3);

   lua_pushstring(L,"is_container");
   lua_pushnumber(L,prop. is_container ? 1.0 : 0.0);
   lua_settable(L,-3);

   return 1;
}

/*! \todo implement */
int ua_lua_scripting::prop_get_special(lua_State* L)
{
   // retrieve common object properties
   ua_lua_scripting& self = get_scripting_from_self(L);
   ua_object_properties prop =
      self.game->get_underworld().get_obj_properties();

   return 0;//1;
}
