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
/*! \file window.hpp

   \brief aaa

   aaa

*/

// include guard
#ifndef uwadv_window_hpp_
#define uwadv_window_hpp_

// needed includes


// forward references


// structs


// classes

//! window base class
class ua_window
{
public:
   //! ctor
   ua_window();
   //! dtor
   virtual ~ua_window();

   //! creates window
   virtual void create(unsigned int xpos, unsigned int ypos,
      unsigned int width, unsigned int height);

   //! destroys window
   virtual void destroy();

   //! draws window contents
   virtual void draw();

   //! processes SDL events; returns true when event shouldn't processed further
   virtual bool process_event(SDL_Event& event);

protected:
};

#endif
