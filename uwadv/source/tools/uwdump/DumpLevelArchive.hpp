//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2003,2019,2021 Underworld Adventures Team
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
/// \file DumpLevelArchive.hpp
/// \brief lev.ark level archive decoding
//
#pragma once

#include <set>
#include <bitset>

class GameStrings;

/// dumps level archives
class DumpLevelArchive
{
public:
   // ctor
   DumpLevelArchive(const GameStrings& gameStrings, bool isUw2)
      :m_isUw2(isUw2),
      m_gameStrings(gameStrings),
      m_currentLevel(0),
      m_currentFollowLevel(0)
   {
   }

   void Start(const std::string& filename);

private:
   void FixLevelData();
   void DrawLevel();
   void ProcessLevel();
   void DumpObjectInfos();
   void DumpSpecialLinkChain(std::bitset<0x400>& visited, unsigned int pos, unsigned int indent = 0);
   void DumpObject(Uint16 pos);
   void DumpNPCInfos(Uint16 pos);

   void FollowLink(Uint16 link, unsigned int tilepos, bool special);

private:
   /// indicates if it's an uw2 lev.ark
   bool m_isUw2;

   /// game strings
   const GameStrings& m_gameStrings;

   /// current underworld level
   unsigned int m_currentLevel;

   /// tilemap first byte
   std::vector<Uint16> tilemap;

   /// recursion level for follow_link()
   unsigned int m_currentFollowLevel;

   /// link chain index from tilemap, 64*64 bytes long
   std::vector<Uint16> tilemap_links;

   /// set with free master object list positions
   std::set<Uint16> freelist;

   /// common object info bytes, 0x400*8 bytes long
   std::vector<Uint16> objinfos;

   /// npc extra info bytes, 0x100*(0x1b-8) bytes long
   std::vector<Uint8> npcinfos;

   /// reference count for all objects, 0x400 long
   std::vector<unsigned int> linkcount;

   /// link reference index, 0x400 long
   std::vector<Uint16> linkref;

   /// tilemap position for all objects, 0x400 long
   std::vector<std::pair<Uint8, Uint8>> item_positions;

   /// texture mapping table
   std::vector<Uint16> texmapping;
};
