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
/*! \file screen.hpp

   base class for user interface screens, e.g. main game screen, conversation screen,
   map screen etc.

*/

// include guard
#ifndef __uwadv_screen_hpp_
#define __uwadv_screen_hpp_

// needed includes
#include "audio.hpp"
#include "texture.hpp"
#include "settings.hpp"
#include "gamestrings.hpp"


// classes

//! user interface abstract base class
class ua_ui_screen_base
{
public:
   //! ctor
   ua_ui_screen_base(){}

   //! dtor
   virtual ~ua_ui_screen_base(){}

   //! sets game core interface
   void set_core(class ua_game_core_interface *gamecore){ core=gamecore; }

   //! initialization
   virtual void init()=0;

   //! clean up
   virtual void done()=0;

   //! handles events
   virtual void handle_event(SDL_Event &event)=0;

   //! renders the screen
   virtual void render()=0;

   //! called on every game tick
   virtual void tick()=0;

protected:
   //! game core interface ptr
   class ua_game_core_interface *core;
};


//! game core abstract base class
class ua_game_core_interface
{
public:
   ua_game_core_interface(){}
   virtual ~ua_game_core_interface(){}

   //! returns screen width
   virtual unsigned int get_screen_width()=0;
   //! returns screen height
   virtual unsigned int get_screen_height()=0;

   //! returns audio interface
   virtual ua_audio_interface &get_audio()=0;

   //! returns game strings
   virtual ua_gamestrings &get_strings()=0;

   //! returns settings object
   virtual ua_settings &get_settings()=0;

   //! returns texture manager object
   virtual ua_texture_manager &get_texmgr()=0;


   //! sets new screen as top screen stack
   virtual void push_screen(ua_ui_screen_base *newscreen)=0;

   //! replaces current screen with new one
   virtual void replace_screen(ua_ui_screen_base *newscreen)=0;

   //! destroys current screen (when finished tick()) and uses the last one
   virtual void pop_screen()=0;
};

#endif
