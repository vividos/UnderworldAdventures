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
/*! \file keymap.cpp

   \brief keyboard key mappings

*/

// needed includes
#include "common.hpp"
#include "keymap.hpp"


// ua_keymap methods

ua_keymap::ua_keymap()
{
}

void ua_keymap::init(ua_settings &settings)
{
   keymap.insert(std::make_pair<ua_key_value,unsigned int>(ua_key_forward,SDLK_w));
   keymap.insert(std::make_pair<ua_key_value,unsigned int>(ua_key_turn_left,SDLK_a));
   keymap.insert(std::make_pair<ua_key_value,unsigned int>(ua_key_turn_right,SDLK_d));

   keymap.insert(std::make_pair<ua_key_value,unsigned int>(ua_key_look_up,SDLK_1));
   keymap.insert(std::make_pair<ua_key_value,unsigned int>(ua_key_look_center,SDLK_2));
   keymap.insert(std::make_pair<ua_key_value,unsigned int>(ua_key_look_down,SDLK_3));
}

unsigned int ua_keymap::get_key(ua_key_value key)
{
   std::map<ua_key_value,unsigned int>::iterator iter =
      keymap.find(key);

   if (iter==keymap.end())
      throw ua_exception("unknown keymap value queried");

   return iter->second;
}
