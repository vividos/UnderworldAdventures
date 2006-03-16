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
/*! \file convglobals.hpp

   \brief conversation globals

*/

// include guard
#ifndef uwadv_underw_convglobals_hpp_
#define uwadv_underw_convglobals_hpp_

// needed includes
#include <vector>

namespace Base
{
   class Savegame;
}

namespace Underworld
{

// classes

//! Conversation globals
/*! Stores all conversation globals for each conversation slot. Conversation
    globals are loaded into the VM before executing the conversation script and
    are retrieved after conversation ends.
*/
class ConvGlobals
{
public:
   // types

   //! list of global variables for one conversation slot
   typedef std::vector<Uint16> ConvSlotGlobals;

   //! global variables for all slots
   typedef std::vector<ConvSlotGlobals> ConvGlobalsList;

   //! ctor
   ConvGlobals(){}

   //! returns number of slots
   unsigned int GetSlotCount() const { return m_vecAllGlobals.size(); }

   // loading / saving

   //! loads globals from savegame
   void Load(Base::Savegame& sg);

   //! saves globals to savegame
   void Save(Base::Savegame& sg) const;

   // direct access

   //! returns a list of globals for a given conv slot
   ConvSlotGlobals& GetVectorGlobals(Uint16 uiConvSlot)
   {
      UaAssert(uiConvSlot < GetSlotCount());
      return m_vecAllGlobals[uiConvSlot];
   }

   //! returns globals for all slots
   ConvGlobalsList& GetVectorGlobalsList(){ return m_vecAllGlobals; }

protected:
   //! list with all globals from all conversations
   ConvGlobalsList m_vecAllGlobals;
};

} // namespace Underworld

#endif
