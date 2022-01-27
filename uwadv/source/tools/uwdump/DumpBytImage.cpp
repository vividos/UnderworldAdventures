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
/// \file DumpBytImage.cpp
/// \brief dumps .byt image files
//
#include "common.hpp"
#include "String.hpp"
#include "File.hpp"
#include "FileSystem.hpp"
#include "ArchiveFile.hpp"

extern void DumpArkArchive(const std::string& filename, const GameStrings& gameStrings, bool isUw2);

/// mapping from filename to palette index
static std::map<std::string, unsigned int> g_filenameToPaletteIndexMap =
{
   // uw_demo
   { "dmain.byt", 0 },
   { "presd.byt", 5 },

   // uw1
   { "blnkmap.byt", 1 },
   { "chargen.byt", 3 },
   { "conv.byt", 0 },
   { "main.byt", 0 },
   { "opscr.byt", 2 },
   { "pres1.byt", 5 },
   { "pres2.byt", 5 },
   { "win1.byt", 7 },
   { "win2.byt", 7 },

   // uw2
   { "byt.ark:0", 1 },
   { "byt.ark:1", 0 },
   { "byt.ark:2", 0 },
   { "byt.ark:3", 0 }, // unused
   { "byt.ark:4", 0 },
   { "byt.ark:5", 0 },
   { "byt.ark:6", 5 },
   { "byt.ark:7", 5 },
   { "byt.ark:8", 0 },
   { "byt.ark:9", 0 },
   { "byt.ark:10", 0 }, // unused

   { "lback000.byt", 0 },
   { "lback001.byt", 0 },
   { "lback002.byt", 0 },
   { "lback003.byt", 0 },
   { "lback004.byt", 0 },
   { "lback005.byt", 0 },
   { "lback006.byt", 0 },
   { "lback007.byt", 0 },
};

/// dumps a single .byt image
void DumpBytImage(const std::string& path, Base::File& file, int imageIndex, bool isUw2)
{
   size_t pos = path.find_last_of(Base::FileSystem::PathSeparator[0]);
   std::string filename = pos == std::string::npos ? path : path.substr(pos + 1);
   Base::String::Lowercase(filename);

   Uint32 fileLength = file.FileLength();

   printf(
      imageIndex >= 0 ? "file %s, index %i, " : "file %s, ",
      filename.c_str(),
      imageIndex);

   printf("file size %u: %s, ",
      fileLength,
      fileLength == 320 * 200 ? "320x200" : "unknown");

   std::string key = filename;
   if (imageIndex >= 0)
      key += ":" + std::to_string(imageIndex);

   auto iter = g_filenameToPaletteIndexMap.find(key);
   if (iter == g_filenameToPaletteIndexMap.end())
      printf("unknown palette!\n");
   else
   {
      unsigned int paletteIndex = g_filenameToPaletteIndexMap[key];
      printf("palette #%u\n", paletteIndex);
   }
}

/// dumps a .byt image file
void DumpBytImage(const std::string& filename, const GameStrings& gameStrings, bool isUw2)
{
   UNUSED(gameStrings);

   Base::File file(filename.c_str(), Base::modeRead);
   if (!file.IsOpen())
   {
      printf("could not open file!\n");
      return;
   }

   DumpBytImage(filename, file, -1, isUw2);
}

/// dumps uw2 byt.ark archive
void DumpBytArkFile(const std::string& filename, const GameStrings& gameStrings, bool isUw2)
{
   // first dump general .ark structure
   DumpArkArchive(filename, gameStrings, isUw2);
   printf("\n");

   printf("byt.ark file dumping\n");

   SDL_RWops* rwops = SDL_RWFromFile(filename.c_str(), "rb");
   Base::ArchiveFile ark{ Base::MakeRWopsPtr(rwops), isUw2 };

   for (size_t fileIndex = 0; fileIndex < ark.GetNumFiles(); fileIndex++)
   {
      if (!ark.IsAvailable(fileIndex))
      {
         printf("archive file image %zu not available\n", fileIndex);
         continue;
      }

      printf("archive file image %zu: ", fileIndex);

      Base::File arkFile = ark.GetFile(fileIndex);
      DumpBytImage(filename, arkFile, static_cast<int>(fileIndex), isUw2);
   }
}
