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
/*! \file message_inl.cpp

   \brief screen message processing implementation

*/

// needed includes
#include "common.hpp"
#include "message.hpp"


// ua_message_processor methods

template <typename T>
void ua_message_processor<T>::message_init(unsigned int screen_width,
   unsigned int screen_height)
{
   scrwidth = screen_width; scrheight = screen_height;
}

template <typename T>
void ua_message_processor<T>::register_area_handler(ua_msg_area_coord& coords,
   ua_msg_area_handler_func_type handler)
{
   area_map.insert(
      std::make_pair<ua_msg_area_coord,ua_msg_area_handler_func_type>(
         coords,handler));
}

template <typename T>
void ua_message_processor<T>::register_keymap_handler(
   ua_keymap* keymap,
   ua_msg_keymap_handler_func_type handler)
{
   keymap_handler = handler;
}

template <typename T>
void ua_message_processor<T>::message_handle_event(SDL_Event& event)
{
   if (event.type != SDL_MOUSEMOTION &&
       event.type != SDL_MOUSEBUTTONDOWN &&
       event.type != SDL_MOUSEBUTTONUP &&
       event.type != SDL_KEYDOWN &&
       event.type != SDL_KEYUP)
      return;

   if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
   {
      call_keymap_handler(ua_dummy,event,keymap_handler);
   }
   else
   {
      // calculate cursor position
      unsigned int cursorx,cursory;
      {
         int x,y;
         SDL_GetMouseState(&x,&y);

         cursorx = unsigned(double(x)/scrwidth*320.0);
         cursory = unsigned(double(y)/scrheight*200.0);
      }

      // search for area handler
      ua_msg_area_handler_func_type handler = NULL;
      {
         typename ua_msg_area_map::iterator iter,stop;
         iter = area_map.begin();
         stop = area_map.end();

         for(;iter != stop; iter++)
            if (iter->first.is_in_area(cursorx,cursory))
            {
               handler = iter->second;
               break;
            }
      }

      if (handler == NULL)
         return;

      // check for event type
      switch(event.type)
      {
      case SDL_MOUSEMOTION: // mouse has moved
         {
            call_area_handler(false,false,false,handler);
         }
         break;

      case SDL_MOUSEBUTTONDOWN: // mouse button was pressed down
      case SDL_MOUSEBUTTONUP: // mouse button was released
         {
            Uint8 state = SDL_GetRelativeMouseState(NULL,NULL);
            Uint8 state2 = SDL_GetMouseState(NULL,NULL);

            bool left_changed =
               (state&SDL_BUTTON_LMASK) != 0 && !leftbuttondown;

            leftbuttondown = (state&SDL_BUTTON_LMASK) != 0;
            rightbuttondown = (state&SDL_BUTTON_RMASK) != 0;

            call_area_handler(true,left_changed,event.type==SDL_MOUSEBUTTONDOWN,handler);
         }
         break;

      default:
         break;
      }
   }
}
