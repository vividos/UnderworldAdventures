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
   GameConfigLoader(IBasicGame& the_game,
      IScripting** the_scripting)
      :game(the_game), scripting(the_scripting)
   {
   }

   /// loads game.cfg from given text file
   void Load(Base::TextFile& file);

   /// returns game name
   const char* get_game_name() { return game_name.c_str(); }

private:
   /// splits a text file line into key and value, when found
   bool SplitKeyValue(std::string& line, std::string& key, std::string& value);

   /// called to load a specific value
   virtual void load_value(const char* name, const char* value);

private:
   /// game interface
   IBasicGame& game;

   /// pointer to pointer to scripting object to initialize
   IScripting** scripting;

   /// game name
   std::string game_name;
};
