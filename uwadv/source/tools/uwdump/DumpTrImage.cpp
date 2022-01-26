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
/// \file DumpTrImage.cpp
/// \brief dumps .tr image files
//
#include "common.hpp"
#include "File.hpp"

/// dumps a .tr texture image file
void DumpTextureImage(const std::string& filename, const GameStrings& gameStrings, bool isUw2)
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
   Uint8 xyResolution = file.Read8();

   Uint16 numEntries = file.Read16();

   printf(
      "file length %u (0x%08x), magic byte 0x%02x, %ux%u, entries: %u\n",
      fileLength,
      fileLength,
      magicByte,
      xyResolution, xyResolution,
      numEntries);
}
