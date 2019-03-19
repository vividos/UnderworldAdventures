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
/// \file LevelList.cpp
/// \brief level list
//
#include "pch.hpp"
#include "LevelList.hpp"
#include "Savegame.hpp"

using Underworld::LevelList;
using Underworld::Level;

void LevelList::Load(Base::Savegame& sg)
{
   sg.BeginSection("levels");

   size_t numLevels = sg.Read32();

   m_levelList.clear();
   m_levelList.resize(numLevels);

   for (size_t levelIndex = 0; levelIndex < numLevels; levelIndex++)
      m_levelList[levelIndex].Load(sg);

   sg.EndSection();
}

void LevelList::Save(Base::Savegame& sg) const
{
   sg.BeginSection("levels");

   size_t numLevels = m_levelList.size();
   sg.Write32(static_cast<Uint32>(numLevels));

   for (size_t levelIndex = 0; levelIndex < m_levelList.size(); levelIndex++)
      m_levelList[levelIndex].Save(sg);

   sg.EndSection();
}
