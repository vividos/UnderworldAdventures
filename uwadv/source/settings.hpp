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
/*! \file settings.hpp

   \brief game settings class

*/

// include guard
#ifndef __uwadv_settings_hpp_
#define __uwadv_settings_hpp_

// needed includes
#include <string>
#include <map>


// enum

//! settings value key
enum ua_settings_key
{
   //! path to the game files (dependent on which game currently plays)
   ua_setting_uw_path = 0,

   //! path to the original uw1 game files (uw1 or uw_demo)
   ua_setting_uw1_path,

   //! path to the uw2 game files
   ua_setting_uw2_path,

   //! boolean value that indicates if uw1 (or uw_demo) game is available
   ua_setting_uw1_avail,

   //! boolean value that indicates if uw2 game is available
   ua_setting_uw2_avail,

   //! path to the "uadata" folder
   ua_setting_uadata_path,

   //! path to the savegame folder
   ua_setting_savegame_folder,

   //! boolean value that indicates fullscreen mode
   ua_setting_fullscreen,

   //! string with narration type
   ua_setting_cuts_narration,

   //! int value with midi device to use
   ua_setting_win32_midi_device,
};

//! game type enum
enum ua_game_type
{
   //! no game available
   ua_game_none=0,

   //! we have the full game
   ua_game_uw1,

   //! only the demo is available
   ua_game_uw_demo,

   //! we have uw2
   ua_game_uw2,

};


typedef std::map<ua_settings_key,std::string> SettingsMap;

// classes

//! config class
class ua_settings
{
public:
   //! ctor
   ua_settings();

   //! loads a config file
   void load(const char *filename);

   //! inits settings class after loading all config files
   void init();

   // settings value access

   //! returns the gametype
   ua_game_type get_gametype(){ return gtype; }

   //! returns string settings value
   std::string get_string(ua_settings_key key);

   //! returns an integer settings value
   int get_int(ua_settings_key key);

   //! returns a boolean settings value
   bool get_bool(ua_settings_key key);

   //! inserts a settings key/value pair
   void insert_value(ua_settings_key key, std::string value);

   //! sets the gametype
   void set_gametype(ua_game_type type){ gtype = type; }

   //! dumps all settings
   void dump();

protected:

   //! game type
   ua_game_type gtype;

   //! settings map
   SettingsMap settings;
};

#endif
