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
/*! \file uwadv_menu.cpp

   \brief uwadv menu implementation

*/

// needed includes
#include "common.hpp"
#include "uwadv_menu.hpp"
#include "settings.hpp"
#include "renderer.hpp"
#include "files.hpp"
#include "start_splash.hpp"
#ifdef HAVE_DEBUG
 #include "screens/start_menu.hpp"
 #include "screens/save_game.hpp"
// #include "screens/ingame_new.hpp"
 #include "screens/ingame_orig.hpp"
 #include "underworld.hpp"
#endif


// ua_uwadv_menu_screen methods

void ua_uwadv_menu_screen::init()
{
   ua_screen::init();

   ua_trace("uwadv menu screen started\n");

   // init 2d camera
   game.get_renderer().setup_camera2d();
   glEnable(GL_TEXTURE_2D);

   // load texture
   SDL_RWops* rwops = game.get_files_manager().get_uadata_file("uwadv-loading.tga");

   if (rwops != NULL)
   {
      tex.init();
      tex.load(rwops);
      tex.upload(0);
   }

   rendered = false;
}
/*
void ua_uwadv_menu_screen::destroy()
{
}
*/
void ua_uwadv_menu_screen::draw()
{
   tex.use();

   // set wrap parameter
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

   // set mipmap parameter
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   const unsigned width = 150;
   const unsigned height = 75;
   const unsigned xpos = (320-width)/2;
   const unsigned ypos = (200-height)/2;

   // draw quad
   glBegin(GL_QUADS);
   glTexCoord2d(0.0, 0.0); glVertex2i(xpos,       ypos);
   glTexCoord2d(1.0, 0.0); glVertex2i(xpos+width, ypos);
   glTexCoord2d(1.0, 1.0); glVertex2i(xpos+width, ypos+height);
   glTexCoord2d(0.0, 1.0); glVertex2i(xpos,       ypos+height);
   glEnd();

   rendered = true;
}
/*
void ua_uwadv_menu_screen::process_event(SDL_Event& event)
{
}
*/
void ua_uwadv_menu_screen::tick()
{
   if (!rendered)
      return;

   ua_settings& settings = game.get_settings();

   // set game prefix to use
   std::string prefix("uw1");
   settings.set_value(ua_setting_game_prefix,prefix);

   ua_trace("selected game: %s\n\n",
      settings.get_gametype() == ua_game_uw1 ? "uw1" :
      settings.get_gametype() == ua_game_uw_demo ? "uw_demo" : "uw2");

   // set generic uw path
   settings.set_value(ua_setting_uw_path,settings.get_string(ua_setting_uw1_path));

   // now that we know the generic uw path, we can init the whole game stuff
   game.init_game();

#ifdef HAVE_DEBUG
   game.get_underworld().import_savegame(game.get_settings(),"data/",true);
   game.replace_screen(new ua_ingame_orig_screen(game),false);

/*
   paste one:
   --------------------
   game.get_underworld().import_savegame(game.get_settings(),"data/",true);
   game.replace_screen(new ua_ingame_orig_screen(game),false);
   --------------------
   game.replace_screen(new ua_save_game_screen(game,true),false);
   --------------------
   game.replace_screen(new ua_start_menu_screen(game),false);
   --------------------
   game.replace_screen(new ua_start_splash_screen(game),false);
   --------------------
   game.get_underworld().import_savegame(game.get_settings(),"data/",true);
   game.get_underworld().get_scripts().lua_started_newgame();
   game.replace_screen(new ua_ingame_orig_screen(game),false);
   --------------------
*/
#else

   // for now, immediately start splash screen
   game.replace_screen(new ua_start_splash_screen(game),false);

#endif
}
