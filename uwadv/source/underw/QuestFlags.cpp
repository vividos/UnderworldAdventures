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
/// \file QuestFlags.cpp
/// \brief quest flags
//
#include "underw.hpp"
#include "QuestFlags.hpp"
#include "Savegame.hpp"

using Underworld::QuestFlags;

void QuestFlags::Load(Base::Savegame& sg)
{
   sg.BeginSection("questflags");

   Uint16 numFlags = sg.Read16();

   m_questFlags.clear();
   m_questFlags.resize(numFlags, 0);

   for (Uint16 flagIndex = 0; flagIndex < numFlags; flagIndex++)
      m_questFlags[flagIndex] = sg.Read16();

   sg.EndSection();
}

void QuestFlags::Save(Base::Savegame& sg) const
{
   sg.BeginSection("questflags");

   Uint16 numFlags = static_cast<Uint16>(GetFlagCount());
   sg.Write16(numFlags);

   for (Uint16 flagIndex = 0; flagIndex < numFlags; flagIndex++)
      sg.Write16(m_questFlags[flagIndex]);

   sg.EndSection();
}
