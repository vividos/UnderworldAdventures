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
/*! \file screen.cpp

   \brief screen base implementation

*/

// needed includes
#include "common.hpp"
#include "screen.hpp"


// ua_screen methods

ua_screen::ua_screen()
{
}

ua_screen::~ua_screen()
{
}

void ua_screen::init()
{
   ua_window::create(0,0,320,200);
}

void ua_screen::destroy()
{
   // destroy all subwindows
   unsigned int max = subwindows.size();
   for(unsigned int i=0; i<max; i++)
      subwindows[i]->destroy();
}

void ua_screen::draw()
{
   // draw all subwindows
   unsigned int max = subwindows.size();
   for(unsigned int i=0; i<max; i++)
      subwindows[i]->draw();
}

bool ua_screen::process_event(SDL_Event& event)
{
   // send event to all subwindows
   {
      unsigned int max = subwindows.size();
      for(unsigned int i=0; i<max; i++)
      {
         if (subwindows[i]->process_event(event))
            break; // no further processing
      }
   }

   // check if a mouse event occured
   if (event.type == SDL_MOUSEMOTION ||
       event.type == SDL_MOUSEBUTTONDOWN ||
       event.type == SDL_MOUSEBUTTONUP)
   {
      // get coordinates
      unsigned int xpos = event.type == SDL_MOUSEMOTION ? event.motion.x : event.button.x;
      unsigned int ypos = event.type == SDL_MOUSEMOTION ? event.motion.y : event.button.y;

      // convert to 320x200 screen coordinates
      SDL_Surface* surf = SDL_GetVideoSurface();

      xpos = unsigned(xpos * 320.0 / surf->w);
      ypos = unsigned(ypos * 200.0 / surf->h);

      // first, send mouse event to main screen window
      bool left_button = event.type != SDL_MOUSEMOTION &&
         ((event.button.button & SDL_BUTTON_LEFT) != 0);

      mouse_event(event.type != SDL_MOUSEMOTION,
         left_button,
         event.type == SDL_MOUSEBUTTONDOWN,
         xpos,ypos);

      // send event to subwindows that are in that area
      unsigned int max = subwindows.size();
      for(unsigned int i=0; i<max; i++)
      {
         ua_window& wnd = *subwindows[i];

         // mouse in area?
         if (xpos > wnd.get_xpos() && xpos < wnd.get_xpos()+wnd.get_width() &&
             ypos > wnd.get_ypos() && ypos < wnd.get_ypos()+wnd.get_height())
         {
            wnd.mouse_event(event.type != SDL_MOUSEMOTION,
               left_button,
               event.type == SDL_MOUSEBUTTONDOWN,
               xpos,ypos);
         }
      }
   }

   return true;
}

void ua_screen::tick()
{
}

void ua_screen::register_window(ua_window* window)
{
   subwindows.push_back(window);
}


/*

// ua_screen_ctrl_base methods

void ua_screen_ctrl_base::init(ua_game_core_interface *thecore)
{
   core = thecore;
   cursorx = cursory = 0;
   leftbuttondown_old = rightbuttondown_old = false;
}

void ua_screen_ctrl_base::handle_event(SDL_Event& event)
{
   switch(event.type)
   {
   case SDL_MOUSEMOTION: // mouse has moved
      {
         // calculate cursor position
         int x,y;
         SDL_GetMouseState(&x,&y);
         cursorx = unsigned(double(x)/core->get_screen_width()*320.0);
         cursory = unsigned(double(y)/core->get_screen_height()*200.0);

         mouse_action(false,false,false);
      }
      break;

   case SDL_MOUSEBUTTONDOWN: // mouse button was pressed down
      {
         Uint8 state = SDL_GetRelativeMouseState(NULL,NULL);
         Uint8 state2 = SDL_GetMouseState(NULL,NULL);

         bool left_changed =
            (state&SDL_BUTTON_LMASK) != 0 && !leftbuttondown_old;

         leftbuttondown_old = (state&SDL_BUTTON_LMASK) != 0;
         rightbuttondown_old = (state&SDL_BUTTON_RMASK) != 0;

         mouse_action(true,left_changed,true);
      }
      break;

   case SDL_MOUSEBUTTONUP: // mouse button was released
      {
         Uint8 state = SDL_GetRelativeMouseState(NULL,NULL);
         Uint8 state2 = SDL_GetMouseState(NULL,NULL);

         bool left_changed =
            (state&SDL_BUTTON_LMASK) == 0 && leftbuttondown_old;

         leftbuttondown_old = (state&SDL_BUTTON_LMASK) != 0;
         rightbuttondown_old = (state&SDL_BUTTON_RMASK) != 0;

         mouse_action(true,left_changed,false);
      }
      break;

   default:
      break;
   }
}

void ua_screen_ctrl_base::mouse_action(bool click, bool left_button, bool pressed)
{
}

unsigned int ua_screen_ctrl_base::get_area(
   const ua_screen_area_data* table,
   unsigned int xpos,unsigned int ypos)
{
   // search for area that first matches the coordinate range
   unsigned int idx=0;
   while(table[idx].area_id != ua_area_none)
   {
      // check ranges
      if (xpos >= table[idx].xmin && xpos <= table[idx].xmax &&
          ypos >= table[idx].ymin && ypos <= table[idx].ymax)
         break;

      idx++;
   }

   return table[idx].area_id;
}
*/
