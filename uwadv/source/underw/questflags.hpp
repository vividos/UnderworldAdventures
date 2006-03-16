/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2002,2003,2004,2005,2006 Michael Fink

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file questflags.hpp

   \brief quest flags

*/

// include guard
#ifndef uwadv_underw_questflags_hpp_
#define uwadv_underw_questflags_hpp_

// needed includes
#include <vector>
#include "savegame.hpp"

namespace Underworld
{

//! Quest flags
/*! Quest flags are used throughout the game to store flags that are persisted
    in savegames, e.g. to track game progress. Quest flags can be queried and
    modified from conversations and by triggers/traps.
*/
class QuestFlags
{
public:
   //! ctor
   QuestFlags(){}

   //! returns number of flags
   unsigned int GetFlagCount() const { return m_vecQuestFlags.size(); }

   //! returns quest flag by index
   Uint16 GetFlag(unsigned int uiIndex)
   {
      UaAssert(uiIndex < GetFlagCount());
      return m_vecQuestFlags[uiIndex];
   }

   //! sets quest flag
   void SetFlag(unsigned int uiIndex, Uint16 uiFlagValue)
   {
      UaAssert(uiIndex < GetFlagCount());
      m_vecQuestFlags[uiIndex] = uiFlagValue;
   }

   // loading / saving

   //! loads flags from savegame
   void Load(Base::Savegame& sg)
   {
      sg.BeginSection("questflags");

      Uint16 uiMax = sg.Read16();

      m_vecQuestFlags.clear();
      m_vecQuestFlags.resize(uiMax, 0);

      for (Uint16 ui=0; ui<uiMax; ui++)
         m_vecQuestFlags[ui] = sg.Read16();

      sg.EndSection();
   }

   //! saves flags to savegame
   void Save(Base::Savegame& sg) const
   {
      sg.BeginSection("questflags");

      Uint16 uiMax = static_cast<Uint16>(GetFlagCount());
      sg.Write16(uiMax);

      for(Uint16 ui=0; ui<uiMax; ui++)
         sg.Write16(m_vecQuestFlags[ui]);

      sg.EndSection();
   }

private:
   //! quest flags
   std::vector<Uint16> m_vecQuestFlags;
};

} // namespace Underworld

#endif
