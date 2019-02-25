//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file GameConfigLoader.cpp
/// \brief game config loader
//
#include "common.hpp"
#include "GameConfigLoader.hpp"
#include "TextFile.hpp"
#include "GameInterface.hpp"
#include "Settings.hpp"
#include "FileSystem.hpp"
#include "ResourceManager.hpp"
#include "Underworld.hpp"
#include "IScripting.hpp"
#include "GameStrings.hpp"
#include "GameStringsImporter.hpp"

/// checks if a file with given filename is available
bool IsFileAvailable(const std::string& base, const char* fname)
{
   std::string filename = base;
   filename += fname;

   return Base::FileSystem::FileExists(filename.c_str());
}

void GameConfigLoader::Load(Base::TextFile& file)
{
   std::string line, key, value;
   while (file.Tell() < file.FileLength())
   {
      file.ReadLine(line);

      if (SplitKeyValue(line, key, value))
         load_value(key.c_str(), value.c_str());
   }
}

bool GameConfigLoader::SplitKeyValue(std::string& line, std::string& key, std::string& value)
{
   if (line.empty())
      return false;

   // trim spaces at start of line
   for (; line.size() > 0 && isspace(line.at(0));)
      line.erase(0, 1);

   // comment line?
   if (line.size() == 0 || line.at(0) == '#' || line.at(0) == ';')
      return false;

   // comment somewhere in the line?
   std::string::size_type pos2 = line.find('#');
   if (pos2 != std::string::npos)
   {
      // remove comment
      line.erase(pos2);
   }

   // trim spaces at end of line
   {
      int len;
      do
      {
         len = line.size() - 1;
         if (isspace(line.at(len)))
            line.erase(len);
         else
            break;
      } while (line.size() > 0);
   }

   // empty line?
   if (line.size() == 0)
      return false;

   // replace all '\t' with ' '
   std::string::size_type pos = 0;
   while ((pos = line.find('\t', pos)) != std::string::npos)
      line.replace(pos, 1, " ");

   // there must be at least one space, to separate key from value
   pos = line.find(' ');
   if (pos == std::string::npos)
      return false;

   // retrieve key and value
   key = line.substr(0, pos);
   value = line.substr(pos + 1);

   // trim spaces at start of "value"
   for (; value.size() > 0 && isspace(value.at(0));)
      value.erase(0, 1);

   return true;
}

/// Processes a keyword/value pair. The following keywords are currently
/// recognized:
/// game-name: specifies game name in future uw menu screens
/// init-scripting: inits scripting engine
/// load-script: loads a given script into scripting engine
/// use-resources: specifies which resources the game has to use (uw1 or uw2)
/// import-strings: imports gamestrings from custom .pak file
/// \param the_name the keyword name
/// \param the_value the keyword value
void GameConfigLoader::load_value(const char* the_name, const char* the_value)
{
   std::string name(the_name);
   std::string value(the_value);

   if (name.compare("game-name") == 0)
   {
      game_name = value;
   }
   else if (name.compare("init-scripting") == 0)
   {
      if (value == "lua")
      {
         // init Lua scripting
         *scripting = IScripting::CreateScripting(scriptingLanguageLua);

         // check if scripting was set
         if (*scripting == NULL)
            throw Base::Exception("could not create scripting object");

         // init scripting
         (*scripting)->Init(&game);
      }
      else
         UaTrace("unsupported scripting language \"%s\"\n", value.c_str());
   }
   else if (name.compare("load-script") == 0)
   {
      // load given lua script name
      game.GetScripting().LoadScript(value.c_str());
   }
   else if (name.compare("use-resources") == 0)
   {
      Base::Settings& settings = game.GetSettings();

      // check if resources to use are available
      if (value.compare("uw1") == 0)
      {
         // check base path
         std::string base = settings.GetString(Base::settingUw1Path);
         if (base.empty())
            throw Base::Exception("path to uw1 was not specified in config file");

         settings.SetGameType(Base::gameUw1);
         settings.SetValue(Base::settingUw1IsUwdemo, false);

         do
         {
            // check for uw game files
            if (IsFileAvailable(base, "data/cnv.ark") ||
               IsFileAvailable(base, "data/strings.pak") ||
               IsFileAvailable(base, "data/pals.dat") ||
               IsFileAvailable(base, "data/allpals.dat"))
            {
               // could be uw1 or uw_demo
               if (IsFileAvailable(base, "uw.exe"))
                  break; // found all needed files
               else
                  // check if we only have the demo
                  if (IsFileAvailable(base, "uwdemo.exe") &&
                     IsFileAvailable(base, "data/level13.st") &&
                     IsFileAvailable(base, "data/level13.anx") &&
                     IsFileAvailable(base, "data/level13.txm"))
                  {
                     // found all needed files for uw_demo
                     settings.SetValue(Base::settingUw1IsUwdemo, true);
                     break;
                  }
            }

            throw Base::Exception("could not find relevant uw1 game files");

         } while (false);

         // set generic uw path to uw1 path
         settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));
      }
      else if (value.compare("uw2") == 0)
      {
         // check base path
         std::string base = settings.GetString(Base::settingUw2Path);
         if (base.empty())
            throw Base::Exception("path to uw2 was not specified in config file");

         // check for uw2 game files
         if (!IsFileAvailable(base, "data/cnv.ark") ||
            !IsFileAvailable(base, "data/strings.pak") ||
            !IsFileAvailable(base, "data/pals.dat") ||
            !IsFileAvailable(base, "data/allpals.dat") ||
            !IsFileAvailable(base, "uw2.exe") ||
            !IsFileAvailable(base, "data/t64.tr"))
            throw Base::Exception("could not find relevant uw2 game files");

         // set generic uw path to uw2 path
         settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw2Path));
         settings.SetGameType(Base::gameUw2);
      }
      else
      {
         // unknown string
         std::string text("unknown use-resources string in game.cfg: ");
         text.append(value);
         throw Base::Exception(text.c_str());
      }
   }
   else if (name.compare("import-strings") == 0)
   {
      // load game strings
      Base::SDL_RWopsPtr rwops = game.GetResourceManager().GetResourceFile(value.c_str());

      if (rwops != NULL)
      {
         // add strings.pak-like file
         Import::GameStringsImporter importer(game.GetGameStrings());
         importer.LoadStringsPakFile(rwops);
      }
      else
         UaTrace("could not load strings file %s\n", value.c_str());
   }
}
