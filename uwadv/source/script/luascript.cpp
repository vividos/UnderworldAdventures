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


// constants

const char* ua_lua_scripting::self_name = "_scripting_self";


// ua_lua_scripting methods

void ua_lua_scripting::init(ua_game_interface* the_game)
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
   //! \todo remove again
   ua_player& pl = game->get_underworld().get_player();
   //pl.set_pos(32.0, 2.0);
   pl.set_pos(13.5, 53.5);
   pl.set_height(96.0);

   lua_getglobal(L,"game_init_new");
   checked_call(0,0);
}

void ua_lua_scripting::eval_critter(unsigned int pos)
{
   lua_getglobal(L,"critter_eval");
   lua_pushnumber(L, static_cast<double>(pos));
   checked_call(1,0);
}

void ua_lua_scripting::do_trigger(unsigned int pos)
{
   lua_getglobal(L,"trigger_do");
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
      lua_getglobal(L,"wall_look");
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

#define lua_ex_register_table(L, n, f) { \
   lua_pushstring(L, n); lua_pushcfunction(L, f); lua_settable(L,-3); }

void ua_lua_scripting::register_functions()
{
   // ui object
   lua_newtable(L);

   lua_ex_register_table(L, "print", ui_print);

   lua_setglobal(L, "ui");
}

int ua_lua_scripting::ui_print(lua_State* L)
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
