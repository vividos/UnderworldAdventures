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
/*! \file uwadv_menu.hpp

   \brief underworld adventures menu

*/

// include guard
#ifndef uwadv_uwadv_menu_hpp_
#define uwadv_uwadv_menu_hpp_

// needed includes
#include "screen.hpp"


// classes

//! start splash screen class
class ua_uwadv_menu_screen: public ua_ui_screen_base
{
public:
   //! ctor
//   ua_uwadv_menu_screen(){}
   //! dtor
//   virtual ~ua_uwadv_menu_screen(){}

   // virtual functions from ua_ui_screen_base

   virtual void init(ua_game_core_interface* core);
//   virtual void done();
//   virtual void handle_event(SDL_Event& event);
//   virtual void render();
   virtual void tick();

protected:

};


#endif
