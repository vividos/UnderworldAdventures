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
/*! \file convglobals.cpp

   \brief conversation globals

*/

// needed includes
#include "underworld.hpp"
#include "convglobals.hpp"
#include "savegame.hpp"

using Underworld::ConvGlobals;

// ConvGlobals methods

void ConvGlobals::Load(Base::Savegame& sg)
{
   sg.BeginSection("conv.globals");

   // get number of slots
   Uint16 uiMaxSlots = sg.Read16();

   m_vecAllGlobals.clear();
   m_vecAllGlobals.reserve(uiMaxSlots);

   for (Uint16 uiSlot=0; uiSlot<uiMaxSlots; uiSlot++)
   {
      // get number of slot globals
      Uint16 uiMaxGlobals = sg.Read16();

      ConvSlotGlobals vecSlotGlobals;
      vecSlotGlobals.reserve(uiMaxGlobals);

      // get all slot globals
      for(Uint16 ui=0; ui<uiMaxGlobals; ui++)
         vecSlotGlobals.push_back(sg.Read16());

      m_vecAllGlobals.push_back(vecSlotGlobals);
   }

   sg.EndSection();
}

void ConvGlobals::Save(Base::Savegame& sg) const
{
   sg.BeginSection("conv.globals");

   // write number of conv slots
   Uint16 uiMaxSlots = static_cast<Uint16>(m_vecAllGlobals.size());
   sg.Write16(uiMaxSlots);

   // for each slot ...
   for (unsigned int uiSlot=0; uiSlot<uiMaxSlots; uiSlot++)
   {
      const ConvSlotGlobals& vecSlotGlobals = m_vecAllGlobals[uiSlot];

      // write number of slot globals
      Uint16 uiMaxGlobals = static_cast<Uint16>(vecSlotGlobals.size());
      sg.Write16(uiMaxGlobals);

      // write all globals
      for(unsigned int ui=0; ui<uiMaxGlobals; ui++)
         sg.Write16(vecSlotGlobals[ui]);
   }

   sg.EndSection();
}
