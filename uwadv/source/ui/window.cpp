/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2003 Underworld Adventures Team

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
/*! \file window.cpp

   \brief window base class implementation

*/

// needed includes
#include "common.hpp"
#include "window.hpp"


// ua_window methods

ua_window::ua_window()
:wnd_xpos(0), wnd_ypos(0), wnd_width(0), wnd_height(0)
{
}

ua_window::~ua_window()
{
}

void ua_window::create(unsigned int xpos, unsigned int ypos,
   unsigned int width, unsigned int height)
{
   wnd_xpos = xpos;
   wnd_ypos = ypos;
   wnd_width = width;
   wnd_height = height;
}

void ua_window::destroy()
{
}

void ua_window::draw()
{
}

bool ua_window::process_event(SDL_Event& event)
{
   return false;
}

/*! note: mousex and mousey parameters are in virtual screen coordinates,
    where the screen is a 320x200 pixel screen, with the origin in the upper
    left. */
void ua_window::mouse_event(bool button_clicked, bool left_button,
   bool button_down, unsigned int mousex, unsigned int mousey)
{
}

bool ua_window::in_window(unsigned int xpos, unsigned int ypos)
{
   return (xpos >= wnd_xpos) && (xpos < wnd_xpos+wnd_width) &&
          (ypos >= wnd_ypos) && (ypos < wnd_ypos+wnd_height);
}

/*! note: the necessary fields in event are only set if it holds an event of
    type SDL_MOUSEMOTION, SDL_MOUSEBUTTONDOWN or SDL_MOUSEBUTTONUP */
void ua_window::calc_mousepos(SDL_Event& event, unsigned int& xpos, unsigned int& ypos)
{
   // get coordinates
   xpos = event.type == SDL_MOUSEMOTION ? event.motion.x : event.button.x;
   ypos = event.type == SDL_MOUSEMOTION ? event.motion.y : event.button.y;

   // convert to 320x200 screen coordinates
   SDL_Surface* surf = SDL_GetVideoSurface();

   if (surf != NULL)
   {
      xpos = unsigned(xpos * 320.0 / surf->w);
      ypos = unsigned(ypos * 200.0 / surf->h);
   }
   else
      xpos = ypos = 0;
}
