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
/*! \file levellist.cpp

   \brief level list

*/

// needed includes
#include "underworld.hpp"
#include "levellist.hpp"
#include "savegame.hpp"

using Underworld::LevelList;
using Underworld::Level;

// LevelList methods

void LevelList::Load(Base::Savegame& sg)
{
   sg.BeginSection("levels");

   std::vector<Level>::size_type uiMax = sg.Read32();

   m_vecLevels.clear();
   m_vecLevels.resize(uiMax);

   for (std::vector<Level>::size_type i=0; i<uiMax; i++)
      m_vecLevels[i].Load(sg);

   sg.EndSection();
}

void LevelList::Save(Base::Savegame& sg) const
{
   sg.BeginSection("levels");

   std::vector<Level>::size_type uiMax = m_vecLevels.size();
   sg.Write32(static_cast<Uint32>(uiMax));

   for (std::vector<Level>::size_type i=0; i<m_vecLevels.size(); i++)
      m_vecLevels[i].Save(sg);

   sg.EndSection();
}
