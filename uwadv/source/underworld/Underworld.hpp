//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Underworld Adventures Team
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
/// \file Underworld.hpp
/// \brief underworld
//
#pragma once

#include "Base.hpp"
#include "LevelList.hpp"
#include "Player.hpp"

namespace Base
{
   class Savegame;
}

/// underworld objects
namespace Underworld
{
   /// The underworld
   class Underworld
   {
   public:
      /// ctor
      Underworld() {}

      // access to underworld components

      /// returns level list
      LevelList& GetLevelList() { return m_levelList; }
      /// returns level list; const version
      const LevelList& GetLevelList() const { return m_levelList; }

      /// returns player
      Player& GetPlayer() { return m_player; }
      /// returns player; const version
      const Player& GetPlayer() const { return m_player; }

      /// returns current player level
      Level& GetCurrentLevel();

      /// returns current player level; const version
      const Level& GetCurrentLevel() const;

      // loading / saving

      /// loads the underworld from a savegame
      void Load(Base::Savegame& sg);

      /// saves the underworld to a savegame
      void Save(Base::Savegame& sg) const;

   private:
      /// list with all levels
      LevelList m_levelList;

      /// player
      Player m_player;
   };

} // namespace Underworld
