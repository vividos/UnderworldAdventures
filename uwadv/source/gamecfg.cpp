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
/*! \file gamecfg.cpp

   \brief game config loader

*/

// needed includes
#include "common.hpp"
#include "gamecfg.hpp"


// ua_gamecfg_loader methods

void ua_gamecfg_loader::init(ua_game_core_interface* thecore)
{
   core = thecore;
}

void ua_gamecfg_loader::load_value(const std::string& name, const std::string& value)
{
   if (name.compare("game-name")==0)
   {
      game_name = value;
   }
   else
   if (name.compare("lua-script")==0)
   {
      // load given lua script name
      lua_State* L = core->get_underworld().get_scripts().get_lua_State();
      core->get_filesmgr().load_lua_script(L,value.c_str());
   }
   else
   if (name.compare("use-resources")==0)
   {
      ua_settings &settings = core->get_settings();

      // check if resources to use are available
      if (value.compare("uw1")==0)
      {
            // TODO check if all game files are available here
            // TODO delete setting ua_setting_uw1_avail, not needed then
            if (!settings.get_bool(ua_setting_uw1_avail))
               throw ua_exception("could not find relevant uw1 game files");

         // select proper game type
         if (settings.get_bool(ua_setting_uw1_is_uw_demo))
            settings.set_gametype(ua_game_uw_demo);
         else
            settings.set_gametype(ua_game_uw1);

         // set generic uw path
         settings.set_value(ua_setting_uw_path,settings.get_string(ua_setting_uw1_path));
      }
      else
      if (value.compare("uw2")==0)
      {
         // set generic uw path
         settings.set_value(ua_setting_uw_path,settings.get_string(ua_setting_uw2_path));
         settings.set_gametype(ua_game_uw2);
      }
      else
      {
         // unknown string
         std::string text("unknown resources string in game.cfg: ");
         text.append(value);
         throw ua_exception(text.c_str());
      }
   }
   else
   if (name.compare("import-strings")==0)
   {
      // load game strings
      SDL_RWops* gstr = core->get_filesmgr().get_uadata_file(value.c_str());

      // TODO check if gstr == NULL

      core->get_strings().load(gstr);
   }
}
