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

   \brief loading functions for the config file

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


// structs

// list of all settings
struct
{
   const char *opt;
   int val;
} ua_settings_allsettings[] =
{
   { "uw1path", ua_uw1path }
};


// functions

inline bool ua_istab(char c){ return c=='\t'; }


// ua_config methods

void ua_settings::load()
{
   std::ifstream cfg(ua_settings_file);

   if (!cfg.is_open())
   {
      throw ua_exception("could not open config file ./uwadv.cfg");
   }

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

   // initialize rest of settings
   init();
}

void ua_settings::init()
{
   // check if uw1_path ends with a slash; add one when not
   if (uw1_path.size()==0)
   {
      throw ua_exception("no uw1_path specified in uwadv.cfg");
   }

   unsigned int last = uw1_path.size()-1;
   if (uw1_path.at(last)!='/' && uw1_path.at(last)!='\\')
      uw1_path.append(1,'/');

   // check for files that have to be available in all games
   if (!file_isavail("data/cnv.ark") &&
       !file_isavail("data/strings.pak") &&
       !file_isavail("data/pals.dat") &&
       !file_isavail("data/allpals.dat"))
   {
      gtype = ua_game_none;
   }
   else
   // check for demo relevant files
   if (file_isavail("uwdemo.exe") &&
       file_isavail("data/level13.st") &&
       file_isavail("data/level13.anx") &&
       file_isavail("data/level13.txm"))
   {
      gtype = ua_game_uw_demo;
   }
   else
   // check for ultima underworld 2
   if (file_isavail("uw2.exe"))
   {
      gtype = ua_game_uw2;
      return;
   }
   else
   // check for ultima underworld 1
   if (file_isavail("uw.exe"))
   {
      gtype = ua_game_uw1;
   }
   else
      gtype = ua_game_none;
}

bool ua_settings::file_isavail(const char *fname)
{
   std::string filename(uw1_path);
   filename.append(fname);

   FILE *fd = fopen(filename.c_str(),"rb");
   if (fd==NULL)
      return false;
   fclose(fd);
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
