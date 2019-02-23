//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2019 Michael Fink
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
// uw1 savegame decoding
//
#include "hacking.h"

//static const char *fname = UWPATH "Save1\\player.dat";
static const char *fname = UWPATH "data\\player.dat";
//static const char *fname = "D:\\uwadv\\uw1\\SAVE4\\player.dat";

const char* statsnskills[23] =
{
   "Strength      ",
   "Dexterity     ",
   "Intelligence  ",
   "Attack        ",
   "Defense       ",
   "Unarmed       ",
   "Sword         ",
   "Axe           ",
   "Mace          ",
   "Missile       ",
   "Mana          ",
   "Lore          ",
   "Casting       ",
   "Traps         ",
   "Search        ",
   "Track         ",
   "Sneak         ",
   "Repair        ",
   "Charm         ",
   "Picklock      ",
   "Acrobat       ",
   "Appraise      ",
   "Swimming      "
};


int sgdecode_main()
{
   FILE* fd = fopen(fname, "rb");

   if (fd == NULL)
   {
      printf("could not open file %s\n", fname);
      return 1;
   }

   bool isEncrypted = strstr(fname, "data\\player.dat") == NULL;

   unsigned char xorvalue = isEncrypted ? fgetc(fd) : 0;

   // read in data
   unsigned char data[311];
   fread(data, 1, 311, fd);

   // descramble data
   if (isEncrypted)
   {
      unsigned char incrnum = 3;

      for (int i = 0; i <= 219; i++)
      {
         if (i == 80 || i == 160) incrnum = 3;
         data[i] ^= (xorvalue + incrnum);
         incrnum += 3;
      }
   }

   // print out character info
   printf("name: %s\n", data);
   printf("vitality: %u out of %u\n", data[0x0035], data[0x0036]);
   printf("mana: %u out of %u\n", data[0x0037], data[0x0038]);
   printf("food: %u, play_level %u\n", data[0x0039], data[0x003D]);

   unsigned int exp_points = data[0x004E] | (unsigned(data[0x004F]) << 8) |
      (unsigned(data[0x0050]) << 16) | (unsigned(data[0x0051]) << 24);
   printf("exp: %4.1f\n", exp_points / 10.f);

   int weight = data[0x004A] | (unsigned(data[0x004B]) << 8);
   printf("weight: %2.1f\n", weight / 10.f);

   printf("position: x=%04x, y=%04x, z=%04x, level=%u\n",
      (unsigned int)(data[0x0055] | Uint16(data[0x0056] << 8)),
      (unsigned int)(data[0x0057] | Uint16(data[0x0058] << 8)),
      (unsigned int)(data[0x0059] | Uint16(data[0x005A] << 8)),
      (unsigned int)(data[0x005C]));

   printf("\nstats & skills:\n");
   for (int n = 0; n < 23; n++)
   {
      printf("%s: %u\t", statsnskills[n], data[n + 0x001E]);
      if (++n < 23)
         printf("%s: %u\t", statsnskills[n], data[n + 0x001E]);
      if (++n < 23)
         printf("%s: %u\n", statsnskills[n], data[n + 0x001E]);
   }
   printf("\n");

   // dump inventory object list
   fseek(fd, SEEK_SET, 0x0137);

   unsigned short obdata[4];

   unsigned int pos = 1;
   do
   {
      size_t len = fread(obdata, 1, 8, fd);
      if (len == 0 || feof(fd))
         break;

      // dump object
      printf("%04x: item_id=%04x flags=%x is_quant=%u "
         "quality=%02x next=%03x owner=%02x sp_link=%03x\n"
         ,
         pos, (obdata[0] & 0x1ff),
         ((obdata[0] >> 9) & 0xf),
         (obdata[0] >> 15), // is_quant


         obdata[2] & 63, // quality
         (obdata[2] >> 6), // next

         obdata[3] & 63, // owner
         (obdata[3] >> 6)  // special link
      );

      pos++;

   } while (!feof(fd));


   fclose(fd);

   return 0;
}

// one inventory item:
// 0001: item_id=00cf flags=0 is_quant=0 quality=28 next=000 owner=00 sp_link=000

// two inventory items:
// 0001: item_id=00cf flags=0 is_quant=0 quality=28 next=002 owner=00 sp_link=000
// 0002: item_id=00c8 flags=0 is_quant=0 quality=28 next=000 owner=00 sp_link=000

// three inventory items, 3rd is container:
// 0001: item_id=00cf flags=0 is_quant=0 quality=28 next=002 owner=00 sp_link=000
// 0002: item_id=00c8 flags=0 is_quant=0 quality=28 next=003 owner=00 sp_link=000
// 0003: item_id=0080 flags=0 is_quant=0 quality=28 next=000 owner=00 sp_link=004
// 0004: item_id=0003 flags=0 is_quant=0 quality=0f next=005 owner=00 sp_link=000
// 0005: item_id=0091 flags=0 is_quant=1 quality=28 next=006 owner=00 sp_link=001
// 0006: item_id=013b flags=0 is_quant=1 quality=28 next=007 owner=00 sp_link=200
// 0007: item_id=00b6 flags=0 is_quant=0 quality=28 next=008 owner=00 sp_link=000
// 0008: item_id=00b1 flags=0 is_quant=0 quality=28 next=009 owner=00 sp_link=000
// 0009: item_id=00b3 flags=0 is_quant=0 quality=28 next=000 owner=00 sp_link=000

// complex inventory:
// 0001: item_id=013b flags=0 is_quant=1 quality=28 next=002 owner=00 sp_link=200
// 0002: item_id=0080 flags=0 is_quant=0 quality=28 next=006 owner=00 sp_link=003
// 0003: item_id=001a flags=0 is_quant=0 quality=28 next=004 owner=00 sp_link=000
// 0004: item_id=0099 flags=0 is_quant=0 quality=28 next=000 owner=00 sp_link=005
// 0005: item_id=0120 flags=c is_quant=1 quality=0a next=000 owner=00 sp_link=231
// 0006: item_id=0121 flags=0 is_quant=1 quality=28 next=007 owner=00 sp_link=001
// 0007: item_id=0026 flags=0 is_quant=0 quality=3d next=008 owner=00 sp_link=000
// 0008: item_id=002d flags=0 is_quant=0 quality=23 next=009 owner=00 sp_link=000
// 0009: item_id=0021 flags=0 is_quant=0 quality=3f next=00a owner=00 sp_link=000
// 000a: item_id=0082 flags=0 is_quant=0 quality=28 next=019 owner=00 sp_link=00b
// 000b: item_id=0106 flags=0 is_quant=0 quality=28 next=00c owner=01 sp_link=000
// 000c: item_id=0102 flags=0 is_quant=1 quality=28 next=00d owner=02 sp_link=001
// 000d: item_id=0109 flags=0 is_quant=1 quality=28 next=00e owner=05 sp_link=001
// 000e: item_id=010b flags=0 is_quant=1 quality=28 next=00f owner=06 sp_link=001
// 000f: item_id=0100 flags=0 is_quant=1 quality=28 next=010 owner=08 sp_link=001
// 0010: item_id=0103 flags=0 is_quant=1 quality=28 next=011 owner=09 sp_link=001
// 0011: item_id=0107 flags=0 is_quant=1 quality=28 next=012 owner=0b sp_link=001
// 0012: item_id=0101 flags=0 is_quant=1 quality=28 next=013 owner=00 sp_link=001
// 0013: item_id=0091 flags=0 is_quant=1 quality=28 next=014 owner=00 sp_link=006
// 0014: item_id=012d flags=0 is_quant=1 quality=28 next=015 owner=00 sp_link=005
// 0015: item_id=0128 flags=0 is_quant=1 quality=28 next=016 owner=00 sp_link=001
// 0016: item_id=0139 flags=c is_quant=1 quality=28 next=017 owner=00 sp_link=20e
// 0017: item_id=00d8 flags=0 is_quant=1 quality=28 next=018 owner=00 sp_link=001
// 0018: item_id=00bc flags=c is_quant=1 quality=28 next=000 owner=00 sp_link=224
// 0019: item_id=008f flags=0 is_quant=0 quality=28 next=01a owner=00 sp_link=000
// 001a: item_id=0024 flags=0 is_quant=0 quality=3e next=01b owner=00 sp_link=000
// 001b: item_id=002a flags=0 is_quant=0 quality=1b next=01c owner=00 sp_link=000
// 001c: item_id=0082 flags=0 is_quant=0 quality=28 next=023 owner=00 sp_link=01d
// 001d: item_id=00ba flags=0 is_quant=1 quality=28 next=01e owner=00 sp_link=001
// 001e: item_id=00b0 flags=0 is_quant=1 quality=28 next=01f owner=00 sp_link=002
// 001f: item_id=00b4 flags=0 is_quant=1 quality=28 next=020 owner=00 sp_link=005
// 0020: item_id=00b1 flags=0 is_quant=1 quality=28 next=021 owner=00 sp_link=002
// 0021: item_id=00b6 flags=0 is_quant=1 quality=28 next=022 owner=00 sp_link=001
// 0022: item_id=00b5 flags=0 is_quant=1 quality=28 next=000 owner=00 sp_link=001
// 0022: item_id=00b5 flags=0 is_quant=1 quality=28 next=000 owner=00 sp_link=001
// 0023: item_id=0080 flags=0 is_quant=0 quality=28 next=030 owner=00 sp_link=024
// 0024: item_id=0113 flags=0 is_quant=1 quality=28 next=025 owner=00 sp_link=001
// 0025: item_id=008e flags=0 is_quant=0 quality=28 next=026 owner=00 sp_link=000
// 0026: item_id=013e flags=0 is_quant=1 quality=28 next=027 owner=00 sp_link=301
// 0027: item_id=011c flags=0 is_quant=1 quality=28 next=028 owner=00 sp_link=001
// 0028: item_id=00b8 flags=0 is_quant=1 quality=28 next=029 owner=00 sp_link=001
// 0029: item_id=00be flags=0 is_quant=1 quality=28 next=02a owner=00 sp_link=001
// 002a: item_id=00a0 flags=0 is_quant=1 quality=28 next=02b owner=00 sp_link=017
// 002b: item_id=00a1 flags=0 is_quant=1 quality=28 next=02c owner=00 sp_link=006
// 002c: item_id=0132 flags=0 is_quant=1 quality=28 next=02d owner=00 sp_link=242
// 002d: item_id=00a6 flags=0 is_quant=1 quality=28 next=02e owner=00 sp_link=002
// 002e: item_id=011e flags=0 is_quant=1 quality=28 next=02f owner=00 sp_link=001
// 002f: item_id=0118 flags=0 is_quant=0 quality=28 next=000 owner=00 sp_link=000
// 0030: item_id=008d flags=0 is_quant=0 quality=28 next=031 owner=00 sp_link=000
// 0031: item_id=003a flags=8 is_quant=1 quality=28 next=032 owner=00 sp_link=222
// 0032: item_id=0084 flags=0 is_quant=0 quality=28 next=036 owner=00 sp_link=033
// 0033: item_id=00a1 flags=0 is_quant=1 quality=28 next=034 owner=00 sp_link=00f
// 0034: item_id=00a2 flags=0 is_quant=1 quality=19 next=035 owner=00 sp_link=001
// 0035: item_id=00a0 flags=0 is_quant=1 quality=28 next=000 owner=00 sp_link=00b
// 0036: item_id=0006 flags=0 is_quant=0 quality=33 next=037 owner=00 sp_link=000
// 0037: item_id=009b flags=0 is_quant=0 quality=28 next=039 owner=00 sp_link=038
// 0038: item_id=0120 flags=c is_quant=1 quality=06 next=000 owner=00 sp_link=202
// 0039: item_id=0094 flags=0 is_quant=1 quality=16 next=000 owner=00 sp_link=001
