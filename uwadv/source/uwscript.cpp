/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

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
/*! \file uwscript.cpp

   \brief underworld Lua script bindings

*/

// needed includes
#include "common.hpp"
#include "uwscript.hpp"
#include "underworld.hpp"


// constants

//! name of lua userdata variable containing the "this" pointer
const char* ua_uwscript_lua_thisptr_name = "self";


// global functions

Uint32 ua_obj_handle_encode(Uint32 objpos, Uint32 level)
{
   return objpos | (level<<10);
}

void ua_obj_handle_decode(Uint32 obj_handle, Uint32 &objpos,
   Uint32& level)
{
   objpos = obj_handle & 0x03ff;
   level = obj_handle>>10;
}


// ua_underworld_script_bindings methods

ua_underworld_script_bindings::ua_underworld_script_bindings()
:callback(NULL)
{
}

void ua_underworld_script_bindings::init(ua_underworld *uw)
{
   // init lua state
   L = lua_open(256);

   // open lualib libraries
   lua_baselibopen(L);
   lua_strlibopen(L);
   lua_mathlibopen(L);

   register_functions();

   // set "this" pointer userdata
   lua_pushuserdata(L,uw);
   lua_setglobal(L,ua_uwscript_lua_thisptr_name);
}

void ua_underworld_script_bindings::register_functions()
{
// template:
//   lua_register(L,"func_name",func_name);

   // ua_player access
   lua_register(L,"underw_change_level",underw_change_level);

   lua_register(L,"player_get_attr",player_get_attr);
   lua_register(L,"player_set_attr",player_set_attr);
   lua_register(L,"player_get_skill",player_get_skill);
   lua_register(L,"player_set_skill",player_set_skill);
   lua_register(L,"player_get_pos",player_get_pos);
   lua_register(L,"player_set_pos",player_set_pos);
   lua_register(L,"player_get_height",player_get_height);
   lua_register(L,"player_set_height",player_set_height);
   lua_register(L,"player_get_angle",player_get_angle);
   lua_register(L,"player_set_angle",player_set_angle);

   lua_register(L,"objlist_get_obj_info",objlist_get_obj_info);
   lua_register(L,"objlist_set_obj_info",objlist_set_obj_info);
   lua_register(L,"objlist_remove_obj",objlist_remove_obj);
   lua_register(L,"objlist_insert_obj",objlist_insert_obj);

   lua_register(L,"tilemap_get_tile",tilemap_get_tile);
   lua_register(L,"tilemap_get_type",tilemap_get_type);
   lua_register(L,"tilemap_set_type",tilemap_set_type);
   lua_register(L,"tilemap_get_floor",tilemap_get_floor);
   lua_register(L,"tilemap_set_floor",tilemap_set_floor);
   lua_register(L,"tilemap_get_ceiling",tilemap_get_ceiling);
   lua_register(L,"tilemap_set_ceiling",tilemap_set_ceiling);
   lua_register(L,"tilemap_get_floor_height",tilemap_get_floor_height);
   lua_register(L,"tilemap_get_objlist_start",tilemap_get_objlist_start);

   lua_register(L,"quest_get_flag",quest_get_flag);
   lua_register(L,"quest_set_flag",quest_set_flag);

   lua_register(L,"conv_is_avail",conv_is_avail);
   lua_register(L,"conv_get_global",conv_get_global);
   lua_register(L,"conv_set_global",conv_set_global);

   lua_register(L,"prop_get_common",prop_get_common);
   lua_register(L,"prop_get_special",prop_get_special);

   lua_register(L,"ui_start_conv",ui_start_conv);
   lua_register(L,"ui_show_cutscene",ui_show_cutscene);
   lua_register(L,"ui_print_string",ui_print_string);
   lua_register(L,"ui_get_gamestring",ui_get_gamestring);
   lua_register(L,"ui_show_ingame_anim",ui_show_ingame_anim);
   lua_register(L,"ui_cursor_use_item",ui_cursor_use_item);
   lua_register(L,"ui_cursor_target",ui_cursor_target);

   lua_register(L,"savegame_store_value",savegame_store_value);
   lua_register(L,"savegame_restore_value",savegame_restore_value);
}

void ua_underworld_script_bindings::checked_lua_call(int params, int retvals)
{
   int ret = lua_call(L,params,retvals);
   if (ret!=0)
      ua_trace("Lua function call error code: %u\n",ret);
//      throw ua_exception("");
}

double ua_underworld_script_bindings::get_lua_constant(const char *name)
{
   lua_getglobal(L,name);

   double val = 0.0;
   if (lua_isnumber(L,-1)) val = lua_tonumber(L,-1);

   lua_pop(L,1);

   return val;
}

void ua_underworld_script_bindings::register_callback(ua_underworld_script_callback* cback)
{
   callback = cback;
}

void ua_underworld_script_bindings::done()
{
   // call function lua_done_script()
   lua_getglobal(L,"lua_done_script");
   checked_lua_call(0,0);

   lua_close(L);
}

void ua_underworld_script_bindings::load_game(ua_savegame &sg)
{
   sg.begin_section("luascript");

   lua_getglobal(L,"lua_savegame_load");
   lua_pushuserdata(L,&sg);
   lua_pushnumber(L,static_cast<double>(sg.get_version()));
   checked_lua_call(2,0);

   sg.end_section();
}

void ua_underworld_script_bindings::save_game(ua_savegame &sg)
{
   sg.begin_section("luascript");

   lua_getglobal(L,"lua_savegame_save");
   lua_pushuserdata(L,&sg);
   checked_lua_call(1,0);

   sg.end_section();
}


// Lua functions callable from C

void ua_underworld_script_bindings::lua_init_script()
{
   // call init function lua_init_script()
   lua_getglobal(L,"lua_init_script");
   checked_lua_call(0,0);
}

void ua_underworld_script_bindings::lua_started_newgame()
{
   lua_getglobal(L,"lua_started_newgame");
   checked_lua_call(0,0);
}

void ua_underworld_script_bindings::lua_game_tick(double curtime)
{
   // call Lua function
   lua_getglobal(L,"lua_game_tick");
   lua_pushnumber(L,curtime);

   checked_lua_call(1,0);
}

void ua_underworld_script_bindings::lua_track()
{
   lua_getglobal(L,"lua_track");
   checked_lua_call(0,0);
}

bool ua_underworld_script_bindings::lua_sleep(bool start)
{
   lua_getglobal(L,"lua_sleep");
   checked_lua_call(1,1);
   return true;
}

void ua_underworld_script_bindings::lua_change_level(unsigned int level)
{
   // call Lua function
   lua_getglobal(L,"lua_change_level");
   lua_pushnumber(L,static_cast<double>(level));
   checked_lua_call(1,0);

   // call user interface function
   if (callback!=NULL)
      callback->ui_changed_level(level);
}

void ua_underworld_script_bindings::lua_wall_look(unsigned int tex_id)
{
   // call Lua function
   lua_getglobal(L,"lua_wall_look");
   lua_pushnumber(L,static_cast<double>(tex_id));
   checked_lua_call(1,0);
}


// lua_objlist_* functions

void ua_underworld_script_bindings::lua_objlist_look(Uint32 level, Uint32 objpos)
{
   Uint32 objhandle = ua_obj_handle_encode(objpos,level);

   // call Lua function
   lua_getglobal(L,"lua_objlist_look");
   lua_pushnumber(L,static_cast<double>(objhandle));
   checked_lua_call(1,0);
}

void ua_underworld_script_bindings::lua_objlist_talk(Uint32 level, Uint32 objpos)
{
   Uint32 objhandle = ua_obj_handle_encode(objpos,level);

   // call Lua function
   lua_getglobal(L,"lua_objlist_talk");
   lua_pushnumber(L,static_cast<double>(objhandle));
   checked_lua_call(1,0);
}

void ua_underworld_script_bindings::lua_objlist_use(Uint32 level, Uint32 objpos)
{
   Uint32 objhandle = ua_obj_handle_encode(objpos,level);

   // call Lua function
   lua_getglobal(L,"lua_objlist_use");
   lua_pushnumber(L,static_cast<double>(objhandle));
   checked_lua_call(1,0);
}


// lua_inventory_* functions

bool ua_underworld_script_bindings::lua_inventory_is_container(Uint16 item_id)
{
   // call Lua function
   lua_getglobal(L,"lua_inventory_is_container");
   lua_pushnumber(L,static_cast<double>(item_id));

   checked_lua_call(1,1);

   // returns nil when no container
   bool ret = lua_isnumber(L,-1)!=0;

   // remove return parameters
   lua_pop(L,1);
   return ret;
}

ua_inv_item_category ua_underworld_script_bindings::lua_inventory_categorize_item(Uint16 item_id)
{
   // call Lua function
   lua_getglobal(L,"lua_inventory_categorize_item");
   lua_pushnumber(L,static_cast<double>(item_id));

   checked_lua_call(1,1);

   // get return value
   ua_inv_item_category cat = ua_inv_cat_normal;

   if (lua_isnumber(L,-1))
   {
      cat = static_cast<ua_inv_item_category>(
         static_cast<unsigned int>(lua_tonumber(L,-1)));
   }

   lua_pop(L,1);
   return cat;
}

void ua_underworld_script_bindings::lua_inventory_look(Uint16 item_pos)
{
}

void ua_underworld_script_bindings::lua_inventory_use(Uint16 item_pos)
{
}

ua_obj_combine_result ua_underworld_script_bindings::lua_inventory_combine_obj(
   Uint16 item_id1, Uint16 item_id2, Uint16& result_id)
{
   // call Lua function
   lua_getglobal(L,"lua_inventory_combine_obj");
   lua_pushnumber(L,static_cast<double>(item_id1));
   lua_pushnumber(L,static_cast<double>(item_id2));

   checked_lua_call(2,2);

   // retrieve return values
   ua_obj_combine_result res = ua_obj_cmb_failed;

   if (lua_isnumber(L,-2))
   {
      res = static_cast<ua_obj_combine_result>(
         static_cast<unsigned int>(lua_tonumber(L,-2)));
   }

   result_id = 0xffff;
   if (lua_isnumber(L,-1))
   {
      result_id = static_cast<Uint16>(lua_tonumber(L,-1));
   }
   else
      res = ua_obj_cmb_failed;

   lua_pop(L,2);

   return res;
}

void ua_underworld_script_bindings::lua_trigger_set_off(Uint32 level, Uint32 objpos)
{
   lua_getglobal(L,"lua_trigger_set_off");
   lua_pushnumber(L,static_cast<double>(ua_obj_handle_encode(objpos,level)));

   checked_lua_call(1,0);
}


/*! params is the number of parameters the current registered function
    expects. it is needed to determine where the "self" userdata value is
    stored. */
ua_underworld& ua_underworld_script_bindings::get_underworld_from_self(lua_State* L)
{
   ua_underworld* self = NULL;

   lua_getglobal(L,ua_uwscript_lua_thisptr_name);
   if (!lua_isuserdata(L,-1))
      throw ua_exception("'self' parameter wasn't set by ua_underworld Lua script");

   // get pointer to underworld
   self = reinterpret_cast<ua_underworld*>(lua_touserdata(L,-1));

   if (self->get_scripts().L != L)
      throw ua_exception("wrong 'self' parameter in ua_underworld Lua script");

   lua_pop(L,1);

   return *self;
}


// registered C functions callable from Lua

// underw_* functions

int ua_underworld_script_bindings::underw_change_level(lua_State* L)
{
   ua_underworld& uw = get_underworld_from_self(L);

   unsigned int level = static_cast<unsigned int>(lua_tonumber(L,-1));

   uw.change_level(level);

   return 0;
}


// player_* functions

int ua_underworld_script_bindings::player_get_attr(lua_State *L)
{
   ua_player &pl = get_underworld_from_self(L).get_player();

   ua_player_attributes attr_type =
      static_cast<ua_player_attributes>(static_cast<int>(lua_tonumber(L,-1)));

   unsigned int attr_value = pl.get_attr(attr_type);
   lua_pushnumber(L,static_cast<double>(attr_value));

   return 1; // one return parameter
}

int ua_underworld_script_bindings::player_set_attr(lua_State *L)
{
   ua_player &pl = get_underworld_from_self(L).get_player();

   ua_player_attributes attr_type =
      static_cast<ua_player_attributes>(static_cast<int>(lua_tonumber(L,-2)));
   unsigned int attr_value = static_cast<unsigned int>(lua_tonumber(L,-1));

   pl.set_attr(attr_type,attr_value);

   return 0;
}

int ua_underworld_script_bindings::player_get_skill(lua_State *L)
{
   ua_player &pl = get_underworld_from_self(L).get_player();

   ua_player_skills skill_type =
      static_cast<ua_player_skills>(static_cast<int>(lua_tonumber(L,-1)));

   unsigned int skill_value = pl.get_skill(skill_type);
   lua_pushnumber(L,static_cast<double>(skill_value));

   return 1;
}

int ua_underworld_script_bindings::player_set_skill(lua_State *L)
{
   ua_player &pl = get_underworld_from_self(L).get_player();

   ua_player_skills skill_type =
      static_cast<ua_player_skills>(static_cast<int>(lua_tonumber(L,-2)));
   unsigned int skill_value = static_cast<unsigned int>(lua_tonumber(L,-1));

   pl.set_skill(skill_type,skill_value);

   return 0;
}

int ua_underworld_script_bindings::player_get_pos(lua_State *L)
{
   ua_player &pl = get_underworld_from_self(L).get_player();

   lua_pushnumber(L,pl.get_xpos());
   lua_pushnumber(L,pl.get_ypos());

   return 2;
}

int ua_underworld_script_bindings::player_set_pos(lua_State *L)
{
   ua_player &pl = get_underworld_from_self(L).get_player();

   double xpos = lua_tonumber(L,-2);
   double ypos = lua_tonumber(L,-1);

   pl.set_pos(xpos,ypos);

   return 0;
}

int ua_underworld_script_bindings::player_get_height(lua_State* L)
{
   ua_player &pl = get_underworld_from_self(L).get_player();

   lua_pushnumber(L,pl.get_height());

   return 1;
}

int ua_underworld_script_bindings::player_set_height(lua_State* L)
{
   ua_player &pl = get_underworld_from_self(L).get_player();

   double height = lua_tonumber(L,-1);
   pl.set_height(height);

   return 0;
}

int ua_underworld_script_bindings::player_get_angle(lua_State *L)
{
   ua_player &pl = get_underworld_from_self(L).get_player();

   lua_pushnumber(L,pl.get_angle_rot());

   return 1;
}

int ua_underworld_script_bindings::player_set_angle(lua_State *L)
{
   ua_player &pl = get_underworld_from_self(L).get_player();

   double angle = lua_tonumber(L,-1);
   pl.set_angle_rot(angle);

   return 0;
}


// objlist_* functions

int ua_underworld_script_bindings::objlist_get_obj_info(lua_State* L)
{
   ua_underworld& uw = get_underworld_from_self(L);

   Uint32 obj_handle = static_cast<Uint32>(lua_tonumber(L,-1));

   // decode handle
   Uint32 objpos=0,level=0;
   ua_obj_handle_decode(obj_handle,objpos,level);

   ua_object& obj = uw.get_level(level).get_mapobjects().get_object(objpos);
   ua_object_info& info = obj.get_object_info();
   ua_object_info_ext& extinfo = obj.get_ext_object_info();

   // create new table and fill it with infos
   lua_newtable(L);

   lua_pushstring(L,"item_id");
   lua_pushnumber(L,static_cast<double>(info.item_id));
   lua_settable(L,-3);

   lua_pushstring(L,"enchanted");
   lua_pushnumber(L,info.enchanted ? 1.0 : 0.0);
   lua_settable(L,-3);

   lua_pushstring(L,"is_quantity");
   lua_pushnumber(L,info.is_quantity ? 1.0 : 0.0);
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


   lua_pushstring(L,"quality");
   lua_pushnumber(L,static_cast<double>(info.quality));
   lua_settable(L,-3);

   lua_pushstring(L,"handle_next");
   lua_pushnumber(L,static_cast<double>(ua_obj_handle_encode(info.link,level)));
   lua_settable(L,-3);

   lua_pushstring(L,"owner");
   lua_pushnumber(L,static_cast<double>(info.owner));
   lua_settable(L,-3);

   lua_pushstring(L,"quantity");
   lua_pushnumber(L,static_cast<double>(
      info.is_quantity ? info.quantity : ua_obj_handle_encode(info.quantity,level)
   ));
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

   return 1;
}

int ua_underworld_script_bindings::objlist_set_obj_info(lua_State* L)
{
   // todo: implement
   return 0;
}

int ua_underworld_script_bindings::objlist_remove_obj(lua_State* L)
{
   // todo: implement
   return 0;
}

int ua_underworld_script_bindings::objlist_insert_obj(lua_State* L)
{
   // todo: implement
   return 0;
}


// tilemap_* functions

Uint32 ua_tile_handle_encode(Uint32 xpos, Uint32 ypos, Uint32 level)
{
   // the tile_handle is just a hash of level, xpos and ypos, but don't tell
   // the Lua programmers :-)
   return (level << 12) | (ypos << 6) | xpos;
}

void ua_tile_handle_decode(Uint32 tile_handle, Uint32& xpos,
   Uint32& ypos, Uint32& level)
{
   // decode the tile_handle hash, as seen in ua_tile_handle_encode()
   xpos = tile_handle & 63;
   ypos = (tile_handle >> 6) & 63;
   level = tile_handle >> 12;
}

ua_levelmap_tile& ua_underworld_script_bindings::get_tile_per_handle(
   ua_underworld& uw, unsigned int tile_handle)
{
   Uint32 xpos=0,ypos=0,level=0;
   ua_tile_handle_decode(tile_handle,xpos,ypos,level);

   ua_level& levelmap = uw.get_level(level);
   return levelmap.get_tile(xpos,ypos);
}

int ua_underworld_script_bindings::tilemap_get_tile(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   int level = static_cast<int>(lua_tonumber(L,-3));
   unsigned int xpos = static_cast<unsigned int>(lua_tonumber(L,-2));
   unsigned int ypos = static_cast<unsigned int>(lua_tonumber(L,-1));

   // get current level, when level is negative
   if (level<0)
      level = uw.get_player().get_attr(ua_attr_maplevel);

   Uint32 tile_handle = ua_tile_handle_encode(xpos,ypos,level);

   lua_pushnumber(L,static_cast<double>(tile_handle));

   return 1;
}

int ua_underworld_script_bindings::tilemap_get_type(lua_State* L)
{
   ua_underworld& uw = get_underworld_from_self(L);

   unsigned int tile_handle = static_cast<unsigned int>(lua_tonumber(L,-1));
   ua_levelmap_tile& tile = get_tile_per_handle(uw,tile_handle);

   lua_pushnumber(L,static_cast<double>(tile.type));

   return 1;
}

int ua_underworld_script_bindings::tilemap_set_type(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   unsigned int tile_handle = static_cast<unsigned int>(lua_tonumber(L,-2));
   ua_levelmap_tile& tile = get_tile_per_handle(uw,tile_handle);

   tile.type = static_cast<ua_levelmap_tiletype>(
      static_cast<unsigned int>(lua_tonumber(L,-1) ));

   return 0;
}

int ua_underworld_script_bindings::tilemap_get_floor(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   unsigned int tile_handle = static_cast<unsigned int>(lua_tonumber(L,-1));
   ua_levelmap_tile& tile = get_tile_per_handle(uw,tile_handle);

   lua_pushnumber(L,static_cast<double>(tile.floor));
   return 1;
}

int ua_underworld_script_bindings::tilemap_set_floor(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   unsigned int tile_handle = static_cast<unsigned int>(lua_tonumber(L,-2));
   ua_levelmap_tile& tile = get_tile_per_handle(uw,tile_handle);

   tile.floor = static_cast<ua_levelmap_tiletype>(
      static_cast<unsigned int>(lua_tonumber(L,-1) ));

   return 0;
}

int ua_underworld_script_bindings::tilemap_get_ceiling(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   unsigned int tile_handle = static_cast<unsigned int>(lua_tonumber(L,-1));
   ua_levelmap_tile& tile = get_tile_per_handle(uw,tile_handle);

   lua_pushnumber(L,static_cast<double>(tile.ceiling));
   return 1;
}

int ua_underworld_script_bindings::tilemap_set_ceiling(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   unsigned int tile_handle = static_cast<unsigned int>(lua_tonumber(L,-2));
   ua_levelmap_tile& tile = get_tile_per_handle(uw,tile_handle);

   tile.ceiling = static_cast<ua_levelmap_tiletype>(
      static_cast<unsigned int>(lua_tonumber(L,-1) ));

   return 0;
}

int ua_underworld_script_bindings::tilemap_get_floor_height(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   int level = static_cast<int>(lua_tonumber(L,-3));
   double xpos = lua_tonumber(L,-2);
   double ypos = lua_tonumber(L,-1);

   // get current level when level is negative
   if (level<0)
      level = uw.get_player().get_attr(ua_attr_maplevel);

   double height = uw.get_level(level).get_floor_height(xpos,ypos);
   lua_pushnumber(L,height);

   return 1;
}

int ua_underworld_script_bindings::tilemap_get_objlist_start(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   Uint32 tile_handle = static_cast<unsigned int>(lua_tonumber(L,-1));

   // decode tile handle
   Uint32 xpos=0,ypos=0,level=0;
   ua_tile_handle_decode(tile_handle,xpos,ypos,level);

   // get first object handle in list
   Uint16 link1 = uw.get_level(level).get_mapobjects().get_tile_list_start(xpos,ypos);
   lua_pushnumber(L,static_cast<double>(ua_obj_handle_encode(link1,level)));

   return 1;
}


// inventory_* functions

int ua_underworld_script_bindings::inventory_rune_avail(lua_State* L)
{
   // todo: implement
   return 1;
}

int ua_underworld_script_bindings::inventory_rune_add(lua_State* L)
{
   // todo: implement
   return 0;
}


// quest_* functions

int ua_underworld_script_bindings::quest_get_flag(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   Uint16 flag_nr = static_cast<Uint16>(lua_tonumber(L,-1));

   if(flag_nr>=uw.get_questflags().size())
   {
      ua_trace("quest_get_flag: flag_nr out of range!\n");
      return 0;
   }

   lua_pushnumber(L,static_cast<double>(uw.get_questflags()[flag_nr]));

   return 1;
}

int ua_underworld_script_bindings::quest_set_flag(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   Uint16 flag_nr = static_cast<Uint16>(lua_tonumber(L,-1));
   Uint16 flag_val = static_cast<Uint16>(lua_tonumber(L,-2));

   if(flag_nr>=uw.get_questflags().size())
      ua_trace("quest_set_flag: flag_nr out of range!\n");
   else
      uw.get_questflags()[flag_nr] = flag_val;

   return 0;
}

// conv_* functions

int ua_underworld_script_bindings::conv_is_avail(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   Uint16 slot = static_cast<Uint16>(lua_tonumber(L,-1));

   std::map<int,std::vector<std::string> >& allstrings = uw.get_strings().get_allstrings();

   if (allstrings.find(slot+0x0e00)  == allstrings.end())
      lua_pushnil(L);
   else
      lua_pushnumber(L,static_cast<double>(slot));

   return 1;
}

int ua_underworld_script_bindings::conv_get_global(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   Uint16 slot = static_cast<Uint16>(lua_tonumber(L,-1));
   unsigned int pos = static_cast<unsigned int>(lua_tonumber(L,-2));

   std::vector<Uint16>& globals = uw.get_conv_globals().get_globals(slot);

   if(pos>=globals.size())
   {
      ua_trace("conv_get_global: globals pos out of range!\n");
      return 0;
   }

   lua_pushnumber(L,static_cast<double>(globals[pos]));

   return 1;
}

int ua_underworld_script_bindings::conv_set_global(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   Uint16 slot = static_cast<Uint16>(lua_tonumber(L,-1));
   unsigned int pos = static_cast<unsigned int>(lua_tonumber(L,-2));
   Uint16 value = static_cast<Uint16>(lua_tonumber(L,-2));

   std::vector<Uint16>& globals = uw.get_conv_globals().get_globals(slot);

   if(pos>=globals.size())
      ua_trace("conv_set_global: globals pos out of range!\n");
   else
   {
      globals[pos] = value;
   }

   return 0;
}


// prop_* functions

int ua_underworld_script_bindings::prop_get_common(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   Uint16 item_id = static_cast<Uint16>(lua_tonumber(L,-1));

   ua_common_obj_property& prop =
      uw.get_obj_properties().get_common_property(item_id);

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

   return 1;
}

int ua_underworld_script_bindings::prop_get_special(lua_State* L)
{
   return 0;
}


// ui_* functions

int ua_underworld_script_bindings::ui_start_conv(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   Uint32 obj_handle = static_cast<Uint32>(lua_tonumber(L,-1));
   Uint32 level,objpos;

   ua_obj_handle_decode(obj_handle,objpos,level);

   ua_underworld_script_callback* cback = uw.get_scripts().callback;
   if (cback)
      cback->ui_start_conv(level,objpos);

   return 0;
}

int ua_underworld_script_bindings::ui_show_cutscene(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   unsigned int cutscene = static_cast<unsigned int>(lua_tonumber(L,-1));

   ua_underworld_script_callback* cback = uw.get_scripts().callback;
   if (cback)
      cback->ui_show_cutscene(cutscene);

   return 0;
}

int ua_underworld_script_bindings::ui_print_string(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   const char* text = lua_tostring(L,-1);

   ua_underworld_script_callback* cback = uw.get_scripts().callback;
   if (cback)
      cback->ui_print_string(text);

   return 0;
}

int ua_underworld_script_bindings::ui_get_gamestring(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   unsigned int block = static_cast<unsigned int>(lua_tonumber(L,-2));
   unsigned int num = static_cast<unsigned int>(lua_tonumber(L,-1));

   // retrieve game string
   std::string str(uw.get_strings().get_string(block,num));
   lua_pushstring(L,str.c_str());

   return 1;
}

int ua_underworld_script_bindings::ui_show_ingame_anim(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   unsigned int anim = static_cast<unsigned int>(lua_tonumber(L,-1));

   ua_underworld_script_callback* cback = uw.get_scripts().callback;
   if (cback)
      cback->ui_show_ingame_anim(anim);

   return 0;
}

int ua_underworld_script_bindings::ui_cursor_use_item(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   Uint16 item_id = static_cast<Uint16>(lua_tonumber(L,-1));

   ua_underworld_script_callback* cback = uw.get_scripts().callback;
   if (cback)
      cback->ui_cursor_use_item(item_id);

   return 0;
}

int ua_underworld_script_bindings::ui_cursor_target(lua_State* L)
{
   ua_underworld &uw = get_underworld_from_self(L);

   ua_underworld_script_callback* cback = uw.get_scripts().callback;
   if (cback)
      cback->ui_cursor_target();

   return 0;
}


// savegame_* functions

int ua_underworld_script_bindings::savegame_store_value(lua_State* L)
{
   ua_savegame* sg = reinterpret_cast<ua_savegame*>(lua_touserdata(L,-2));
   double value = lua_tonumber(L,-1);

   sg->write32(static_cast<Uint32>(value));
   double fract = value-static_cast<int>(value);
   sg->write16(static_cast<Uint16>( static_cast<Sint16>(32768.0*fract) ));

   return 0;
}

int ua_underworld_script_bindings::savegame_restore_value(lua_State* L)
{
   ua_savegame* sg = reinterpret_cast<ua_savegame*>(lua_touserdata(L,-1));

   double value = static_cast<double>(sg->read32());
   value += static_cast<Sint16>(sg->read16())/32768.0;

   lua_pushnumber(L,value);

   return 1;
}
