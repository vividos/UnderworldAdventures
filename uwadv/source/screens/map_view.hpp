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
/*! \file map_view.hpp

   \brief level map view

*/

// include guard
#ifndef uwadv_map_view_hpp_
#define uwadv_map_view_hpp_

// needed includes
#include "screen.hpp"


// classes

//! level map view screen class
class ua_map_view_screen: public ua_ui_screen_base
{
public:
   //! ctor
   ua_map_view_screen(){}
   //! dtor
   virtual ~ua_map_view_screen(){}

   // virtual functions from ua_ui_screen_base
/*
   virtual void init(ua_game_core_interface* core);
   virtual void done();
   virtual bool handle_event(SDL_Event &event);
   virtual void render();*/
   virtual void tick();
};

#endif
