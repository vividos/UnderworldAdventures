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
/*! \file gamecfg.cpp

   \brief game config loader

*/

// needed includes
#include "common.hpp"
#include "gamecfg.hpp"
#include "game_interface.hpp"
#include "settings.hpp"
#include "files.hpp"
#include "underworld.hpp"
#include "scripting.hpp"
#include "gamestrings.hpp"


// global functions

//! checks if a file with given filename is available
bool ua_file_isavail(const std::string& base, const char* fname)
{
   std::string filename = base;
   filename += fname;

   return ua_file_exists(filename.c_str());
}


// ua_gamecfg_loader methods

/*! Processes a keyword/value pair. The following keywords are currently
    recognized:
    game-name: specifies game name in future uw menu screens
    init-scripting: inits scripting engine
    load-script: loads a given script into scripting engine
    use-resources: specifies which resources the game has to use (uw1 or uw2)
    import-strings: imports gamestrings from custom .pak file

    \param the_name the keyword name
    \param the_value the keyword value
*/
void ua_gamecfg_loader::load_value(const char* the_name, const char* the_value)
{
   std::string name(the_name);
   std::string value(the_value);

   if (name.compare("game-name")==0)
   {
      game_name = value;
   }
   else
   if (name.compare("init-scripting")==0)
   {
      if (value == "lua")
      {
         // init Lua scripting
         *scripting = ua_scripting::create_scripting(ua_script_lang_lua);

         // check if scripting was set
         if (*scripting == NULL)
            throw ua_exception("could not create scripting object");

         // init scripting
         (*scripting)->init(&game);
      }
      else
         ua_trace("unsupported scripting language \"%s\"\n", value.c_str());
   }
   if (name.compare("load-script")==0)
   {
      // load given lua script name
      game.get_scripting().load_script(value.c_str());
   }
   else
   if (name.compare("use-resources")==0)
   {
      ua_settings& settings = game.get_settings();

      // check if resources to use are available
      if (value.compare("uw1")==0)
      {
         // check base path
         std::string base = settings.get_string(ua_setting_uw1_path);
         if (base.empty())
            throw ua_exception("path to uw1 was not specified in config file");

         settings.set_gametype(ua_game_uw1);
         settings.set_value(ua_setting_uw1_is_uw_demo, false);

         do
         {
            // check for uw game files
            if (ua_file_isavail(base,"data/cnv.ark") ||
                ua_file_isavail(base,"data/strings.pak") ||
                ua_file_isavail(base,"data/pals.dat") ||
                ua_file_isavail(base,"data/allpals.dat"))
            {
               // could be uw1 or uw_demo
               if (ua_file_isavail(base,"uw.exe"))
                  break; // found all needed files
               else
               // check if we only have the demo
               if (ua_file_isavail(base,"uwdemo.exe") &&
                   ua_file_isavail(base,"data/level13.st") &&
                   ua_file_isavail(base,"data/level13.anx") &&
                   ua_file_isavail(base,"data/level13.txm"))
               {
                  // found all needed files for uw_demo
                  settings.set_value(ua_setting_uw1_is_uw_demo, true);
                  break;
               }
            }

            throw ua_exception("could not find relevant uw1 game files");

         } while(false);

         // set generic uw path to uw1 path
         settings.set_value(ua_setting_uw_path,settings.get_string(ua_setting_uw1_path));
      }
      else
      if (value.compare("uw2")==0)
      {
         // check base path
         std::string base = settings.get_string(ua_setting_uw2_path);
         if (base.empty())
            throw ua_exception("path to uw2 was not specified in config file");

         // check for uw2 game files
         if (!ua_file_isavail(base,"data/cnv.ark") ||
             !ua_file_isavail(base,"data/strings.pak") ||
             !ua_file_isavail(base,"data/pals.dat") ||
             !ua_file_isavail(base,"data/allpals.dat") ||
             !ua_file_isavail(base,"uw2.exe") ||
             !ua_file_isavail(base,"data/t64.tr"))
            throw ua_exception("could not find relevant uw2 game files");

         // set generic uw path to uw2 path
         settings.set_value(ua_setting_uw_path,settings.get_string(ua_setting_uw2_path));
         settings.set_gametype(ua_game_uw2);
      }
      else
      {
         // unknown string
         std::string text("unknown use-resources string in game.cfg: ");
         text.append(value);
         throw ua_exception(text.c_str());
      }
   }
   else
   if (name.compare("import-strings")==0)
   {
      // load game strings
      SDL_RWops* gstr = game.get_files_manager().get_uadata_file(value.c_str());

      if (gstr != NULL)
      {
         // add strings.pak-like file
         game.get_gamestrings().add_pak_file(gstr);
         // note: don't call SDL_RWclose, the ua_gamestrings file will do that
      }
      else
         ua_trace("could not load strings file %s\n", value.c_str());
   }
}
