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
/*! \file save_game.hpp

   \brief level map view

*/

// include guard
#ifndef __uwadv_save_game_hpp_
#define __uwadv_save_game_hpp_

// needed includes
#include "screen.hpp"
#include "textscroll.hpp"


// classes

//! level map view screen class
class ua_save_game_screen: public ua_ui_screen_base
{
public:
   //! ctor
   ua_save_game_screen(){}

   //! dtor
   virtual ~ua_save_game_screen(){}

   // virtual functions from ua_ui_screen_base

   virtual void init();
   virtual void done();
   virtual void handle_event(SDL_Event &event);
   virtual void render();
   virtual void tick();

protected:
   //! text scroll
   ua_textscroll scroll;

   //! number of game to load
   int game_nr;

   //! indicates to write a new game
   bool write_new;

   bool have_desc;

   std::string savegame_desc;
};

#endif
