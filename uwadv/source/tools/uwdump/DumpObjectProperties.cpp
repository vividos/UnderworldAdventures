//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2003,2004,2019,2021 Underworld Adventures Team
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

void DumpCommonObjectProperties(const std::string& filename, const GameStrings& gameStrings, bool isUw2)
{
   UNUSED(isUw2);

   printf("common object properties dumping\n");

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

   Uint16 magicWord = file.Read16();

   printf("file length: %04x bytes, magic word: %04x\n",
      filelen, magicWord);

   printf("\nmelee weapons\n");
   for (size_t index = 0; index < 16; index++)
   {
      Uint8 data[8];
      file.ReadBuffer(data, sizeof(data));

      printf("item_id=%04x ", index);
      printf("slash=%02x ", data[0]);
      printf("bash=%02x ", data[1]);
      printf("stab=%02x ", data[2]);

      printf("unknown=%02x %02x %02x ", data[3], data[4], data[5]);

      const char* skill;
      switch (data[6])
      {
      case 3: skill = "sword"; break;
      case 4: skill = "axe"; break;
      case 5: skill = "mace"; break;
      case 6: skill = "unarmed"; break;
      default: skill = "unknown"; break;
      }
      printf("skill=(%02x) % 7s ", data[6], skill);

      printf("durability=%02x ", data[7]);

      printf("name=%s\n", gameStrings.GetString(4, index).c_str());
   }

   printf("\nranged weapons\n");
   for (size_t index = 0; index < 16; index++)
   {
      Uint8 data[3];
      file.ReadBuffer(data, sizeof(data));

      printf("item_id=%04x ", index + 0x0010);

      Uint16 word = data[0] | (data[1] << 8);
      printf("unknown=%04x ", word);

      printf("durability=%02x ", data[2]);

      printf("name=%s\n", gameStrings.GetString(4, index + 0x0010).c_str());
   }

   printf("\narmour and wearables\n");
   for (size_t index = 0; index < 32; index++)
   {
      Uint8 data[4];
      file.ReadBuffer(data, sizeof(data));

      printf("item_id=%04x ", index + 0x0020);

      printf("protection=%02x ", data[0]);
      printf("durability=%02x ", data[1]);
      printf("unknown=%02x ", data[2]);

      const char* category;
      switch (data[3])
      {
      case 0: category = "shield/none?"; break;
      case 1: category = "body armour"; break;
      case 3: category = "leggings"; break;
      case 4: category = "gloves"; break;
      case 5: category = "boots"; break;
      case 8: category = "hat"; break;
      case 9: category = "ring"; break;
      default: category = "unknown"; break;
      }
      printf("category=(%02x) %- 11s ", data[3], category);

      printf("name=%s\n", gameStrings.GetString(4, index + 0x0020).c_str());
   }

   printf("\ncritters\n");
   for (size_t index = 0; index < 64; index++)
   {
      Uint8 data[48];
      file.ReadBuffer(data, sizeof(data));

      printf("item_id=%04x ", index + 0x0040);

      printf("npc_level=%02x ", data[0]);

      printf("name=%s\n", gameStrings.GetString(4, index + 0x0040).c_str());
   }

   printf("\ncontainer\n");
   for (size_t index = 0; index < 16; index++)
   {
      printf("item_id=%04x ", index + 0x0080);

      Uint8 data[3];
      file.ReadBuffer(data, sizeof(data));

      printf("capacity=%02x ", data[0]);

      const char* type;
      switch (data[1])
      {
      case 0: type = "runes"; break;
      case 1: type = "arrows"; break;
      case 2: type = "scrolls"; break;
      case 3: type = "edibles"; break;
      case 4: type = "keys"; break;
      case 0xff: type = "any"; break;
      default: type = "unknown"; break;
      }
      printf("objects=(%02x) %- 7s ", data[1], type);

      printf("num_slots=%02x ", data[2]);

      printf("name=%s\n", gameStrings.GetString(4, index + 0x0080).c_str());
   }

   printf("\nlight sources\n");
   for (size_t index = 0; index < 16; index++)
   {
      printf("item_id=%04x ", index + 0x0090);

      Uint8 brightness = file.Read8();
      Uint8 duration = file.Read8();

      printf("brightness=%02x ", brightness);
      printf("duration=%02x ", duration);

      printf("name=%s\n", gameStrings.GetString(4, index + 0x0090).c_str());
   }

   printf("\nanimation objects\n");
   for (size_t index = 0; index < 16; index++)
   {
      printf("item_id=%04x ", index + 0x01c0);

      Uint8 value = file.Read8();
      printf("value=%02x ", value);

      printf("name=%s\n", gameStrings.GetString(4, index + 0x01c0).c_str());
   }
}
