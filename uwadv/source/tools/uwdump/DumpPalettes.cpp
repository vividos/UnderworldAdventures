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
#include "String.hpp"
#include "Color3ub.hpp"
#include <filesystem>

class GameStrings;

/// mapping from RGB color hex values to CSS color names (only level 1)
/// \see https://developer.mozilla.org/en-US/docs/Web/CSS/color_value
std::map<Uint32, std::string> c_colorMapping =
{
   { 0x000000, "black" },
   { 0xc0c0c0, "silver" },
   { 0x808080, "gray" },
   { 0xffffff, "white" },
   { 0x800000, "maroon" },
   { 0xff0000, "red" },
   { 0x800080, "purple" },
   { 0xff00ff, "fuchsia" },
   { 0x008000, "green" },
   { 0x00ff00, "lime" },
   { 0x808000, "olive" },
   { 0xffff00, "yellow" },
   { 0x000080, "navy" },
   { 0x0000ff, "blue" },
   { 0x008080, "teal" },
   { 0x00ffff, "aqua" },
   { 0xffa500, "orange" },
};

std::string GetColorName(unsigned int paletteIndex, Uint32 argb)
{
   if (paletteIndex == 0)
      return "transparent";

   if (c_colorMapping.find(argb) != c_colorMapping.end())
   {
      return c_colorMapping[argb].c_str();
   }

   Uint32 foundColor = 0;
   double currentColorDistance = 1.0;

   for (const auto& iter : c_colorMapping)
   {
      double colorDistance = Color3ub::CalcColorDistance(Color3ub(argb), Color3ub(iter.first));
      if (colorDistance < currentColorDistance)
      {
         currentColorDistance = colorDistance;
         foundColor = iter.first;
      }
   }

   int similarity = int(currentColorDistance * 100);

   return Base::String::Format("%s, similarity %i%%", c_colorMapping[foundColor].c_str(), similarity);
}

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
   Uint8 pals[11][256][3] = {};
   for (size_t numPalette = 0; numPalette < maxPaletteNum; numPalette++)
   {
      printf("dumping palette #%zu\n", numPalette);

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

         Uint32 argb =
            (((Uint32)paletteEntry[0] << 16) << 2) |
            (((Uint32)paletteEntry[1] << 8) << 2) |
            ((Uint32)paletteEntry[2] << 2);

         std::string colorName = GetColorName(palIndex, argb);

         printf(" (%s)\n", colorName.c_str());
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

         printf("index 0x%02x -> 0x%02x: raw %02x %02x %02x, RGB #%02x%02x%02x",
            paletteIndex,
            auxpalIndex,
            paletteEntry[0],
            paletteEntry[1],
            paletteEntry[2],
            paletteEntry[0] << 2,
            paletteEntry[1] << 2,
            paletteEntry[2] << 2);

         Uint32 argb =
            (((Uint32)paletteEntry[0] << 16) << 2) |
            (((Uint32)paletteEntry[1] << 8) << 2) |
            ((Uint32)paletteEntry[2] << 2);

         std::string colorName = GetColorName(auxpalIndex, argb);

         printf(" (%s)\n", colorName.c_str());
      }

      printf("\n");
   }
}
