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
/*! \file start_splash.cpp

   a

*/

// needed includes
#include "common.hpp"
#include "start_splash.hpp"
#include "ingame_orig.hpp"


// ua_start_splash_screen methods

void ua_start_splash_screen::init()
{
   tickcount=10*20;
   core->get_audio().start_music(1);
}

void ua_start_splash_screen::done()
{
}

void ua_start_splash_screen::handle_event(SDL_Event &event)
{
   switch(event.type)
   {
   case SDL_KEYDOWN:
   case SDL_MOUSEBUTTONDOWN:
      // ends start screen
      tickcount=0;
      break;
   }
}

void ua_start_splash_screen::render()
{
   // clear color and depth buffers
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void ua_start_splash_screen::tick()
{
   glClearColor(0,0,(200-tickcount)/200.f,0);
   if (tickcount--==0)
   {
      core->replace_screen(new ua_ingame_orig_screen);
   }
}
