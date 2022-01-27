//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2021 Underworld Adventures Team
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
/// \file AutomapGenerator.hpp
/// \brief auto-map generator
//
#pragma once

#include "Font.hpp"
#include <array>

class IndexedImage;
class ImageManager;

namespace Base
{
   class ResourceManager;
}

namespace Underworld
{
   class Tilemap;
   class MapNotes;
   class Player;
   enum TilemapTileType;
   enum AutomapFlag;
}

namespace UI
{
   /// generator for automap images from tilemap
   class AutomapGenerator
   {
   public:
      /// ctor
      AutomapGenerator(Base::ResourceManager& resourceManager,
         ImageManager& imageManager, const Underworld::Tilemap& tilemap);

      /// draws level number
      void DrawLevelNumber(IndexedImage& image, size_t levelIndex,
         const std::string& levelName) const;

      /// generates the automap image
      void DrawTiles(IndexedImage& image) const;

      /// draws all map notes
      void DrawMapNotes(IndexedImage& image, const Underworld::MapNotes& mapNotes) const;

      /// draws the player's pin
      void DrawPlayerPin(IndexedImage& image, const Underworld::Player& player) const;

   private:
      /// draws single tile
      void DrawTile(IndexedImage& image, unsigned int tileX, unsigned int tileY) const;

      /// fills tile pixels for solid tile
      void FillOpenTilePixels(Underworld::AutomapFlag automapFlag,
         unsigned int tileX, unsigned int tileY,
         std::array<Uint8, 9>& tilePixels) const;

      /// fills tile pixels for diagonal tile types
      void FillDiagonalTilePixels(Underworld::AutomapFlag automapFlag,
         Underworld::TilemapTileType tileType, std::array<Uint8, 9>& tilePixels) const;

      /// returns if tile in a direction is open in in regards to this tile
      /// (which is assumed to be solid)
      bool IsTileOpen(unsigned int tileX, unsigned int tileY,
         int offsetToCheckX, int offsetToCheckY) const;

      /// fills tile pixels for solid tile
      void FillSolidTilePixels(unsigned int tileX, unsigned int tileY,
         std::array<Uint8, 9>& tilePixels) const;

   private:
      /// tilemap to use for generating automap
      const Underworld::Tilemap& m_tilemap;

      /// big font for level number
      Font m_bigFont;

      /// player pin image
      IndexedImage m_playerPinImage;
   };

} // namespace UI
