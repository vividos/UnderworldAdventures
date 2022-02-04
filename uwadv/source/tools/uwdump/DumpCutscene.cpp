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
/// \file DumpCutscene.cpp
/// \brief dumps cutscene animation files
//
#include "common.hpp"
#include "File.hpp"
#include "Path.hpp"

extern void DumpSinglePaletteColors(const Uint8* palette, bool scaleUpValues);

/// dumps an .n00 animation control file
void DumpAnimationControlFile(Base::File& file)
{
   Uint32 fileLength = file.FileLength();

   printf("n00 header, file length %u (0x%08x)\n",
      fileLength, fileLength);
}

/// dumps file containing .anm header and large pages
void DumpLargePageFile(Base::File& file)
{
   Uint32 fileLength = file.FileLength();

   // dump .anm header
   char fileId[5] = {};
   file.ReadBuffer(reinterpret_cast<Uint8*>(fileId), 4);

   printf(
      "file length %u (0x%08x), file ID '%s'\n",
      fileLength,
      fileLength,
      fileId);

   if (strcmp(fileId, "LPF ") != 0)
   {
      printf("invalid .anm file!");
      return;
   }

   Uint16 maxLargePages = file.Read16();
   Uint16 numLargePages = file.Read16();

   Uint32 numRecords = file.Read32();
   Uint16 maxRecordsInOneLargePage = file.Read16();

   Uint16 offsetLargePageTable = file.Read16();

   printf(
      "max. large pages: %u\nnumber of large pages: %u\n",
      maxLargePages,
      numLargePages);

   printf(
      "number of records: %u\nmax. number of records in one large page: %u\n",
      numRecords,
      maxRecordsInOneLargePage);

   printf(
      "offset to large page table: 0x%04x\n",
      offsetLargePageTable);

   char contentType[5] = {};
   file.ReadBuffer(reinterpret_cast<Uint8*>(contentType), 4);

   printf("content type '%s'\n", contentType);

   if (strcmp(contentType, "ANIM") != 0)
   {
      printf("invalid content type: %s", contentType);
      return;
   }

   Uint16 width = file.Read16();
   Uint16 height = file.Read16();
   Uint8 variant = file.Read8();
   Uint8 version = file.Read8();
   Uint8 hasLastDelta = file.Read8();
   Uint8 isLastDeltaValid = file.Read8();

   printf("size: %ux%u, variant %s, version: %i cycles/s, %s, last delta valid: %s\n",
      width, height,
      variant == 0 ? "ANIM" : "unknown",
      version == 0 ? 18 : version == 1 ? 70 : -1,
      hasLastDelta ? "loop" : "no loop",
      isLastDeltaValid ? "yes" : "no");

   Uint8 pixelType = file.Read8();
   Uint8 compressionType = file.Read8();
   Uint8 otherRecordsPerFrame = file.Read8();
   Uint8 bitmapType = file.Read8();

   printf("pixel type: %s, compression: %s, otherRecordsPerFrame: %u, bitmap type: %s\n",
      pixelType == 0 ? "256 color" : "unknown",
      compressionType == 1 ? "RunSkipDump" : "unknown",
      otherRecordsPerFrame,
      bitmapType == 1 ? "320x200 256 color" : "unknown");

   // skip over Uint8 recordTypes[32]
   file.Seek(32, Base::seekCurrent);

   Uint32 numFrames = file.Read32();
   Uint16 framesPerSecond = file.Read16();

   printf("number of frames: %u, fps %u\n", numFrames, framesPerSecond);

   // skip over Uint16 pad2[29]
   file.Seek(29 * 2, Base::seekCurrent);

   // color cycling info
   for (size_t colorCyclingIndex = 0; colorCyclingIndex < 16; colorCyclingIndex++)
   {
      Uint16 count = file.Read16();
      Uint16 rate = file.Read16();
      Uint16 flags = file.Read16();
      Uint8 low = file.Read8();
      Uint8 high = file.Read8();

      if (count == 0 && rate == 0 && flags == 0 && low == 0 && high == 0)
         printf("cycling #%zu: empty entry\n", colorCyclingIndex);
      else
         printf("cycling #%zu: %u-%u, count %u, fps %u, flags %04x\n",
            colorCyclingIndex,
            low, high,
            count, rate, flags);
   }

   Uint8 palette[256][3] = {};

   for (size_t paletteIndex = 0; paletteIndex < 256; paletteIndex++)
   {
      palette[paletteIndex][0] = file.Read8();
      palette[paletteIndex][1] = file.Read8();
      palette[paletteIndex][2] = file.Read8();
      Uint8 unusedPaletteValue = file.Read8();
      if (unusedPaletteValue)
         printf("note: palette index #%zu had unused palette value 0x%02x\n",
            paletteIndex, unusedPaletteValue);
   }

   printf("palette:\n");
   DumpSinglePaletteColors(palette[0], false);
   printf("\n");

   printf("large page headers, %u entries:\n", numLargePages);

   size_t maxLargePageIndex = std::min<size_t>(numLargePages, 256);
   for (size_t largePageIndex = 0; largePageIndex < maxLargePageIndex; largePageIndex++)
   {
      Uint16 baseRecord = file.Read16();
      Uint16 numRecordsInPage = file.Read16();
      Uint16 numBytesContent = file.Read16();

      bool hasContinuationFromPreviousLargePage = (numRecordsInPage & (1 << 15)) != 0;
      bool continuesInNextLargePage = (numRecordsInPage & (1 << 14)) != 0;
      numRecordsInPage &= ~0xc000;

      printf("large page #%u: records %u-%u, total %u records, %u bytes content%s%s\n",
         largePageIndex,
         baseRecord, baseRecord + numRecordsInPage - 1,
         numRecordsInPage,
         numBytesContent,
         hasContinuationFromPreviousLargePage ? ", with continuation from last lp" : "",
         continuesInNextLargePage ? ", continues in next lp" : "");
   }
}

/// dumps a .n?? cutscene animation file
void DumpCutscene(const std::string& filename, const GameStrings& gameStrings, bool isUw2)
{
   UNUSED(gameStrings);
   UNUSED(isUw2);

   Base::File file(filename.c_str(), Base::modeRead);
   if (!file.IsOpen())
   {
      printf("could not open file!\n");
      return;
   }

   printf("DeluxePaint Animation .anm dumping\n");

   std::string extension = Base::Path::Extension(filename);
   Base::String::Lowercase(extension);

   if (extension == ".n00")
      DumpAnimationControlFile(file);
   else
      DumpLargePageFile(file);
}
