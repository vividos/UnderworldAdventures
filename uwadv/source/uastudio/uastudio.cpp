/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2004,2005 Underworld Adventures Team

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
/*! \file uastudio.cpp

   \brief underworld adventures studio main

*/

// needed includes
#include "common.hpp"
#include "base/settings.hpp"
#include "base/files.hpp"
#include "script/scripting.hpp"
#include "underw/underworld.hpp"
#include "game_interface.hpp"
#include "gamestrings.hpp"
#include "debug.hpp"
#include "gamecfg.hpp"
#include <ctime>


// to have console output, use a genuine main(), not the SDL_main one
#ifdef HAVE_CONSOLE
#undef main
#endif


class uastudio: public ua_basic_game_interface
{
public:
   //! ctor
   uastudio(){}

   void init();
   void done();

   void run();

   // virtual ua_basic_game_interface methods
   virtual double get_tickrate(){ return 20.0; }
   virtual bool pause_game(bool pause);
   virtual ua_settings& get_settings(){ return settings; }
   virtual ua_files_manager& get_files_manager(){ return files_manager; }
   virtual ua_savegames_manager& get_savegames_manager(){ return savegames_manager; };
   virtual ua_scripting& get_scripting(){ return *scripting; }
   virtual ua_debug_server& get_debugger(){ return debug_server; }
   virtual ua_gamestrings& get_gamestrings(){ return gamestrings; };
   virtual ua_underworld& get_underworld(){ return underworld; }
   virtual void init_game();
   virtual void done_game();

protected:
   ua_settings settings;
   ua_files_manager files_manager;
   ua_savegames_manager savegames_manager;

   ua_underworld underworld;
   ua_scripting* scripting;
   ua_debug_server debug_server;
   ua_gamestrings gamestrings;

   //! indicates if the game is currently paused
   bool paused;
};



void uastudio::init()
{
   ua_trace("Underworld Adventures Studio\n\n");

   // init random number generator
   srand((unsigned)time(NULL));

   // init files manager; settings are loaded here, too
   files_manager.init(settings);

   // init savegames manager
   savegames_manager.init(settings);

   // set uw1 as path
   std::string prefix("uw1");
   settings.set_value(ua_setting_uw_path, settings.get_string(ua_setting_uw1_path));
   settings.set_value(ua_setting_game_prefix, prefix);

   // init game components; uw prefix and path is now known
   init_game();

   // init debug server and start
   debug_server.init();
   debug_server.start_debugger(this);


   paused = true;
}

void uastudio::done()
{
   done_game();

   debug_server.shutdown();

   SDL_Quit();
}

void uastudio::run()
{
   ua_trace("\nuastudio main loop started\n");

   Uint32 now, then = SDL_GetTicks();

   bool exit_game = false;

   paused = true;

   const double tickrate = 20.0;
   unsigned int tickcount = 0;

   // main game loop
   while(!exit_game)
   {
      now = SDL_GetTicks();

      while ((now - then) > (1000.0/tickrate))
      {
         then += Uint32(1000.0/tickrate);

         // do game logic
         if (!paused)
         {
            underworld.eval_underworld(double(tickcount)/tickrate);

            tickcount++;
         }

         // check if debugger is still running
         if (!debug_server.is_debugger_running())
            exit_game = true;
      }

      // do server side debug processing
      debug_server.tick();

      Sleep(10);
   }

   ua_trace("uastudio main loop ended\n\n");
}

bool uastudio::pause_game(bool pause)
{
   bool old_paused = paused;
   paused = pause;
   return old_paused;
}

void uastudio::init_game()
{
   std::string prefix(settings.get_string(ua_setting_game_prefix));

   ua_trace("initializing game; prefix: %s\n", prefix.c_str());

   // load game config file
   std::string gamecfg_name(prefix);
   gamecfg_name.append("/game.cfg");

   // set new game prefix
   savegames_manager.set_game_prefix(prefix.c_str());

   // try to load %prefix%/game.cfg
   {
      ua_gamecfg_loader cfgloader(*this, &scripting);

      SDL_RWops* gamecfg = files_manager.get_uadata_file(gamecfg_name.c_str());

      // no game.cfg found? too bad ...
      if (gamecfg == NULL)
      {
         std::string text("could not find game.cfg for game prefix ");
         text.append(prefix.c_str());
         throw ua_exception(text.c_str());
      }

      cfgloader.load(gamecfg);
   }

   // init underworld
   underworld.init(settings,files_manager);
   underworld.set_scripting(scripting);

   // init game strings
   gamestrings.init(settings);

   ua_trace("using generic uw-path: %s\n",
      settings.get_string(ua_setting_uw_path).c_str());

   // TODO: load savegame
   {
      // load initial game
      ua_uw_import import;
      import.load_underworld(get_underworld(), get_settings(), "data/", true);

      // init new game
      get_scripting().init_new_game();
   }
}

void uastudio::done_game()
{
   underworld.done();

   if (scripting != NULL)
   {
      scripting->done();
      delete scripting;
      scripting = NULL;
   }
}


//! uastudio main function
int main(int /*argc*/, char* /*argv*/[])
{
#ifndef HAVE_DEBUG // in debug mode the debugger catches exceptions
   try
#endif
   {
      uastudio studio;

      studio.init();
      studio.run();
      studio.done();
   }
#ifndef HAVE_DEBUG
   catch (ua_exception& e)
   {
      std::string text("caught ua_exception:\n\r");
      text.append(e.what());

//      studio.error_msg(text.c_str());
   }
   catch (std::exception& e)
   {
      e;
      ua_trace("caught std::exception\n");
//      studio.error_msg("std::exception");
   }
#endif

   return 0;
}
