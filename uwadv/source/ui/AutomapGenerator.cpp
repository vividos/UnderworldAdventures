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
/// \file AutomapGenerator.cpp
/// \brief auto-map generator
//
#include "pch.hpp"
#include "AutomapGenerator.hpp"
#include "ResourceManager.hpp"
#include "ImageManager.hpp"
#include "IndexedImage.hpp"
#include "Tilemap.hpp"
#include "Player.hpp"
#include <random>

using UI::AutomapGenerator;

/// size of one tile
const unsigned int c_tileSize = 3;

/// pixel X offset for map
const unsigned int c_mapOffsetX = 4;

/// pixel Y offset for map
const unsigned int c_mapOffsetY = 2;

/// palette 0 indices for wall pixels
const std::vector<Uint8> c_wallPaletteIndices = { 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48 };

/// palette 0 indices for floor pixels
const std::vector<Uint8> c_floorPaletteIndices = { 0x3c, 0x3e, 0x3f, 0x40 };

/// palette 0 indices for door pixels
const std::vector<Uint8> c_doorPaletteIndices = { 0x44, 0x45, 0x43, 0x46, 0x47, 0x48 };

/// palette 0 indices for teleport pixels
const std::vector<Uint8> c_teleportPaletteIndices = { 0x43, 0x44, 0x45, 0x46, 0x47 };

/// palette 0 indices for water pixels
const std::vector<Uint8> c_waterPaletteIndices = { 0xb1, 0xb2 };

/// palette 0 indices for bridge pixels
const std::vector<Uint8> c_bridgePaletteIndices = { 0xe9, 0xea, 0xeb };

/// palette 0 indices for lava pixels
const std::vector<Uint8> c_lavaPaletteIndices = { 0xb5, 0xb6 };

/// returns a random palette index from the list of indices
Uint8 GetRandomPaletteIndex(const std::vector<Uint8>& paletteIndices)
{
   static std::mt19937 rng{ std::random_device()() };

   std::uniform_int_distribution<> dist{ 0, static_cast<int>(paletteIndices.size() - 1) };
   size_t index = dist(rng);

   return paletteIndices[index];
}


AutomapGenerator::AutomapGenerator(Base::ResourceManager& resourceManager,
   ImageManager& imageManager, const Underworld::Tilemap& tilemap)
   :m_tilemap(tilemap)
{
   m_bigFont.Load(resourceManager, fontBig);
   imageManager.Load(m_playerPinImage, "buttons", 63, 1);
}

void AutomapGenerator::DrawLevelNumber(IndexedImage& image, size_t levelIndex,
   const std::string& levelName) const
{
   std::string numberText = std::to_string(levelIndex + 1);

   IndexedImage numberImage;
   m_bigFont.CreateString(numberImage, numberText, 0x2d);

   image.PasteImage(numberImage, 286, 5, true);
}

void AutomapGenerator::DrawTiles(IndexedImage& image) const
{
   for (size_t tileX = 0; tileX < 64; tileX++)
   {
      for (size_t tileY = 0; tileY < 64; tileY++)
      {
         Underworld::AutomapFlag automapFlag = m_tilemap.GetTileInfo(tileX, tileY).m_automapFlag;
         if (automapFlag == Underworld::AutomapFlag::automapUndiscovered)
            continue;

         DrawTile(image, tileX, tileY);
      }
   }
}

void AutomapGenerator::DrawPlayerPin(IndexedImage& image, const Underworld::Player& player) const
{
   unsigned int playerX = static_cast<unsigned int>(player.GetXPos());
   unsigned int playerY = static_cast<unsigned int>(player.GetYPos());

   image.PasteImage(
      m_playerPinImage,
      playerX * c_tileSize + c_mapOffsetX + 1,
      (63 - playerY) * c_tileSize + c_mapOffsetY + 3 - m_playerPinImage.GetYRes(),
      true);
}

void AutomapGenerator::DrawTile(IndexedImage& image, size_t tileX, size_t tileY) const
{
   unsigned int x = tileX * c_tileSize + c_mapOffsetX + 1;
   unsigned int y = (63 - tileY) * c_tileSize + c_mapOffsetY + 1;

   std::array<Uint8, 9> tilePixels =
   {
      0, 0, 0,
      0, 0, 0,
      0, 0, 0,
   };

   auto tileType = m_tilemap.GetTileInfo(tileX, tileY).m_type;
   auto automapType = m_tilemap.GetTileInfo(tileX, tileY).m_automapFlag;

   switch (tileType)
   {
   case Underworld::tileOpen:
   case Underworld::tileSlope_n:
   case Underworld::tileSlope_e:
   case Underworld::tileSlope_s:
   case Underworld::tileSlope_w:
      FillOpenTilePixels(automapType, tilePixels);
      break;

   case Underworld::tileDiagonal_se:
   case Underworld::tileDiagonal_sw:
   case Underworld::tileDiagonal_nw:
   case Underworld::tileDiagonal_ne:
      FillDiagonalTilePixels(automapType, tileType, tilePixels);
      break;

   case Underworld::tileSolid:
      FillSolidTilePixels(tileX, tileY, tilePixels);
      break;
   }

   IndexedImage tileImage;
   tileImage.Create(3, 3);
   memcpy(tileImage.GetPixels().data(), tilePixels.data(), 9);

   image.PasteImage(tileImage, x, y, true);
}

std::reference_wrapper<const std::vector<Uint8>> GetPaletteIndicesByAutomapFlag(
   Underworld::AutomapFlag automapFlag)
{
   switch (automapFlag)
   {
   case Underworld::automapDoor:
      return c_doorPaletteIndices;

   case Underworld::automapTeleport:
      return c_teleportPaletteIndices;

   case Underworld::automapWater:
      return c_waterPaletteIndices;

   case Underworld::automapBridge:
      return c_bridgePaletteIndices;

   case Underworld::automapLava:
      return c_lavaPaletteIndices;

   default:
      return c_floorPaletteIndices;
   }
}

void AutomapGenerator::FillOpenTilePixels(Underworld::AutomapFlag automapFlag,
   std::array<Uint8, 9>& tilePixels)
{
   std::reference_wrapper<const std::vector<Uint8>> paletteIndices =
      GetPaletteIndicesByAutomapFlag(automapFlag);

   for (Uint8& pixel : tilePixels)
      pixel = GetRandomPaletteIndex(paletteIndices);
}

void AutomapGenerator::FillDiagonalTilePixels(
   Underworld::AutomapFlag automapFlag, Underworld::TilemapTileType tileType,
   std::array<Uint8, 9>& tilePixels) const
{
   std::reference_wrapper<const std::vector<Uint8>> paletteIndices =
      GetPaletteIndicesByAutomapFlag(automapFlag);

   switch (tileType)
   {
   case Underworld::tileDiagonal_se: // diagonal wall, open corner to the lower right
      tilePixels[2] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[4] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[5] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[6] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[7] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[8] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[1] = GetRandomPaletteIndex(c_wallPaletteIndices);
      tilePixels[3] = GetRandomPaletteIndex(c_wallPaletteIndices);
      break;

   case Underworld::tileDiagonal_sw: // diagonal wall, open corner to the lower left
      tilePixels[0] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[3] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[4] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[6] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[7] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[8] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[1] = GetRandomPaletteIndex(c_wallPaletteIndices);
      tilePixels[5] = GetRandomPaletteIndex(c_wallPaletteIndices);
      break;

   case Underworld::tileDiagonal_nw: // diagonal wall, open corner to the upper left
      tilePixels[0] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[1] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[2] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[3] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[4] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[6] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[5] = GetRandomPaletteIndex(c_wallPaletteIndices);
      tilePixels[7] = GetRandomPaletteIndex(c_wallPaletteIndices);
      break;

   case Underworld::tileDiagonal_ne: // diagonal wall, open corner to the upper right
      tilePixels[0] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[1] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[2] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[4] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[5] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[8] = GetRandomPaletteIndex(paletteIndices);
      tilePixels[3] = GetRandomPaletteIndex(c_wallPaletteIndices);
      tilePixels[7] = GetRandomPaletteIndex(c_wallPaletteIndices);
      break;

   default:
      UaAssertMsg(false, "unhandled tile type");
      break;
   }
}

bool AutomapGenerator::IsTileOpen(unsigned int tileX, unsigned int tileY,
   int offsetToCheckX, int offsetToCheckY) const
{
   if ((tileX == 0 && offsetToCheckX < 0) ||
      (tileY == 0 && offsetToCheckY < 0) ||
      (tileX == 63 && offsetToCheckX > 0) ||
      (tileY == 63 && offsetToCheckY > 0))
      return false; // edge cases are always solid

   // determine adjacent tile, based on the offsets
   bool toLeft = offsetToCheckX < 0 && offsetToCheckY == 0;
   bool toRight = offsetToCheckX > 0 && offsetToCheckY == 0;
   bool toTop = offsetToCheckX == 0 && offsetToCheckY > 0;
   bool toBottom = offsetToCheckX == 0 && offsetToCheckY < 0;

   Underworld::TilemapTileType adjacentTileType =
      m_tilemap.GetTileInfo(tileX + offsetToCheckX, tileY + offsetToCheckY).m_type;

   switch (adjacentTileType)
   {
   case Underworld::tileSolid:
      return false;

   case Underworld::tileOpen:
   case Underworld::tileSlope_n:
   case Underworld::tileSlope_e:
   case Underworld::tileSlope_s:
   case Underworld::tileSlope_w:
      return true;

      // for adjacent diagonal tiles, it matters where the solid tile is
   case Underworld::tileDiagonal_se: // diagonal wall, open corner to the lower right
      return (toTop || toLeft) && !(toBottom || toRight);
   case Underworld::tileDiagonal_sw: // diagonal wall, open corner to the lower left
      return (toTop || toRight) && !(toBottom || toLeft);
   case Underworld::tileDiagonal_nw: // diagonal wall, open corner to the upper left
      return (toBottom || toRight) && !(toTop || toLeft);
   case Underworld::tileDiagonal_ne: // diagonal wall, open corner to the upper right
      return (toBottom || toLeft) && !(toTop || toRight);

   default:
      UaAssertMsg(false, "invalid tile type");
      return false;
   }
}

void AutomapGenerator::FillSolidTilePixels(
   unsigned int tileX, unsigned int tileY,
   std::array<Uint8, 9>& tilePixels) const
{
   // check if there's an adjacent non-solid tile and set the wall pixel
   bool leftTileIsOpen = IsTileOpen(tileX, tileY, -1, 0);
   bool rightTileIsOpen = IsTileOpen(tileX, tileY, 1, 0);
   bool topTileIsOpen = IsTileOpen(tileX, tileY, 0, 1);
   bool bottomTileIsOpen = IsTileOpen(tileX, tileY, 0, -1);

   if (leftTileIsOpen || topTileIsOpen) tilePixels[0] = GetRandomPaletteIndex(c_wallPaletteIndices);
   if (topTileIsOpen) tilePixels[1] = GetRandomPaletteIndex(c_wallPaletteIndices);
   if (rightTileIsOpen || topTileIsOpen) tilePixels[2] = GetRandomPaletteIndex(c_wallPaletteIndices);

   if (leftTileIsOpen) tilePixels[3] = GetRandomPaletteIndex(c_wallPaletteIndices);
   if (rightTileIsOpen) tilePixels[5] = GetRandomPaletteIndex(c_wallPaletteIndices);

   if (leftTileIsOpen || bottomTileIsOpen) tilePixels[6] = GetRandomPaletteIndex(c_wallPaletteIndices);
   if (bottomTileIsOpen) tilePixels[7] = GetRandomPaletteIndex(c_wallPaletteIndices);
   if (rightTileIsOpen || bottomTileIsOpen) tilePixels[8] = GetRandomPaletteIndex(c_wallPaletteIndices);

   // also set some corner wall pixels when the adjacent tile is a diagonal tile
   std::array<std::tuple<int, int>, 4> pairs =
   {
      std::tuple<int, int>(-1, 0),
      std::tuple<int, int>(1, 0),
      std::tuple<int, int>(0, 1),
      std::tuple<int, int>(0, -1),
   };

   for (auto& pair : pairs)
   {
      int offsetToCheckX = std::get<0>(pair);
      int offsetToCheckY = std::get<1>(pair);

      // determine adjacent tile, based on the offsets
      bool toLeft = offsetToCheckX < 0 && offsetToCheckY == 0;
      bool toRight = offsetToCheckX > 0 && offsetToCheckY == 0;
      bool toTop = offsetToCheckX == 0 && offsetToCheckY > 0;
      bool toBottom = offsetToCheckX == 0 && offsetToCheckY < 0;

      Underworld::TilemapTileType adjacentTileType =
         m_tilemap.GetTileInfo(tileX + offsetToCheckX, tileY + offsetToCheckY).m_type;

      if (adjacentTileType == Underworld::tileDiagonal_nw && (toLeft || toTop))
         tilePixels[0] = GetRandomPaletteIndex(c_wallPaletteIndices);

      if (adjacentTileType == Underworld::tileDiagonal_ne && (toRight || toTop))
         tilePixels[2] = GetRandomPaletteIndex(c_wallPaletteIndices);

      if (adjacentTileType == Underworld::tileDiagonal_sw && (toLeft || toBottom))
         tilePixels[6] = GetRandomPaletteIndex(c_wallPaletteIndices);

      if (adjacentTileType == Underworld::tileDiagonal_se && (toRight || toBottom))
         tilePixels[8] = GetRandomPaletteIndex(c_wallPaletteIndices);
   }
}
