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

   loading functions for the config file

*/

// needed includes
#include "common.hpp"
#include "settings.hpp"
#include <cctype>
#include <fstream>
#include <algorithm>


// constants

const char *ua_settings_file = "./uwadv.cfg";

// settings enum

enum
{
   ua_uw1path
};


// all settings

struct
{
   const char *opt;
   int val;
} ua_settings_allsettings[] =
{
   { "uw1path", ua_uw1path }
};


inline bool ua_istab(char c){ return c=='\t'; }


// ua_config methods

bool ua_settings::load()
{
   std::ifstream cfg(ua_settings_file);

   if (!cfg.is_open())
      return false;

   // read in all lines
   std::string line;

   while(!cfg.eof())
   {
      std::getline(cfg,line);

      if (line.size()==0) continue;

      if (line.at(line.size()-1)=='\n')
         line.erase(line.size()-1);

      // trim spaces at start of line
      for(;line.size()>0 && isspace(line.at(0));)
         line.erase(0,1);

      if (line.size()==0 || line.at(0)=='#') continue;

      // trim spaces at end of line
      int len;
      do
      {
         len = line.size()-1;
         if (isspace(line.at(len)))
            line.erase(len);
         else
            break;
      }
      while(line.size()>0);

      if (line.size()==0) continue;

      // replace all '\t' with ' '
      std::replace_if(line.begin(),line.end(),ua_istab,' ');

      // we now have a real config line
      for(int i=0; i<SDL_TABLESIZE(ua_settings_allsettings); i++)
      {
         // search through all option entries
         int len = strlen(ua_settings_allsettings[i].opt);

         if (strncmp(ua_settings_allsettings[i].opt,line.c_str(),len)==0)
         {
            line.erase(0,len);

            // trim spaces
            for(;line.size()>0 && isspace(line.at(0));)
               line.erase(0,1);

            process_option(ua_settings_allsettings[i].val,line.c_str());

            break;
         }
      }
   }

   cfg.close();

   return true;
}

void ua_settings::process_option(int option, const char *value)
{
   switch(option)
   {
   case ua_uw1path:
      uw1_path = value;
      break;
   }
}
