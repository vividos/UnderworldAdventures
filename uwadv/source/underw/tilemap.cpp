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
/*! \file tilemap.cpp

   \brief tilemap

*/

// needed includes
#include "underworld.hpp"
#include "tilemap.hpp"
#include "savegame.hpp"
//#include <cmath> // todo!
#include <math.h>

using Underworld::Tilemap;
using Underworld::TileInfo;

// Tilemap methods

double Tilemap::GetFloorHeight(double xpos, double ypos)
{
   if (xpos < 0.0 || xpos >= 64.0 || ypos < 0.0 || ypos >= 64.0)
      return 0.0;

   double dHeight = 0.0;

   TileInfo& tile = GetTileInfo(static_cast<Uint8>(xpos), static_cast<Uint8>(ypos));

   switch (tile.type)
   {
   case tileSolid:
      dHeight = tile.uiCeiling; // player shouldn't get there, though
      break;

   case tileOpen:
      dHeight = tile.uiFloor;
      break;

      // diagonal tiles
   case tileDiagonal_se:
      if (fmod(xpos, 1.0) - fmod(ypos, 1.0) < 0.0)
         dHeight = tile.uiCeiling;
      else
         dHeight = tile.uiFloor;
      break;
   case tileDiagonal_sw:
      if (fmod(xpos, 1.0) + fmod(ypos, 1.0) > 1.0)
         dHeight = tile.uiCeiling;
      else
         dHeight = tile.uiFloor;
      break;
   case tileDiagonal_nw:
      if (fmod(xpos, 1.0) - fmod(ypos, 1.0) > 0.0)
         dHeight = tile.uiCeiling;
      else
         dHeight = tile.uiFloor;
      break;
   case tileDiagonal_ne:
      if (fmod(xpos, 1.0) + fmod(ypos, 1.0) < 1.0)
         dHeight = tile.uiCeiling;
      else
         dHeight = tile.uiFloor;
      break;

      // sloped tiles
   case tileSlope_n:
      dHeight = tile.uiFloor +
         static_cast<double>(tile.uiSlope) * fmod(ypos, 1.0);
      break;
   case tileSlope_s:
      dHeight = (tile.uiFloor+tile.uiSlope) -
         static_cast<double>(tile.uiSlope)*fmod(ypos,1.0);
      break;
   case tileSlope_e:
      dHeight = tile.uiFloor +
         static_cast<double>(tile.uiSlope)*fmod(xpos,1.0);
      break;
   case tileSlope_w:
      dHeight = (tile.uiFloor+tile.uiSlope) -
         static_cast<double>(tile.uiSlope)*fmod(xpos,1.0);
      break;

   default:
      UaAssert(false);
      break;
   };

   return dHeight;
}

TileInfo& Tilemap::GetTileInfo(unsigned int xpos, unsigned int ypos)
{
   xpos %= 64; ypos %= 64;
   return m_vecTiles[ypos*64 + xpos];
}

const TileInfo& Tilemap::GetTileInfo(unsigned int xpos, unsigned int ypos) const
{
   xpos %= 64; ypos %= 64;
   return m_vecTiles[ypos*64 + xpos];
}

void Tilemap::LoadTilemap(Base::Savegame& sg)
{
   sg.BeginSection("tilemap");

   m_bUsed = sg.Read8() != 0;

   if (!m_bUsed)
      return; // don't read empty tilemaps

   // read tilemap
   Create();

   for(unsigned int n=0; n<64*64; n++)
   {
      TileInfo& tile = m_vecTiles[n];

      tile.type = static_cast<TilemapTileType>(sg.Read8());
      tile.uiFloor = sg.Read16();
      tile.uiCeiling = sg.Read16();
      tile.uiSlope = sg.Read8();
      tile.uiTextureWall = sg.Read16();
      tile.uiTextureFloor = sg.Read16();
      tile.uiTextureCeiling = sg.Read16();

      m_setUsedTextures.insert(tile.uiTextureWall);
      m_setUsedTextures.insert(tile.uiTextureFloor);
      m_setUsedTextures.insert(tile.uiTextureCeiling);
   }

   sg.EndSection();
}

void Tilemap::SaveTilemap(Base::Savegame& sg) const
{
   sg.BeginSection("tilemap");

   sg.Write8(m_bUsed ? 1 : 0);

   if (!m_bUsed)
      return; // don't write empty tilemaps

   // write tilemap
   for(unsigned int n=0; n<64*64; n++)
   {
      const TileInfo& tile = m_vecTiles[n];

      sg.Write8(static_cast<Uint8>(tile.type));
      sg.Write16(tile.uiFloor);
      sg.Write16(tile.uiCeiling);
      sg.Write8(tile.uiSlope);
      sg.Write16(tile.uiTextureWall);
      sg.Write16(tile.uiTextureFloor);
      sg.Write16(tile.uiTextureCeiling);
   }

   sg.EndSection();
}
