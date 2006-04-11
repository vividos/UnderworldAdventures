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
/*! \file objloader.cpp

   \brief objects list loader

*/

// needed includes
#include "import.hpp"
#include "objectlist.hpp"

using Import::GetBits;

namespace Detail
{

//! loads object list
class ObjectListLoader: public Base::NonCopyable
{
public:
   //! ctor
   ObjectListLoader(Underworld::ObjectList& objectList,
      std::vector<Uint16>& vecObjectWords,
      std::vector<Uint8>& vecNpcInfos,
      std::vector<Uint16>& vecTextureMapping)
      :m_objectList(objectList),
      m_vecObjectWords(vecObjectWords),
      m_vecNpcInfos(vecNpcInfos),
      m_vecTextureMapping(vecTextureMapping)
   {}

   //! follows link and adds all objects it finds (may get called recursively)
   void FollowLink(Uint16 uiLink, Uint8 xpos, Uint8 ypos);

   //! adds object to object list
   Underworld::ObjectPtr AddObject(Uint16 uiPos, Uint8 xpos, Uint8 ypos,
      Uint16 uiObjectWord[4], Uint8* pNpcInfos);

   //! adds NPC infos to object
   void AddNpcInfos(Underworld::ObjectPtr& obj, Uint8* pNpcInfos);

private:
   //! object list
   Underworld::ObjectList& m_objectList;

   //! object list words (4x Uint16 for each slot)
   std::vector<Uint16>& m_vecObjectWords;

   //! NPC info bytes (19x Uint8 for each NPC)
   std::vector<Uint8>& m_vecNpcInfos;

   //! texture mapping
   std::vector<Uint16>& m_vecTextureMapping;
};

// ObjectListLoader methods

void ObjectListLoader::FollowLink(Uint16 uiLink, Uint8 xpos, Uint8 ypos)
{
   while (uiLink != 0)
   {
      UaAssert(uiLink > 0);

      // object already visited?
      if (m_objectList.GetObject(uiLink).get() != NULL)
      {
         // object is chained from more than one object

         // item must be a trap
         Uint16 uiItemID = m_objectList.GetObject(uiLink)->GetObjectInfo().m_uiItemID;
         UaAssert(uiItemID >= 0x0180 && uiItemID <= 0x0190);
/*
         Underworld::ObjectPositionInfo& posInfo = m_objectList.GetObject(uiLink)->GetPosInfo();
         UaTrace("object already visited: pos%04x, item_id=%04x, at x=%02x y=%02x\n",
            uiLink, uiItemID, posInfo.m_xpos, posInfo.m_ypos);
*/
         return;
      }

      // add object to object list
      Uint8* pNpcData = uiLink < 0x0100 ? &m_vecNpcInfos[uiLink*19] : NULL;

      Underworld::ObjectPtr obj = AddObject(uiLink, xpos, ypos, 
         &m_vecObjectWords[uiLink*4], pNpcData);

      // check special property and add recursively
      if (!obj->GetObjectInfo().m_bIsQuantity)
      {
         Uint16 uiQuantity = obj->GetObjectInfo().m_uiQuantity;
         if (uiQuantity != 0)
            FollowLink(uiQuantity, Underworld::g_uiTileNotAPos, Underworld::g_uiTileNotAPos);
      }

      uiLink = obj->GetObjectInfo().m_uiLink;
   }
}

Underworld::ObjectPtr ObjectListLoader::AddObject(Uint16 uiPos, Uint8 xpos, Uint8 ypos,
   Uint16 uiObjectWord[4], Uint8* pNpcInfos)
{
   Underworld::ObjectPtr obj = uiPos < 0x100 ?
      Underworld::ObjectPtr(new Underworld::NpcObject) : Underworld::ObjectPtr(new Underworld::Object);

   // set object properties
   Underworld::ObjectInfo& objInfo = obj->GetObjectInfo();
   Underworld::ObjectPositionInfo& objPosInfo = obj->GetPosInfo();

   // word 0000
   objInfo.m_uiItemID =    GetBits(uiObjectWord[0], 0, 9);
   objInfo.m_uiFlags =     GetBits(uiObjectWord[0], 9, 4);
   objInfo.m_bEnchanted =  GetBits(uiObjectWord[0], 12, 1) != 0;
   objInfo.m_bIsQuantity = GetBits(uiObjectWord[0], 15, 1) != 0;
   objInfo.m_bHidden =     GetBits(uiObjectWord[0], 14, 1) != 0;

   // word 0002
   objPosInfo.m_zpos =    static_cast<Uint8>(GetBits(uiObjectWord[1], 0, 7));
   objPosInfo.m_uiHeading = static_cast<Uint8>(GetBits(uiObjectWord[1], 7, 3));
   objPosInfo.m_ypos =    static_cast<Uint8>(GetBits(uiObjectWord[1], 10, 3));
   objPosInfo.m_xpos =    static_cast<Uint8>(GetBits(uiObjectWord[1], 13, 3));

   // word 0004
   objInfo.m_uiQuality =  GetBits(uiObjectWord[2], 0, 6);
   objInfo.m_uiLink  =    GetBits(uiObjectWord[2], 6, 10);

   // word 0006
   objInfo.m_uiOwner =    GetBits(uiObjectWord[3], 0, 6);
   objInfo.m_uiQuantity = GetBits(uiObjectWord[3], 6, 10);

   objPosInfo.m_uiTileX = xpos;
   objPosInfo.m_uiTileY = ypos;

   if (uiPos < 0x0100)
      AddNpcInfos(obj, pNpcInfos);

   // object modifications
   {
      Uint16 uiItemID = objInfo.m_uiItemID;

      objInfo.m_bHidden = false; // unhide all objects for now

      // fix is_quantity flag for triggers and "delete object" traps
      if ((uiItemID >= 0x01a0 && uiItemID <= 0x01bf) ||
          uiItemID == 0x018b)
         objInfo.m_bIsQuantity = false;

      // special tmap object
      if (uiItemID == 0x016e || uiItemID == 0x016f)
      {
         if (objInfo.m_uiOwner >= 64)
            objInfo.m_uiOwner = 0;
         objInfo.m_uiOwner = m_vecTextureMapping[objInfo.m_uiOwner]; // resolve texture
      }

      // a_bridge
      if (uiItemID == 0x0164)
      {
         if (objInfo.m_uiFlags < 2)
            objInfo.m_uiFlags = (30 + objInfo.m_uiFlags + Base::c_uiStockTexturesTmobj);
         else
            objInfo.m_uiFlags = m_vecTextureMapping[objInfo.m_uiFlags - 2 + 48];
      }

      // a_door objects: store texture id in "flags"
      if (uiItemID >= 0x0140 && uiItemID < 0x0150 && (uiItemID & 7) < 6)
         objInfo.m_uiFlags = m_vecTextureMapping[(uiItemID & 7)+64];
   }

   m_objectList.SetObject(uiPos, obj);
   return obj;
}

/*! \todo find out more npc object properties */
void ObjectListLoader::AddNpcInfos(Underworld::ObjectPtr& obj, Uint8* pNpcInfos)
{
   UaAssert(IsNpcObject(obj) == true);
   UaAssert(pNpcInfos != NULL);

   Underworld::NpcObject& npc = CastToNpcObject(obj);
   Underworld::NpcInfo npcInfo = npc.GetNpcInfo();

   npcInfo.m_npc_hp = pNpcInfos[0x0000];

   Uint16 uiValue03 = pNpcInfos[0x0003] | (pNpcInfos[0x0004]<<8);
   npcInfo.m_npc_goal = static_cast<Uint8>(GetBits(uiValue03, 0, 3));
   npcInfo.m_npc_gtarg = static_cast<Uint8>(GetBits(uiValue03, 4, 8));

   Uint16 uiValue05 = pNpcInfos[0x0005] | (pNpcInfos[0x0006]<<8);
   npcInfo.m_npc_level = static_cast<Uint8>(GetBits(uiValue05, 0, 3));
   npcInfo.m_bNpc_talkedto = GetBits(uiValue05, 13, 1) == 1;
   npcInfo.m_npc_attitude = static_cast<Uint8>(GetBits(uiValue05, 14, 2));

   Uint16 uiValue0E = pNpcInfos[0x000e] | (pNpcInfos[0x000f]<<8);
   npcInfo.m_npc_xhome = static_cast<Uint8>(GetBits(uiValue0E, 10, 6));
   npcInfo.m_npc_yhome = static_cast<Uint8>(GetBits(uiValue0E, 4, 6));

   npcInfo.m_npc_whoami = pNpcInfos[0x0012];
}

} // namespace Detail

// LevelImporter methods

void Import::LevelImporter::LoadObjectList(Underworld::ObjectList& objectList,
   const TileStartLinkList& tileStartLinkList, std::vector<Uint16>& vecTextureMapping)
{
   objectList.Destroy();
   objectList.Create();

   // read in master object list
   std::vector<Uint16> vecObjectWords;
   vecObjectWords.reserve(0x0400*4);

   std::vector<Uint8> vecNpcInfos;
   vecNpcInfos.resize(0x0100*19);

   for(Uint16 uiItem=0; uiItem < 0x400; uiItem++)
   {
      vecObjectWords.push_back(m_file.Read16());
      vecObjectWords.push_back(m_file.Read16());
      vecObjectWords.push_back(m_file.Read16());
      vecObjectWords.push_back(m_file.Read16());

      // read NPC info bytes
      if (uiItem < 0x100)
         m_file.ReadBuffer(&vecNpcInfos[uiItem*19], 19);
   }

   Detail::ObjectListLoader loader(objectList, vecObjectWords, vecNpcInfos, vecTextureMapping);

   // follow each reference in all tiles
   for (Uint8 ypos=0; ypos<64; ypos++)
   for (Uint8 xpos=0; xpos<64; xpos++)
   {
      Uint16 uiLink = tileStartLinkList.GetLinkStart(xpos, ypos);

      if (uiLink != 0)
         loader.FollowLink(uiLink, xpos, ypos);
   }
}
