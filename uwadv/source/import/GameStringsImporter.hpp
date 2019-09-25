//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2003,2004,2005,2006,2019 Underworld Adventures Team
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
/// \file GameStringsImporter.hpp
/// \brief game data import
//
#pragma once

#include "Base.hpp"
#include "File.hpp"
#include <map>
#include <vector>

class GameStrings;

namespace Base
{
   class ResourceManager;
}

namespace Import
{
   /// importer for game strings
   class GameStringsImporter
   {
   public:
      /// ctor; takes gamestrings object to populate
      GameStringsImporter(GameStrings& gs)
         :m_gs(gs)
      {
      }

      /// loads the default strings.pak file
      void LoadDefaultStringsPakFile(Base::ResourceManager& resourceManager);

      /// loads strings.pak file from given filename
      void LoadStringsPakFile(const char* filename);

      /// loads strings.pak file from RWops object
      void LoadStringsPakFile(Base::SDL_RWopsPtr rwops);

   private:
      /// loads string block from open .pak file
      void LoadStringBlock(Uint16 blockId);

   private:
      /// strings.pak huffman node structure
      struct StringsPakHuffmanNode
      {
         int symbol; ///< character symbol in that node
         int parent; ///< parent node
         int left;   ///< left node (-1 when no node)
         int right;  ///< right node
      };

      /// game strings object to populate
      GameStrings& m_gs;

      /// .pak file
      Base::File m_pak;

      /// file size of .pak file
      long m_fileSize;

      /// a map of all blocks and their offsets available in the file
      std::map<Uint16, Uint32> m_allBlockOffsets;

      /// a vector with all huffman nodes for the given .pak file
      std::vector<StringsPakHuffmanNode> m_allNodes;
   };

} // namespace Import
