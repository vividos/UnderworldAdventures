//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2022 Underworld Adventures Team
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
/// \file ImageLoader.cpp
/// \brief *.gr, *.byt files and palette loading
//
#include "pch.hpp"
#include "ImageLoader.hpp"
#include "IndexedImage.hpp"
#include "ResourceManager.hpp"
#include <algorithm>

using Import::ImageLoader;

/// decodes the underworld rle format, for word lengths up to 8 bit, and
/// stores the pixels in an array.
void ImageDecodeRLE(Base::File& file, Uint8* pixels, unsigned int bits,
   unsigned int datalen, unsigned int maxpix, unsigned char* auxPalettes,
   unsigned int padding = 0, unsigned int lineWidth = 0)
{
   unsigned int linecount = 0;
   unsigned int bufpos = 0;

   // bit extraction variables
   unsigned int bits_avail = 0;
   unsigned int rawbits = 0;
   unsigned int bitmask = ((1 << bits) - 1) << (8 - bits);
   unsigned int nibble;

   // rle decoding vars
   unsigned int pixcount = 0;
   unsigned int stage = 0; // we start in stage 0
   unsigned int count = 0;
   unsigned int record = 0; // we start with record 0=repeat (3=run)
   unsigned int repeatcount = 0;

   while (datalen > 0 && pixcount < maxpix)
   {
      // get new bits
      if (bits_avail < bits)
      {
         // not enough bits available
         if (bits_avail > 0)
         {
            nibble = ((rawbits & bitmask) >> (8 - bits_avail));
            nibble <<= (bits - bits_avail);
         }
         else
            nibble = 0;

         rawbits = (unsigned int)file.Read8();
         if ((int)rawbits == EOF)
            return;

         //printf("fgetc: %02x\n",rawbits);

         unsigned int shiftval = 8 - (bits - bits_avail);

         nibble |= (rawbits >> shiftval);

         rawbits = (rawbits << (8 - shiftval)) & 0xFF;

         bits_avail = shiftval;
      }
      else
      {
         // we still have enough bits
         nibble = (rawbits & bitmask) >> (8 - bits);
         bits_avail -= bits;
         rawbits <<= bits;
      }

      //printf("nibble: %02x\n",nibble);

      // now that we have a nibble
      datalen--;

      switch (stage)
      {
      case 0: // we retrieve a new count
         if (nibble == 0)
            stage++;
         else
         {
            count = nibble;
            stage = 6;
         }
         break;

      case 1:
         count = nibble;
         stage++;
         break;

      case 2:
         count = (count << 4) | nibble;
         if (count == 0)
            stage++;
         else
            stage = 6;
         break;

      case 3:
      case 4:
      case 5:
         count = (count << 4) | nibble;
         stage++;
         break;
      }

      if (stage < 6)
         continue;

      switch (record)
      {
      case 0:
         // repeat record stage 1
         //printf("repeat: new count: %x\n",count);

         if (count == 1)
         {
            record = 3; // skip this record; a run follows
            break;
         }

         if (count == 2)
         {
            record = 2; // multiple run records
            break;
         }

         record = 1; // read next nibble; it's the color to repeat
         continue;

      case 1:
         // repeat record stage 2

      {
         // repeat 'nibble' color 'count' times
         for (unsigned int n = 0; n < count; n++)
         {
            pixels[bufpos++] = auxPalettes[nibble];

            // add padding space when needed
            if (padding != 0 && ++linecount >= lineWidth)
            {
               linecount = 0;
               bufpos += padding;
            }

            if (++pixcount >= maxpix)
               break;
         }
      }

      //printf("repeat: wrote %x times a '%x'\n",count,nibble);

      if (repeatcount == 0)
      {
         record = 3; // next one is a run record
      }
      else
      {
         repeatcount--;
         record = 0; // continue with repeat records
      }
      break;

      case 2:
         // multiple repeat stage

         // 'count' specifies the number of repeat record to appear
         //printf("multiple repeat: %u\n",count);
         repeatcount = count - 1;
         record = 0;
         break;

      case 3:
         // run record stage 1
         // copy 'count' nibbles

         //printf("run: count: %x\n",count);

         record = 4; // retrieve next nibble
         continue;

      case 4:
         // run record stage 2

         // now we have a nibble to write
         pixels[bufpos++] = auxPalettes[nibble];
         pixcount++;

         // add padding space when needed
         if (padding != 0 && ++linecount >= lineWidth)
         {
            linecount = 0;
            bufpos += padding;
         }

         if (--count == 0)
         {
            //printf("run: finished\n");
            record = 0; // next one is a repeat again
         }
         else
            continue;
         break;
      }

      stage = 0;
      // end of while loop
   }
}

void ImageLoader::LoadPalettes(Palette256Ptr allPalettes[8])
{
   const char* allPalettesName = "data/pals.dat";
   Base::File file = m_resourceManager.GetUnderworldFile(Base::resourceGameUw, allPalettesName);
   if (!file.IsOpen())
   {
      std::string text("could not open palette file ");
      text.append(allPalettesName);
      throw Base::Exception(text.c_str());
   }

   // palettes are stored in ancient vga color format
   for (unsigned int pal = 0; pal < 8; pal++)
   {
      allPalettes[pal] = std::make_shared<Palette256>();

      Palette256& palette = *allPalettes[pal];

      for (unsigned int color = 0; color < 256; color++)
      {
         // palette is in RGB format, and uses only 6 lower bits
         palette.Set(static_cast<Uint8>(color), 0, file.Read8() << 2);
         palette.Set(static_cast<Uint8>(color), 1, file.Read8() << 2);
         palette.Set(static_cast<Uint8>(color), 2, file.Read8() << 2);
         palette.Set(static_cast<Uint8>(color), 3, color == 0 ? 0 : 255);
      }
   }
}

void ImageLoader::LoadAuxPalettes(Uint8 allAuxPalettes[32][16])
{
   const char* auxPalettesName = "data/allpals.dat";

   Base::File file = m_resourceManager.GetUnderworldFile(Base::resourceGameUw, auxPalettesName);
   if (!file.IsOpen())
      throw Base::Exception("could not open file allpals.dat");

   // the C++ standard guarantees that the [32][16] array is contiguous
   Uint8* buffer = &allAuxPalettes[0][0];
   file.ReadBuffer(buffer, 32 * 16);
}

void ImageLoader::LoadImageGr(IndexedImage& image, const char* imageName,
   unsigned int imageNumber, Uint8 allAuxPalettes[32][16])
{
   Base::File file = m_resourceManager.GetUnderworldFile(Base::resourceGameUw, imageName);
   if (!file.IsOpen())
   {
      std::string text("could not open image: ");
      text.append(imageName);
      throw Base::Exception(text.c_str());
   }

   unsigned long fileLength = file.FileLength();

   // read in toc
   Uint8 id = file.Read8(); // always 1 for .gr files
   UaAssert(id == 1);
   UNUSED(id);

   Uint16 entries = file.Read16();

   if (imageNumber >= entries)
      imageNumber = 0; // image number not valid

   // seek to offset
   file.Seek(4 * imageNumber, Base::seekCurrent);
   Uint32 offset = file.Read32();

   if (offset >= fileLength)
      return;

   file.Seek(offset, Base::seekBegin);

   // load image into pixel vector
   bool isSpecialPanelsGr = (strstr("panels.gr", imageName) != NULL);
   bool isUw2 = m_resourceManager.IsUnderworldFileAvailable("data/scd.ark");

   LoadImageGrImpl(image, file, allAuxPalettes, isSpecialPanelsGr, isUw2);
}

void ImageLoader::LoadImageByt(const char* imageName, Uint8* pixels)
{
   Base::File file = m_resourceManager.GetUnderworldFile(Base::resourceGameUw, imageName);
   if (!file.IsOpen())
   {
      std::string text("could not open raw image: ");
      text.append(imageName);
      throw Base::Exception(text.c_str());
   }

   // raw image
   file.ReadBuffer(&pixels[0], 320 * 200);
}

void ImageLoader::LoadImageGrList(std::vector<IndexedImage>& imageList,
   const char* imageName, unsigned int imageFrom, unsigned int imageTo,
   Uint8 allAuxPalettes[32][16])
{
   Base::File file = m_resourceManager.GetUnderworldFile(Base::resourceGameUw, imageName);
   if (!file.IsOpen())
   {
      std::string text("could not open image list: ");
      text.append(imageName);
      throw Base::Exception(text.c_str());
   }

   unsigned long fileLength = file.FileLength();

   // read in toc
   Uint8 id = file.Read8(); // always 1
   UaAssert(id == 1);
   UNUSED(id);

   Uint16 entries = file.Read16();

   if (imageTo == 0)
      imageTo = entries;

   if (imageFrom >= entries || imageTo < imageFrom)
   {
      return;
   }

   // read in all offsets
   std::vector<Uint32> offsets;
   offsets.resize(entries, 0);

   for (Uint16 i = 0; i < entries; i++)
      offsets[i] = file.Read32();

   bool isSpecialPanelsGr = (strstr(imageName, "panels.gr") != NULL);
   bool isUw2 = m_resourceManager.IsUnderworldFileAvailable("data/scd.ark");

   for (unsigned int j = imageFrom; j < imageTo; j++)
   {
      if (offsets[j] >= fileLength)
         continue;

      file.Seek(offsets[j], Base::seekBegin);

      IndexedImage image;
      imageList.push_back(image);

      IndexedImage& lastImage = imageList.back();

      // load image
      LoadImageGrImpl(lastImage, file, allAuxPalettes, isSpecialPanelsGr, isUw2);
   }
}

void ImageLoader::LoadImageGrImpl(IndexedImage& img, Base::File& file,
   Uint8 auxPalettes[32][16], bool isSpecialPanelsGr, bool isUw2)
{
   Uint8 type, width, height, auxpal = 0;
   Uint16 datalen;

   if (isSpecialPanelsGr)
   {
      // special case for "panels.gr" that lacks a normal header
      type = 4; // 8-bit uncompressed
      width = !isUw2 ? 83 : 79;
      height = !isUw2 ? 114 : 112;
      datalen = width * height;
   }
   else
   {
      // read in image header
      type = file.Read8();
      width = file.Read8();
      height = file.Read8();

      // read in auxpal, when needed
      if (type == 0x08 || type == 0x0a)
         auxpal = file.Read8();

      if (auxpal > 0x1f)
         auxpal = 0;

      // read in data length
      datalen = file.Read16();
   }

   img.Create(width, height);

   switch (type)
   {
   case 0x04: // 8-bit uncompressed
   {
      unsigned int pixcount = 0;

      while (datalen > 0)
      {
         Uint16 size = std::min<Uint16>(datalen, 1024);

         file.ReadBuffer(&img.GetPixels()[pixcount], size);

         datalen -= size;
         pixcount += size;
      }
   }
   break;

   case 0x08: // 4-bit rle compressed
      ImageDecodeRLE(file, &img.GetPixels()[0], 4, datalen, width * height, auxPalettes[auxpal], 0, 0);
      break;

   case 0x0a: // 4-bit uncompressed
   {
      unsigned int pixcount = 0, maxpix = width * height;
      Uint8* pal = auxPalettes[auxpal];
      Uint8 rawbyte;
      Uint8* pixels = &img.GetPixels()[0];

      while (datalen > 0)
      {
         rawbyte = file.Read8();
         pixels[pixcount++] = pal[rawbyte >> 4];

         if (pixcount >= maxpix)
            break;

         pixels[pixcount++] = pal[rawbyte & 0x0f];
         datalen--;
      }
   }
   break;
   }
}
