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

*/
/*! \file create_character.hpp

   \brief create character screen

   shows the screen where the user can create a player character for a 
   new game.

*/

// include guard
#ifndef __uwadv_create_character_hpp_
#define __uwadv_create_character_hpp_

// needed includes
#include "screen.hpp"
#include "font.hpp"


// classes

//! create character screen class
class ua_create_character_screen: public ua_ui_screen_base
{
public:
   //! ctor
   ua_create_character_screen(){}
   //! dtor
   virtual ~ua_create_character_screen(){}

   // virtual functions from ua_ui_screen_base

   virtual void init();
   virtual void done();
   virtual void handle_event(SDL_Event &event);
   virtual void render();
   virtual void tick();

protected:
   //! does a button press
   void press_button();

   //! determines clicked button in current group, -1 if none
   int getbuttonclicked(int buttongroup);

   //! draws text at a coordinate (xalign: 0=left, 1=center, 2=right)
   void drawtext(const char *str, int x, int y, int xalign = 0, unsigned char color = 0);
   void drawtext(int strnum, int x, int y, int xalign = 0, unsigned char color = 0);

   //! draw a number at a coordinate, number is always right-aligned
   void drawnumber(unsigned int num, int x, int y, unsigned char color = 0);

   //! draws a button at a coordinate (xc is horizontal center, y vertical top)
   void drawbutton(int buttontype, bool highlight, int strnum, int xc, int y);

   //! draws a buttongroup
   void drawcurrentbuttongroup();

   //! draws selected options in right part of the screen
   void drawselectedoptions();

protected:
   //! button and text font
   ua_font font;

   //! ended
   bool ended;

   //! the player
   ua_player *pplayer;

   //! current creation step
   int step;

   //! previous creation step
   int prevstep;

   //! current stage
   unsigned int stage;

   //! current tickcount
   unsigned int tickcount;

   //! number of current buttongroup
   int current_buttongroup;

   //! indicates if the mouse button is down
   bool buttondown;

   //! image
   ua_image img;

   //! bg image
   ua_image bgimg;

   //! texture object for background
   ua_texture tex;

   //! image list with buttons
   ua_image_list img_buttons;

   //! background image palette
   ua_onepalette palette;

   //! number of selected button, or -1 if none
   int selected_button;

   //! number of previous button
   int prev_button;
};

#endif
