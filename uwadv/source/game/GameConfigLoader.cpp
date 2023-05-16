//
// Underworld Adventures - an Ultima Underworld remake project
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
#include "pch.hpp"
#include "GameConfigLoader.hpp"
#include "TextFile.hpp"
#include "KeyValuePairTextFileReader.hpp"
#include "GameInterface.hpp"
#include "Settings.hpp"
#include "FileSystem.hpp"
#include "ResourceManager.hpp"
#include "Underworld.hpp"
#include "IScripting.hpp"
#include "GameStrings.hpp"
#include "GameStringsImporter.hpp"

void GameConfigLoader::Load(Base::TextFile& file)
{
   Base::KeyValuePairTextFileReader reader(file);

   std::string key, value;
   while (reader.Next(key, value))
   {
      ProcessParameter(key, value);
   }
}

/// Processes a keyword/value pair. The following keywords are currently
/// recognized:
/// game-name: specifies game name in future uw menu screens
/// scripting-lang: inits scripting engine
/// scripting-file: loads a given script into scripting engine
/// base-game: specifies which resources the game has to use (uw1 or uw2)
/// strings-file: imports gamestrings from custom .pak file
/// \param name the keyword name
/// \param value the keyword value
void GameConfigLoader::ProcessParameter(std::string name, std::string value)
{
   if (name.compare("game-name") == 0)
   {
      m_gameName = value;
   }
   else if (name.compare("scripting-lang") == 0)
   {
      if (value == "lua")
      {
         // init Lua scripting
         *m_scripting = IScripting::CreateScripting(scriptingLanguageLua);

         // check if scripting was set
         if (*m_scripting == NULL)
            throw Base::Exception("could not create scripting object");

         (*m_scripting)->Init(&m_game);
      }
      else
         UaTrace("unsupported scripting language \"%s\"\n", value.c_str());
   }
   else if (name.compare("scripting-file") == 0)
   {
      // load given lua script name
      m_game.GetScripting().LoadScript(value.c_str());
   }
   else if (name.compare("base-game") == 0)
   {
      Base::Settings& settings = m_game.GetSettings();

      // check if resources to use are available
      if (value.compare("uw1") == 0)
      {
         // check base path
         std::string base = settings.GetString(Base::settingUw1Path);
         if (base.empty())
            throw Base::Exception("path to uw1 was not specified in config file");

         // check game files
         Base::ResourceManager& resourceManager = m_game.GetResourceManager();

         bool uw1IsDemo = false;
         if (resourceManager.CheckUw1GameFilesAvailable(uw1IsDemo))
         {
            settings.SetGameType(Base::gameUw1);
            settings.SetValue(Base::settingUw1IsUwdemo, uw1IsDemo);
         }
         else
            throw Base::Exception("could not find relevant uw1 game files");

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
         Base::ResourceManager& resourceManager = m_game.GetResourceManager();

         if (resourceManager.CheckUw2GameFilesAvailable())
            settings.SetGameType(Base::gameUw2);
         else
            throw Base::Exception("could not find relevant uw2 game files");

         // set generic uw path to uw2 path
         settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw2Path));
         settings.SetGameType(Base::gameUw2);
      }
      else
      {
         std::string text("unknown base-game string in game.cfg: ");
         text.append(value);
         throw Base::Exception(text.c_str());
      }
   }
   else if (name.compare("strings-file") == 0)
   {
      // load game strings
      Base::SDL_RWopsPtr rwops = m_game.GetResourceManager().GetResourceFile(value.c_str());

      if (rwops != NULL)
      {
         // add strings.pak-like file
         Import::GameStringsImporter importer(m_game.GetGameStrings());
         importer.LoadStringsPakFile(rwops);
      }
      else
         UaTrace("could not load strings file %s\n", value.c_str());
   }
}
