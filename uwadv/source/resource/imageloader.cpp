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

   \brief *.gr and *.byt files loading implementation

   ua_image_decode_rle() decodes the underworld rle format, for word lengths
   up to 8 bit and stores the pixels in an array.

   ua_image_load() does the image loading for "*.gr" files from the start of
   an image header

   ua_image::load() takes a graphic file name and loads it. used without the
   .gr extension (e.g. only "charhead" for name)

   ua_image_list::load() does the same, but for a range of images in a graphic
   file.

   ua_image::load_raw() loads a raw image, stored in "*.byt" files. the full
   path from the uw1 root directory must be given.

*/

// needed includes
#include "common.hpp"
#include "image.hpp"
#include "fread_endian.hpp"


// global methods

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


// ua_image methods

void ua_image::load_image(FILE *fd,Uint8 auxpalidx[32][16], bool special_panels)
{
   Uint8 type, width, height,auxpal=0;
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

   create(width,height);

   switch(type)
   {
   case 0x04: // 8-bit uncompressed
      {
         unsigned int pixcount=0;

         while (datalen>0)
         {
            unsigned int size = ua_min(datalen,1024);

            fread(&pixels[pixcount],1,size,fd);

            datalen -= size;
            pixcount += size;
         }
      }
      break;

   case 0x08: // 4-bit rle compressed
      ua_image_decode_rle(fd,&pixels[0],4,datalen,width*height,auxpalidx[auxpal],0,0);
      break;

   case 0x0a: // 4-bit uncompressed
      {
         unsigned int pixcount=0, maxpix = width*height;
         Uint8 *pal = auxpalidx[auxpal];
         Uint8 rawbyte;

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

void ua_image::load(ua_settings &settings, const char *name, unsigned int which,
   unsigned int pal)
{
   Uint8 auxpalidx[32][16];

   // load all auxiliary palettes
   {
      std::string allauxpalname(settings.get_string(ua_setting_uw_path));
      allauxpalname.append("data/allpals.dat");

      FILE *fd = fopen(allauxpalname.c_str(),"rb");
      if (fd==NULL)
         throw ua_exception("could not open file allpals.dat");

      fread(auxpalidx,1,32*16,fd);
      fclose(fd);
   }

   // create filename
   std::string filename(settings.get_string(ua_setting_uw_path));
   filename.append("data/");
   filename.append(name);
   filename.append(".gr");

   // open file
   FILE *fd = fopen(filename.c_str(),"rb");
   if (fd==NULL)
   {
      std::string text("could not open image: ");
      text.append(filename);
      throw ua_exception(text.c_str());
   }

   // get file length
   fseek(fd,0,SEEK_END);
   unsigned long filelen = ftell(fd);
   fseek(fd,0,SEEK_SET);

   // read in toc
   Uint8 id = fgetc(fd); // always 1 (?)
   Uint16 entries = fread16(fd);

   if (which>=entries)
   {
      fclose(fd);
      return;
   }

   // seek to offset
   fseek(fd,4*which,SEEK_CUR);
   Uint32 offset = fread32(fd);

   if (offset>=filelen)
      return;

   fseek(fd,offset,SEEK_SET);

   // load image into pixel vector
   bool special_panels = (strcmp("panels",name)==0);
   load_image(fd,auxpalidx,special_panels);
   palette = pal;

   fclose(fd);
}

void ua_image::load_raw(ua_settings &settings, const char *name, unsigned int pal)
{
   // create filename
   std::string filename(settings.get_string(ua_setting_uw_path));
   filename.append(name);

   // open file
   FILE *fd = fopen(filename.c_str(),"rb");
   if (fd==NULL)
   {
      std::string text("could not open raw image: ");
      text.append(filename);
      throw ua_exception(text.c_str());
   }

   // create and load raw image
   create(320,200,0,pal);

   fread(&pixels[0],1,320*200,fd);

   fclose(fd);
}

void ua_image_list::load(ua_settings &settings, const char *name, unsigned int from,
   unsigned int to, unsigned int palette)
{
   Uint8 auxpalidx[32][16];

   // load all auxiliary palettes
   {
      std::string allauxpalname(settings.get_string(ua_setting_uw_path));
      allauxpalname.append("data/allpals.dat");

      FILE *fd = fopen(allauxpalname.c_str(),"rb");
      if (fd==NULL)
         throw ua_exception("could not open file allpals.dat");

      fread(auxpalidx,1,32*16,fd);
      fclose(fd);
   }

   // create filename
   std::string filename(settings.get_string(ua_setting_uw_path));
   filename.append("data/");
   filename.append(name);
   filename.append(".gr");

   // open file
   FILE *fd = fopen(filename.c_str(),"rb");
   if (fd==NULL)
   {
      std::string text("could not open image list: ");
      text.append(filename);
      throw ua_exception(text.c_str());
   }

   // get file length
   fseek(fd,0,SEEK_END);
   unsigned long filelen = ftell(fd);
   fseek(fd,0,SEEK_SET);

   // read in toc
   Uint8 id = fgetc(fd); // always 1 (?)
   Uint16 entries = fread16(fd);

   if (to==0) to=entries;

   if (from>=entries || to<from)
   {
      fclose(fd);
      return;
   }

   // read in all offsets
   std::vector<Uint32> offsets;
   offsets.resize(entries,0);

   for(Uint16 i=0; i<entries; i++)
      offsets[i]=fread32(fd);

   bool special_panels = (strcmp("panels",name)==0);

   for(Uint16 j=from; j<to; j++)
   {
      if (offsets[j]>=filelen)
         continue;

      fseek(fd,offsets[j],SEEK_SET);

      // load image into pixel vector
      ua_image img;
      img.load_image(fd,auxpalidx,special_panels);
      img.palette = palette;

      allimages.push_back(img);
   }

   fclose(fd);
}
