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
/// \file Underworld.cpp
/// \brief underworld object
//
#include "pch.hpp"
#include "Underworld.hpp"
#include "Savegame.hpp"

Underworld::Level& Underworld::Underworld::GetCurrentLevel()
{
   unsigned int currentLevel = GetPlayer().GetAttribute(::Underworld::attrMapLevel);
   return GetLevelList().GetLevel(currentLevel);
}

const Underworld::Level& Underworld::Underworld::GetCurrentLevel() const
{
   unsigned int currentLevel = GetPlayer().GetAttribute(::Underworld::attrMapLevel);
   return GetLevelList().GetLevel(currentLevel);
}

void Underworld::Underworld::Load(Base::Savegame& sg)
{
   if (sg.GetVersion() < 3)
   {
      UaTrace("cannot load savegames prior version 3!\n");
      return;
   }

   m_levelList.Load(sg);
   m_player.Load(sg);
}

void Underworld::Underworld::Save(Base::Savegame& sg) const
{
   m_levelList.Save(sg);
   m_player.Save(sg);
}
