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
/*! \file screen.cpp

   \brief screen base implementation

*/

// needed includes
#include "common.hpp"
#include "screen.hpp"
#include "core.hpp"


// ua_screen_ctrl_base methods

void ua_screen_ctrl_base::init(ua_game_core_interface *thecore)
{
   core = thecore;
   cursorx = cursory = 0;
   leftbuttondown = rightbuttondown = false;
}

bool ua_screen_ctrl_base::handle_event(SDL_Event &event)
{
   bool handled = false;

   switch(event.type)
   {
   case SDL_MOUSEMOTION: // mouse has moved
      {
         // calculate cursor position
         int x,y;
         SDL_GetMouseState(&x,&y);
         cursorx = unsigned(double(x)/core->get_screen_width()*320.0);
         cursory = unsigned(double(y)/core->get_screen_height()*200.0);

         handled = mouse_action(false,false,false);
      }
      break;

   case SDL_MOUSEBUTTONDOWN: // mouse button was pressed down
      {
         Uint8 state = SDL_GetRelativeMouseState(NULL,NULL);
         Uint8 state2 = SDL_GetMouseState(NULL,NULL);

         bool left_changed =
            (state&SDL_BUTTON_LMASK) != 0 && !leftbuttondown;

         leftbuttondown = (state&SDL_BUTTON_LMASK) != 0;
         rightbuttondown = (state&SDL_BUTTON_RMASK) != 0;

         handled = mouse_action(true,left_changed,true);
      }
      break;

   case SDL_MOUSEBUTTONUP: // mouse button was released
      {
         Uint8 state = SDL_GetRelativeMouseState(NULL,NULL);
         Uint8 state2 = SDL_GetMouseState(NULL,NULL);

         bool left_changed =
            (state&SDL_BUTTON_LMASK) == 0 && leftbuttondown;

         leftbuttondown = (state&SDL_BUTTON_LMASK) != 0;
         rightbuttondown = (state&SDL_BUTTON_RMASK) != 0;

         handled = mouse_action(true,left_changed,false);
      }
      break;

   default:
      break;
   }

   return handled;
}

bool ua_screen_ctrl_base::mouse_action(bool click, bool left_button, bool pressed)
{
   return false;
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
