//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2022 Underworld Adventures Team
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
/// \file DumpGrImage.cpp
/// \brief dumps .gr image files
//
#include "common.hpp"
#include "File.hpp"
#include "Path.hpp"
#include "String.hpp"

/// dumps a .gr image file
void DumpGrImage(const std::string& filename, const GameStrings& gameStrings, bool isUw2)
{
   UNUSED(gameStrings);

   Base::File file(filename.c_str(), Base::modeRead);
   if (!file.IsOpen())
   {
      printf("could not open file!\n");
      return;
   }

   Uint32 fileLength = file.FileLength();

   Uint8 magicByte = file.Read8();
   Uint8 numEntries = file.Read16();

   printf(
      "file length %u (0x%08x), magic byte 0x%02x, entries: %u\n",
      fileLength,
      fileLength,
      magicByte,
      numEntries);

   std::vector<Uint32> offsets;
   offsets.resize(numEntries);

   for (size_t offsetIndex = 0; offsetIndex < numEntries; offsetIndex++)
      offsets[offsetIndex] = file.Read32();

   std::string filenameOnly = Base::Path::FilenameOnly(filename);
   Base::String::Lowercase(filenameOnly);

   bool isPanelImage = filenameOnly == "panels";

   for (size_t imageIndex = 0; imageIndex < numEntries; imageIndex++)
   {
      if (offsets[imageIndex] == 0 ||
         offsets[imageIndex] >= fileLength)
      {
         printf("image %u: skipping, invalid offset 0x%08x\n", imageIndex, offsets[imageIndex]);
         continue;
      }

      file.Seek(offsets[imageIndex], Base::seekBegin);

      Uint8 type, width, height;
      if (isPanelImage)
      {
         type = 4;
         width = !isUw2 ? 83 : 79;
         height = !isUw2 ? 114 : 112;
      }
      else
      {
         type = file.Read8();
         width = file.Read8();
         height = file.Read8();
      }

      const char* typeText;
      switch (type)
      {
      case 0x8: typeText = "4-bit run-length"; break;
      case 0xa: typeText = "4-bit uncompressed"; break;
      case 0x4: typeText = "8-bit uncompressed"; break;
      default: typeText = "unknown"; break;
      }

      printf("image %u, offset=%08x, size=%ux%u, type=%s (%02x)",
         imageIndex,
         offsets[imageIndex],
         width, height,
         typeText, type);

      if (type == 0x8 || type == 0xa)
      {
         Uint8 auxpal = file.Read8();
         printf(", auxpal 0x%02x", auxpal);
      }

      Uint16 dataLength = isPanelImage
         ? width * height
         : file.Read16();

      printf(", dataLength %u (0x%04x) bytes\n",
         dataLength, dataLength);
   }
}
