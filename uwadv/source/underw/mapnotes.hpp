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
/*! \file mapnotes.hpp

   \brief map notes class

*/
//! \ingroup underworld

//@{

// include guard
#ifndef uwadv_mapnotes_hpp_
#define uwadv_mapnotes_hpp_

// needed includes
#include "savegame.hpp"


// structs

//! map notes entry
struct ua_map_notes_entry
{
   //! position on screen
   unsigned int xpos,ypos;

   //! text to appear
   std::string text;
};


// classes

//! map notes class
class ua_map_notes
{
public:
   //! ctor
   ua_map_notes(){}

   //! returns list of notes for given page
   std::vector<ua_map_notes_entry>& get_page_notes(unsigned int page);

   // loading / saving

   //! loads a savegame
   void load_game(ua_savegame& sg);

   //! saves to a savegame
   void save_game(ua_savegame& sg);

protected:
   //! map with lists of all notes
   std::map<unsigned int,std::vector<ua_map_notes_entry> > mapnotes;
};


#endif
//@}
