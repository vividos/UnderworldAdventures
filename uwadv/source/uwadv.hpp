/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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

   game class definition

*/

// include guard
#ifndef __uwadv_uwadv_hpp_
#define __uwadv_uwadv_hpp_

// needed includes
#include "settings.hpp"
#include "texture.hpp"
#include "gamestrings.hpp"
#include "audio.hpp"
#include "screen.hpp"


// main game class

class ua_game: public ua_game_core_interface
{
public:
   //! ctor
   ua_game();

   //! initializes video, audio, etc.
   virtual void init();

   //! runs the game's main loop
   void run();

   //! cleans up all stuff
   virtual void done();

protected:
   //! controls how many ticks per second are done
   const unsigned int tickrate;

   //! screen width
   unsigned int width;

   //! screen height
   unsigned int height;

   //! game configuration
   ua_settings settings;

   //! texture manager
   ua_texture_manager texmgr;

   //! indicates if game can be exited
   bool exit_game;

   //! hey, it's all the game strings!
   ua_gamestrings gstr;

   //! audio interface
   ua_audio_interface *audio;

   //! current user interface screen
   ua_ui_screen_base *screen;

   //! stack of user interface screens
   std::vector<ua_ui_screen_base*> screenstack;

   //! screen queued to destroy
   ua_ui_screen_base *screen_to_destroy;

protected:
   // ua_game_core_interface virtual methods

   virtual unsigned int get_screen_width(){ return width; };
   virtual unsigned int get_screen_height(){ return height; };
   virtual ua_audio_interface &get_audio(){ return *audio; }
   virtual ua_gamestrings &get_strings(){ return gstr; };
   virtual ua_settings &get_settings(){ return settings; };
   virtual ua_texture_manager &get_texmgr(){ return texmgr; };

   virtual void push_screen(ua_ui_screen_base *newscreen);
   virtual void replace_screen(ua_ui_screen_base *newscreen);
   virtual void pop_screen();

private:
   //! sets up general OpenGL stuff
   void setup_opengl();

   //! processes SDL events
   void process_events();

   //! called when receiving a system dependent message
   virtual void system_message(SDL_SysWMEvent &syswm){}

   //! handles keyboard events
   void handle_key_down(SDL_keysym *keysym);

   //! renders the screen
   void draw_screen();

   //! does the game logic
   void game_logic();
};

#endif
