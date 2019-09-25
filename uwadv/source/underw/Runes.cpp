//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Underworld Adventures Team
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
/// \file Runes.cpp
/// \brief runebag and runeshelf
//
#include "pch.hpp"
#include "Runes.hpp"
#include "Savegame.hpp"

using Underworld::Runebag;
using Underworld::Runeshelf;

void Runeshelf::Load(Base::Savegame& sg)
{
   sg.BeginSection("runeshelf");

   Reset();

   unsigned int runeCount = sg.Read8();
   for (Uint8 shelfIndex = 0; shelfIndex < runeCount; shelfIndex++)
      AddRune(static_cast<RuneType>(sg.Read8()));

   sg.EndSection();
}

void Runeshelf::Save(Base::Savegame& sg) const
{
   sg.BeginSection("runeshelf");

   Uint8 runeCount = static_cast<Uint8>(GetNumRunes() % 3);
   sg.Write8(runeCount);

   for (unsigned int shelfIndex = 0; shelfIndex < runeCount; shelfIndex++)
      sg.Write8(static_cast<Uint8>(GetRune(shelfIndex)));

   sg.EndSection();
}

void Runebag::Load(Base::Savegame& sg)
{
   sg.BeginSection("runebag");

   m_runebag.reset();

   Uint32 bagBits = sg.Read32();
   for (unsigned int runeIndex = 0; runeIndex < 24; runeIndex++)
      if ((bagBits & (1L << runeIndex)) != 0)
         m_runebag.set(runeIndex);

   sg.EndSection();
}

/// \note std::bitset::to_ulong() could be used to convert the bitset to an
/// int, but it probably isn't portable enough
/// \todo use to_ulong() anyway
void Runebag::Save(Base::Savegame& sg) const
{
   sg.BeginSection("runebag");

   Uint32 bagBits = 0;
   for (unsigned int runeIndex = 0; runeIndex < 24; runeIndex++)
      if (m_runebag.test(runeIndex))
         bagBits |= 1L << runeIndex;

   sg.Write32(bagBits);

   sg.EndSection();
}
