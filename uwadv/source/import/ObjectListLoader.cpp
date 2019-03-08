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
/// \file ObjectListLoader.cpp
/// \brief objects list loader
//
#include "Import.hpp"
#include "LevelImporter.hpp"
#include "ObjectList.hpp"

using Import::GetBits;

namespace Detail
{
   /// loads object list
   class ObjectListLoader
   {
   public:
      /// ctor
      ObjectListLoader(Underworld::ObjectList& objectList,
         std::vector<Uint16>& objectWordsList,
         std::vector<Uint8>& npcInfosList,
         std::vector<Uint16>& textureMapping)
         :m_objectList(objectList),
         m_objectWordsList(objectWordsList),
         m_npcInfosList(npcInfosList),
         m_textureMapping(textureMapping)
      {}

      /// follows link and adds all objects it finds (may get called recursively)
      void FollowLink(Uint16 link, Uint8 xpos, Uint8 ypos);

      /// adds object to object list
      Underworld::ObjectPtr AddObject(Uint16 uiPos, Uint8 xpos, Uint8 ypos,
         Uint16 uiObjectWord[4], Uint8* npcInfos);

      /// adds NPC infos to object
      void AddNpcInfos(Underworld::ObjectPtr& obj, Uint8* npcInfos);

   private:
      /// deleted copy ctor
      ObjectListLoader(const ObjectListLoader&) = delete;

      /// deleted assignment operator
      ObjectListLoader& operator=(const ObjectListLoader&) = delete;

   private:
      /// object list
      Underworld::ObjectList& m_objectList;

      /// object list words (4x Uint16 for each slot)
      std::vector<Uint16>& m_objectWordsList;

      /// NPC info bytes (19x Uint8 for each NPC)
      std::vector<Uint8>& m_npcInfosList;

      /// texture mapping
      std::vector<Uint16>& m_textureMapping;
   };

} // namespace Detail

using Detail::ObjectListLoader;

void ObjectListLoader::FollowLink(Uint16 link, Uint8 xpos, Uint8 ypos)
{
   while (link != 0)
   {
      UaAssert(link > 0);

      // object already visited?
      if (m_objectList.GetObject(link).get() != NULL)
      {
         // object is chained from more than one object
         Underworld::ObjectPositionInfo& posInfo = m_objectList.GetObject(link)->GetPosInfo();

         // object already has valid tile coordinates?
         if (posInfo.m_xpos == Underworld::c_tileNotAPos && posInfo.m_ypos == Underworld::c_tileNotAPos)
         {
            // no, so set them
            posInfo.m_xpos = xpos;
            posInfo.m_ypos = ypos;
         }
         /*
                  Uint16 itemID = m_objectList.GetObject(link)->GetObjectInfo().m_itemID;
                  UaTrace("object already visited: pos%04x, item_id=%04x, at x=%02x y=%02x\n",
                     link, itemID, posInfo.m_xpos, posInfo.m_ypos);
         */
         return;
      }

      // add object to object list
      Uint8* pNpcData = link < 0x0100 ? &m_npcInfosList[link * 19] : NULL;

      Underworld::ObjectPtr obj = AddObject(link, xpos, ypos,
         &m_objectWordsList[link * 4], pNpcData);

      // check special property and add recursively
      if (!obj->GetObjectInfo().m_isQuantity)
      {
         Uint16 uiQuantity = obj->GetObjectInfo().m_quantity;
         if (uiQuantity != 0)
            FollowLink(uiQuantity, Underworld::c_tileNotAPos, Underworld::c_tileNotAPos);
      }

      link = obj->GetObjectInfo().m_link;
   }
}

Underworld::ObjectPtr ObjectListLoader::AddObject(Uint16 uiPos, Uint8 xpos, Uint8 ypos,
   Uint16 uiObjectWord[4], Uint8* npcInfos)
{
   Underworld::ObjectPtr obj = uiPos < 0x100 ?
      Underworld::ObjectPtr(new Underworld::NpcObject) : Underworld::ObjectPtr(new Underworld::Object);

   // set object properties
   Underworld::ObjectInfo& objInfo = obj->GetObjectInfo();
   Underworld::ObjectPositionInfo& objPosInfo = obj->GetPosInfo();

   // word 0000
   objInfo.m_itemID = GetBits(uiObjectWord[0], 0, 9);
   objInfo.m_flags = GetBits(uiObjectWord[0], 9, 4);
   objInfo.m_isEnchanted = GetBits(uiObjectWord[0], 12, 1) != 0;
   objInfo.m_isQuantity = GetBits(uiObjectWord[0], 15, 1) != 0;
   objInfo.m_isHidden = GetBits(uiObjectWord[0], 14, 1) != 0;

   // word 0002
   objPosInfo.m_zpos = static_cast<Uint8>(GetBits(uiObjectWord[1], 0, 7));
   objPosInfo.m_heading = static_cast<Uint8>(GetBits(uiObjectWord[1], 7, 3));
   objPosInfo.m_ypos = static_cast<Uint8>(GetBits(uiObjectWord[1], 10, 3));
   objPosInfo.m_xpos = static_cast<Uint8>(GetBits(uiObjectWord[1], 13, 3));

   // word 0004
   objInfo.m_quality = GetBits(uiObjectWord[2], 0, 6);
   objInfo.m_link = GetBits(uiObjectWord[2], 6, 10);

   // word 0006
   objInfo.m_owner = GetBits(uiObjectWord[3], 0, 6);
   objInfo.m_quantity = GetBits(uiObjectWord[3], 6, 10);

   objPosInfo.m_tileX = xpos;
   objPosInfo.m_tileY = ypos;

   if (uiPos < 0x0100)
      AddNpcInfos(obj, npcInfos);

   // object modifications
   {
      Uint16 itemID = objInfo.m_itemID;

      objInfo.m_isHidden = false; // unhide all objects for now

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

   m_objectList.SetObject(uiPos, obj);
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

void Import::LevelImporter::LoadObjectList(Underworld::ObjectList& objectList,
   const TileStartLinkList& tileStartLinkList, std::vector<Uint16>& textureMapping)
{
   objectList.Destroy();
   objectList.Create();

   // read in master object list
   std::vector<Uint16> objectWordsList;
   objectWordsList.reserve(0x0400 * 4);

   std::vector<Uint8> npcInfosList;
   npcInfosList.resize(0x0100 * 19);

   for (Uint16 itemIndex = 0; itemIndex < 0x400; itemIndex++)
   {
      objectWordsList.push_back(m_file.Read16());
      objectWordsList.push_back(m_file.Read16());
      objectWordsList.push_back(m_file.Read16());
      objectWordsList.push_back(m_file.Read16());

      // read NPC info bytes
      if (itemIndex < 0x100)
         m_file.ReadBuffer(&npcInfosList[itemIndex * 19], 19);
   }

   Detail::ObjectListLoader loader(objectList, objectWordsList, npcInfosList, textureMapping);

   // follow each reference in all tiles
   for (Uint8 ypos = 0; ypos < 64; ypos++)
      for (Uint8 xpos = 0; xpos < 64; xpos++)
      {
         Uint16 link = tileStartLinkList.GetLinkStart(xpos, ypos);

         if (link != 0)
            loader.FollowLink(link, xpos, ypos);
      }
}
