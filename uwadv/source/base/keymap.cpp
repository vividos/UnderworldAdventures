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
/*! \file keymap.cpp

   \brief keyboard key mappings

*/

// needed includes
#include "common.hpp"
#include "keymap.hpp"
#include <cctype>


// tables

//! mapping of all keytype keywords to keytypes
struct
{
   const char* keytype; //!< type name that occurs in keymap.cfg
   ua_key_value key;    //!< enum value of key
} ua_keymap_keytype_mapping[] =
{
   { "menu-up",               ua_key_menu_up },
   { "menu-down",             ua_key_menu_down },
   { "menu-left",             ua_key_menu_left },
   { "menu-right",            ua_key_menu_right },
   { "menu-top-of-list",      ua_key_menu_top_of_list },
   { "menu-top-of-list2",     ua_key_menu_top_of_list2 },
   { "menu-bottom-of-list",   ua_key_menu_bottom_of_list },
   { "menu-bottom-of-list2",  ua_key_menu_bottom_of_list2 },
   { "menu-press-button",     ua_key_menu_press_button },

   { "run-forward",                 ua_key_run_forward },
   { "run-forward-easymove",        ua_key_run_forward_easymove },
   { "walk-forward",                ua_key_walk_forward },
   { "walk-forward-easymove",       ua_key_walk_forward_easymove },
   { "turn-left",                   ua_key_turn_left },
   { "turn-left-easymove",          ua_key_turn_left_easymove },
   { "turn-right",                  ua_key_turn_right },
   { "turn-right-easymove",         ua_key_turn_right_easymove },
   { "slide-left",                  ua_key_slide_left },
   { "slide-right",                 ua_key_slide_right },
   { "walk-backwards",              ua_key_walk_backwards },
   { "walk-backwards-easymove",     ua_key_walk_backwards_easymove },

   { "fly-up",             ua_key_fly_up },
   { "fly-down",           ua_key_fly_down },
   { "look-down",          ua_key_look_down },
   { "center-view",        ua_key_center_view },
   { "look-up",            ua_key_look_up },
   { "standing-long-jump", ua_key_standing_long_jump },
   { "jump",               ua_key_jump },

   { "combat-bash",        ua_key_combat_bash },
   { "combat-slash",       ua_key_combat_slash },
   { "combat-thrust",      ua_key_combat_thrust },

   { "special-options",    ua_key_special_options    },
   { "special-talk-mode",  ua_key_special_talk_mode  },
   { "special-get-mode",   ua_key_special_get_mode   },
   { "special-look-mode",  ua_key_special_look_mode  },
   { "special-fight-mode", ua_key_special_fight_mode },
   { "special-use-mode",   ua_key_special_use_mode   },
   { "special-flip-panel", ua_key_special_flip_panel },
   { "special-cast-spell", ua_key_special_cast_spell },
   { "special-use-track",  ua_key_special_use_track  },
   { "special-sleep",      ua_key_special_sleep      },

   { "special-quicksave",  ua_key_special_quicksave },
   { "special-quickload",  ua_key_special_quickload },

   { "game-save-game",      ua_key_game_save_game      },
   { "game-restore-game",   ua_key_game_restore_game   },
   { "game-change-music",   ua_key_game_change_music   },
   { "game-change-sfx",     ua_key_game_change_sfx     },
   { "game-change-lod",     ua_key_game_change_lod     },
   { "game-return-to-game", ua_key_game_return_to_game },

   { "cursor-hotarea-right", ua_key_cursor_hotarea_right },
   { "cursor-hotarea-left",  ua_key_cursor_hotarea_left  },

   { "cursor-dir-sw", ua_key_cursor_dir_sw },
   { "cursor-dir-s",  ua_key_cursor_dir_s  },
   { "cursor-dir-se", ua_key_cursor_dir_se },
   { "cursor-dir-w",  ua_key_cursor_dir_w  },
   { "cursor-dir-e",  ua_key_cursor_dir_e  },
   { "cursor-dir-nw", ua_key_cursor_dir_nw },
   { "cursor-dir-n",  ua_key_cursor_dir_n  },
   { "cursor-dir-ne", ua_key_cursor_dir_ne },

   { "cursor-button-left",  ua_key_cursor_button_left  },
   { "cursor-button-right", ua_key_cursor_button_right },

   { "ua-debug",       ua_key_ua_debug       },
   { "ua-return-menu", ua_key_ua_return_menu },
   { "ua-screenshot",  ua_key_ua_screenshot  },
   { "ua-level-up",    ua_key_ua_level_up    },
   { "ua-level-down" , ua_key_ua_level_down  },
};

//! mapping of all settings keywords to keys
struct
{
   const char* keytype;
   SDLKey key;
} ua_keymap_key_mapping[] =
{
   { "f1", SDLK_F1 },
   { "f2", SDLK_F2 },
   { "f3", SDLK_F3 },
   { "f4", SDLK_F4 },
   { "f5", SDLK_F5 },
   { "f6", SDLK_F6 },
   { "f7", SDLK_F7 },
   { "f8", SDLK_F8 },
   { "f9", SDLK_F9 },
   { "f10", SDLK_F10 },
   { "f11", SDLK_F11 },
   { "f12", SDLK_F12 },

   { "semicolon", SDLK_SEMICOLON },
   { "period", SDLK_PERIOD },
   { "tab", SDLK_TAB },
   { "esc", SDLK_ESCAPE },
   { "pgup", SDLK_PAGEUP },
   { "pgdown", SDLK_PAGEDOWN },
   { "home", SDLK_HOME },
   { "end", SDLK_END },
   { "cursor-up", SDLK_UP },
   { "cursor-down", SDLK_DOWN },
   { "cursor-left", SDLK_LEFT },
   { "cursor-right", SDLK_RIGHT },

   { "backspace", SDLK_BACKSPACE },
   { "return", SDLK_RETURN },
   { "enter", SDLK_RETURN },
   { "pause", SDLK_PAUSE },
   { "space", SDLK_SPACE },
   { "comma", SDLK_COMMA },
   { "minus", SDLK_MINUS },
   { "colon", SDLK_COLON },
   { "less", SDLK_LESS },
   { "delete", SDLK_DELETE },

   { "num0", SDLK_KP0 },
   { "num1", SDLK_KP1 },
   { "num2", SDLK_KP2 },
   { "num3", SDLK_KP3 },
   { "num4", SDLK_KP4 },
   { "num5", SDLK_KP5 },
   { "num6", SDLK_KP6 },
   { "num7", SDLK_KP7 },
   { "num8", SDLK_KP8 },
   { "num9", SDLK_KP9 },

   { "num-divide", SDLK_KP_DIVIDE },
   { "num-multipl", SDLK_KP_MULTIPLY },
   { "num-minus", SDLK_KP_MINUS },
   { "num-plus", SDLK_KP_PLUS },
   { "num-enter", SDLK_KP_ENTER },
   { "num-period", SDLK_KP_PERIOD },
};


// ua_keymap methods

void ua_keymap::init(ua_settings& settings)
{
}

ua_key_value ua_keymap::find_key(Uint32 keymod)
{
   std::map<Uint32,ua_key_value>::iterator iter =
      keymap.find(keymod);

   return iter == keymap.end() ? ua_key_nokey : iter->second;
}

void ua_keymap::load_value(const char* name, const char* value)
{
   // search "keymap key" key from name
   ua_key_value key = ua_key_nokey;
   {
      std::string keyname(name);

      // make lowercase
      ua_str_lowercase(keyname);

      // search whole table
      unsigned int max = SDL_TABLESIZE(ua_keymap_keytype_mapping);

      for(unsigned int i=0; i < max; i++)
      if (0==keyname.compare(ua_keymap_keytype_mapping[i].keytype))
      {
         key = ua_keymap_keytype_mapping[i].key;
         break;
      }

      if (key==ua_key_nokey)
      {
         ua_trace("keymap: unknown keyboard action: %s\n",keyname.c_str());
         return;
      }
   }

   // search keymod for key
   Uint32 keymod=0;
   {
      std::string keyval(value);

      // make lowercase
      ua_str_lowercase(keyval);

      // check for modifiers
      std::string::size_type pos = keyval.find("ctrl");
      if (pos != std::string::npos)
      {
         keymod = ua_make_keymod(0,KMOD_CTRL);
         keyval.erase(pos,4);
      }

      pos = keyval.find("alt");
      if (pos != std::string::npos)
      {
         keymod = ua_make_keymod(0,KMOD_ALT);
         keyval.erase(pos,3);
      }

      pos = keyval.find("shift");
      if (pos != std::string::npos)
      {
         keymod = ua_make_keymod(0,KMOD_SHIFT);
         keyval.erase(pos,5);
      }

      // trim string
      for(;keyval.size()>0 && isspace(keyval.at(0));)
         keyval.erase(0,1);

      // check for alphanumeric chars
      if (keyval.size()==1 && isalnum(keyval.at(0)))
      {
         keymod |= keyval.at(0);
      }
      else
      {
         // check for known key keywords

         // search whole table
         unsigned int max = SDL_TABLESIZE(ua_keymap_key_mapping);

         bool found = false;
         for(unsigned int i=0; i < max; i++)
         if (0==keyval.compare(ua_keymap_key_mapping[i].keytype))
         {
            keymod |= ua_keymap_key_mapping[i].key;
            found = true;
            break;
         }

         if (!found)
         {
            ua_trace("keymap: didn't find keycode for: %s\n",keyval.c_str());
            return;
         }
      }
   }

   // insert key
   keymap[keymod] = key;
}
