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


// ua_load_game_screen methods

void ua_load_game_screen::init()
{
}

void ua_load_game_screen::done()
{
}

void ua_load_game_screen::handle_event(SDL_Event &event)
{
}

void ua_load_game_screen::render()
{
}

void ua_load_game_screen::tick()
{
   // go to ingame screen
   core->replace_screen(new ua_ingame_orig_screen);
}
