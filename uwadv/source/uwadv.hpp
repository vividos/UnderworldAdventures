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
/*! \file uwadv.hpp

   \brief game class definition

*/

// include guard
#ifndef uwadv_uwadv_hpp_
#define uwadv_uwadv_hpp_

// needed includes
#include "settings.hpp"
#include "files.hpp"
#include "screen.hpp"
#include "audio/audio.hpp"
#include "renderer/renderer.hpp"
#include "script/script.hpp"
#include "underworld.hpp"
#include "game_interface.hpp"


// classes

//! main game class
class ua_uwadv_game: public ua_game_interface
{
public:
   //! ctor
   ua_uwadv_game();
   //! dtor
   virtual ~ua_uwadv_game(){}

   //! initializes video, audio, etc.
   virtual void init();

   //! parses command line arguments
   void parse_args(unsigned int argc, const char** argv);

   //! runs the game's main loop
   void run();

   //! cleans up all stuff
   virtual void done();

   //! shows the user an error message
   virtual void error_msg(const char* msg);

   // ua_game_interface methods

   virtual double get_tickrate();
   virtual void init_game();
   virtual ua_audio_manager& get_audio_manager();
   virtual ua_settings& get_settings();
   virtual ua_files_manager& get_files_manager();
   virtual ua_savegames_manager& get_savegames_manager();
   virtual ua_renderer& get_renderer();
   virtual ua_scripting& get_scripting();
   virtual ua_underworld& get_underworld();
   virtual void replace_screen(ua_screen* new_screen, bool save_current);
   virtual void remove_screen();

protected:
   //! initializes SDL and creates a window
   void init_sdl();

   //! processes SDL events
   void process_events();

   //! deletes current screen and pops off last screen from screenstack
   void pop_screen();

protected:
   //! resource files manager
   ua_files_manager files_manager;

   //! game configuration
   ua_settings settings;

   //! screen width
   unsigned int width;

   //! screen height
   unsigned int height;

   //! controls how many ticks per second are done
   const unsigned int tickrate;

   //! indicates if game can be exited
   bool exit_game;

   //! action to carry out after init
   /*! actions: 0=normal game start, 1=load savegame, 2=load custom game */
   unsigned int init_action;

   //! savegame to load
   std::string savegame_name;

   //! current screen
   ua_screen* curscreen;

   //! stack of screens
   std::vector<ua_screen*> screenstack;

   //! true when tick timer should be resetted for the next cycle
   bool reset_tick_timer;

   //! audio manager object
   ua_audio_manager audio_manager;

   //! savegames manager
   ua_savegames_manager savegames_manager;

   //! renderer class
   ua_renderer renderer;

   //! scripting class
   ua_scripting scripting;

   //! underworld object
   ua_underworld underworld;

   //! screen queued to destroy
   ua_screen* screen_to_destroy;

/*
   //! texture manager
   ua_texture_manager texmgr;

   //! critter pool
   ua_critter_pool critter_pool;

   //! 3d models manager
   ua_model3d_manager model_manager;

   //! debug interface
   ua_debug_interface* debug;
*/
};

// inline methods

inline double ua_uwadv_game::get_tickrate()
{
   return tickrate;
}

inline ua_audio_manager& ua_uwadv_game::get_audio_manager()
{
   return audio_manager;
}

inline ua_settings& ua_uwadv_game::get_settings()
{
   return settings;
}

inline ua_files_manager& ua_uwadv_game::get_files_manager()
{
   return files_manager;
}

inline ua_savegames_manager& ua_uwadv_game::get_savegames_manager()
{
   return savegames_manager;
}

inline ua_renderer& ua_uwadv_game::get_renderer()
{
   return renderer;
}

inline ua_scripting& ua_uwadv_game::get_scripting()
{
   return scripting;
}

inline ua_underworld& ua_uwadv_game::get_underworld()
{
   return underworld;
}


// macros

#define ua_uwadv_game_implement(gameclass) \
   ua_uwadv_game* ua_game_create(){ \
      return new gameclass; \
   }


#endif
