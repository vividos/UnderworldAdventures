/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2004 Underworld Adventures Team

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
/*! \file runes.hpp

   \brief runebag and runeshelf

*/
//! \ingroup underworld

//@{

// include guard
#ifndef uwadv_runes_hpp_
#define uwadv_runes_hpp_

// needed includes
#include "savegame.hpp"
#include <bitset>


// classes

//! runes class
/*! The runes class contains the runeshelf and the runebag contents. Note that
    there are only 24 runes, since X and Y don't have associated rune stones.
*/
class ua_runes
{
public:
   //! ctor
   ua_runes(){}

   // runeshelf related

   //! returns number of runes on shelf (max. 3 runes)
   unsigned int get_runeshelf_count();

   //! returns rune on runeshelf; 0-based
   Uint8 get_runeshelf_rune(unsigned int pos);

   //! adds a rune to the runeshelf
   void add_runeshelf_rune(Uint8 rune);

   //! resets runeshelf contents
   void reset_runeshelf();


   // runebag related

   //! returns runebag as bitset
   std::bitset<24>& get_runebag();


   // loading/saving/importing

   //! loads a savegame
   void load_game(ua_savegame& sg);

   //! saves to a savegame
   void save_game(ua_savegame& sg);

protected:
   //! runeshelf contents
   std::vector<Uint8> runeshelf;

   //! rune bag
   std::bitset<24> runebag;
};


// inline methods
inline unsigned int ua_runes::get_runeshelf_count()
{
   return runeshelf.size();
}

inline Uint8 ua_runes::get_runeshelf_rune(unsigned int pos)
{
   return pos > runeshelf.size() ? 0 : runeshelf[pos];
}

inline void ua_runes::add_runeshelf_rune(Uint8 rune)
{
   if (runeshelf.size()>2) runeshelf.erase(runeshelf.begin());
   runeshelf.push_back(rune);
}

inline void ua_runes::reset_runeshelf()
{
   runeshelf.empty();
}

inline std::bitset<24>& ua_runes::get_runebag()
{
   return runebag;
}


#endif
//@}
