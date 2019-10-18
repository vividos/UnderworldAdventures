//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2003,2004,2019 Underworld Adventures Team
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
/// \file DumpCombineData.cpp
/// \brief dumps combine data
//
#include "common.hpp"
#include "GameStrings.hpp"
#include "File.hpp"

/// dumps cmb.dat, containing item combine data
void DumpCombineData(const std::string& filename, const GameStrings& gameStrings, bool isUw2)
{
   UNUSED(isUw2);

   printf("combine data dumping\n");

   Base::File file(filename.c_str(), Base::modeRead);
   if (!file.IsOpen())
   {
      printf("could not open file!\n");
      return;
   }

   Uint32 fileLength = file.FileLength();

   unsigned int entries = fileLength / 6;

   printf("file length: %04x bytes, %u entries, %u bytes extra data\n",
      fileLength, entries, fileLength % 6);

   for (unsigned int i = 0; i < entries; i++)
   {
      Uint16 data[3];

      printf("[");
      for (unsigned int j = 0; j < 3; j++)
      {
         data[j] = file.Read16();

         printf("%04x%s", data[j], j == 2 ? "" : " ");
      }
      printf("] ");

      if (data[0] == 0 && data[1] == 0 && data[2] == 0)
      {
         printf("end of table marker\n");
         continue;
      }

      if (data[0] == 0Xffff && data[1] == 0xffff && data[2] == 0xffff)
      {
         printf("empty table entry\n");
         continue;
      }

      std::string item1 = gameStrings.GetString(4, data[0] & 0x01ff);
      std::string item2 = gameStrings.GetString(4, data[1] & 0x01ff);
      std::string newItem = gameStrings.GetString(4, data[2] & 0x01ff);

      printf("\"%s\" + \"%s\" = \"%s\"",
         item1.c_str(),
         item2.c_str(),
         newItem.c_str());

      bool item1Destroyed = (data[0] & 0x8000) != 0;
      bool item2Destroyed = (data[1] & 0x8000) != 0;

      if (item1Destroyed && item2Destroyed)
      {
         printf(" (both objects are destroyed)");
      }
      else if (item1Destroyed)
      {
         printf(" (\"%s\" is destroyed)", item1.c_str());
      }
      else if (item2Destroyed)
      {
         printf(" (\"%s\" is destroyed)", item2.c_str());
      }
      else
         printf(" (no object is destroyed?!?)");

      printf("\n");
   }
}
