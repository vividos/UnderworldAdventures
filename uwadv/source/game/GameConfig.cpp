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
/// \file GameConfig.cpp
/// \brief game config
//
#include "pch.hpp"
#include "GameConfig.hpp"
#include "TextFile.hpp"
#include "KeyValuePairTextFileReader.hpp"

/// mapping from base game string to base game enum value
static std::map<std::string, BaseGame> c_baseGameMap =
{
   { "uw_demo", BaseGame::BaseGameUwDemo },
   { "uw1", BaseGame::BaseGameUw1 },
   { "uw2", BaseGame::BaseGameUw2 },
   { "none", BaseGame::BaseGameNone },
};

/// mapping from scripting language string to scripting language enum value
static std::map<std::string, ScriptingLanguage> c_scriptingLanguageMap =
{
   { "lua", ScriptingLanguage::scriptingLanguageLua },
};

GameConfig::GameConfig(BaseGame baseGame)
   :m_baseGame(baseGame)
{
   m_isReadOnly = false;
}

std::string GameConfig::GetFilename() const
{
   return m_gameConfigFilename;
}

bool GameConfig::IsReadOnly() const
{
   return m_isReadOnly;
}

BaseGame GameConfig::GetBaseGame() const
{
   return m_baseGame;
}

std::string GameConfig::GetGameName() const
{
   return m_gameName;
}

void GameConfig::SetGameName(std::string gameName)
{
   UaAssert(!IsReadOnly());
   m_gameName = gameName;
}

ScriptingLanguage GameConfig::GetScriptingLanguage() const
{
   return m_scriptingLanguage;
}

const std::vector<std::string>& GameConfig::GetScriptingFilesList() const
{
   return m_scriptingFilesList;
}

std::vector<std::string>& GameConfig::GetScriptingFilesList()
{
   return m_scriptingFilesList;
}

const std::vector<std::string>& GameConfig::GetGameStringsFilesList() const
{
   return m_gameStringsFilesList;
}

std::vector<std::string>& GameConfig::GetGameStringsFilesList()
{
   return m_gameStringsFilesList;
}

void GameConfig::Load(Base::SDL_RWopsPtr gameConfig, std::string gameConfigFilename)
{
   m_gameConfigFilename = gameConfigFilename;

   Base::TextFile textFile{ gameConfig };
   Base::KeyValuePairTextFileReader reader{ textFile };

   std::string key, value;
   while (reader.Next(key, value))
   {
      ProcessParameter(key, value);
   }
}

void GameConfig::Save()
{
   // TODO implement
}

void GameConfig::ProcessParameter(std::string name, std::string value)
{
   if (name == "base-game")
   {
      const auto& iter = c_baseGameMap.find(value);
      if (iter != c_baseGameMap.end())
         m_baseGame = iter->second;
      else
      {
         std::string text("unknown base-game string in game.cfg: ");
         text += value;
         throw Base::Exception(text.c_str());
      }
   }
   else if (name == "game-name")
   {
      m_gameName = value;
   }
   else if (name == "scripting-lang")
   {
      const auto& iter = c_scriptingLanguageMap.find(value);
      if (iter != c_scriptingLanguageMap.end())
         m_scriptingLanguage = iter->second;
      else
         UaTrace("unsupported scripting language \"%s\"\n", value.c_str());
   }
   else if (name == "scripting-file")
   {
      m_scriptingFilesList.push_back(value);
   }
   else if (name == "strings-file")
   {
      m_gameStringsFilesList.push_back(value);
   }
}
