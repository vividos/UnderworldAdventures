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

int bytdecode_main()
{
#ifndef HAVE_UW2
   // uw1 doesn't have a byt.ark file
   return 0;
#endif

   // get 256 colors palette
   char palette[11][256 * 3];
   {
      FILE* pal = fopen(UWPATH"data\\pals.dat", "rb");

      fread(palette, 1, 256 * 3 * 11, pal);

      for (int palidx = 0; palidx < 11; palidx++)
      {
         // scale
         for (int j = 0; j < 256 * 3; j++)
            palette[palidx][j] <<= 2;

         // swap R and B
         for (int k = 0; k < 256; k++)
         {
            char h = palette[palidx][k * 3 + 0];
            palette[palidx][k * 3 + 0] = palette[palidx][k * 3 + 2];
            palette[palidx][k * 3 + 2] = h;
         }
      }

      fclose(pal);
   }

   FILE* fd = fopen(UWPATH"data\\byt.ark", "rb");

   if (fd == NULL)
   {
      printf("could not open file\n");
      return 0;
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

   return 0;
}
