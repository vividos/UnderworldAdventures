/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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
#include "common.hpp"
#include "mapnotes.hpp"


// ua_map_notes methods

void ua_map_notes::load_game(ua_savegame& sg)
{
   sg.begin_section("mapnotes");

   Uint16 numentries = sg.read16();

   mapnotes.clear();
   mapnotes.resize(numentries);

   for(unsigned int i=0; i<numentries; i++)
   {
      ua_map_notes_entry& note = mapnotes[i];

      // read map note
      note.xpos = sg.read16();
      note.ypos = sg.read16();

      sg.read_string(note.text);
   }

   sg.end_section();
}

void ua_map_notes::save_game(ua_savegame& sg)
{
   sg.begin_section("mapnotes");

   Uint16 numentries = mapnotes.size();
   sg.write16(numentries);

   for(unsigned int i=0; i<numentries; i++)
   {
      ua_map_notes_entry& note = mapnotes[i];

      sg.write16(note.xpos);
      sg.write16(note.ypos);
      sg.write_string(note.text.c_str());
   }

   sg.end_section();
}
