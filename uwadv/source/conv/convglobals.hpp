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
/*! \file convglobals.hpp

   \brief conversation globals

*/
//! \ingroup conv

//@{

// include guard
#ifndef uwadv_convglobals_hpp_
#define uwadv_convglobals_hpp_

// needed includes
#include <vector>
#include "savegame.hpp"


// classes

//! stores all conversation globals
/*! The class contains all conversation globals for each conversation slot and
    should be persisted when conversation or game exits.
*/
class ua_conv_globals
{
public:
   //! ctor
   ua_conv_globals(){}

   //! returns a list of globals for a given conv slot
   std::vector<Uint16>& get_globals(Uint16 conv)
   {
      ua_assert(conv<=allglobals.size());
      return allglobals[conv];
   }

   // loading / saving

   //! loads a savegame
   void load_game(ua_savegame &sg);

   //! saves to a savegame
   void save_game(ua_savegame &sg);

protected:
   friend class ua_uw_import;

   //! returns list of all globals
   std::vector< std::vector<Uint16> >& get_allglobals(){ return allglobals; }

protected:
   //! list with all globals from all conversations
   std::vector< std::vector<Uint16> > allglobals;
};


#endif
//@}
