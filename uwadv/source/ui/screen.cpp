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

ua_screen::ua_screen(ua_game_interface& game_interface)
:game(game_interface),scr_keymap(NULL)
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
      unsigned int xpos,ypos;
      calc_mousepos(event, xpos, ypos);

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
         if (wnd.in_window(xpos,ypos))
         {
            wnd.mouse_event(event.type != SDL_MOUSEMOTION,
               left_button,
               event.type == SDL_MOUSEBUTTONDOWN,
               xpos,ypos);
         }
      }
   }

   // key event
   if (scr_keymap != NULL && (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP))
   {
      // mask out numlock mod key
      Uint16 mod = event.key.keysym.mod & ~KMOD_NUM;

      // be sure to set left and right keys for alt and ctrl
      if ((mod & KMOD_ALT) != 0) mod |= KMOD_ALT;
      if ((mod & KMOD_CTRL) != 0) mod |= KMOD_CTRL;

      // make keymod value
      Uint32 keymod = ua_make_keymod(event.key.keysym.sym, mod);

      ua_key_value key = scr_keymap->find_key(keymod);

      // process key
      if (key != ua_key_nokey)
         key_event(event.type == SDL_KEYDOWN, key);
   }

   return true;
}

/*! note that SDL_events must be passed to ua_screen::process_event() to
    let key messages to be processed and sent to key_event() */
void ua_screen::key_event(bool key_down, ua_key_value key)
{
}

void ua_screen::tick()
{
   // send tick to all subwindows
   unsigned int max = subwindows.size();
   for(unsigned int i=0; i<max; i++)
      subwindows[i]->tick();
}

/*! note that for all subwindows the draw() and destroy() functions are
    called */
void ua_screen::register_window(ua_window* window)
{
   subwindows.push_back(window);
}

void ua_screen::register_keymap(ua_keymap* keymap)
{
   scr_keymap = keymap;
}
