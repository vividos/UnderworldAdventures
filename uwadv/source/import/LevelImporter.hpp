//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2003,2004,2005,2006,2019 Underworld Adventures Team
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
/// \file LevelImporter.hpp
/// \brief Level importer
//
#pragma once

#include "Base.hpp"
#include "File.hpp"
#include <vector>

namespace Base
{
   class ResourceManager;
}

namespace Underworld
{
   class LevelList;
   class ObjectList;
   class Tilemap;
}

namespace Import
{
   /// stores tilemap object list start links; used while loading
   class TileStartLinkList
   {
   public:
      /// ctor
      TileStartLinkList() { m_linkList.resize(64 * 64); }

      /// returns link start for given tile
      Uint16 GetLinkStart(Uint16 xpos, Uint16 ypos) const { return m_linkList[ypos * 64 + xpos]; }

      /// sets link start for given tile
      void SetLinkStart(Uint16 xpos, Uint16 ypos, Uint16 link) { m_linkList[ypos * 64 + xpos] = link; }

   private:
      /// array with links
      std::vector<Uint16> m_linkList;
   };


   /// imports levels
   class LevelImporter
   {
   public:
      /// ctor
      LevelImporter(Base::ResourceManager& resourceManager)
         :m_resourceManager(resourceManager)
      {
      }

      /// loads uw_demo level into level list
      void LoadUwDemoLevel(Underworld::LevelList& levelList);

      /// loads uw1 levels into level list
      void LoadUw1Levels(Underworld::LevelList& levelList);

      /// loads uw2 levels into level list
      void LoadUw2Levels(Underworld::LevelList& levelList);

   private:
      /// common uw1 and uw2 level loading
      void LoadUwLevels(Underworld::LevelList& levelList, bool uw2Mode,
         unsigned int numLevels, unsigned int textureMapOffset);

      /// loads texture mapping from current file
      void LoadTextureMapping(std::vector<Uint16>& textureMapping, bool uw2Mode);

      /// loads tilemap from current file
      void LoadTilemap(Underworld::Tilemap& tilemap, std::vector<Uint16>& textureMapping,
         TileStartLinkList& tileStartLinkList, bool uw2Mode);

      /// loads object list from current file
      void LoadObjectList(Underworld::ObjectList& objectList, const TileStartLinkList& tileStartLinkList,
         std::vector<Uint16>& textureMapping);

   private:
      /// resource manager
      Base::ResourceManager& m_resourceManager;

      /// current file
      Base::File m_file;
   };

} // namespace Import
