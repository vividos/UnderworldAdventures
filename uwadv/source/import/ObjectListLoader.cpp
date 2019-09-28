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
/// \file ObjectListLoader.cpp
/// \brief objects list loader
//
#include "pch.hpp"
#include "ObjectListLoader.hpp"
#include "LevelImporter.hpp"
#include "ObjectList.hpp"
#include "Constants.hpp"

using Import::GetBits;
using Import::ObjectListLoader;

void ObjectListLoader::FollowLink(Uint16 link, Uint8 tilePosX, Uint8 tilePosY)
{
   while (link != 0)
   {
      // object already visited?
      if (m_objectList.GetObject(link).get() != nullptr)
      {
         // object is chained from more than one object
         Underworld::ObjectPositionInfo& posInfo = m_objectList.GetObject(link)->GetPosInfo();

         // object already has valid tile coordinates?
         if (posInfo.m_tileX == Underworld::c_tileNotAPos && posInfo.m_tileY == Underworld::c_tileNotAPos)
         {
            // no, so set them
            posInfo.m_tileX = tilePosX;
            posInfo.m_tileY = tilePosY;
         }
#ifdef HAVE_DEBUG
         else if (tilePosX != Underworld::c_tileNotAPos && tilePosY != Underworld::c_tileNotAPos)
         {
            Uint16 itemID = m_objectList.GetObject(link)->GetObjectInfo().m_itemID;
            UaTrace("object already visited: pos=%04x, itemID=%04x, at x=%02x y=%02x\n",
               link, itemID, posInfo.m_tileX, posInfo.m_tileY);
         }
#endif
         return;
      }

      // add object to object list
      Uint8* npcInfos = link < 0x0100 ? &m_npcInfosList[link * 19] : nullptr;

      Underworld::ObjectPtr obj = AddObject(link, tilePosX, tilePosY,
         &m_objectWordsList[link * 4], npcInfos);

      // check special property and add recursively
      if (!obj->GetObjectInfo().m_isQuantity)
      {
         Uint16 quantity = obj->GetObjectInfo().m_quantity;
         if (quantity != 0)
            FollowLink(quantity, Underworld::c_tileNotAPos, Underworld::c_tileNotAPos);
      }

      link = obj->GetObjectInfo().m_link;
   }
}

Underworld::ObjectPtr ObjectListLoader::AddObject(Uint16 pos, Uint8 tilePosX, Uint8 tilePosY,
   Uint16 objectWord[4], Uint8* npcInfos)
{
   Underworld::ObjectPtr obj = pos < 0x100 ?
      Underworld::ObjectPtr(new Underworld::NpcObject) : Underworld::ObjectPtr(new Underworld::Object);

   // set object properties
   Underworld::ObjectInfo& objInfo = obj->GetObjectInfo();
   Underworld::ObjectPositionInfo& objPosInfo = obj->GetPosInfo();

   // word 0000
   objInfo.m_itemID = GetBits(objectWord[0], 0, 9);
   objInfo.m_flags = GetBits(objectWord[0], 9, 4);
   objInfo.m_isEnchanted = GetBits(objectWord[0], 12, 1) != 0;
   objInfo.m_isQuantity = GetBits(objectWord[0], 15, 1) != 0;
   objInfo.m_isHidden = GetBits(objectWord[0], 14, 1) != 0;

   // word 0002
   objPosInfo.m_zpos = static_cast<Uint8>(GetBits(objectWord[1], 0, 7));
   objPosInfo.m_heading = static_cast<Uint8>(GetBits(objectWord[1], 7, 3));
   objPosInfo.m_ypos = static_cast<Uint8>(GetBits(objectWord[1], 10, 3));
   objPosInfo.m_xpos = static_cast<Uint8>(GetBits(objectWord[1], 13, 3));

   // word 0004
   objInfo.m_quality = GetBits(objectWord[2], 0, 6);
   objInfo.m_link = GetBits(objectWord[2], 6, 10);

   // word 0006
   objInfo.m_owner = GetBits(objectWord[3], 0, 6);
   objInfo.m_quantity = GetBits(objectWord[3], 6, 10);

   objPosInfo.m_tileX = tilePosX;
   objPosInfo.m_tileY = tilePosY;

   if (pos < 0x0100)
      AddNpcInfos(obj, npcInfos);

   // object modifications
   {
      Uint16 itemID = objInfo.m_itemID;

      // fix is_quantity flag for triggers and "delete object" traps
      if ((itemID >= 0x01a0 && itemID <= 0x01bf) ||
         itemID == 0x018b)
         objInfo.m_isQuantity = false;

      // special tmap object
      if (itemID == 0x016e || itemID == 0x016f)
      {
         if (objInfo.m_owner >= 64)
            objInfo.m_owner = 0;
         objInfo.m_owner = m_textureMapping[objInfo.m_owner]; // resolve texture
      }

      // a_bridge
      if (itemID == 0x0164)
      {
         if (objInfo.m_flags < 2)
            objInfo.m_flags = (30 + objInfo.m_flags + Base::c_stockTexturesTmobj);
         else
            objInfo.m_flags = m_textureMapping[objInfo.m_flags - 2 + 48];
      }

      // a_door objects: store texture id in "flags"
      if (itemID >= 0x0140 && itemID < 0x0150 && (itemID & 7) < 6)
      {
         size_t doorTextureOffset = m_textureMapping.size() - 6;
         objInfo.m_flags = m_textureMapping[(itemID & 7) + doorTextureOffset];
      }
   }

   m_objectList.SetObject(pos, obj);
   return obj;
}

/// \todo find out more npc object properties
void ObjectListLoader::AddNpcInfos(Underworld::ObjectPtr& obj, Uint8* npcInfos)
{
   UaAssert(obj->IsNpcObject() == true);
   UaAssert(npcInfos != NULL);

   Underworld::NpcObject& npc = obj->GetNpcObject();
   Underworld::NpcInfo& npcInfo = npc.GetNpcInfo();

   npcInfo.m_npc_hp = npcInfos[0x0000];

   Uint16 value03 = npcInfos[0x0003] | (npcInfos[0x0004] << 8);
   npcInfo.m_npc_goal = static_cast<Uint8>(GetBits(value03, 0, 3));
   npcInfo.m_npc_gtarg = static_cast<Uint8>(GetBits(value03, 4, 8));

   Uint16 value05 = npcInfos[0x0005] | (npcInfos[0x0006] << 8);
   npcInfo.m_npc_level = static_cast<Uint8>(GetBits(value05, 0, 3));
   npcInfo.m_npc_talkedto = GetBits(value05, 13, 1) == 1;
   npcInfo.m_npc_attitude = static_cast<Uint8>(GetBits(value05, 14, 2));

   Uint16 value0E = npcInfos[0x000e] | (npcInfos[0x000f] << 8);
   npcInfo.m_npc_xhome = static_cast<Uint8>(GetBits(value0E, 10, 6));
   npcInfo.m_npc_yhome = static_cast<Uint8>(GetBits(value0E, 4, 6));

   npcInfo.m_npc_whoami = npcInfos[0x0012];
}
