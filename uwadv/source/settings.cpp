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
/*! \file settings.cpp

   \brief settings functions

*/

// needed includes
#include "common.hpp"
#include "settings.hpp"
#include <algorithm>
#include <iostream>


// structs

//! mapping of all settings keywords to keys
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
   { "enable-uwadv-features", ua_setting_uwadv_features },
   { "screen-resolution", ua_setting_screen_resolution },
   { "fullscreen", ua_setting_fullscreen },
   { "cutscene-narration", ua_setting_cuts_narration },
   { "audio-enabled", ua_setting_audio_enabled },
   { "win32-midi-device", ua_setting_win32_midi_device },
};


// ua_settings methods

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
      std::transform(val.begin(),val.end(),val.begin(),::tolower);

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
   char buffer[16];
   sprintf(buffer,"%i",value);

   std::string strval(buffer);
   set_value(key,strval);
}

void ua_settings::dump()
{
   std::map<ua_settings_key,std::string>::iterator iter;

   for (iter = settings.begin();
        iter != settings.end();
        ++iter)
   {
      std::cout << iter->first << "=" << iter->second << std::endl;
   }
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

void ua_settings::load_value(const std::string& name, const std::string& value)
{
   ua_trace("settings key/value: %s => %s\n",name.c_str(), value.c_str());

   // retrieve settings key
   ua_settings_key key;

   if (!search_key_from_string(name.c_str(),key))
      ua_trace("didn't find settings key \"%s\" in table.\n",name.c_str());
   else
      set_value(key,value);
}

void ua_settings::write_replace(const std::string& name, std::string& value)
{
   // search for key by name
   ua_settings_key key;

   if (search_key_from_string(name.c_str(),key))
   {
      // retrieve new value for that setting
      value.assign(settings[key]);
   }
}
