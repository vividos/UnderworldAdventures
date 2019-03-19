//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Michael Fink
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
/// \file ConvGlobals.cpp
/// \brief conversation globals
//
#include "pch.hpp"
#include "ConvGlobals.hpp"
#include "Savegame.hpp"

using Underworld::ConvGlobals;

void ConvGlobals::Load(Base::Savegame& sg)
{
   sg.BeginSection("conv.globals");

   Uint16 maxSlotIndex = sg.Read16();

   m_allGlobalsList.clear();
   m_allGlobalsList.reserve(maxSlotIndex);

   for (Uint16 slotIndex = 0; slotIndex < maxSlotIndex; slotIndex++)
   {
      Uint16 maxGlobalsIndex = sg.Read16();

      ConvSlotGlobals slotGlobalsList;
      slotGlobalsList.reserve(maxGlobalsIndex);

      for (Uint16 globalsIndex = 0; globalsIndex < maxGlobalsIndex; globalsIndex++)
         slotGlobalsList.push_back(sg.Read16());

      m_allGlobalsList.push_back(slotGlobalsList);
   }

   sg.EndSection();
}

void ConvGlobals::Save(Base::Savegame& sg) const
{
   sg.BeginSection("conv.globals");

   Uint16 maxSlotIndex = static_cast<Uint16>(m_allGlobalsList.size());
   sg.Write16(maxSlotIndex);

   for (unsigned int slotIndex = 0; slotIndex < maxSlotIndex; slotIndex++)
   {
      const ConvSlotGlobals& slotGlobalsList = m_allGlobalsList[slotIndex];

      Uint16 maxGlobalsIndex = static_cast<Uint16>(slotGlobalsList.size());
      sg.Write16(maxGlobalsIndex);

      for (unsigned int globalsIndex = 0; globalsIndex < maxGlobalsIndex; globalsIndex++)
         sg.Write16(slotGlobalsList[globalsIndex]);
   }

   sg.EndSection();
}
