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
/*! \file gamestrings.hpp

   \brief game strings class

*/
//! \ingroup game

//@{

// include guard
#ifndef uwadv_gamestrings_hpp_
#define uwadv_gamestrings_hpp_

// needed includes
#include <map>
#include <vector>
#include <string>
#include "settings.hpp"


// forward-references
//class ua_strings_pak_file;
#include "import.hpp"


// classes

//! game strings class
/* Game strings are contained in blocks that contain a list of strings. Each
   block contains specific strings, for item descriptions, cutscene text or
   conversations.
*/
class ua_gamestrings
{
public:
   //! ctor
   ua_gamestrings(){}

   //! inits game strings object
   void init(ua_settings& settings);

   //! adds an external .pak file
   void add_pak_file(const char* filename);

   //! adds an external .pak file from rwops structure
   void add_pak_file(SDL_RWops* rwops);

   //! returns if block id is available
   bool is_avail(Uint16 block_id);

   //! returns a whole string block
   void get_stringblock(Uint16 block_id, std::vector<std::string>& strblock);

   //! returns a string from given block
   std::string get_string(Uint16 block_id, unsigned int string_nr);

protected:
   //! loads string block into allstrings map
   void load_stringblock(Uint16 block_id);

   //! decrease lifetime of all blocks in map lifetimes
   void decrease_lifetimes(Uint16 except_for_block_id);

protected:
   //! a map with all string blocks
   std::map<Uint16, std::vector<std::string> > allstrings;

   //! lifetimes for blocks that can be garbage-collected
   std::map<Uint16, unsigned int> lifetimes;

   //! all strings.pak files available for loading strings
   std::vector<ua_strings_pak_file> allpakfiles;
};


#endif
//@}
