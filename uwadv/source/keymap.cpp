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
   keymap[ua_key_run_forward] = SDLK_w;
   keymap[ua_key_run_forward_easymove] = ua_make_keymod(SDLK_w,KMOD_SHIFT);
   keymap[ua_key_walk_forward] = SDLK_s;
   keymap[ua_key_walk_forward_easymove] = ua_make_keymod(SDLK_s,KMOD_SHIFT);
   keymap[ua_key_turn_left] = SDLK_a;
   keymap[ua_key_turn_left_easymove] = ua_make_keymod(SDLK_a,KMOD_SHIFT);
   keymap[ua_key_turn_right] = SDLK_d;
   keymap[ua_key_turn_right_easymove] = ua_make_keymod(SDLK_d,KMOD_SHIFT);
   keymap[ua_key_slide_left] = SDLK_y;
   keymap[ua_key_slide_right] = SDLK_c;
   keymap[ua_key_walk_backwards] = SDLK_x;
   keymap[ua_key_walk_backwards_easymove] = ua_make_keymod(SDLK_x,KMOD_SHIFT);

   keymap[ua_key_fly_up] = SDLK_e;
   keymap[ua_key_fly_down] = SDLK_q;
   keymap[ua_key_look_down] = SDLK_1;
   keymap[ua_key_center_view] = SDLK_2;
   keymap[ua_key_look_up] = SDLK_3;
   keymap[ua_key_standing_long_jump] = ua_make_keymod(SDLK_j,KMOD_SHIFT);
   keymap[ua_key_jump] = SDLK_j;

   keymap[ua_key_game_quit_game] = ua_make_keymod(SDLK_q,KMOD_ALT|KMOD_NUM);
}

bool ua_keymap::is_key(ua_key_value key, Uint32 keymod)
{
   Uint32 keymod2 = get_key(key);

   // compare keysym and keymod values separately
   return (( keymod&0xffff) == (keymod2&0xffff) &&
      ((keymod>>16) & (keymod2>>16)) != 0);
}

Uint32 ua_keymap::get_key(ua_key_value key)
{
   std::map<ua_key_value,unsigned int>::iterator iter =
      keymap.find(key);

   if (iter==keymap.end())
      throw ua_exception("unknown keymap value queried");

   return iter->second;
}
