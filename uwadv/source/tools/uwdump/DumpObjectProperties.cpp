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
/// \file DumpObjectProperties.cpp
/// \brief object properties decoding implementation
//
#include "common.hpp"
#include "GameStrings.hpp"
#include "File.hpp"
#include <algorithm>
#include <bitset>

void DumpObjectProperties(const std::string& filename, const GameStrings& gameStrings, bool isUw2)
{
   UNUSED(isUw2);

   printf("object properties dumping\n");

   Base::File file(filename.c_str(), Base::modeRead);
   if (!file.IsOpen())
   {
      printf("could not open file!\n");
      return;
   }

   Uint32 filelen = file.FileLength();

   unsigned int entries = (filelen - 2) / 11;

   printf("file length: %04x bytes, %u entries, %u bytes extra data\n",
      filelen, entries, (filelen - 2) % 11);

   {
      Uint8 start1 = file.Read8();
      Uint8 start2 = file.Read8();

      printf("start bytes: %02x %02x\n\n", start1, start2);
   }

   for (unsigned int i = 0; i < entries; i++)
   {
      Uint8 data[11];

      printf("%04x: [", i);

      for (unsigned int j = 0; j < 11; j++)
      {
         data[j] = file.Read8();

         if (j == 1 || j == 4)
         {
            data[++j] = file.Read8();
            printf("%04x ", data[j - 1] | (data[j] << 8));
         }
         else
            printf("%02x%c", data[j], j == 10 ? ']' : ' ');
      }

      Uint16 word1 = data[1] | (data[2] << 8);
      printf(" [height=%02x ", data[0]);
      printf("radius=%02x ", word1 & 7);
      printf("unk1=%01x ", (word1 >> 3) & 1);
      printf("mass=%03x] ", word1 >> 4);

      printf("pick_up=%01x ", (data[3] >> 5) & 1);

      Uint16 word4 = data[4] | (data[5] << 8);
      printf("value4=%04x ", word4);

      printf("q_class=%01x ", (data[6] >> 2) & 3);
      printf("q_type=%01x ", data[10] & 15);

      printf("belong_to=%01x ", data[7] >> 7);
      printf("type=%01x ", (data[7] >> 1) & 15);
      printf("look_at=%01x ", (data[10] >> 4) & 1);

      printf("name=%s\n", gameStrings.GetString(4, i).c_str());
   }
}
