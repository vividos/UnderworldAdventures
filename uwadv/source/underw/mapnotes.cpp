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
/// \file mapnotes.cpp
/// \brief map notes implementation
//
#include "underw.hpp"
#include "mapnotes.hpp"
#include "savegame.hpp"
#include <limits>

using Underworld::MapNotes;
using Underworld::MapNote;

void MapNotes::Load(Base::Savegame& sg)
{
   sg.BeginSection("mapnotes");

   Uint16 numEntries = sg.Read16();

   m_mapNotesList.clear();
   m_mapNotesList.resize(numEntries);

   for (unsigned int entryIndex = 0; entryIndex < numEntries; entryIndex++)
   {
      MapNote& mapNote = m_mapNotesList[entryIndex];

      mapNote.m_xpos = static_cast<double>(sg.Read16()) / std::numeric_limits<Uint16>::max();
      mapNote.m_ypos = static_cast<double>(sg.Read16()) / std::numeric_limits<Uint16>::max();

      sg.ReadString(mapNote.m_text);
   }
}

void MapNotes::Save(Base::Savegame& sg) const
{
   sg.BeginSection("mapnotes");

   Uint16 numEntries = static_cast<Uint16>(m_mapNotesList.size());
   sg.Write16(numEntries);

   for (unsigned int entryIndex = 0; entryIndex < numEntries; entryIndex++)
   {
      const MapNote& mapNote = m_mapNotesList[entryIndex];

      sg.Write16(static_cast<Uint16>(mapNote.m_xpos * std::numeric_limits<Uint16>::max()));
      sg.Write16(static_cast<Uint16>(mapNote.m_ypos * std::numeric_limits<Uint16>::max()));
      sg.WriteString(mapNote.m_text);
   }

   sg.EndSection();
}
