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
/*! \file map_view.cpp

   \brief level map view implementation

*/

// needed includes
#include "common.hpp"
#include "map_view.hpp"


// ua_map_view_screen methods
/*
void ua_map_view_screen::init(ua_game_core_interface* thecore)
{
   ua_ui_screen_base::init(thecore);
}

void ua_map_view_screen::done()
{
}

bool ua_map_view_screen::handle_event(SDL_Event &event)
{
   return false;
}

void ua_map_view_screen::render()
{
}
*/
void ua_map_view_screen::tick()
{
   core->pop_screen();
}
