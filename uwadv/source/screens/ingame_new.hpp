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
/*! \file ingame_new.hpp

   \brief new ingame screen user interface

*/

// include guard
#ifndef uwadv_ingame_new_hpp_
#define uwadv_ingame_new_hpp_

// needed includes
#include "screen.hpp"
#include "mousecursor.hpp"


// classes

//! user interface abstract base class
class ua_ingame_new_screen: public ua_screen
//, public ua_underworld_script_callback
{
public:
   //! ctor
   ua_ingame_new_screen(ua_game_interface& game):ua_screen(game){}
   //! dtor
   virtual ~ua_ingame_new_screen(){}

   // virtual functions from ua_screen
   virtual void init();
   virtual void destroy();
   virtual void draw();
   virtual bool process_event(SDL_Event& event);
   virtual void key_event(bool key_down, ua_key_value key);
   virtual void tick();

protected:
   //! keymap
   ua_keymap keymap;


   //! mouse cursor
   ua_mousecursor mousecursor;


   //! indicates if there is a movement key pressed
   bool move_turn_left, move_turn_right,
      move_walk_forward, move_run_forward, move_walk_backwards;

   //! tickcount used for time bookkeeping
   unsigned int tickcount;

};


#endif
