/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Dirk Manders

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

// include guard
#ifndef __uwadv_mousecursor_hpp_
#define __uwadv_mousecursor_hpp_

// needed includes
#include "core.hpp"

// classes

class ua_mousecursor
{
public:
   //! ctor
   ua_mousecursor(){}

   //! dtor
   ~ua_mousecursor(){ done(); }

   //! initializes mouse cursor class
   void init(ua_game_core_interface* core, int initialtype=0);

   //! show or hide the cursor
   void show(bool show);

   //! set the type of cursor (the image used)
   void settype(int type);

   //! sets custom image as cursor
   void set_custom(ua_image& cursorimg);

   //! update the current position of the mouse
   void updatepos();

   //! update current position with given relative mouse coordinates
   void updatepos(unsigned int relx, unsigned int rely);

   //! update and draw the mouse texture
   void draw();

   //! cleans up mouse cursor
   void done();

protected:
   //! screen size
   unsigned int screen_width,screen_height;

   //! cursor visible
   bool isvisible;

   //! mouse cursor image list 
   ua_image_list img_cursors;

   //! mouse cursor coordinates
   unsigned int cursorx,cursory;

   //! mouse cursor height/width
   unsigned int cursorw, cursorh;

   //! texture object for mouse
   ua_texture mousetex;
};

#endif
