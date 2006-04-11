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
/*! \file levelloader.cpp

   \brief level loader

*/

// needed includes
#include "import.hpp"
#include "levellist.hpp"
#include "level.hpp"
#include "resourcemanager.hpp"
#include "archivefile.hpp"

using Import::LevelImporter;

// LevelImporter methods

void LevelImporter::LoadUwDemoLevel(Underworld::LevelList& levelList)
{
   UaTrace("importing uw_demo level map\n");

   std::vector<Underworld::Level>& vecLevels = levelList.GetVectorLevels();

   vecLevels.clear();
   vecLevels.resize(1);

   // load uw_demo texture map
   m_file = Base::File(m_resourceManager.GetUnderworldFile(Base::resourceGameUw, "data/level13.txm"));

   std::vector<Uint16> vecTextureMapping;
   LoadTextureMapping(vecTextureMapping, false);

   // load tilemap
   m_file = Base::File(m_resourceManager.GetUnderworldFile(Base::resourceGameUw, "data/level13.st"));

   TileStartLinkList tileStartLinkList;
   LoadTilemap(vecLevels[0].GetTilemap(), vecTextureMapping, tileStartLinkList, false);

   // load object list
   LoadObjectList(vecLevels[0].GetObjectList(), tileStartLinkList, vecTextureMapping);
}

void LevelImporter::LoadUw1Levels(Underworld::LevelList& levelList)
{
   LoadUwLevels(levelList, false, 9, 18);
}

void LevelImporter::LoadUw2Levels(Underworld::LevelList& levelList)
{
   LoadUwLevels(levelList, true, 80, 80);
}

void LevelImporter::LoadUwLevels(Underworld::LevelList& levelList, bool bUw2Mode, unsigned int uiNumLevels, unsigned int uiTexMapOffset)
{
   std::vector<Underworld::Level>& vecLevels = levelList.GetVectorLevels();

   vecLevels.clear();
   vecLevels.resize(uiNumLevels);

   Base::ArchiveFile levArkFile(m_resourceManager.GetUnderworldFile(Base::resourceGameUw, "data/lev.ark"), bUw2Mode);

   for (unsigned int ui=0; ui<uiNumLevels; ui++)
   {
      if (!levArkFile.IsAvailable(ui))
         continue;

      Underworld::Level& level = vecLevels[ui];

      // load texture mapping
      UaAssert(true == levArkFile.IsAvailable(ui+uiTexMapOffset));
      m_file = levArkFile.GetFile(ui+uiTexMapOffset);

      std::vector<Uint16> vecTextureMapping;
      LoadTextureMapping(vecTextureMapping, false);

      // load tilemap
      m_file = levArkFile.GetFile(ui);

      TileStartLinkList tileStartLinkList;
      LoadTilemap(level.GetTilemap(), vecTextureMapping, tileStartLinkList, bUw2Mode);

      // load object list
      LoadObjectList(level.GetObjectList(), tileStartLinkList, vecTextureMapping);
   }
}
