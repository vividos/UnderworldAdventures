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
/// \file MapNotes.hpp
/// \brief map notes
//
#pragma once

#include <vector>

namespace Base
{
   class Savegame;
}

namespace Underworld
{
   /// map note entry
   struct MapNote
   {
      /// ctor
      MapNote()
         :m_xpos(0.0),
         m_ypos(0.0)
      {}

      /// relative x position on map
      double m_xpos;

      /// relative x position on map
      double m_ypos;

      /// note text
      std::string m_text;
   };

   /// map notes class
   class MapNotes
   {
   public:
      /// ctor
      MapNotes() { }

      /// returns number of notes
      size_t GetMapNoteCount() const { return m_mapNotesList.size(); }

      /// returns a single note
      MapNote& GetNote(size_t mapNoteIndex)
      {
         UaAssert(mapNoteIndex < GetMapNoteCount());
         return m_mapNotesList[mapNoteIndex];
      }

      /// returns list of notes
      std::vector<MapNote>& GetMapNotesList() { return m_mapNotesList; }

      // loading / saving

      /// loads notes from savegame
      void Load(Base::Savegame& sg);

      /// saves notes to savegame
      void Save(Base::Savegame& sg) const;

   private:
      /// list of all map notes
      std::vector<MapNote> m_mapNotesList;
   };

} // namespace Underworld
