//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2003,2004,2019,2021 Underworld Adventures Team
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
/// \file DumpArkArchive.cpp
/// \brief dumps .ark archive file
//
#include "common.hpp"
#include "GameStrings.hpp"
#include "File.hpp"
#include "ArchiveFile.hpp"
#include <algorithm>

/// dumps .ark files
void DumpArkArchive(const std::string& filename, const GameStrings& gameStrings, bool isUw2)
{
   printf(".ark archive file dumping\n");

   Base::File file(filename.c_str(), Base::modeRead);
   if (!file.IsOpen())
   {
      printf("could not open file!\n");
      return;
   }

   Uint32 fileLength = file.FileLength();

   Uint16 count = file.Read16();

   printf("file length: %04x bytes, %u entries\n",
      fileLength, count);

   if (isUw2)
   {
      Uint32 unknown = file.Read32(); // extra In32 in uw2 mode
      printf("extra unknown Int32 in uw2 archive: %04x\n", unknown);
   }

   std::vector<Uint32> offsetList;
   offsetList.resize(count);

   std::vector<Uint32> orderedOffsetList;
   orderedOffsetList.resize(count);

   for (size_t index = 0; index < count; index++)
      orderedOffsetList[index] = offsetList[index] = file.Read32();

   std::vector<Base::ArchiveFileEntryInfo> fileEntryInfoList;
   std::vector<Uint32> flagsList;

   // read in extended tables, in uw2 mode
   if (isUw2)
   {
      fileEntryInfoList.resize(count);
      flagsList.resize(count);

      for (size_t index = 0; index < count; index++)
      {
         Uint32 flags = file.Read32();
         flagsList[index] = flags;

         //UaAssert((uiFlags & 1) != 0); // flag is always set, except for scd.ark
         fileEntryInfoList[index].m_isCompressed = (flags & 2) != 0;
         fileEntryInfoList[index].m_allocatedExtraSpace = (flags & 4) != 0;
      }

      for (size_t index = 0; index < count; index++)
         fileEntryInfoList[index].m_dataSize = file.Read32();

      for (size_t index = 0; index < count; index++)
         fileEntryInfoList[index].m_availSize = file.Read32();
   }

   bool isCnvArk = filename.find("cnv") != std::string::npos ||
      filename.find("CNV") != std::string::npos;

   bool isLevArk = filename.find("lev") != std::string::npos ||
      filename.find("LEV") != std::string::npos;

   std::sort(orderedOffsetList.begin(), orderedOffsetList.end());
   orderedOffsetList.push_back(fileLength);

   size_t firstEmptyBlock = (size_t)-1;
   for (size_t index = 0; index < count; index++)
   {
      bool isEmptyBlock =
         offsetList[index] == 0 ||
         isUw2 && fileEntryInfoList[index].m_dataSize == 0;

      // detect range of empty blocks
      if (isEmptyBlock)
      {
         if (firstEmptyBlock == (size_t)-1)
            firstEmptyBlock = index;

         continue;
      }
      else
      {
         if (firstEmptyBlock != (size_t)-1)
         {
            if (index - firstEmptyBlock > 1)
               printf("blocks %zu to %zu are empty\n", firstEmptyBlock, index - 1);
            else
               printf("block %zu, empty block\n", firstEmptyBlock); // only one
         }

         firstEmptyBlock = (size_t)-1;
      }

      printf("block %zu", index);

      if (isEmptyBlock)
      {
         printf(", empty block\n");
         continue;
      }

      printf(", offset 0x%08x", offsetList[index]);

      if (!isUw2)
      {
         Uint32 offset = offsetList[index];
         auto iter = std::lower_bound(orderedOffsetList.begin(), orderedOffsetList.end(), offset);
         if (iter == orderedOffsetList.end() ||
            ++iter == orderedOffsetList.end())
            printf(", unknown size");
         else
         {
            Uint32 dataSize = *iter - offset;
            printf(", size: %08x", dataSize);
         }
      }
      else
      {
         const Base::ArchiveFileEntryInfo& info = fileEntryInfoList[index];

         printf(", flags [%08x]", flagsList[index]);
         printf(", unknownFlag: %s", (flagsList[index] & 1) != 0 ? "yes" : " no");
         printf(", compressed: %s", info.m_isCompressed ? "yes" : " no");
         printf(", allocExtraSpace: %s", info.m_allocatedExtraSpace ? "yes" : " no");
         printf(", size: %08x, avail: %08x", info.m_dataSize, info.m_availSize);
      }

      if (isCnvArk)
      {
         printf(", conversation for %s",
            gameStrings.GetString(7, index + 16).c_str());
      }

      if (isLevArk)
      {
         if (!isUw2)
         {
            printf(", type: %s",
               index < 9 ? "tile map + object list" :
               index < 18 ? "animation overlay info" :
               index < 27 ? "texture mapping table" :
               index < 36 ? "automap info" : "map notes");
         }
         else
         {
            printf(", type: %s",
               index < 80 ? "tile map + object list" :
               index < 160 ? "texture mapping table" :
               index < 240 ? "automap info" :
               index < 320 ? "map notes" : "unknown");
         }
      }

      printf("\n");
   }

   if (isCnvArk)
      printf("decompile conversations from cnv.ark using the convdec tool!\n");
}
