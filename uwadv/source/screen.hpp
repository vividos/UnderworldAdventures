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
/*! \file screen.hpp

   \brief user interface screens base class

   base class for user interface screens, e.g. main game screen, conversation screen,
   map screen etc.

*/

// include guard
#ifndef __uwadv_screen_hpp_
#define __uwadv_screen_hpp_

// needed includes
#include "core.hpp"


// classes

//! user interface abstract base class
class ua_ui_screen_base
{
public:
   //! ctor
   ua_ui_screen_base(){}

   //! dtor
   virtual ~ua_ui_screen_base(){}

   //! sets game core interface
   void set_core(class ua_game_core_interface *gamecore){ core=gamecore; }

   //! initialization
   virtual void init()=0;

   //! called when screen gets deactivated
   virtual void suspend(){}

   //! called when screen gets active again
   virtual void resume(){}

   //! clean up
   virtual void done()=0;

   //! handles events
   virtual void handle_event(SDL_Event &event)=0;

   //! renders the screen
   virtual void render()=0;

   //! called on every game tick
   virtual void tick()=0;

protected:
   //! game core interface ptr
   class ua_game_core_interface *core;
};

#endif
