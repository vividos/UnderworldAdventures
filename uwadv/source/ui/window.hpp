/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2003,2004 Underworld Adventures Team

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

   \brief window base class

*/
/*! \defgroup userinterface User Interface Documentation

   user interface documentation yet to come ...

*/
//@{

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

   //! processes mouse event specific to that window
   virtual void mouse_event(bool button_clicked, bool left_button,
      bool button_down, unsigned int mousex, unsigned int mousey);

   //! returns window position x coordinate
   unsigned int get_xpos() const { return wnd_xpos; }
   //! returns window position y coordinate
   unsigned int get_ypos() const { return wnd_ypos; }

   //! returns window width
   unsigned int get_width() const { return wnd_width; }
   //! returns window height
   unsigned int get_height() const { return wnd_height; }

   //! determines if point is inside window
   bool in_window(unsigned int xpos, unsigned int ypos);

   //! calculates mouse position from event.motion.x/y
   void calc_mousepos(SDL_Event& event, unsigned int& xpos, unsigned int& ypos);

protected:
   //! window position
   unsigned int wnd_xpos, wnd_ypos;

   //! window size
   unsigned int wnd_width, wnd_height;
};

#endif
//@}
