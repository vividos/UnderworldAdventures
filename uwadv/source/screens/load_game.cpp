/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Underworld Adventures Team

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
/*! \file load_game.cpp

   \brief level map view implementation

*/

// needed includes
#include "common.hpp"
#include "load_game.hpp"
#include "ingame_orig.hpp"
#include <sstream>


// ua_load_game_screen methods

void ua_load_game_screen::init()
{
   // setup orthogonal projection
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0,320,0,200);
   glMatrixMode(GL_MODELVIEW);

   // set OpenGL flags
   glBindTexture(GL_TEXTURE_2D,0);
   glClearColor(140.0/256.0, 104.0/256.0, 84.0/256.0, 0.0);

   glDisable(GL_DEPTH_TEST);

   SDL_ShowCursor(1);

   scroll.init(*core,10,10, 300,180, 25, 42);
   scroll.set_color(1);

   scroll.print("Underworld Adventures");
   scroll.print("Load a Game:");
   scroll.print("");

   // list all games
   ua_savegames_manager& sgmgr = core->get_savegames_mgr();
   sgmgr.rescan();

   unsigned int max = sgmgr.get_savegames_count();
   for(unsigned int i=0; i<max; i++)
   {
      ua_savegame_info info;
      sgmgr.get_savegame_info(i,info);

      std::string filename(sgmgr.get_savegame_filename(i));

      std::ostringstream buffer;

      buffer << i << ". \"" << info.title << "\" (" << info.type
         << ") - " << filename.c_str();

      scroll.print(buffer.str().c_str());
   }

   scroll.print("enter number of game to load: ");
   scroll.enter_input_mode();

   game_nr = -1;
}

void ua_load_game_screen::done()
{
   scroll.done();

   SDL_ShowCursor(0);
}

void ua_load_game_screen::handle_event(SDL_Event &event)
{
   if (scroll.handle_event(event))
   {
      std::string text;
      if (scroll.is_input_done(text))
      {
         // find out game number to load
         game_nr = strtol(text.c_str(),NULL,10);

         if (unsigned(game_nr)>=core->get_savegames_mgr().get_savegames_count())
         {
            game_nr = -1;
            scroll.print("wrong game number");
            scroll.print("enter number of game to load: ");
            scroll.enter_input_mode();
         }
         else
            scroll.print("loading game ...");
      }
      return;
   }
}

void ua_load_game_screen::render()
{
   glClear(GL_COLOR_BUFFER_BIT);

   glColor3ub(255,255,255);
   scroll.render();
}

void ua_load_game_screen::tick()
{
   if (game_nr>=0)
   {
      ua_savegames_manager& sgmgr = core->get_savegames_mgr();
      ua_savegame sg = sgmgr.get_savegame_load(static_cast<unsigned int>(game_nr));
      core->get_underworld().load_game(sg);

      // go to ingame screen
      core->replace_screen(new ua_ingame_orig_screen);
   }
}
