/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

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

std::vector<ua_map_notes_entry>& ua_map_notes::get_page_notes(
   unsigned int page)
{
   return mapnotes[page];
}

void ua_map_notes::load_game(ua_savegame& sg)
{
   mapnotes.clear();

   Uint8 endpages = sg.read8();

   while(endpages != 0)
   {
      // read page number
      unsigned int page = sg.read32();

      std::vector<ua_map_notes_entry> noteslist;

      Uint8 endlist = sg.read8();

      while(endlist != 0)
      {
         ua_map_notes_entry note;

         // read map note
         note.xpos = sg.read16();
         note.ypos = sg.read16();

         std::string text;
         sg.read_string(note.text);

         noteslist.push_back(note);

         endlist = sg.read8();
      }

      // insert current page's list
      mapnotes.insert( std::make_pair<unsigned int,std::vector<ua_map_notes_entry> >(
         page,noteslist) );
   }
}

void ua_map_notes::save_game(ua_savegame& sg)
{
   sg.begin_section("mapnotes");

   std::map<unsigned int,std::vector<ua_map_notes_entry> >::iterator iter,stop;
   iter = mapnotes.begin();
   stop = mapnotes.end();

   for(;iter!=stop; iter++)
   {
      sg.write8(1);
      sg.write32(iter->first);

      const std::vector<ua_map_notes_entry>& noteslist = iter->second;

      for(unsigned int i=0; i<noteslist.size(); i++)
      {
         sg.write16(1);
         sg.write16(noteslist[i].xpos);
         sg.write16(noteslist[i].ypos);
         sg.write_string(noteslist[i].text.c_str());
      }
      sg.write16(0); // end marker
   }

   sg.write8(0); // end marker

   sg.end_section();
}
