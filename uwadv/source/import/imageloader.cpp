//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
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
/// \file imageloader.cpp
/// \brief *.gr, *.byt files and palette loading
//
#include "import.hpp"
#include "imageloader.hpp"
#include "indexedimage.hpp"
#include <algorithm>

using Import::ImageLoader;

// decodes the underworld rle format, for word lengths up to 8 bit, and
// stores the pixels in an array.
void ImageDecodeRLE(Base::File& file, Uint8* pixels, unsigned int bits,
   unsigned int datalen, unsigned int maxpix, unsigned char *auxpalidx,
   unsigned int padding = 0, unsigned int linewidth = 0)
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

      if (stage < 6) continue;

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
            pixels[bufpos++] = auxpalidx[nibble];

            // add padding space when needed
            if (padding != 0 && ++linecount >= linewidth)
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
         pixels[bufpos++] = auxpalidx[nibble];
         pixcount++;

         // add padding space when needed
         if (padding != 0 && ++linecount >= linewidth)
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

void ImageLoader::LoadPalettes(const char* allpalname,
   Palette256Ptr allpalettes[8])
{
   Base::File file(allpalname, Base::modeRead);
   if (!file.IsOpen())
   {
      std::string text("could not open palette file ");
      text.append(allpalname);
      throw Base::Exception(text.c_str());
   }

   // palettes are stored in ancient vga color format
   for (unsigned int pal = 0; pal < 8; pal++)
   {
      allpalettes[pal] = std::make_shared<Palette256>();

      Palette256& palette = *allpalettes[pal];

      for (unsigned int color = 0; color < 256; color++)
      {
         palette.set(static_cast<Uint8>(color), 0, file.Read8() << 2);
         palette.set(static_cast<Uint8>(color), 1, file.Read8() << 2);
         palette.set(static_cast<Uint8>(color), 2, file.Read8() << 2);
         palette.set(static_cast<Uint8>(color), 3, color == 0 ? 0 : 255);
      }
   }
}

void ImageLoader::LoadAuxPalettes(const char* auxpalname,
   Uint8 allauxpals[32][16])
{
   Base::File file(auxpalname, Base::modeRead);
   if (!file.IsOpen())
      throw Base::Exception("could not open file allpals.dat");

   // the standard guarantees that the [32][16] array is contiguous
   Uint8* buffer = &allauxpals[0][0];
   file.ReadBuffer(buffer, 32 * 16);
}

void ImageLoader::LoadImageGr(IndexedImage& img, const char* imgname,
   unsigned int imgnum, Uint8 auxpalettes[32][16])
{
   Base::File file(imgname, Base::modeRead);
   if (!file.IsOpen())
   {
      std::string text("could not open image: ");
      text.append(imgname);
      throw Base::Exception(text.c_str());
   }

   unsigned long filelen = file.FileLength();

   // read in toc
   Uint8 id = file.Read8(); // always 1 for .gr files
   UaAssert(id == 1); id;

   Uint16 entries = file.Read16();

   if (imgnum >= entries)
      imgnum = 0; // image number not valid

   // seek to offset
   file.Seek(4 * imgnum, Base::seekCurrent);
   Uint32 offset = file.Read32();

   if (offset >= filelen)
      return;

   file.Seek(offset, Base::seekBegin);

   // load image into pixel vector
   bool special_panels = (strstr("panels.gr", imgname) != NULL);
   LoadImageGrImpl(img, file, auxpalettes, special_panels);
}

void ImageLoader::LoadImageByt(const char* imgname, Uint8* pixels)
{
   // open file
   Base::File file(imgname, Base::modeRead);
   if (!file.IsOpen())
   {
      std::string text("could not open raw image: ");
      text.append(imgname);
      throw Base::Exception(text.c_str());
   }

   // raw image
   file.ReadBuffer(&pixels[0], 320 * 200);
}

void ImageLoader::LoadImageGrList(std::vector<IndexedImage>& imglist,
   const char* imgname, unsigned int img_from, unsigned int img_to,
   Uint8 auxpalettes[32][16])
{
   Base::File file(imgname, Base::modeRead);
   if (!file.IsOpen())
   {
      std::string text("could not open image list: ");
      text.append(imgname);
      throw Base::Exception(text.c_str());
   }

   unsigned long filelen = file.FileLength();

   // read in toc
   Uint8 id = file.Read8(); // always 1
   UaAssert(id == 1); id;

   Uint16 entries = file.Read16();

   if (img_to == 0)
      img_to = entries;

   if (img_from >= entries || img_to < img_from)
   {
      return;
   }

   // read in all offsets
   std::vector<Uint32> offsets;
   offsets.resize(entries, 0);

   for (Uint16 i = 0; i < entries; i++)
      offsets[i] = file.Read32();

   bool special_panels = (strstr(imgname, "panels.gr") != NULL);

   for (unsigned int j = img_from; j < img_to; j++)
   {
      if (offsets[j] >= filelen)
         continue;

      file.Seek(offsets[j], Base::seekBegin);

      IndexedImage img;
      imglist.push_back(img);

      IndexedImage& lastimg = imglist.back();

      // load image
      LoadImageGrImpl(lastimg, file, auxpalettes, special_panels);
   }
}

void ImageLoader::LoadImageGrImpl(IndexedImage& img, Base::File& file,
   Uint8 auxpalidx[32][16], bool special_panels)
{
   Uint8 type, width, height, auxpal = 0;
   Uint16 datalen;

   if (special_panels)
   {
      // special case for "panels.gr" that lacks a normal header
      type = 4; // 8-bit uncompressed
      width = 83;
      height = 114;
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

      if (auxpal > 0x1f) auxpal = 0;

      // read in data length
      datalen = file.Read16();
   }

   img.create(width, height);

   switch (type)
   {
   case 0x04: // 8-bit uncompressed
   {
      unsigned int pixcount = 0;

      while (datalen > 0)
      {
         Uint16 size = std::min<Uint16>(datalen, 1024);

         file.ReadBuffer(&img.get_pixels()[pixcount], size);

         datalen -= size;
         pixcount += size;
      }
   }
   break;

   case 0x08: // 4-bit rle compressed
      ImageDecodeRLE(file, &img.get_pixels()[0], 4, datalen, width*height, auxpalidx[auxpal], 0, 0);
      break;

   case 0x0a: // 4-bit uncompressed
   {
      unsigned int pixcount = 0, maxpix = width * height;
      Uint8* pal = auxpalidx[auxpal];
      Uint8 rawbyte;
      Uint8* pixels = &img.get_pixels()[0];

      while (datalen > 0)
      {
         rawbyte = file.Read8();
         pixels[pixcount++] = pal[rawbyte >> 4];
         if (pixcount >= maxpix) break;
         pixels[pixcount++] = pal[rawbyte & 0x0f];
         datalen--;
      }
   }
   break;
   }
}
