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
/*! \file mousecursor.hpp

   \brief mouse cursor

   Lightweight mouse cursor class that wraps functionality needed for
   every screen that shows a cursor, nothing more, nothing less. All
   texture/graphic options must be set by the user of the class.
*/
//! \ingroup userinterface

//@{

// include guard
#ifndef uwadv_mousecursor_hpp_
#define uwadv_mousecursor_hpp_

// needed includes
#include "window.hpp"
#include "image.hpp"
#include "texture.hpp"


// forward references
class ua_game_interface;


// classes

//! mouse cursor class
class ua_mousecursor: public ua_window
{
public:
   //! ctor
   ua_mousecursor(){}
   //! dtor
   virtual ~ua_mousecursor(){}

   //! initializes mouse cursor class
   void init(ua_game_interface& game, unsigned int initialtype=0);

   //! show or hide the cursor
   void show(bool show);

   //! set the type of cursor (the image used)
   void set_type(unsigned int type);

   //! sets custom image as cursor
   void set_custom(ua_image& cursorimg);

   // virtual methods from ua_window

   virtual void destroy();
   virtual void draw();
   bool process_event(SDL_Event& event);

protected:
   //! cursor visible
   bool isvisible;

   //! mouse cursor image list
   std::vector<ua_image> img_cursors;

   //! texture object for mouse
   ua_texture mousetex;

   //! indicates if mouse cursor is drawn using smooth (filtered) pixels
   bool smooth_ui;
};


#endif
//@}
