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
/*! \file mapnotes.cpp

   \brief map notes implementation

*/

// needed includes
#include "underw.hpp"
#include "mapnotes.hpp"
#include "savegame.hpp"
#include <limits>

using Underworld::MapNotes;
using Underworld::MapNote;

// MapNotes methods

void MapNotes::Load(Base::Savegame& sg)
{
   sg.BeginSection("mapnotes");

   Uint16 uiEntries = sg.Read16();

   m_vecNotes.clear();
   m_vecNotes.resize(uiEntries);

   for (unsigned int i=0; i<uiEntries; i++)
   {
      MapNote& note = m_vecNotes[i];

      // read map note
      note.m_xpos = static_cast<double>(sg.Read16()) / std::numeric_limits<Uint16>::max();
      note.m_ypos = static_cast<double>(sg.Read16()) / std::numeric_limits<Uint16>::max();

      sg.ReadString(note.m_strText);
   }
}

void MapNotes::Save(Base::Savegame& sg) const
{
   sg.BeginSection("mapnotes");

   Uint16 uiEntries = static_cast<Uint16>(m_vecNotes.size());
   sg.Write16(uiEntries);

   for (unsigned int i=0; i<uiEntries; i++)
   {
      const MapNote& note = m_vecNotes[i];

      sg.Write16(static_cast<Uint16>(note.m_xpos * std::numeric_limits<Uint16>::max()));
      sg.Write16(static_cast<Uint16>(note.m_ypos * std::numeric_limits<Uint16>::max()));
      sg.WriteString(note.m_strText);
   }

   sg.EndSection();
}
