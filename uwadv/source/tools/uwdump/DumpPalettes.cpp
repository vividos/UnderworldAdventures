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
/// \file DumpPalettes.cpp
/// \brief palettes decoding implementation
//
#include "common.hpp"
#include "File.hpp"
#include <filesystem>

class GameStrings;

void DumpPalettes(const std::string& filename, const GameStrings& gameStrings, bool isUw2)
{
   UNUSED(isUw2);

   printf("pals.dat palettes dumping\n");

   Base::File file(filename.c_str(), Base::modeRead);
   if (!file.IsOpen())
   {
      printf("could not open file!\n");
      return;
   }

   size_t maxPaletteNum = isUw2 ? 11 : 8;
   Uint8 pals[11][256][3];
   for (size_t numPalette = 0; numPalette < maxPaletteNum; numPalette++)
   {
      printf("dumping palette #%lu\n", numPalette);

      file.ReadBuffer(&pals[numPalette][0][0], 256 * 3);

      if (numPalette > 1)
      {
         bool foundPaletteMatch = false;
         for (size_t checkPalette = 0; checkPalette < numPalette; checkPalette++)
         {
            if (0 == memcmp(pals[numPalette], pals[checkPalette], 256 * 3))
            {
               printf("palette #%lu matches palette #%lu\n\n", numPalette, checkPalette);
               foundPaletteMatch = true;
               break;
            }
         }

         if (foundPaletteMatch)
            continue;
      }

      for (size_t palIndex = 0; palIndex < 256; palIndex++)
      {
         Uint8* paletteEntry = pals[numPalette][palIndex];
         printf("0x%02x: raw %02x %02x %02x, RGB #%02x%02x%02x",
            palIndex,
            paletteEntry[0],
            paletteEntry[1],
            paletteEntry[2],
            paletteEntry[0] << 2,
            paletteEntry[1] << 2,
            paletteEntry[2] << 2);

         printf("\n");
      }

      printf("\n");
   }
}

/// loads palette 0 from pals.dat
void LoadPalette0(const std::string& palsDatFilename, Uint8* palette0)
{
   Base::File palsFile{ palsDatFilename, Base::modeRead };
   if (!palsFile.IsOpen())
   {
      printf("could not open pals.dat file!\n");
      return;
   }

   palsFile.ReadBuffer(palette0, 256 * 3);
   palsFile.Close();
}

void DumpAuxPalettes(const std::string& filename, const GameStrings& gameStrings, bool isUw2)
{
   UNUSED(gameStrings);
   UNUSED(isUw2);

   printf("allpals.dat 4-bit palettes dumping\n");

   Base::File file(filename.c_str(), Base::modeRead);
   if (!file.IsOpen())
   {
      printf("could not open file!\n");
      return;
   }

   Uint32 fileLength = file.FileLength();

   unsigned int numEntries = fileLength / 16;

   printf("file length: %04x bytes, %u entries, %u bytes extra data\n",
      fileLength, numEntries, fileLength % 16);

   // load pals.dat
   Uint8 palette0[256][3];

   std::string palettesFilename =
      std::filesystem::path{ filename }.parent_path().append("pals.dat").generic_string();
   LoadPalette0(palettesFilename, &palette0[0][0]);


   for (unsigned int entryIndex = 0; entryIndex < numEntries; entryIndex++)
   {
      printf("auxpal #%u\n", entryIndex);

      for (unsigned int paletteIndex = 0; paletteIndex < 16; paletteIndex++)
      {
         Uint8 auxpalIndex = file.Read8();
         Uint8* paletteEntry = palette0[auxpalIndex];

         printf("index 0x%02x -> 0x%02x: raw %02x %02x %02x, RGB #%02x%02x%02x\n",
            paletteIndex,
            auxpalIndex,
            paletteEntry[0],
            paletteEntry[1],
            paletteEntry[2],
            paletteEntry[0] << 2,
            paletteEntry[1] << 2,
            paletteEntry[2] << 2);
      }

      printf("\n");
   }
}
