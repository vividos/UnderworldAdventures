//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019,2022 Underworld Adventures Team
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
/// \file MapNotes.cpp
/// \brief map notes implementation
//
#include "pch.hpp"
#include "MapNotes.hpp"
#include "Savegame.hpp"
#include <limits>

using Underworld::MapNotes;
using Underworld::MapNote;

void MapNotes::Load(Base::Savegame& sg)
{
   sg.BeginSection("mapnotes");

   Uint16 numEntries = sg.Read16();

   m_mapNotesList.clear();
   m_mapNotesList.resize(numEntries);

   for (size_t entryIndex = 0; entryIndex < numEntries; entryIndex++)
   {
      MapNote& mapNote = m_mapNotesList[entryIndex];

      mapNote.m_xpos = sg.Read16();
      mapNote.m_ypos = sg.Read16();

      sg.ReadString(mapNote.m_text);
   }
}

void MapNotes::Save(Base::Savegame& sg) const
{
   sg.BeginSection("mapnotes");

   Uint16 numEntries = static_cast<Uint16>(m_mapNotesList.size());
   sg.Write16(numEntries);

   for (size_t entryIndex = 0; entryIndex < numEntries; entryIndex++)
   {
      const MapNote& mapNote = m_mapNotesList[entryIndex];

      sg.Write16(mapNote.m_xpos);
      sg.Write16(mapNote.m_ypos);
      sg.WriteString(mapNote.m_text);
   }

   sg.EndSection();
}
