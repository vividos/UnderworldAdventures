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


// ua_settings methods

ua_settings::ua_settings()
:gtype(ua_game_none)
{
}

void ua_settings::init()
{
   // set the generic uw path
   insert_value(ua_setting_uw_path,get_string(ua_setting_uw1_path).c_str());

   //:uw1_path("./"), uadata_path("./uadata/"), fullscreen(false), cutsntype(ua_cutscenenar_subtitles)

/*
   // check if uw1_path ends with a slash; add one when not
   if (uw1_path.size()==0)
   {
      throw ua_exception("no uw1_path specified in uwadv.cfg");
   }

   unsigned int last = uw1_path.size()-1;
   if (uw1_path.at(last)!='/' && uw1_path.at(last)!='\\')
      uw1_path.append(1,'/');*/
}

std::string ua_settings::get_string(ua_settings_key key)
{
   // try to find key
   std::map<ua_settings_key,std::string>::iterator iter =
      settings.find(key);

   // return string
   return iter == settings.end() ?  std::string("") : iter->second;
}

int ua_settings::get_int(ua_settings_key key)
{
   // try to find key
   std::map<ua_settings_key,std::string>::iterator iter =
      settings.find(key);

   // return integer
   return iter == settings.end() ? 0 :
      static_cast<int>(strtol(iter->second.c_str(),NULL,10));
}

bool ua_settings::get_bool(ua_settings_key key)
{
   // try to find key
   std::map<ua_settings_key,std::string>::iterator iter =
      settings.find(key);

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

void ua_settings::insert_value(ua_settings_key key, const char *value)
{
   settings.insert( std::make_pair<ua_settings_key,std::string>(key,std::string(value)) );
}
