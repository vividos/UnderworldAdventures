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
/// \file Runes.hpp
/// \brief runebag and runeshelf
//
#pragma once

#include <vector>
#include <bitset>

namespace Base
{
   class Savegame;
}

namespace Underworld
{
   /// \brief Rune type
   /// \note in uw1 and uw2 there is no rune for Xen and Zu.
   enum RuneType
   {
      runeAn = 0,
      runeBet,
      runeCorp,
      runeDes,
      runeEx,
      runeFlam,
      runeGrav,
      runeHur,
      runeIn,
      runeJux,
      runeKal,
      runeLor,
      runeMani,
      runeNox,
      runeOrt,
      runePor,
      runeQuas,
      runeRel,
      runeSanct,
      runeTym,
      runeUus,
      runeVas,
      runeWis,
      runeYlem,

      runeLast = runeYlem
   };

   /// Runeshelf class
   class Runeshelf
   {
   public:
      /// ctor
      Runeshelf() {}

      /// returns number of runes on shelf (max. 3 runes)
      unsigned int GetNumRunes() const { return m_runeshelf.size(); }

      /// returns rune on shelf position
      RuneType GetRune(unsigned int shelfIndex) const
      {
         UaAssert(shelfIndex < m_runeshelf.size());
         return m_runeshelf[shelfIndex];
      }

      /// adds a rune to the runeshelf
      void AddRune(RuneType rune)
      {
         if (m_runeshelf.size() > 2)
            m_runeshelf.erase(m_runeshelf.begin());

         m_runeshelf.push_back(rune);
      }

      /// resets runeshelf contents
      void Reset()
      {
         m_runeshelf.clear();
      }

      // loading / saving

      /// loads runebag from savegame
      void Load(Base::Savegame& sg);

      /// saves runebag to a savegame
      void Save(Base::Savegame& sg) const;

   private:
      /// runeshelf contents
      std::vector<RuneType> m_runeshelf;
   };

   /// Runebag class
   class Runebag
   {
   public:
      /// ctor
      Runebag() {}

      /// checks if rune is in bag
      bool IsInBag(RuneType rune) const { return m_runebag.test(static_cast<size_t>(rune)); }

      /// sets availability of rune in bag
      void SetRune(RuneType rune, bool isAvail) { m_runebag.set(static_cast<size_t>(rune), isAvail); }

      /// returns runebag as bitset
      std::bitset<24>& GetBitsetRunebag() { return m_runebag; }

      // loading / saving

      /// loads runebag from savegame
      void Load(Base::Savegame& sg);

      /// saves runebag to a savegame
      void Save(Base::Savegame& sg) const;

   private:
      /// rune bag
      std::bitset<24> m_runebag;
   };

} // namespace Underworld
