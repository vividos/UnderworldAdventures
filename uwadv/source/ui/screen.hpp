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
/*! \file screen.hpp

   \brief user interface screens base classes

   base class for user interface screens, e.g. main game screen, conversation screen,
   map screen etc.
*/
//! \ingroup userinterface

//@{

// include guard
#ifndef uwadv_screen_hpp_
#define uwadv_screen_hpp_

// needed includes
#include "window.hpp"
#include "game_interface.hpp"
#include "keymap.hpp"


// classes

//! screen base class
class ua_screen: public ua_window
{
public:
   //! ctor
   ua_screen();
   //! dtor
   virtual ~ua_screen();

   //! inits the screen
   virtual void init();

   //! destroys window
   virtual void destroy();

   //! draws window contents
   virtual void draw();

   //! processes SDL events; returns true when event shouldn't processed further
   virtual bool process_event(SDL_Event& event);

   //! processes key event
   virtual void key_event(bool key_down, ua_key_value key);

   //! called every game tick
   virtual void tick();

   //! registers a window as subwindow
   void register_window(ua_window* window);

   //! registers a keymap
   void register_keymap(ua_keymap* keymap);

   //! sets pointer to game interface
   void set_game_interface(ua_game_interface* game_interface);

protected:
   //! list of all subwindows controlled by the screen
   std::vector<ua_window*> subwindows;

   //! game interface pointer
   ua_game_interface* game;

   //! screen keymap
   ua_keymap* scr_keymap;
};


// inline methods

inline void ua_screen::set_game_interface(ua_game_interface* game_interface)
{
   game = game_interface;
}


#endif
//@}
