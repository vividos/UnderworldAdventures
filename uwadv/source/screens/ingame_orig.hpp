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

   original, old style game screen user interface

*/

// include guard
#ifndef __uwadv_ingame_orig_hpp_
#define __uwadv_ingame_orig_hpp_

// needed includes
#include "screen.hpp"
#include "underworld.hpp"


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
   //! sets up OpenGL stuff, viewport, flags, etc.
   void setup_opengl();

   void handle_key_down(SDL_keysym &keysym);
   void handle_mouse_action(SDL_Event &event);

protected:
   ua_underworld uworld;

   float xangle,yangle;

   bool leftbuttondown,rightbuttondown;

   float xpos,ypos;

   float playerx,playery;

   float playerxangle,playeryangle;

   bool mapmode;

};

#endif
