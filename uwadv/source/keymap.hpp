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
/*! \file keymap.hpp

   \brief keyboard key mappings

*/

// include guard
#ifndef __uwadv_keymap_hpp_
#define __uwadv_keymap_hpp_

// needed includes
#include "settings.hpp"
#include "SDL_keysym.h"
#include <map>


// enums

//! logical key values
enum ua_key_value
{
   ua_key_forward=0,
   ua_key_turn_left,
   ua_key_turn_right,
   ua_key_look_up,
   ua_key_look_center,
   ua_key_look_down,
};


// classes

//! keymap class
class ua_keymap
{
public:
   //! ctor
   ua_keymap();

   //! initializes keymap
   void init(ua_settings &settings);

   //! returns SDL keysym value for a given key
   unsigned int get_key(ua_key_value key);

protected:
   //! mapping from key value to SDL keysym value
   std::map<ua_key_value,unsigned int> keymap;
};


#endif
