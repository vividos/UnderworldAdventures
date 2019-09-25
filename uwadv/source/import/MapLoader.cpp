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
/// \file MapLoader.cpp
/// \brief tilemap and texture mapping loading
//
#include "pch.hpp"
#include "LevelImporter.hpp"
#include "Tilemap.hpp"
#include "Constants.hpp"

using Import::LevelImporter;
using Import::TileStartLinkList;
using Import::GetBits;

// tables

/// maps type numbers read from lev.ark to level map tile type enum
Underworld::TilemapTileType g_tileTypeMapping[16] =
{
   Underworld::tileSolid,
   Underworld::tileOpen,
   Underworld::tileDiagonal_se,
   Underworld::tileDiagonal_sw,
   Underworld::tileDiagonal_ne,
   Underworld::tileDiagonal_nw,
   Underworld::tileSlope_n,
   Underworld::tileSlope_s,
   Underworld::tileSlope_e,
   Underworld::tileSlope_w,
   Underworld::tileSolid,
   Underworld::tileSolid,
   Underworld::tileSolid,
   Underworld::tileSolid,
   Underworld::tileSolid,
   Underworld::tileSolid
};

void LevelImporter::LoadTilemap(Underworld::Tilemap& tilemap, std::vector<Uint16>& textureMapping,
   TileStartLinkList& tileStartLinkList, bool uw2Mode)
{
   tilemap.Destroy();
   tilemap.Create();

   // read in map info
   for (Uint16 ypos = 0; ypos < 64; ypos++)
      for (Uint16 xpos = 0; xpos < 64; xpos++)
      {
         Underworld::TileInfo& tileInfo = tilemap.GetTileInfo(xpos, ypos);

         Uint32 uiTileInfo1 = m_file.Read16();
         Uint32 uiTileInfo2 = m_file.Read16();

         // extract infos from tile word
         tileInfo.m_type = g_tileTypeMapping[GetBits(uiTileInfo1, 0, 4)];

         // all size values in height units
         tileInfo.m_floor = static_cast<Uint16>(GetBits(uiTileInfo1, 4, 4) << 3);
         tileInfo.m_ceiling = 128;
         tileInfo.m_slope = 8; // height units per tile

         // texture indices
         Uint8 floorIndex = static_cast<Uint8>(GetBits(uiTileInfo1, 10, 4)); // 4 bit wide
         Uint8 wallIndex = static_cast<Uint8>(GetBits(uiTileInfo2, 0, 6)); // 6 bit wide

         if (!uw2Mode)
         {
            // restrict to proper texture map indices
            if (floorIndex >= 10)
               floorIndex = 0;
            if (wallIndex >= 48)
               wallIndex = 0;
         }

         tileInfo.m_textureWall = textureMapping[wallIndex];
         tileInfo.m_textureFloor = textureMapping[floorIndex + (uw2Mode ? 0 : 48)];
         tileInfo.m_textureCeiling = textureMapping[uw2Mode ? 32 : (9 + 48)];

         // tile object list start
         Uint16 uiLink = static_cast<Uint16>(GetBits(uiTileInfo2, 6, 10));
         tileStartLinkList.SetLinkStart(xpos, ypos, uiLink);

         // special flags
         tileInfo.m_isMagicDisabled = GetBits(uiTileInfo1, 14, 1) != 0;
         tileInfo.m_isDoorPresent = GetBits(uiTileInfo1, 15, 1) != 0;
         tileInfo.m_isSpecialLightFeature = GetBits(uiTileInfo1, 8, 1) != 0;

         // bit 9 is always 0
         UaAssert(0 == GetBits(uiTileInfo1, 9, 1));
      }
}

/// The texture mapping is stored in the textureMapping as indices into
/// stock textures. In uw1 mode there are 48 wall textures, followed by 10
/// floor textures, followed by 6 empty entries, ending with 6 door textures.
/// In uw2 mode there are 64 textures used for wall and floor (floor textures
/// only use a 4 bit mapping and so only use the first 16 entries), followed
/// by 6 door textures. textureMapping always contains 70 entries after
/// loading.
void LevelImporter::LoadTextureMapping(std::vector<Uint16>& textureMapping, bool uw2Mode)
{
   if (!uw2Mode)
   {
      // uw1 mapping
      textureMapping.resize(48 + 10);

      // wall textures
      for (unsigned int textureIndex = 0; textureIndex < 48; textureIndex++)
         textureMapping[textureIndex] = m_file.Read16() + Base::c_stockTexturesWall;

      // floor textures
      for (unsigned int textureIndex = 48; textureIndex < 48 + 10; textureIndex++)
         textureMapping[textureIndex] = m_file.Read16() + Base::c_stockTexturesFloor;
   }
   else
   {
      // uw2 mapping
      textureMapping.resize(64);

      // combined wall/floor textures
      for (unsigned int textureIndex = 0; textureIndex < 64; textureIndex++)
         textureMapping[textureIndex] = m_file.Read16();
   }

   // door textures
   for (unsigned int textureIndex = 0; textureIndex < 6; textureIndex++)
      textureMapping.push_back(m_file.Read8() + Base::c_stockTexturesDoors);
}
