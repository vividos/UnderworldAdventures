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

   The ua_settings class manages uwadv's global settings values. The values
   can have the types "boolean", "int" or "std::string". The values are stored
   in a config file that is read via the ua_cfgfile class. The ua_settings
   class also manages a game type value that determines which type of game
   is currently running. To load the settings, just use ua_cfgfile::load().

*/
/*! \defgroup base Base Components Documentation

   base component documentation yet to com

*/
//@{

// include guard
#ifndef uwadv_settings_hpp_
#define uwadv_settings_hpp_

// needed includes
#include <string>
#include <map>
#include "cfgfile.hpp"


// enums

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

   //! true when the uw1 installation is the uw_demo
   ua_setting_uw1_is_uw_demo,

   //! boolean value that indicates if uw2 game is available
   ua_setting_uw2_avail,

   //! path to the "uadata" folder
   ua_setting_uadata_path,

   //! path to the savegame folder
   ua_setting_savegame_folder,

   //! custom keymap file to load
   ua_setting_custom_keymap,

   //! boolean value that indicates if uwadv-specific features are enabled
   ua_setting_uwadv_features,

   //! prefix for the uadata folder path to locate the game.cfg and other cfg files
   ua_setting_game_prefix,

   //! screen resolution string, in the form <xres> x <yres>
   ua_setting_screen_resolution,

   //! boolean value that indicates fullscreen mode
   ua_setting_fullscreen,

   //! string with narration type
   ua_setting_cuts_narration,

   //! boolean value that is true when audio system should be enabled
   ua_setting_audio_enabled,

   //! int value with midi device to use
   ua_setting_win32_midi_device,
};

//! game type enum
enum ua_game_type
{
   //! we have the full game
   ua_game_uw1,

   //! only the demo is available
   ua_game_uw_demo,

   //! we have uw2
   ua_game_uw2,
};


// classes

//! config class
class ua_settings: public ua_cfgfile
{
public:
   //! ctor
   ua_settings();

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
   void set_value(ua_settings_key key, std::string value);

   //! inserts a boolean value
   void set_value(ua_settings_key key, bool value);

   //! inserts an integer value
   void set_value(ua_settings_key key, int value);

   //! sets the gametype
   void set_gametype(ua_game_type type){ gtype = type; }

   //! dumps all settings
   void dump();

protected:
   //! searches key value from string name
   bool search_key_from_string(const char* keyname, ua_settings_key& key);

   //! called to load a specific value
   virtual void load_value(const std::string& name, const std::string& value);

   //! called to replace a value
   virtual void write_replace(const std::string& name, std::string& value);

protected:
   //! settings map typedef
   typedef std::map<ua_settings_key,std::string> ua_settings_map_type;

   //! game type
   ua_game_type gtype;

   //! settings map
   ua_settings_map_type settings;
};


#endif
//@}
