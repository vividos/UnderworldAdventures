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
/// \file DumpConversationArchive.cpp
/// \brief dumps cnv.ark archive details
//
#include "common.hpp"
#include "GameStrings.hpp"
#include "Base.hpp"
#include "ArchiveFile.hpp"

extern void DumpArkArchive(const std::string& filename, const GameStrings& gameStrings, bool isUw2);

/// dumps cnv.ark details
void DumpConversationArchive(const std::string& filename, const GameStrings& gameStrings, bool isUw2)
{
   DumpArkArchive(filename, gameStrings, isUw2);
   printf("\n");

   printf("conversation details dumping\n");

   Base::SDL_RWopsPtr rwops = Base::MakeRWopsPtr(SDL_RWFromFile(filename.c_str(), "rb"));

   if (rwops == nullptr)
   {
      printf("could not open file!\n");
      return;
   }

   Base::ArchiveFile arkFile{ rwops, isUw2 };

   for (size_t index = 0, maxIndex = arkFile.GetNumFiles(); index < maxIndex; index++)
   {
      if (!arkFile.IsAvailable(index))
         continue;

      Base::File convSlot = arkFile.GetFile(index);

      if (!convSlot.IsOpen())
      {
         printf("could not open ark file slot %zu!\n", index);
         continue;
      }

      printf("block %zu, conversation for %s\n",
         index,
         gameStrings.GetString(7, index + 16).c_str());

      Uint16 magicWord = convSlot.Read16();
      Uint16 unknown1 = convSlot.Read16();
      Uint16 codeSize = convSlot.Read16();
      Uint16 unknown2 = convSlot.Read16();
      Uint16 unknown3 = convSlot.Read16();
      Uint16 stringsBlock = convSlot.Read16();
      Uint16 reservedStackMemory = convSlot.Read16();
      Uint16 numImportEntries = convSlot.Read16();

      printf("magic word: %04x\n", magicWord);
      printf("unknown1: %04x%s\n", unknown1, unknown1 != 0 ? " non-default value!" : "");
      printf("unknown2: %04x%s\n", unknown2, unknown2 != 0 ? " non-default value!" : "");
      printf("code size: %04x\n", codeSize);
      printf("unknown3: %04x%s\n", unknown3, unknown3 != 0 ? " non-default value!" : "");
      printf("strings block: %04x\n", stringsBlock);
      printf("reserved stack memory: %04x\n", reservedStackMemory);
      printf("number of import entries: %u\n", numImportEntries);

      printf("entry  ID/mem  unknown  type             return         name\n");

      for (Uint16 importEntryIndex = 0; importEntryIndex < numImportEntries; importEntryIndex++)
      {
         Uint16 nameLength = convSlot.Read16();
         std::vector<char> name(nameLength + 1, (char)0);
         convSlot.ReadBuffer(reinterpret_cast<Uint8*>(&name[0]), nameLength);

         printf("%02u     ", importEntryIndex);
         Uint16 idOrMemAddress = convSlot.Read16();
         Uint16 unknown4 = convSlot.Read16();
         Uint16 importType = convSlot.Read16();
         Uint16 returnType = convSlot.Read16();

         printf("%04x    ", idOrMemAddress);
         printf("%04x%s    ", unknown4, unknown4 != 1 ? "!" : " ");

         printf("(%04x) %6s  ", importType,
            importType == 0x010F ? "variable" :
            importType == 0x0111 ? "function" : "unknown");

         const char* returnTypeText;
         switch (returnType)
         {
         case 0x0000: returnTypeText = "void"; break;
         case 0x0129: returnTypeText = "int"; break;
         case 0x012B: returnTypeText = "string"; break;
         default: returnTypeText = "unknown"; break;
         }
         printf("(%04x) %-7s   ", returnType, returnTypeText);

         printf("%s\n", name.data());
      }

      printf("\n");
   }
}
