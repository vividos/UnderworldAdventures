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
/// \file LevelImporter.cpp
/// \brief Level import
//
#include "pch.hpp"
#include "LevelImporter.hpp"
#include "LevelList.hpp"
#include "Level.hpp"
#include "ResourceManager.hpp"
#include "ArchiveFile.hpp"
#include "ObjectListLoader.hpp"

using Import::LevelImporter;

void LevelImporter::LoadUwDemoLevel(Underworld::LevelList& levelList)
{
   UaTrace("importing uw_demo level map\n");

   std::vector<Underworld::Level>& allLevels = levelList.GetVectorLevels();

   allLevels.clear();
   allLevels.resize(1);

   // load uw_demo texture map
   m_file = Base::File(m_resourceManager.GetUnderworldFile(Base::resourceGameUw, "data/level13.txm"));

   std::vector<Uint16> textureMapping;
   LoadTextureMapping(textureMapping, false);

   // load tilemap
   m_file = Base::File(m_resourceManager.GetUnderworldFile(Base::resourceGameUw, "data/level13.st"));

   TileStartLinkList tileStartLinkList;
   LoadTilemap(allLevels[0].GetTilemap(), textureMapping, tileStartLinkList, false);

   // load object list
   LoadObjectList(allLevels[0].GetObjectList(), tileStartLinkList, textureMapping);
}

void LevelImporter::LoadUw1Levels(Underworld::LevelList& levelList)
{
   LoadUwLevels(levelList, false, 9, 18);
}

void LevelImporter::LoadUw2Levels(Underworld::LevelList& levelList)
{
   LoadUwLevels(levelList, true, 80, 80);
}

void LevelImporter::LoadUwLevels(Underworld::LevelList& levelList, bool uw2Mode, unsigned int numLevels, unsigned int textureMapOffset)
{
   std::vector<Underworld::Level>& allLevels = levelList.GetVectorLevels();

   allLevels.clear();
   allLevels.resize(numLevels);

   Base::ArchiveFile levArkFile(m_resourceManager.GetUnderworldFile(Base::resourceGameUw, "data/lev.ark"), uw2Mode);

   for (unsigned int levelIndex = 0; levelIndex < numLevels; levelIndex++)
   {
      if (!levArkFile.IsAvailable(levelIndex))
         continue;

      Underworld::Level& level = allLevels[levelIndex];

      // load texture mapping
      UaAssert(true == levArkFile.IsAvailable(levelIndex + textureMapOffset));
      m_file = levArkFile.GetFile(levelIndex + textureMapOffset);

      std::vector<Uint16> textureMapping;
      LoadTextureMapping(textureMapping, false);

      // load tilemap
      m_file = levArkFile.GetFile(levelIndex);

      TileStartLinkList tileStartLinkList;
      LoadTilemap(level.GetTilemap(), textureMapping, tileStartLinkList, uw2Mode);

      // load object list
      LoadObjectList(level.GetObjectList(), tileStartLinkList, textureMapping);
   }
}

void Import::LevelImporter::LoadObjectList(Underworld::ObjectList& objectList,
   const TileStartLinkList& tileStartLinkList, std::vector<Uint16>& textureMapping)
{
   objectList.Destroy();
   objectList.Create();

   // read in master object list
   std::vector<Uint16> objectWordsList;
   objectWordsList.reserve(0x0400 * 4);

   std::vector<Uint8> npcInfosList;
   npcInfosList.resize(0x0100 * 19);

   for (Uint16 itemIndex = 0; itemIndex < 0x400; itemIndex++)
   {
      objectWordsList.push_back(m_file.Read16());
      objectWordsList.push_back(m_file.Read16());
      objectWordsList.push_back(m_file.Read16());
      objectWordsList.push_back(m_file.Read16());

      // read NPC info bytes
      if (itemIndex < 0x100)
         m_file.ReadBuffer(&npcInfosList[itemIndex * 19], 19);
   }

   ObjectListLoader loader(objectList, objectWordsList, npcInfosList, textureMapping);

   // follow each reference in all tiles
   for (Uint8 ypos = 0; ypos < 64; ypos++)
      for (Uint8 xpos = 0; xpos < 64; xpos++)
      {
         Uint16 link = tileStartLinkList.GetLinkStart(xpos, ypos);

         if (link != 0)
         {
            loader.FollowLink(link, xpos, ypos);

            objectList.SetListStart(link, xpos, ypos);
         }
      }
}
