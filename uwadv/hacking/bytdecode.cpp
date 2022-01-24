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
// byt.ark decoding
//
#include "hacking.h"
#include <algorithm>

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
   { "lback000.byt", 0 },
   { "lback001.byt", 0 },
   { "lback002.byt", 0 },
   { "lback003.byt", 0 },
   { "lback004.byt", 0 },
   { "lback005.byt", 0 },
   { "lback006.byt", 0 },
   { "lback007.byt", 0 },
};

static std::map<unsigned short, unsigned int> g_entryIndexToPaletteMap =
{
   { 0, 1 },
   { 1, 0 },
   { 2, 0 },
   { 3, 0 }, // unused
   { 4, 0 },
   { 5, 0 },
   { 6, 5 },
   { 7, 5 },
   { 8, 0 },
   { 9, 0 },
   { 10, 0 }, // unused
};

bool uw2_uncompress(const unsigned char* compressedData, size_t compressedSize,
   unsigned char* uncompressedData, size_t uncompressedSize)
{
   Uint32 destCount = 0;

   if (uncompressedData == NULL)
      uncompressedSize = 1; // when not passing a buffer, just set dest size to 1 to keep loop running

   // compressed data
   Uint8* ubuf = uncompressedData;
   const Uint8* cp = compressedData + 4; // for some reason the first 4 bytes are not used
   const Uint8* ce = compressedData + compressedSize;
   Uint8* up = ubuf;
   Uint8* ue = ubuf + uncompressedSize;

   while (up < ue && cp < ce)
   {
      unsigned char bits = *cp++;

      for (int i = 0; i < 8; i++, bits >>= 1)
      {
         if (bits & 1)
         {
            if (up != NULL)
               *up++ = *cp++;
            else
               cp++;

            destCount++;
         }
         else
         {
            signed int m1 = *cp++; // m1: pos
            signed int m2 = *cp++; // m2: run

            m1 |= (m2 & 0xF0) << 4;

            // correct for sign bit
            if (m1 & 0x800)
               m1 |= 0xFFFFF000;

            // add offsets
            m2 = (m2 & 0x0F) + 3;
            m1 += 18;

            destCount += m2; // add count

            if (up != NULL)
            {
               if (m1 > up - ubuf)
                  return false; // TODO should not happen

               // adjust pos to current 4k segment
               while (m1 < (up - ubuf - 0x1000))
                  m1 += 0x1000;

               while (m2-- && up < ue)
                  *up++ = ubuf[m1++];
            }
         }

         // stop if source or destination buffer pointer overrun
         if (up >= ue || cp >= ce)
            break;
      }
   }

   return true;
}

void load_palettes(char* allPalettes, unsigned int numPalettes)
{
   FILE* pal = fopen(UWPATH"data\\pals.dat", "rb");

   fread(allPalettes, 1, numPalettes * 256 * 3, pal);

   for (unsigned int palidx = 0; palidx < numPalettes; palidx++)
   {
      char* palette = &allPalettes[palidx * 256 * 3];
      // scale
      for (int j = 0; j < 256 * 3; j++)
         palette[j] <<= 2;

      // swap R and B
      for (int k = 0; k < 256; k++)
      {
         char h = palette[k * 3 + 0];
         palette[k * 3 + 0] = palette[k * 3 + 2];
         palette[k * 3 + 2] = h;
      }
   }

   fclose(pal);
}

void decode_byt_ark_uw2()
{
   // get 256 colors palette
   char palette[11][256 * 3];
   load_palettes(&palette[0][0], 11);

   FILE* fd = fopen(UWPATH"data\\byt.ark", "rb");

   if (fd == NULL)
   {
      printf("could not open file\n");
      return;
   }

   fseek(fd, 0, SEEK_END);
   unsigned int flen = ftell(fd);
   fseek(fd, 0, SEEK_SET);

   // read in toc

   unsigned short entries = fread16(fd);
   fread32(fd);

   printf("byt.ark: %u entries\n", entries);

   // read all offset
   std::vector<unsigned int> offsets;
   offsets.resize(entries);
   fread(offsets.data(), sizeof(unsigned int), entries, fd);

   unsigned char* entryFlags = new unsigned char[entries];
   fread(entryFlags, sizeof(unsigned char), entries, fd);

   std::vector<unsigned int> dataSizes;
   dataSizes.resize(entries);
   fread(dataSizes.data(), sizeof(unsigned int), entries, fd);

   for (unsigned short offsetIndex = 0; offsetIndex < entries; offsetIndex++)
   {
      if (offsets[offsetIndex] == 0)
      {
         printf("skipping image at index %i", offsetIndex);
         continue;
      }

      fseek(fd, offsets[offsetIndex], SEEK_SET);

      unsigned int compressedSize = offsetIndex < entries - 1
         ? offsets[offsetIndex + 1] - offsets[offsetIndex]
         : flen - offsets[offsetIndex];

      unsigned int width = 320;
      unsigned int height = 200;

      std::vector<unsigned char> compressedData;
      compressedData.resize(compressedSize);
      fread(compressedData.data(), sizeof(unsigned char), compressedSize, fd);

      std::vector<unsigned char> bitmap;
      bitmap.resize(width * height);

      uw2_uncompress(compressedData.data(), compressedData.size(),
         bitmap.data(), bitmap.size());

      unsigned int paletteIndex = g_entryIndexToPaletteMap[offsetIndex];

      char fname[256];
      sprintf(fname, "byt-ark-image%02u.tga", offsetIndex);

      FILE* tga = fopen(fname, "wb");

      tga_writeheader(tga, width, height, 1, 1);

      // write palette
      fwrite(palette[paletteIndex], 1, 256 * 3, tga);

      fwrite(bitmap.data(), 1, width * height, tga);

      fclose(tga);

   } // end for

   fclose(fd);
}

void save_image(const char* prefix, const char* path, const char* filename, char* allPalettes)
{
   FILE* fd = fopen(path, "rb");

   if (fd == NULL)
   {
      printf("could not open file\n");
      return;
   }

   fseek(fd, 0, SEEK_END);
   unsigned long flen = ftell(fd);
   fseek(fd, 0, SEEK_SET);

   std::vector<unsigned char> bitmap;
   bitmap.resize(flen);
   fread(bitmap.data(), sizeof(unsigned char), flen, fd);

   fclose(fd);

   if (flen != 64000)
   {
      printf("%s: invalid size; %u\n", filename, flen);
      return;
   }

   std::string lowerFilename{ filename };
   std::transform(lowerFilename.begin(), lowerFilename.end(), lowerFilename.begin(), ::tolower);

   unsigned int width = 320;
   unsigned int height = 200;

   unsigned int paletteIndex = g_filenameToPaletteIndexMap[lowerFilename];

   char tga_fname[256];
   sprintf(tga_fname, "%s-%s.tga", prefix, lowerFilename.c_str());

   FILE* tga = fopen(tga_fname, "wb");

   tga_writeheader(tga, width, height, 1, 1);

   // write palette
   fwrite(allPalettes + paletteIndex * 256 * 3, 1, 256 * 3, tga);

   fwrite(bitmap.data(), 1, width * height, tga);

   fclose(tga);
}

void decode_lback_byt_uw2()
{
   // get 256 colors palette
   char palette[11][256 * 3];
   load_palettes(&palette[0][0], 11);

   _finddata_t find;
   long hnd = _findfirst(UWPATH"cuts\\lback00?.byt", &find);

   if (hnd == -1)
   {
      printf("no files found!\n");
      return;
   }

   do
   {
      char fname[256];
      sprintf(fname, UWPATH"cuts\\%s", find.name);

      save_image("cuts", fname, find.name, &palette[0][0]);

   } while (0 == _findnext(hnd, &find));

   _findclose(hnd);
}

void decode_data_byt_uw1()
{
   // get 256 colors palette
   char palette[8][256 * 3];
   load_palettes(&palette[0][0], 8);

   _finddata_t find;
   long hnd = _findfirst(UWPATH"data\\*.byt", &find);

   if (hnd == -1)
   {
      printf("no files found!\n");
      return;
   }

   do
   {
      char fname[256];
      sprintf(fname, UWPATH"data\\%s", find.name);

      save_image("data", fname, find.name, &palette[0][0]);

   } while (0 == _findnext(hnd, &find));

   _findclose(hnd);
}

int bytdecode_main()
{
#ifndef HAVE_UW2
   decode_data_byt_uw1();
#else
   decode_byt_ark_uw2();
   decode_lback_byt_uw2();
#endif

   return 0;
}
