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
/// \file QuestFlags.hpp
/// \brief quest flags
//
#pragma once

#include <vector>
#include "Savegame.hpp"

namespace Underworld
{
   /// \brief Quest flags
   /// \details Quest flags are used throughout the game to store flags that are persisted
   /// in savegames, e.g. to track game progress. Quest flags can be queried and
   /// modified from conversations and by triggers/traps.
   class QuestFlags
   {
   public:
      /// ctor
      QuestFlags() {}

      /// returns number of flags
      unsigned int GetFlagCount() const { return m_questFlags.size(); }

      /// returns quest flag by index
      Uint16 GetFlag(unsigned int flagIndex) const
      {
         UaAssert(flagIndex < GetFlagCount());
         return m_questFlags[flagIndex];
      }

      /// sets quest flag
      void SetFlag(unsigned int flagIndex, Uint16 uiFlagValue)
      {
         UaAssert(flagIndex < GetFlagCount());
         m_questFlags[flagIndex] = uiFlagValue;
      }

      // loading / saving

      /// loads flags from savegame
      void Load(Base::Savegame& sg);

      /// saves flags to savegame
      void Save(Base::Savegame& sg) const;

   private:
      /// quest flags
      std::vector<Uint16> m_questFlags;
   };

} // namespace Underworld
