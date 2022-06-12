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
#include "String.hpp"
#include "GameStrings.hpp"

extern void DumpSinglePaletteColors(const Uint8* palette, bool scaleUpValues);

/// command name for each command
static std::map<Uint16, const char*> c_cutseneCommandNames =
{
   { 0x0000, "show-text" },
   { 0x0000, "set-flag" },
   { 0x0002, "no-op" },
   { 0x0003, "pause" },

   { 0x0004, "to-frame" },
   { 0x0006, "end-cutsc" },
   { 0x0007, "rep-seg" },

   { 0x0008, "open-file" },
   { 0x0009, "fade-out" },
   { 0x000a, "fade-in" },

   { 0x000d, "text-play" },
   { 0x000f, "klang" },

   // uw2
   { 0x0012, "no-op2" },
   { 0x001a, "no-op3" },
};

/// number of arguments per command
static std::map<Uint16, size_t> c_cutseneCommandNumArgs =
{
   { 0x0000, 2 },
   { 0x0001, 0 },
   { 0x0002, 2 },
   { 0x0003, 1 },

   { 0x0004, 2 },
   { 0x0005, 1 }, // uw2: 0
   { 0x0006, 0 },
   { 0x0007, 1 },

   { 0x0008, 2 },
   { 0x0009, 1 },
   { 0x000a, 1 },
   { 0x000b, 1 },

   { 0x000c, 1 },
   { 0x000d, 3 },
   { 0x000e, 2 },
   { 0x000f, 0 },

   // uw2
   { 0x0010, 0 }, // arg0 * 3 + 2
   { 0x0011, 0 }, // arg0 * 3 + 4
   { 0x0012, 4 },
   { 0x0013, 3 },

   { 0x0014, 3 },
   { 0x0015, 2 },
   { 0x0016, 3 },
   { 0x0017, 3 },

   { 0x0018, 1 },
   { 0x0019, 1 },
   { 0x001a, 1 },
   { 0x001b, 1 },
};

bool GetStringBlockFromFilename(std::string filename, unsigned int& blockNum)
{
   Base::String::Lowercase(filename);

   std::string octalCutscene = filename.substr(
      filename.find(".n00") - 3, 3);

   if (octalCutscene.size() != 3)
      return false;

   blockNum = 0x0c00 +
      (octalCutscene[0] - '0') * 64 +
      (octalCutscene[1] - '0') * 8 +
      octalCutscene[2] - '0';

   return true;
}

/// dumps an .n00 animation control file
void DumpAnimationControlFile(Base::File& file,
   const std::string& filename, const GameStrings& gameStrings, bool isUw2)
{
   unsigned int blockNum;
   if (!GetStringBlockFromFilename(filename, blockNum))
   {
      printf("error: couldn't get strings block number from filename %s",
         filename.c_str());
      return;
   }

   Uint32 fileLength = file.FileLength();

   bool isBlockAvail = gameStrings.IsBlockAvail(blockNum);

   printf("n00 header, file length %u (0x%08x), strings block 0x%04x%s\n\n",
      fileLength, fileLength,
      blockNum,
      isBlockAvail ? "" : " (not available)");

   Uint16 maxCommand = !isUw2 ? 0x0f : 0x1b;

   for (Uint32 fileIndex = 0; fileIndex < fileLength; fileIndex += 4)
   {
      Uint16 frameNumber = file.Read16();
      Uint16 command = file.Read16();

      const char* commandName =
         c_cutseneCommandNames.find(command) == c_cutseneCommandNames.end()
         ? "????"
         : c_cutseneCommandNames[command];

      printf("0x%04x: at frame 0x%04x: command 0x%02x (%- 9s)",
         fileIndex, frameNumber, command, commandName);

      if (command > maxCommand)
      {
         printf(" oh dear, bad cutscene command 0x%02x at 0x%04x\n",
            command, fileIndex);
         break;
      }

      UaAssert(c_cutseneCommandNumArgs.find(command) != c_cutseneCommandNumArgs.end());
      size_t commandNumArgs = c_cutseneCommandNumArgs[command];

      if (isUw2 && command == 0x0005)
         commandNumArgs = 0;

      // adjust variable sized args
      if (command == 0x0010)
         commandNumArgs = 2;
      else if (command == 0x0011)
         commandNumArgs = 4;

      if (commandNumArgs > 0)
      {
         std::vector<Uint16> args;
         for (size_t i = 0; i < commandNumArgs; i++)
            args.push_back(file.Read16());

         printf(" [");
         for (size_t i = 0; i < commandNumArgs; i++)
         {
            if (fileIndex + i >= fileLength)
               break;

            if (i > 0)
               printf(" ");

            printf("%04x", args[i]);
         }

         printf("]");

         if (command == 0x00 || command == 0x0d)
         {
            std::string text = args[1] == 0xffff
               ? "no text"
               : "\"" + gameStrings.GetString(blockNum, args[1]) + "\"";

            Base::String::Replace(text, "\n", "\\n");

            printf(": %s", text.c_str());
         }
         else if (command == 0x08)
            printf(": cs%03o.n%02o", args[0], args[1]);
      }

      fileIndex += commandNumArgs * 2;

      printf("\n");
   }

   printf("end of file\n");
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
      DumpAnimationControlFile(file, filename, gameStrings, isUw2);
   else
      DumpLargePageFile(file);
}
