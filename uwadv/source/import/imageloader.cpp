/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file imageloader.cpp

   \brief *.gr, *.byt files and palette loading

*/

// needed includes
#include "common.hpp"
#include "image.hpp"
#include "import.hpp"
#include "io_endian.hpp"


// global methods

/*! decodes the underworld rle format, for word lengths up to 8 bit, and
    stores the pixels in an array.
*/
void ua_image_decode_rle(FILE *fd, Uint8* pixels, unsigned int bits,
   unsigned int datalen, unsigned int maxpix, unsigned char *auxpalidx,
   unsigned int padding=0, unsigned int linewidth=0)
{
   unsigned int linecount = 0;
   unsigned int bufpos = 0;

   // bit extraction variables
   unsigned int bits_avail = 0;
   unsigned int rawbits = 0;
   unsigned int bitmask = ((1<<bits)-1) << (8-bits);
   unsigned int nibble;

   // rle decoding vars
   unsigned int pixcount=0;
   unsigned int stage=0; // we start in stage 0
   unsigned int count=0;
   unsigned int record=0; // we start with record 0=repeat (3=run)
   unsigned int repeatcount=0;

   while(datalen>0 && pixcount<maxpix)
   {
      // get new bits
      if (bits_avail<bits)
      {
         // not enough bits available
         if (bits_avail>0)
         {
            nibble = ((rawbits & bitmask) >> (8-bits_avail));
            nibble <<= (bits - bits_avail);
         }
         else
            nibble = 0;

         rawbits = (unsigned int)fgetc(fd);
         if ((int)rawbits == EOF)
            return;

//         printf("fgetc: %02x\n",rawbits);

         unsigned int shiftval = 8 - (bits - bits_avail);

         nibble |= (rawbits >> shiftval);

         rawbits = (rawbits << (8-shiftval)) & 0xFF;

         bits_avail = shiftval;
      }
      else
      {
         // we still have enough bits
         nibble = (rawbits & bitmask)>>(8-bits);
         bits_avail -= bits;
         rawbits <<= bits;
      }

//      printf("nibble: %02x\n",nibble);

      // now that we have a nibble
      datalen--;

      switch(stage)
      {
      case 0: // we retrieve a new count
         if (nibble==0)
            stage++;
         else
         {
            count = nibble;
            stage=6;
         }
         break;
      case 1:
         count = nibble;
         stage++;
         break;

      case 2:
         count = (count<<4) | nibble;
         if (count==0)
            stage++;
         else
            stage = 6;
         break;

      case 3:
      case 4:
      case 5:
         count = (count<<4) | nibble;
         stage++;
         break;
      }

      if (stage<6) continue;

      switch(record)
      {
      case 0:
         // repeat record stage 1
//         printf("repeat: new count: %x\n",count);

         if (count==1)
         {
            record=3; // skip this record; a run follows
            break;
         }

         if (count==2)
         {
            record=2; // multiple run records
            break;
         }

         record=1; // read next nibble; it's the color to repeat
         continue;

      case 1:
         // repeat record stage 2

         {
            // repeat 'nibble' color 'count' times
            for(unsigned int n=0; n<count; n++)
            {
               pixels[bufpos++] = auxpalidx[nibble];

               // add padding space when needed
               if (padding != 0 && ++linecount >= linewidth)
               {
                  linecount = 0;
                  bufpos += padding;
               }

               if (++pixcount>=maxpix)
                  break;
            }
         }

//         printf("repeat: wrote %x times a '%x'\n",count,nibble);

         if (repeatcount==0)
         {
            record=3; // next one is a run record
         }
         else
         {
            repeatcount--;
            record=0; // continue with repeat records
         }
         break;

      case 2:
         // multiple repeat stage

         // 'count' specifies the number of repeat record to appear
//         printf("multiple repeat: %u\n",count);
         repeatcount = count-1;
         record=0;
         break;

      case 3:
         // run record stage 1
         // copy 'count' nibbles

//         printf("run: count: %x\n",count);

         record=4; // retrieve next nibble
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

         if (--count==0)
         {
//            printf("run: finished\n");
            record = 0; // next one is a repeat again
         }
         else
            continue;
         break;
      }

      stage=0;
      // end of while loop
   }
}


// ua_uw_import methods

void ua_uw_import::load_palettes(const char* allpalname,
   ua_palette256_ptr allpalettes[8])
{
   FILE* fd = fopen(allpalname,"rb");
   if (fd==NULL)
   {
      std::string text("could not open palette file ");
      text.append(allpalname);
      throw ua_exception(text.c_str());
   }

   // palettes are stored in ancient vga color format
   for(unsigned int pal=0; pal<8; pal++)
   {
      allpalettes[pal] = ua_palette256_ptr(new ua_palette256);

      ua_palette256& palette = *allpalettes[pal];

      for(unsigned int color=0; color<256; color++)
      {
         palette.set(color,0, fgetc(fd)<<2);
         palette.set(color,1, fgetc(fd)<<2);
         palette.set(color,2, fgetc(fd)<<2);
         palette.set(color,3, color==0 ? 0 : 255);
      }
   }

   fclose(fd);
}

void ua_uw_import::load_aux_palettes(const char* auxpalname,
   Uint8 allauxpals[32][16])
{
   FILE *fd = fopen(auxpalname,"rb");
   if (fd==NULL)
      throw ua_exception("could not open file allpals.dat");

   // the standard guarantees me that the [32][16] array is contiguous
   fread(allauxpals, 32*16, 1, fd);
   fclose(fd);
}

void ua_uw_import::load_image_gr(ua_image& img, const char* imgname,
   unsigned int imgnum, Uint8 auxpalettes[32][16])
{
   // open file
   FILE* fd = fopen(imgname,"rb");
   if (fd==NULL)
   {
      std::string text("could not open image: ");
      text.append(imgname);
      throw ua_exception(text.c_str());
   }

   // get file length
   fseek(fd,0,SEEK_END);
   unsigned long filelen = ftell(fd);
   fseek(fd,0,SEEK_SET);

   // read in toc
   Uint8 id = fgetc(fd); // always 1 for .gr files
   Uint16 entries = fread16(fd);

   if (imgnum >= entries)
      imgnum = 0; // image number not valid

   // seek to offset
   fseek(fd,4*imgnum,SEEK_CUR);
   Uint32 offset = fread32(fd);

   if (offset >= filelen)
      return;

   fseek(fd,offset,SEEK_SET);

   // load image into pixel vector
   bool special_panels = (strstr("panels.gr",imgname) != NULL);
   load_image_gr_impl(img,fd,auxpalettes,special_panels);

   fclose(fd);
}

void ua_uw_import::load_image_byt(const char* imgname, Uint8* pixels)
{
   // open file
   FILE* fd = fopen(imgname,"rb");
   if (fd == NULL)
   {
      std::string text("could not open raw image: ");
      text.append(imgname);
      throw ua_exception(text.c_str());
   }

   // raw image
   fread(&pixels[0],1,320*200,fd);

   fclose(fd);
}

void ua_uw_import::load_image_gr_list(std::vector<ua_image>& imglist,
   const char* imgname, unsigned int img_from, unsigned int img_to,
   Uint8 auxpalettes[32][16])
{
   // open file
   FILE* fd = fopen(imgname,"rb");
   if (fd==NULL)
   {
      std::string text("could not open image list: ");
      text.append(imgname);
      throw ua_exception(text.c_str());
   }

   // get file length
   fseek(fd,0,SEEK_END);
   unsigned long filelen = ftell(fd);
   fseek(fd,0,SEEK_SET);

   // read in toc
   Uint8 id = fgetc(fd); // always 1
   Uint16 entries = fread16(fd);

   if (img_to==0) img_to=entries;

   if (img_from >= entries || img_to < img_from)
   {
      fclose(fd);
      return;
   }

   // read in all offsets
   std::vector<Uint32> offsets;
   offsets.resize(entries,0);

   for(Uint16 i=0; i<entries; i++)
      offsets[i]=fread32(fd);

   bool special_panels = (strstr("panels.gr",imgname) != NULL);

   for(Uint16 j=img_from; j<img_to; j++)
   {
      if (offsets[j]>=filelen)
         continue;

      fseek(fd,offsets[j],SEEK_SET);

      ua_image img;
      imglist.push_back(img);

      ua_image& lastimg = imglist.back();//[imglist.size()-1];

      // load image
      load_image_gr_impl(lastimg,fd,auxpalettes,special_panels);
   }

   fclose(fd);
}

void ua_uw_import::load_image_gr_impl(ua_image& img, FILE* fd,
   Uint8 auxpalidx[32][16], bool special_panels)
{
   Uint8 type, width, height, auxpal=0;
   Uint16 datalen;

   if (special_panels)
   {
      // special case for "panels.gr" that lacks a normal header
      type = 4; // 8-bit uncompressed
      width = 83;
      height = 114;
      datalen = width*height;
   }
   else
   {
      // read in image header
      type = fgetc(fd);
      width = fgetc(fd);
      height = fgetc(fd);

      // read in auxpal, when needed
      if (type==0x08 || type==0x0a)
         auxpal = fgetc(fd);

      if (auxpal>0x1f) auxpal=0;

      // read in data length
      datalen = fread16(fd);
   }

   img.create(width,height);

   switch(type)
   {
   case 0x04: // 8-bit uncompressed
      {
         unsigned int pixcount=0;

         while (datalen>0)
         {
            unsigned int size = ua_min(datalen,1024);

            fread(&img.get_pixels()[pixcount],1,size,fd);

            datalen -= size;
            pixcount += size;
         }
      }
      break;

   case 0x08: // 4-bit rle compressed
      ua_image_decode_rle(fd,&img.get_pixels()[0],4,datalen,width*height,auxpalidx[auxpal],0,0);
      break;

   case 0x0a: // 4-bit uncompressed
      {
         unsigned int pixcount=0, maxpix = width*height;
         Uint8* pal = auxpalidx[auxpal];
         Uint8 rawbyte;
         Uint8* pixels = &img.get_pixels()[0];

         while (datalen>0)
         {
            rawbyte = fgetc(fd);
            pixels[pixcount++] = pal[rawbyte >> 4];
            if (pixcount>=maxpix) break;
            pixels[pixcount++] = pal[rawbyte&0x0f];
            datalen--;
         }
      }
      break;
   }
}
