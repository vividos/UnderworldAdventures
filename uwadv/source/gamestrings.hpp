/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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

   game strings are contained in blocks that contain a list of strings. each
   block contains specific strings, for item descriptions, cutscene text
   or conversation strings.

*/

// include guard
#ifndef __uwadv_gamestrings_hpp_
#define __uwadv_gamestrings_hpp_

// needed includes
#include <map>
#include <vector>
#include <string>
#include "settings.hpp"


// classes

//! game string container class
class ua_gamestrings
{
public:
   //! ctor
   ua_gamestrings(){}

   //! loads all game strings
   void load(ua_settings &settings) throw(ua_exception);

   //! loads all game strings from a file
   void load(const char *filename) throw(ua_exception);

   //! loads all game strings from a RWops
   void load(SDL_RWops *rwops);

   //! returns the whole strings block
   std::map<int,std::vector<std::string> > &get_allstrings(){ return allstrings; }

   //! returns a whole string block
   std::vector<std::string> &get_block(unsigned int block);

   //! returns a game string
   std::string get_string(unsigned int block, unsigned int string_nr);

protected:
   //! game string container
   std::map<int,std::vector<std::string> > allstrings;
};

#endif
