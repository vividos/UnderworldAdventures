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
/*! \file start_menu.hpp

   \brief start menu screen

   shows the menu where the user can select "introduction", "create
   character", "acknowledgements" or "journey onward" (if a game can
   be loaded).

*/

// include guard
#ifndef __uwadv_start_menu_hpp_
#define __uwadv_start_menu_hpp_

// needed includes
#include "screen.hpp"


// classes

//! start menu screen class
class ua_start_menu_screen: public ua_ui_screen_base
{
public:
   //! ctor
   ua_start_menu_screen(){}
   //! dtor
   virtual ~ua_start_menu_screen(){}

   // virtual functions from ua_ui_screen_base

   virtual void init();
   virtual void suspend();
   virtual void resume();
   virtual void done();
   virtual void handle_event(SDL_Event &event);
   virtual void render();
   virtual void tick();

protected:
   //! does a button press
   void press_button();

   //! determines selected area by mouse coordinates
   int get_selected_area();

protected:

   // constants

   //! time to fade in/out
   static const double fade_time;

   //! palette shifts per second
   static const double palette_shifts_per_second;

   //! current stage
   unsigned int stage;

   //! current tickcount
   unsigned int tickcount;

   //! count for palette shifting
   double shiftcount;

   //! number of selected area, or -1 if none
   int selected_area;

   //! indicates if "journey onward" is available
   bool journey_avail;

   //! indicates if the mouse button is down
   bool buttondown;

   //! background image
   ua_image img;

   //! texture object for background
   ua_texture tex;

   //! image list with buttons
   ua_image_list img_buttons;

   //! background image palette
   ua_onepalette palette;
};

#endif
