//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2023 Underworld Adventures Team
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
/// \file GameConfig.hpp
/// \brief game config
//
#pragma once

#include "IScripting.hpp"

/// specifies the base game that the game config references
enum BaseGame
{
   BaseGameUwDemo,   ///< the uw demo is enough
   BaseGameUw1,      ///< using uw1 as base game
   BaseGameUw2,      ///< using uw2 as base game
   BaseGameNone,     ///< not using any game as base game
};

/// game configuration
class GameConfig
{
public:
   /// ctor
   GameConfig(BaseGame baseGame = BaseGameNone);

   /// returns the full file name where the game.cfg file was stored (may be
   /// in a zip archive)
   std::string GetFilename() const;

   /// returns if a game config (file) is read-only, e.g. when it's in a zip file
   /// or it's one of the two base game 'game.cfg' files for uw1 or uw2.
   bool IsReadOnly() const;

   /// returns which base game is needed by this game config
   BaseGame GetBaseGame() const;

   /// returns the game's name
   std::string GetGameName() const;

   /// sets a new game name; only possible when the game config is not read-only
   void SetGameName(std::string gameName);

   /// returns the scripting language used for this game
   ScriptingLanguage GetScriptingLanguage() const;

   /// returns list of scripting files; const version
   const std::vector<std::string>& GetScriptingFilesList() const;
   /// returns list of scripting files
   std::vector<std::string>& GetScriptingFilesList();

   /// returns list of additional game strings files; const version
   const std::vector<std::string>& GetGameStringsFilesList() const;
   /// returns list of additional game strings files
   std::vector<std::string>& GetGameStringsFilesList();

   /// loads the game config from text file
   void Load(Base::SDL_RWopsPtr gameConfig, std::string gameConfigFilename);

   /// saves the game config to the file name that it was loaded from
   void Save();

private:
   /// processes parameter when loading game config file
   void ProcessParameter(std::string name, std::string value);

private:
   /// filename of the game.cfg file
   std::string m_gameConfigFilename;

   /// indicates if the game config file is read only
   bool m_isReadOnly = false;

   /// the base game type
   BaseGame m_baseGame;

   /// the game's name
   std::string m_gameName;

   /// the scripting language used by the game
   ScriptingLanguage m_scriptingLanguage = scriptingLanguageLua;

   /// list of scripting files to load
   std::vector<std::string> m_scriptingFilesList;

   /// list of additional game strings files to load
   std::vector<std::string> m_gameStringsFilesList;
};
