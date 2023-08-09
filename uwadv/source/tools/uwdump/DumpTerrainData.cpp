//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2023 Underworld Adventures Team
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
/// \file DumpTerrainData.cpp
/// \brief dumps terrain data
//
#include "common.hpp"
#include "GameStrings.hpp"
#include "File.hpp"

/// names of terrain type words
static const std::map<Uint16, const char*> c_terrainTypeNames =
{
   // uw1
   { 0x0000, "Normal (solid) wall or floor" },
   { 0x0002, "Ankh mural (shrines)" },
   { 0x0003, "Stairs up" },
   { 0x0004, "Stairs down" },
   { 0x0005, "Pipe" },
   { 0x0006, "Grating" },
   { 0x0007, "Drain" },
   { 0x0008, "Chained-up princess" },
   { 0x0009, "Window" },
   { 0x000a, "Tapestry" },
   { 0x000b, "Textured door (used for the lock to the Key of Infinity)" },
   { 0x0010, "Water (not waterfall)" },
   { 0x0020, "Lava (not lavafall)" },

   // uw2
   { 0x0040, "Waterfall (wall texture)" },
   { 0x0048, "Swamp / poison" },
   { 0x0050, "Purple fluid, water" },
   { 0x0058, "Water" },
   { 0x0060, "Swamp / poison" },
   { 0x0080, "Lavafall (wall texture)" },
   { 0x00C0, "Ice wall" },
   { 0x00C8, "Ice wall, with hole" },
   { 0x00D8, "Ice wall, cracked" },
   { 0x00E8, "Ice wall, smooth" },
   { 0x00F8, "Ice floor, cracked" },
};

/// dumps terrain.dat, containing terrain data words
void DumpTerrainData(const std::string& filename, const GameStrings& gameStrings, bool isUw2)
{
   printf("terrain data dumping\n");

   Base::File file(filename.c_str(), Base::modeRead);
   if (!file.IsOpen())
   {
      printf("could not open file!\n");
      return;
   }

   Uint32 fileLength = file.FileLength();

   unsigned int numEntries = fileLength / 2;

   printf("file length: %04x bytes, %u entries\n",
      fileLength, numEntries);

   if ((!isUw2 && numEntries != 512) ||
      (isUw2 && numEntries != 256))
      printf("warning: expected %u entries for uw%u game!",
         isUw2 ? 256 : 512,
         isUw2 ? 2 : 1);

   for (unsigned int terrainIndex = 0; terrainIndex < numEntries; terrainIndex++)
   {
      Uint16 terrainDataWord = file.Read16();

      // in uw1, walls start at offset 0, and ceilings go down from 510
      size_t gameStringsIndex =
         isUw2
         ? (510 - terrainIndex)
         : (terrainIndex < 256 ? terrainIndex : 510 - (terrainIndex - 0x100));

      const char* terrainTypeName =
         c_terrainTypeNames.find(terrainDataWord) != c_terrainTypeNames.end()
         ? c_terrainTypeNames.find(terrainDataWord)->second
         : "???";

      printf("%s.tr index % 3u: type 0x%04x (%s), look desc \"%s\"",
         isUw2 ? "t64" : (terrainIndex < 256 ? "w64" : "f32"),
         isUw2 ? terrainIndex : (terrainIndex % 256),
         terrainDataWord,
         terrainTypeName,
         gameStrings.GetString(10, gameStringsIndex).c_str());

      printf("\n");
   }
}
