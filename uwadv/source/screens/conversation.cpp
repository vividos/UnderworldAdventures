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
/*! \file conversation.cpp

   \brief conversation screen implementation

*/

// needed includes
#include "common.hpp"
#include "conversation.hpp"


// ua_conversation_screen methods

void ua_conversation_screen::init()
{
}

void ua_conversation_screen::done()
{
}

void ua_conversation_screen::handle_event(SDL_Event &event)
{
}

void ua_conversation_screen::render()
{
}

void ua_conversation_screen::tick()
{
   core->pop_screen();
}
