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
/*! \file settingsloader.cpp

   \brief loading functions for the ua_settings class

*/

// needed includes
#include "common.hpp"
#include "settings.hpp"
#include <cctype>
#include <fstream>
#include <algorithm>
#include <iostream>


// structs

// mapping of all settings keywords to keys
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
   { "screen-resolution", ua_setting_screen_resolution },
   { "fullscreen", ua_setting_fullscreen },
   { "cutscene-narration", ua_setting_cuts_narration },
   { "audio-enabled", ua_setting_audio_enabled },
   { "win32-midi-device", ua_setting_win32_midi_device },
};


// ua_settings methods

void ua_settings::load(const char *filename)
{
   std::ifstream cfg(filename);

   if (!cfg.is_open())
   {
      std::string text("could not open config file ");
      text.append(filename);
      throw ua_exception(text.c_str());
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
      std::string::size_type pos = 0;
      while( (pos = line.find_first_of('\t',pos) != std::string::npos) )
         line.replace(pos,1," ");

      // we now have a real config line
      for(int i=0; i<SDL_TABLESIZE(ua_settings_mapping); i++)
      {
         // search through all option entries
         int len = strlen(ua_settings_mapping[i].optname);

         if (strncmp(ua_settings_mapping[i].optname,line.c_str(),len)==0)
         {
            line.erase(0,len);

            // trim spaces
            for(;line.size()>0 && isspace(line.at(0));)
               line.erase(0,1);

            ua_trace("settings key/value: %s => %s\n",
               ua_settings_mapping[i].optname,line.c_str());

            set_value(ua_settings_mapping[i].key,line);

            break;
         }
      }
   }

   cfg.close();
}

void ua_settings::write(const char *origfile, const char *newfile)
{
   std::ifstream cfg(origfile);

   if (!cfg.is_open())
   {
      std::string text("could not open config file ");
      text.append(origfile);
      throw ua_exception(text.c_str());
   }

   std::ofstream out(newfile);
   if (!out.is_open())
   {
      std::string text("could not open new config file ");
      text.append(newfile);
      throw ua_exception(text.c_str());
   }

   // read in all lines
   std::string line,linecopy;

   while(!cfg.eof())
   {
      std::getline(cfg,line);
      linecopy = line;

      bool found = false;
      do
      {
         if (line.size()==0) break;

         if (line.at(line.size()-1)=='\n')
            line.erase(line.size()-1);

         // trim spaces at start of line
         for(;line.size()>0 && isspace(line.at(0));)
            line.erase(0,1);

         if (line.size()==0 || line.at(0)=='#') break;

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

         if (line.size()==0) break;

         // replace all '\t' with ' '
         std::string::size_type pos = 0;
         while( (pos = line.find_first_of('\t',pos) != std::string::npos) )
            line.replace(pos,1," ");


         // we now have a real config line
         for(int i=0; i<SDL_TABLESIZE(ua_settings_mapping); i++)
         {
            // search through all option entries
            int len = strlen(ua_settings_mapping[i].optname);

            if (strncmp(ua_settings_mapping[i].optname,line.c_str(),len)==0)
            {
               found = true;

               // write new value

               std::string newline(ua_settings_mapping[i].optname);
               newline.append(" ");
               newline.append(get_string(ua_settings_mapping[i].key));

               out << newline << std::endl;
               break;
            }
         }
      } while(0);

      // no settings key found? write out original line
      if (!found)
         out << linecopy << std::endl;
   }

   cfg.close();
}
