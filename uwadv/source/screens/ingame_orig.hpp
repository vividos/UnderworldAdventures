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
/*! \file ingame_orig.hpp

   \brief original, old style game screen user interface

*/

// include guard
#ifndef __uwadv_ingame_orig_hpp_
#define __uwadv_ingame_orig_hpp_

// needed includes
#include "screen.hpp"
#include "image.hpp"
#include "font.hpp"


// classes

//! user interface abstract base class
class ua_ingame_orig_screen: public ua_ui_screen_base
{
public:
   //! ctor
   ua_ingame_orig_screen(){}
   virtual ~ua_ingame_orig_screen(){}

   // virtual functions
   virtual void init();
   virtual void done();
   virtual void handle_event(SDL_Event &event);
   virtual void render();
   virtual void tick();

protected:
   //! sets up OpenGL stuff, flags, etc.
   void setup_opengl();

   //! renders 2d user interface
   void render_ui();

   void handle_key_action(Uint8 type, SDL_keysym &keysym);
   void handle_mouse_action(SDL_Event &event);

protected:

   //! indicates if player moves forward
   bool walk;
   double walk_dir;

   bool leftbuttondown,rightbuttondown;

   //! field of view angle
   double fov;
   //! current player angle
   double playeryangle;

   //! current mouse cursor
   unsigned int cursor_image;
   //! mouse cursor coordinates
   unsigned int cursorx,cursory;

   //! start of inventory slots the user sees
   unsigned int slot_start;

   //! 2d ui image background
   ua_image img_back;

   //! temp image to assemble 2d ui image
   ua_image img_temp;
   //! main 2d ui texture
   ua_texture tex;

   //! normal font
   ua_font font_normal;

   //! compass graphics
   ua_image_list img_compass;

   //! player appearance body graphics
   ua_image_list img_bodies;

   //! cursor images
   ua_image_list img_cursors;

   //! 2d object images
   ua_image_list img_objects;

   //! paperdoll armour images
   ua_image_list img_armor;
};

#endif
