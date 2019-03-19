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
/// \file GameStringsImporter.cpp
/// \brief game strings loader
//
#include "pch.hpp"
#include "GameStringsImporter.hpp"
#include "ResourceManager.hpp"
#include "GameStrings.hpp"

using Import::GameStringsImporter;

// define this when you can be sure the strings.pak is correct;
// speeds up loading a bit
#define HAVE_CORRECT_STRINGS_FILE

/// Opens the strings.pak file in the data folder of the game. Files for uw1
/// and uw2 have the same format.
/// \param resourceManager resource manager
void GameStringsImporter::LoadDefaultStringsPakFile(Base::ResourceManager& resourceManager)
{
   LoadStringsPakFile(resourceManager.GetUnderworldFile(Base::resourceGameUw, "data/strings.pak"));
}

/// Opens a strings.pak file or a file that has the same format (e.g. created
/// with the "strpak" tool).
/// \param filename filename of the strings.pak file
void GameStringsImporter::LoadStringsPakFile(const char* filename)
{
   SDL_RWops* rwops = SDL_RWFromFile(filename, "rb");
   if (rwops == NULL)
      throw Base::Exception("could not open file strings.pak");

   LoadStringsPakFile(Base::MakeRWopsPtr(rwops));
}

void GameStringsImporter::LoadStringsPakFile(Base::SDL_RWopsPtr rwops)
{
   m_pak = Base::File(rwops);

   m_fileSize = m_pak.FileLength();

   m_allNodes.clear();
   m_allBlockOffsets.clear();

   Uint16 numNodes = m_pak.Read16();

   m_allNodes.resize(numNodes);
   for (Uint16 nodeIndex = 0; nodeIndex < numNodes; nodeIndex++)
   {
      m_allNodes[nodeIndex].symbol = m_pak.Read8();
      m_allNodes[nodeIndex].parent = m_pak.Read8();
      m_allNodes[nodeIndex].left = m_pak.Read8();
      m_allNodes[nodeIndex].right = m_pak.Read8();
   }

   Uint16 numBlocks = m_pak.Read16();

   for (Uint16 blockIndex = 0; blockIndex < numBlocks; blockIndex++)
   {
      Uint16 blockId = m_pak.Read16();
      Uint32 offset = m_pak.Read32();

      m_allBlockOffsets[blockId] = offset;
   }

   for (auto iter = m_allBlockOffsets.begin(); iter != m_allBlockOffsets.end(); iter++)
   {
      Uint16 blockId = iter->first;

      if (!m_gs.IsBlockAvail(blockId))
      {
         m_gs.GetStringBlockSet().insert(blockId);

         m_gs.GetAllStrings()[blockId] = std::vector<std::string>();
      }

      LoadStringBlock(blockId);
   }
}

/// Loads a single string block from file.
/// \param blockId block id of string block to load
/// \param strblock strings array that gets filled
void GameStringsImporter::LoadStringBlock(Uint16 blockId)
{
   std::vector<std::string>& stringBlock = m_gs.GetAllStrings()[blockId];
   stringBlock.clear();

   std::map<Uint16, Uint32>::iterator iter = m_allBlockOffsets.find(blockId);
   if (iter == m_allBlockOffsets.end())
   {
      UaTrace("loading string block %04x failed\n", blockId);
      return;
   }

   Uint32 offset = m_allBlockOffsets[blockId];

   m_pak.Seek(offset, Base::seekBegin);

   Uint16 numStrings = m_pak.Read16();

   // all string offsets
   std::vector<Uint16> stringOffsets;
   stringOffsets.resize(numStrings);

   for (Uint16 offsetIndex = 0; offsetIndex < numStrings; offsetIndex++)
      stringOffsets[offsetIndex] = m_pak.Read16();

   Uint32 currentOffset = offset + (numStrings + 1) * sizeof(Uint16);
   unsigned int nodenum = m_allNodes.size();

   for (Uint16 stringIndex = 0; stringIndex < numStrings; stringIndex++)
   {
      m_pak.Seek(currentOffset + stringOffsets[stringIndex], Base::seekBegin);

      char c;
      std::string str;

      int bit = 0;
      int raw = 0;

      do
      {
         int node = nodenum - 1; // starting node

         // huffman tree decode loop
         while (char(m_allNodes[node].left) != -1 && char(m_allNodes[node].right) != -1)
         {
            if (bit == 0)
            {
               bit = 8;
               raw = m_pak.Read8();

#ifndef HAVE_CORRECT_STRINGS_FILE
               if (m_pak.Tell() >= m_fileSize)
               {
                  // premature end of file, should not happen
                  stringIndex = numStrings;
                  //blockId=sblocks;
                  break;
               }
#endif
            }

            // decide which node is next
            node = raw & 0x80 ? short(m_allNodes[node].right)
               : short(m_allNodes[node].left);

            raw <<= 1;
            bit--;
         }
#ifndef HAVE_CORRECT_STRINGS_FILE
         if (m_pak.Tell() >= m_fileSize)
            break;
#endif

         // have a new symbol
         c = static_cast<char>(m_allNodes[node].symbol);
         if (c != '|')
            str.append(1, c);

      } while (c != '|');

      stringBlock.push_back(str);
   }

   // remove excess memory
   std::vector<std::string>(stringBlock).swap(stringBlock);
}
