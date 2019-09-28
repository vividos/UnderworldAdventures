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
/// \file ObjectListLoader.hpp
/// \brief objects list loader
//
#pragma once

#include "ObjectList.hpp"

namespace Import
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
      void FollowLink(Uint16 link, Uint8 tilePosX, Uint8 tilePosY);

      /// adds object to object list
      Underworld::ObjectPtr AddObject(Uint16 uiPos, Uint8 tilePosX, Uint8 tilePosY,
         Uint16 objectWord[4], Uint8* npcInfos);

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

