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
/*! \file tilemap.hpp

   \brief level tilemap

*/

// include guard
#ifndef uwadv_underw_tilemap_hpp_
#define uwadv_underw_tilemap_hpp_

// needed includes
#include <vector>
#include <set>

namespace Base
{
   class Savegame;
}

namespace Underworld
{

// enums

//! tile types
enum TilemapTileType
{
   tileSolid = 0x00,       //!< solid tile
   tileOpen  = 0x01,       //!< open tile; player can go through tile
   tileDiagonal_se = 0x02, //!< diagonal wall, open corner to the lower right
   tileDiagonal_sw = 0x03, //!< diagonal wall, open corner to the lower left
   tileDiagonal_nw = 0x04, //!< diagonal wall, open corner to the upper left
   tileDiagonal_ne = 0x05, //!< diagonal wall, open corner to the upper right
   tileSlope_n = 0x06,     //!< open tile with sloped floor going up north
   tileSlope_e = 0x07,     //!< open tile with sloped floor going up east
   tileSlope_s = 0x08,     //!< open tile with sloped floor going up south
   tileSlope_w = 0x09      //!< open tile with sloped floor going up west
};

// structs

//! tilemap tile info
struct TileInfo
{
   //! ctor
   TileInfo():type(tileSolid){}

   //! tile type
   TilemapTileType type;

   //! floor height
   Uint16 uiFloor;

   //! ceiling height
   Uint16 uiCeiling;

   //! slope from this tile to next
   Uint8 uiSlope;

   //! texture id for wall
   Uint16 uiTextureWall;

   //! texture id for floor
   Uint16 uiTextureFloor;

   //! texture id for ceiling
   Uint16 uiTextureCeiling;
};


// classes

//! tilemap
class Tilemap
{
public:
   //! ctor
   Tilemap():m_bUsed(false){}

   //! creates new tilemap
   void Create()
   {
      m_vecTiles.clear();
      m_vecTiles.resize(64*64);
      m_setUsedTextures.clear();
      m_bUsed = true;
   }

   //! destroy all tiles in tilemap
   void Destroy()
   {
      m_vecTiles.clear();
      m_setUsedTextures.clear();
      m_bUsed = false;
   }

   //! returns floor height on specific position
   double GetFloorHeight(double xpos, double xpos);

   //! returns a tile info struct
   TileInfo& GetTileInfo(unsigned int xpos, unsigned int ypos);

   //! returns a tile info struct
   const TileInfo& GetTileInfo(unsigned int xpos, unsigned int ypos) const;

   //! returns vector of used stock texture ids
   std::set<Uint16>& GetUsedTextures(){ return m_setUsedTextures; }

   // loading / saving

   //! loads tilemap from savegame
   void Load(Base::Savegame& sg);

   //! saves tilemap to savegame
   void Save(Base::Savegame& sg) const;

   //! returns tiles list
   std::vector<TileInfo>& GetVectorTileInfo(){ return m_vecTiles; }

protected:
   //! all levelmap tiles; 64x64 tiles always assumed
   std::vector<TileInfo> m_vecTiles;

   //! set with all used texture ids
   std::set<Uint16> m_setUsedTextures;

   //! indicates if tilemap is filled with actual tiles
   bool m_bUsed;
};

} // namespace Underworld

#endif
