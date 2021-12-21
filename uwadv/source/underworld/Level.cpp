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
/// \file Level.cpp
/// \brief level
//
#include "pch.hpp"
#include "Level.hpp"

Underworld::AutomapFlag Underworld::Level::GetAutomapFlagFromTile(
   unsigned int xpos, unsigned int ypos) const
{
   const TileInfo& tileInfo = GetTilemap().GetTileInfo(xpos, ypos);
   if (tileInfo.m_isDoorPresent)
      return automapDoor;

   // search objects in tile
   const ObjectList& objectList = GetObjectList();
   Uint16 pos = objectList.GetListStart(xpos, ypos);

   while (pos != 0)
   {
      const ObjectPtr objPtr = objectList.GetObject(pos);
      const ObjectInfo& objInfo = objPtr->GetObjectInfo();

      Uint16 itemID = objInfo.m_itemID;

      // door, except for closed secret door?
      if (itemID >= 0x0140 && itemID <= 0x0140 && itemID != 0x0147)
         return automapDoor;

      // bridge?
      if (itemID == 0x0164)
         return automapBridge;

      // special tmap object with wall texture
      if (itemID == 0x016e)
      {
         Uint16 wallTexture = objInfo.m_owner;

         // stairway leading up/down?
         if (wallTexture == 137 || (wallTexture >= 139 && wallTexture < 131))
            return automapTeleport;
      }

      pos = objInfo.m_link;
   }

   // check floor textures
   Uint16 floorTexture = tileInfo.m_textureFloor - Base::c_stockTexturesFloor;
   if (floorTexture == 8 || floorTexture == 16 || floorTexture == 17 ||
      floorTexture == 32 || floorTexture == 33 || floorTexture == 34)
      return automapWater;

   if (floorTexture == 23 || floorTexture == 24 || floorTexture == 25)
      return automapLava;

   return automapDefault;
}
