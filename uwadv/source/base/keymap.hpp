/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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
#ifndef uwadv_keymap_hpp_
#define uwadv_keymap_hpp_

// needed includes
#include "settings.hpp"
#include "cfgfile.hpp"
#include "SDL_keysym.h"
#include <map>


// enums

//! logical key values
/*! defines all keys that are used during the game. */
enum ua_key_value
{
   // menus
   ua_key_menu_up=0,       //!< cursor up
   ua_key_menu_down,       //!< cursor down
   ua_key_menu_left,       //!< cursor left
   ua_key_menu_right,      //!< cursor right
   ua_key_menu_top_of_list,     //!< pgup
   ua_key_menu_top_of_list2,    //!< home
   ua_key_menu_bottom_of_list,  //!< pgdn
   ua_key_menu_bottom_of_list2, //!< end
   ua_key_menu_press_button,    //!< enter

   // normal movement
   ua_key_run_forward,            //!< w
   ua_key_run_forward_easymove,   //!< shift w
   ua_key_walk_forward,           //!< s
   ua_key_walk_forward_easymove,  //!< shift s
   ua_key_turn_left,              //!< a
   ua_key_turn_left_easymove,     //!< shift a
   ua_key_turn_right,             //!< d
   ua_key_turn_right_easymove,    //!< shift d
   ua_key_slide_left,             //!< z, y on non-us keyboards
   ua_key_slide_right,            //!< c
   ua_key_walk_backwards,         //!< x
   ua_key_walk_backwards_easymove,//!< shift x

   // other movement
   ua_key_fly_up,                 //!< e
   ua_key_fly_down,               //!< q
   ua_key_look_down,              //!< 1
   ua_key_center_view,            //!< 2
   ua_key_look_up,                //!< 3
   ua_key_standing_long_jump,     //!< shift j
   ua_key_jump,                   //!< j

   // combat mode keys
   ua_key_combat_bash,   //!< p
   ua_key_combat_slash,  //!< ;
   ua_key_combat_thrust, //!< .

   // special function keys
   ua_key_special_options,    //!< f1
   ua_key_special_talk_mode,  //!< f2
   ua_key_special_get_mode,   //!< f3
   ua_key_special_look_mode,  //!< f4
   ua_key_special_fight_mode, //!< f5
   ua_key_special_use_mode,   //!< f6
   ua_key_special_flip_panel, //!< f7
   ua_key_special_cast_spell, //!< f8
   ua_key_special_use_track,  //!< f9
   ua_key_special_sleep,      //!< f10

   ua_key_special_quicksave,  //!< f11
   ua_key_special_quickload,  //!< f12

   // game options
   ua_key_game_save_game,      //!< ctrl s
   ua_key_game_restore_game,   //!< ctrl r
   ua_key_game_change_music,   //!< ctrl m
   ua_key_game_change_sfx,     //!< ctrl f
   ua_key_game_change_lod,     //!< ctrl d
   ua_key_game_return_to_game, //!< esc

   // mouse cursor movement keys
   ua_key_cursor_hotarea_right, //!< tab
   ua_key_cursor_hotarea_left,  //!< shift tab

   // numeric keypad
   ua_key_cursor_dir_sw, //!< num1
   ua_key_cursor_dir_s,  //!< num2
   ua_key_cursor_dir_se, //!< num3
   ua_key_cursor_dir_w,  //!< num4
   ua_key_cursor_dir_e,  //!< num6
   ua_key_cursor_dir_nw, //!< num7
   ua_key_cursor_dir_n,  //!< num8
   ua_key_cursor_dir_ne, //!< num9

   ua_key_cursor_button_left,  //!< num0
   ua_key_cursor_button_right, //!< numperiod

   // special uwadv keys
   ua_key_ua_debug,        //!< alt d, starts debugger application
   ua_key_ua_return_menu,  //!< alt q, returns to main menu
   ua_key_ua_screenshot,   //!< alt c, takes screenshot
   ua_key_ua_level_up,     //!< alt page up, only in debug mode
   ua_key_ua_level_down,   //!< alt page down, only in debug mode

   ua_key_nokey
};


// classes

//! keymap class
/*! The class keeps all key mappings from key/modifier values (that can be
    created with ua_make_keymod() to ua_key_value values that can be processed
    in the game then.

    Note: to load the keymap, just call ua_keymap::load()
*/
class ua_keymap: public ua_cfgfile
{
public:
   //! ctor
   ua_keymap(){}

   //! initializes keymap
   void init(ua_settings& settings);

   //! finds key by given keymod value
   ua_key_value find_key(Uint32 keymod);

protected:
   //! called to load a specific value
   virtual void load_value(const std::string& name, const std::string& value);

protected:
   //! mapping from key value to SDL keysym value
   std::map<Uint32,ua_key_value> keymap;
};


// inline functions

//! creates a combined keymod value from a SDLKey and a SDLMod value
inline Uint32 ua_make_keymod(Uint16 key, Uint16 mod)
{
   return Uint32(key) | (Uint32(mod)<<16);
};


#endif
