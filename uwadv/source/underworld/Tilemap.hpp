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
/// \file Tilemap.hpp
/// \brief level tilemap
//
#pragma once

#include "Base.hpp"
#include "Constants.hpp"
#include <vector>
#include <set>

namespace Base
{
   class Savegame;
}

namespace Underworld
{
   /// default tilemap size for underworld games
   const unsigned int c_underworldTilemapSize = 64;

   /// tile types
   enum TilemapTileType
   {
      tileSolid = 0x00,       ///< solid tile
      tileOpen = 0x01,       ///< open tile; player can go through tile
      tileDiagonal_se = 0x02, ///< diagonal wall, open corner to the lower right
      tileDiagonal_sw = 0x03, ///< diagonal wall, open corner to the lower left
      tileDiagonal_nw = 0x04, ///< diagonal wall, open corner to the upper left
      tileDiagonal_ne = 0x05, ///< diagonal wall, open corner to the upper right
      tileSlope_n = 0x06,     ///< open tile with sloped floor going up north
      tileSlope_e = 0x07,     ///< open tile with sloped floor going up east
      tileSlope_s = 0x08,     ///< open tile with sloped floor going up south
      tileSlope_w = 0x09      ///< open tile with sloped floor going up west
   };

   /// tilemap tile info
   struct TileInfo
   {
      /// ctor
      TileInfo()
         :m_type(tileSolid),
         m_floor(0),
         m_ceiling(128),
         m_slope(8),
         m_textureWall(Base::c_stockTexturesWall),
         m_textureFloor(Base::c_stockTexturesFloor),
         m_textureCeiling(Base::c_stockTexturesFloor),
         m_isMagicDisabled(false),
         m_isDoorPresent(false),
         m_isSpecialLightFeature(false)
      {
      }

      /// tile type
      TilemapTileType m_type;

      /// floor height
      Uint16 m_floor;

      /// ceiling height
      Uint16 m_ceiling;

      /// slope from this tile to next
      Uint8 m_slope;

      /// texture id for wall
      Uint16 m_textureWall;

      /// texture id for floor
      Uint16 m_textureFloor;

      /// texture id for ceiling
      Uint16 m_textureCeiling;

      /// indicates if magic is disabled in this tile
      bool m_isMagicDisabled;

      /// indicates if a door is present in this tile
      bool m_isDoorPresent;

      /// special light feature \todo rename to a better name
      bool m_isSpecialLightFeature;
   };

   /// tilemap
   class Tilemap
   {
   public:
      /// ctor
      Tilemap()
         :m_isUsed(false)
      {
      }

      /// creates new tilemap
      void Create()
      {
         m_tilesList.clear();
         m_tilesList.resize(c_underworldTilemapSize * c_underworldTilemapSize);
         m_setUsedTextures.clear();
         m_isUsed = true;
      }

      /// destroy all tiles in tilemap
      void Destroy()
      {
         m_tilesList.clear();
         m_setUsedTextures.clear();
         m_isUsed = false;
      }

      /// returns if the tilemap is used (it contains tiles)
      bool IsUsed() const { return m_isUsed; }

      /// returns floor height on specific position
      double GetFloorHeight(double xpos, double ypos) const;

      /// returns a tile info struct
      TileInfo& GetTileInfo(unsigned int xpos, unsigned int ypos);

      /// returns a tile info struct
      const TileInfo& GetTileInfo(unsigned int xpos, unsigned int ypos) const;

      /// returns set of used stock texture ids
      const std::set<Uint16>& GetUsedTextures() const { return m_setUsedTextures; }

      /// returns set of used stock texture ids; non-const version
      std::set<Uint16>& GetUsedTextures() { return m_setUsedTextures; }

      /// returns tiles list
      std::vector<TileInfo>& GetVectorTileInfo() { return m_tilesList; }

      // loading / saving

      /// loads tilemap from savegame
      void Load(Base::Savegame& sg);

      /// saves tilemap to savegame
      void Save(Base::Savegame& sg) const;

   private:
      /// all levelmap tiles; 64x64 tiles always assumed
      std::vector<TileInfo> m_tilesList;

      /// set with all used texture ids
      std::set<Uint16> m_setUsedTextures;

      /// indicates if tilemap is filled with actual tiles
      bool m_isUsed;
   };

} // namespace Underworld
