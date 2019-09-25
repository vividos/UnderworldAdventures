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
/// \file GameConfigLoader.hpp
/// \brief game config loader
//
#pragma once

#include "ConfigFile.hpp"
#include <string>

class IBasicGame;
class IScripting;

/// \brief game.cfg loader
/// Loads game configuration from game.cfg file found in uadata subfolders
/// prefixed with the current game prefix (%game-prefix%/game.cfg).
///
/// To start loading, call Base::ConfigFile::Load().
class GameConfigLoader
{
public:
   /// ctor
   GameConfigLoader(IBasicGame& game,
      IScripting** scripting)
      :m_game(game),
      m_scripting(scripting)
   {
   }

   /// loads game.cfg from given text file
   void Load(Base::TextFile& file);

   /// returns game name
   const char* GetGameName() { return m_gameName.c_str(); }

private:
   /// processes a parameter from the game config file
   void ProcessParameter(std::string name, std::string value);

private:
   /// game interface
   IBasicGame& m_game;

   /// pointer to pointer to scripting object to initialize
   IScripting** m_scripting;

   /// game name
   std::string m_gameName;
};
