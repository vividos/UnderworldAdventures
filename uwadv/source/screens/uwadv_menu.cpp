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
/*! \file uwadv_menu.cpp

   \brief uwadv menu implementation

*/

// needed includes
#include "common.hpp"
#include "uwadv_menu.hpp"
#include "start_splash.hpp"
#ifdef HAVE_DEBUG
 #include "screens/start_menu.hpp"
 #include "screens/ingame_orig.hpp"
#endif


// ua_uwadv_menu_screen methods

void ua_uwadv_menu_screen::init(ua_game_core_interface* thecore)
{
   ua_ui_screen_base::init(thecore);

   ua_trace("uwadv menu screen started\n");

   ua_settings &settings = core->get_settings();

   // set game prefix to use
   std::string prefix("uw1");
   settings.set_value(ua_setting_game_prefix,prefix);

   ua_trace("selected game: %s\n\n",
      settings.get_gametype() == ua_game_uw1 ? "uw1" :
      settings.get_gametype() == ua_game_uw_demo ? "uw_demo" : "uw2");

   // now that we know the generic uw path, we can init the core stuff
   core->init_game();
}
/*
void ua_uwadv_menu_screen::done()
{
}

void ua_uwadv_menu_screen::handle_event(SDL_Event &event)
{
}

void ua_uwadv_menu_screen::render()
{
}
*/
void ua_uwadv_menu_screen::tick()
{
#ifdef HAVE_DEBUG

#if 0
//   core->push_screen(new ua_start_splash_screen);
   core->push_screen(new ua_start_menu_screen);
#else
   core->get_underworld().import_savegame(core->get_settings(),"data/",true);
   core->get_underworld().get_scripts().lua_started_newgame();

   core->push_screen(new ua_ingame_orig_screen);
#endif

#else
   // for now, immediately start splash screen
   core->replace_screen(new ua_start_splash_screen);
#endif
}
