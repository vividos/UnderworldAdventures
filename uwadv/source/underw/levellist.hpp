//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Michael Fink
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
/// \file levellist.hpp
/// \brief level list
//
#pragma once

#include "level.hpp"

namespace Base
{
   class Savegame;
}

namespace Underworld
{
   /// list of all levels
   class LevelList
   {
   public:
      /// ctor
      LevelList() {}

      /// returns number of levels in list
      unsigned int GetNumLevels() const { return m_levelList.size(); }

      /// returns level
      Level& GetLevel(unsigned int levelIndex)
      {
         UaAssert(levelIndex < GetNumLevels());
         return m_levelList[levelIndex];
      }

      /// returns level; const version
      const Level& GetLevel(unsigned int levelIndex) const
      {
         UaAssert(levelIndex < GetNumLevels());
         return m_levelList[levelIndex];
      }

      // loading/saving

      /// loads levelmaps from savegame
      void Load(Base::Savegame& sg);

      /// saves levelmaps to savegame
      void Save(Base::Savegame& sg) const;

      /// returns all levelmaps in a vector
      std::vector<Level>& GetVectorLevels() { return m_levelList; }

   private:
      /// all underworld levels
      std::vector<Level> m_levelList;
   };

} // namespace Underworld
