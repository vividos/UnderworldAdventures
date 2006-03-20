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
/*! \file runes.cpp

   \brief runebag and runeshelf

*/

// needed includes
#include "underworld.hpp"
#include "runes.hpp"
#include "savegame.hpp"

using Underworld::Runebag;
using Underworld::Runeshelf;

// Runebag methods

void Runebag::Load(Base::Savegame& sg)
{
   sg.BeginSection("runebag");

   // load runebag
   m_runebag.reset();

   Uint32 uiBagbits = sg.Read32();
   for(unsigned int i=0; i<24; i++)
      if ((uiBagbits & (1L << i)) != 0)
         m_runebag.set(i);

   sg.EndSection();
}

/*! \note std::bitset::to_ulong() could be used to convert the bitset to an
    int, but it probably isn't portable enough
*/
void Runebag::Save(Base::Savegame& sg) const
{
   sg.BeginSection("runebag");

   // store runebag
   Uint32 uiBagbits = 0;
   for(unsigned int i=0; i<24; i++)
      if (m_runebag.test(i))
         uiBagbits |= 1L << i;

   sg.Write32(uiBagbits);

   sg.EndSection();
}

/* \todo load runeshelf
      // load runeshelf
      runes.reset_runeshelf();
      unsigned int count = sg.read8();
      for(Uint8 n=0; n<count; n++)
         runes.add_runeshelf_rune(sg.read8());

      // store runeshelf
      Uint8 count = runes.get_runeshelf_count() % 3;
      sg.write8(count);
      for(unsigned int n=0; n<count; n++)
         sg.write8(runes.get_runeshelf_rune(n));
*/
