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
/*! \file settings.cpp

   \brief settings functions

*/

// needed includes
#include "common.hpp"
#include "settings.hpp"
#include <sstream>


// structs

//! mapping of all settings keywords to keys
/*! The mapping contains strings that are to be mapped to ua_settings_key enum
    values when loading the settings file. Add new entries to the end when
    needed.
*/
struct
{
   const char *optname;
   ua_settings_key key;
} ua_settings_mapping[] =
{
   { "uw1-path", ua_setting_uw1_path },
   { "uw2-path", ua_setting_uw2_path },
   { "uadata-path", ua_setting_uadata_path },
   { "savegame-folder", ua_setting_savegame_folder },
   { "custom-keymap", ua_setting_custom_keymap },
   { "enable-uwadv-features", ua_setting_uwadv_features },
   { "screen-resolution", ua_setting_screen_resolution },
   { "smooth-ui", ua_setting_ui_smooth },
   { "fullscreen", ua_setting_fullscreen },
   { "cutscene-narration", ua_setting_cuts_narration },
   { "audio-enabled", ua_setting_audio_enabled },
   { "win32-midi-device", ua_setting_win32_midi_device },
};


// ua_settings methods

/*! Constructor. Sets default values for some settings. */
ua_settings::ua_settings()
:gtype(ua_game_uw1)
{
   // set some initial values
   set_value(ua_setting_uadata_path,std::string("./uadata/"));
   set_value(ua_setting_savegame_folder,std::string("./uasave/"));
   set_value(ua_setting_fullscreen,false);
   set_value(ua_setting_cuts_narration,std::string("sound"));
   set_value(ua_setting_win32_midi_device,-1);
}

std::string ua_settings::get_string(ua_settings_key key)
{
   // try to find key
   ua_settings_map_type::iterator iter = settings.find(key);

   // return string
   return iter == settings.end() ?  std::string("") : iter->second;
}

int ua_settings::get_int(ua_settings_key key)
{
   // try to find key
   ua_settings_map_type::iterator iter = settings.find(key);

   // return integer
   return iter == settings.end() ? 0 :
      static_cast<int>(strtol(iter->second.c_str(),NULL,10));
}

/*! Boolean values in the settings can be expressed either with the words
    "true", "yes" or "1". All other values mean false.
*/
bool ua_settings::get_bool(ua_settings_key key)
{
   // try to find key
   ua_settings_map_type::iterator iter = settings.find(key);

   if (iter == settings.end())
      return false;
   else
   {
      std::string val(iter->second);

      // make lowercase
      ua_str_lowercase(val);

      // check for boolean keywords
      return (val.compare("true")==0 || val.compare("1")==0 || val.compare("yes")==0);
   }
}

void ua_settings::set_value(ua_settings_key key, std::string value)
{
   settings[key] = value;
}

void ua_settings::set_value(ua_settings_key key, bool value)
{
   std::string strval(value ? "true" : "false");
   set_value(key,strval);
}

void ua_settings::set_value(ua_settings_key key, int value)
{
   std::ostringstream buffer;
   buffer << value << std::ends;
   set_value(key,buffer.str());
}

void ua_settings::dump()
{
   std::map<ua_settings_key,std::string>::iterator iter;

   for (iter = settings.begin(); iter != settings.end(); ++iter)
      ua_trace("%s = %s\n", iter->first, iter->second.c_str());
}

bool ua_settings::search_key_from_string(const char* keyname, ua_settings_key& key)
{
   // search the whole settings table
   for(unsigned int i=0; i < SDL_TABLESIZE(ua_settings_mapping); i++)
   {
      // search through all option entries
      if (strcmp(ua_settings_mapping[i].optname,keyname)==0)
      {
         // found
         key = ua_settings_mapping[i].key;
         return true;
      }
   }
   return false;
}

void ua_settings::load_value(const char* name, const char* value)
{
   ua_trace("settings key/value: %s => %s\n",name,value);

   // retrieve settings key
   ua_settings_key key;

   if (!search_key_from_string(name,key))
      ua_trace("didn't find settings key \"%s\" in table.\n",name);
   else
   {
      std::string str_value(value);
      set_value(key,str_value);
   }
}

void ua_settings::write_replace(const char* name, std::string& value)
{
   // search for key by name
   ua_settings_key key;

   if (search_key_from_string(name,key))
   {
      // retrieve new value for that setting
      value.assign(settings[key]);
   }
}
