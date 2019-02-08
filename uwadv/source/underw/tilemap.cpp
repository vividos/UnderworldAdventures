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
/// \file tilemap.cpp
/// \brief tilemap
//
#include "underw.hpp"
#include "tilemap.hpp"
#include "savegame.hpp"
#include <cmath>

using Underworld::Tilemap;
using Underworld::TileInfo;

double Tilemap::GetFloorHeight(double xpos, double ypos)
{
   if (xpos < 0.0 ||
      xpos >= double(c_underworldTilemapSize) ||
      ypos < 0.0 ||
      ypos >= double(c_underworldTilemapSize))
      return 0.0;

   double height = 0.0;

   TileInfo& tile = GetTileInfo(static_cast<Uint8>(xpos), static_cast<Uint8>(ypos));

   switch (tile.m_type)
   {
   case tileSolid:
      height = tile.m_ceiling; // player shouldn't get there, though
      break;

   case tileOpen:
      height = tile.m_floor;
      break;

      // diagonal tiles
   case tileDiagonal_se:
      if (fmod(xpos, 1.0) - fmod(ypos, 1.0) < 0.0)
         height = tile.m_ceiling;
      else
         height = tile.m_floor;
      break;
   case tileDiagonal_sw:
      if (fmod(xpos, 1.0) + fmod(ypos, 1.0) > 1.0)
         height = tile.m_ceiling;
      else
         height = tile.m_floor;
      break;
   case tileDiagonal_nw:
      if (fmod(xpos, 1.0) - fmod(ypos, 1.0) > 0.0)
         height = tile.m_ceiling;
      else
         height = tile.m_floor;
      break;
   case tileDiagonal_ne:
      if (fmod(xpos, 1.0) + fmod(ypos, 1.0) < 1.0)
         height = tile.m_ceiling;
      else
         height = tile.m_floor;
      break;

      // sloped tiles
   case tileSlope_n:
      height = tile.m_floor +
         static_cast<double>(tile.m_slope) * fmod(ypos, 1.0);
      break;
   case tileSlope_s:
      height = (tile.m_floor + tile.m_slope) -
         static_cast<double>(tile.m_slope)*fmod(ypos, 1.0);
      break;
   case tileSlope_e:
      height = tile.m_floor +
         static_cast<double>(tile.m_slope)*fmod(xpos, 1.0);
      break;
   case tileSlope_w:
      height = (tile.m_floor + tile.m_slope) -
         static_cast<double>(tile.m_slope)*fmod(xpos, 1.0);
      break;

   default:
      UaAssert(false);
      break;
   };

   return height;
}

TileInfo& Tilemap::GetTileInfo(unsigned int xpos, unsigned int ypos)
{
   xpos %= c_underworldTilemapSize; ypos %= c_underworldTilemapSize;
   return m_tilesList[ypos * c_underworldTilemapSize + xpos];
}

const TileInfo& Tilemap::GetTileInfo(unsigned int xpos, unsigned int ypos) const
{
   xpos %= c_underworldTilemapSize; ypos %= c_underworldTilemapSize;
   return m_tilesList[ypos * c_underworldTilemapSize + xpos];
}

void Tilemap::Load(Base::Savegame& sg)
{
   sg.BeginSection("tilemap");

   m_isUsed = sg.Read8() != 0;

   if (!m_isUsed)
      return; // don't read empty tilemaps

   Create();

   for (unsigned int tileIndex = 0; tileIndex < c_underworldTilemapSize *c_underworldTilemapSize; tileIndex++)
   {
      TileInfo& tile = m_tilesList[tileIndex];

      tile.m_type = static_cast<TilemapTileType>(sg.Read8());
      tile.m_floor = sg.Read16();
      tile.m_ceiling = sg.Read16();
      tile.m_slope = sg.Read8();
      tile.m_textureWall = sg.Read16();
      tile.m_textureFloor = sg.Read16();
      tile.m_textureCeiling = sg.Read16();

      tile.m_isMagicDisabled = sg.Read8() != 0;
      tile.m_isDoorPresent = sg.Read8() != 0;
      tile.m_isSpecialLightFeature = sg.Read8() != 0;

      m_setUsedTextures.insert(tile.m_textureWall);
      m_setUsedTextures.insert(tile.m_textureFloor);
      m_setUsedTextures.insert(tile.m_textureCeiling);
   }

   sg.EndSection();
}

void Tilemap::Save(Base::Savegame& sg) const
{
   sg.BeginSection("tilemap");

   sg.Write8(m_isUsed ? 1 : 0);

   if (!m_isUsed)
      return; // don't write empty tilemaps

   for (unsigned int tileIndex = 0; tileIndex < c_underworldTilemapSize *c_underworldTilemapSize; tileIndex++)
   {
      const TileInfo& tile = m_tilesList[tileIndex];

      sg.Write8(static_cast<Uint8>(tile.m_type));
      sg.Write16(tile.m_floor);
      sg.Write16(tile.m_ceiling);
      sg.Write8(tile.m_slope);
      sg.Write16(tile.m_textureWall);
      sg.Write16(tile.m_textureFloor);
      sg.Write16(tile.m_textureCeiling);

      sg.Write8(tile.m_isMagicDisabled ? 1 : 0);
      sg.Write8(tile.m_isDoorPresent ? 1 : 0);
      sg.Write8(tile.m_isSpecialLightFeature ? 1 : 0);
   }

   sg.EndSection();
}
