/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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
// graphics decoding

#include "../hacking.h"


#if 0
# define my_printf printf
#else
inline my_printf(const char*fmt,...){}
#endif

void decode_rle(FILE *fd,FILE *out,unsigned int bits,unsigned int datalen,unsigned char *auxpalidx)
{
   // bit extraction variables
   unsigned int bits_avail=0;
   unsigned int rawbits;
   unsigned int bitmask = ((1<<bits)-1) << (8-bits);
   unsigned int nibble;

   // rle decoding vars
   unsigned int pixcount=0;
   int stage=0; // we start in stage 0
   int count;
   int record=0; // we start with record 0 = repeat (1=run)
   int repeatcount=0;

   while(pixcount<datalen)
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

         rawbits = fgetc(fd);
         if (rawbits==EOF)
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

      if (nibble>=32)
         _asm int 3;

//      printf("nibble: %x\n",nibble);

      // now that we have a nibble

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
            for(int n=0; n<count; n++)
            {
               fputc(auxpalidx[nibble],out);
               pixcount++;
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
         fputc(auxpalidx[nibble],out);
         pixcount++;

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



void decode_4bit_rle(int datalen, FILE *fd, FILE *tga, unsigned char auxpalidx[16])
{
   int pixcount=0;
   int nib_avail=0;
   int rbyte;
   int nibble;
   int stage=0; // we start in stage 0
   int count;
   int record=0; // we start with record 0 = repeat (1=run)
   int repeatcount=0;
   int n;

   while (datalen>0)
   {
      // get new byte
      if (nib_avail==0)
      {
         rbyte = fgetc(fd);
         nib_avail = 2;
      }

      // get next nibble
      nibble = nib_avail==2 ? rbyte >> (4*(nib_avail-1)) : rbyte & 0x0f;
      nib_avail--;
      datalen--;

      my_printf(" new nibble: %x\n",nibble);

      // now that we have a nibble

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
         my_printf("repeat: new count: %x\n",count);

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

         // repeat 'nibble' color 'count' times
         for(n=0; n<count; n++)
         {
            fputc(auxpalidx[nibble],tga);
            pixcount++;
         }

         my_printf("repeat: wrote %x times a '%x'\n",count,nibble);

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
         my_printf("multiple repeat: %u\n",count);
         repeatcount = count-1;
         record=0;
         break;

      case 3:
         // run record stage 1
         // copy 'count' nibbles

         my_printf("run: count: %x\n",count);

         record=4; // retrieve next nibble
         continue;

      case 4:
         // run record stage 2

         // now we have a nibble to write
         fputc(auxpalidx[nibble],tga);
         pixcount++;

         if (--count==0)
         {
            my_printf("run: finished\n");
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

void decode_4bit(int datalen, FILE *fd, FILE *tga, unsigned char auxpalidx[16])
{
   int nib_avail=0;
   int rbyte;
   int nibble;

   while (datalen>=0)
   {
      // get new byte
      if (nib_avail==0)
      {
         rbyte = fgetc(fd);
         nib_avail = 2;
         datalen--;
      }

      // get next nibble
      nibble = nib_avail==2 ? rbyte >> (4*(nib_avail-1)) : rbyte & 0x0f;
      nib_avail--;

      my_printf(" new nibble: %x\n",nibble);

      fputc(auxpalidx[nibble],tga);
   }
}

int main(int argc, char* argv[])
{
   time_t starttime = time(NULL);

   char fname[256];


   // get 256 colors palette
   char palette[256*3];
   {
      FILE *pal = fopen(UWPATH"data\\pals.dat","rb");

      fread(palette,1,256*3,pal);

      for(int j=0; j<256*3; j++) palette[j]<<=2;

      for(int k=0; k<256; k++)
      {
         char h = palette[k*3+0];
         palette[k*3+0] = palette[k*3+2];
         palette[k*3+2] = h;
      }

      fclose(pal);
   }

   // get 16 bit aux palette index table
   unsigned char auxpalidx[32][16];
   {
      FILE *pal = fopen(UWPATH"data\\allpals.dat","rb");

      fread(auxpalidx,1,32*16,pal);
      fclose(pal);
   }



   _finddata_t find;
   long hnd = _findfirst(UWPATH"data\\*.gr",&find);

   if (hnd==-1)
   {
      printf("no files found!\n");
      return 0;
   }

   do
   {
      char basename[64];

      // get basename
      strcpy(basename,find.name);

      char *pos = strrchr(basename,'.');
      if (pos != NULL)
         *pos = 0;


      // construct name
      sprintf(fname,UWPATH"data\\%s.gr",basename);

      FILE *fd = fopen(fname,"rb");

      if (fd==NULL)
      {
         printf("could not open file\n");
         return 0;
      }

      fseek(fd,0,SEEK_END);
      unsigned long flen = ftell(fd);
      fseek(fd,0,SEEK_SET);

      // read in toc
      int id = fgetc(fd); // always 1 (?)

      unsigned short entries;
      fread(&entries,1,2,fd);
      printf("%s: we have %u entries\n",fname,entries);

      // read in all offsets
      unsigned int *offsets = new unsigned int[entries];
      fread(offsets,sizeof(unsigned int),entries,fd);

      for(int img=0; img<entries; img++)
      {
         if (offsets[img]>=flen)
         {
            printf("skipping image %u: invalid offset %08x\n",img,offsets[img]);
            continue;
         }

         // seek to bitmap info
         fseek(fd,offsets[img],SEEK_SET);

         int type, width, height;

         if (stricmp("panels",basename)==0)
         {
            type = 4;
            width = 83;
            height = 114;
         }
         else
         {
            // reading in the image header

            type = fgetc(fd);
            width = fgetc(fd);
            height = fgetc(fd);
         }

         my_printf("image %3u: size=%2ux%2u ",img,width,height);

         int auxpal=0;

         switch(type)
         {
         case 0x8:
            auxpal = fgetc(fd);
            my_printf("4-bit run-length, palette %02x",auxpal);
            break;
         case 0xa:
            auxpal = fgetc(fd);
            my_printf("4-bit uncompressed, palette %02x",auxpal);
            break;
         case 0x4:
            my_printf("8-bit uncompressed");
            break;
         default:
            my_printf("strange type:%u",type);
         }

         if (auxpal>0x1f)
         {
            my_printf("skipping, invalid auxilliary palette %02x\n",auxpal);
            continue;
         }

         my_printf("\n");

         unsigned short datalen;

         if (stricmp("panels",basename)==0)
         {
            datalen = width*height;
         }
         else
         {
            // data length
            fread(&datalen,1,2,fd);
         }

         // now open and write a tga file

         sprintf(fname,"%s%03u.tga",basename,img);

         FILE *tga = fopen(fname,"wb");

         tga_writeheader(tga,width,height,1,1);

         // write palette
         fwrite(palette,1,256*3,tga);

         // write actual data
         switch(type)
         {
         case 4: // 8-bit
            {
               char buffer[1024];

               while (datalen>0)
               {
                  int size = datalen > 1024 ? 1024 : datalen;
                  fread(buffer,1,size,fd);
                  fwrite(buffer,1,size,tga);
                  datalen-=size;
               }
            }
            break;

         case 0xa:
            {
               decode_4bit(datalen,fd,tga,auxpalidx[auxpal]);
            }
            break;
         case 8: // 4-bit rle
            {
               decode_rle(fd,tga,4,width*height,auxpalidx[auxpal]);
               //decode_4bit_rle(datalen,fd,tga,auxpalidx[auxpal]);
            }
            break;
         }

         fclose(tga);
      }

      delete[] offsets;

      fclose(fd);


   } while(0==_findnext(hnd,&find));

   _findclose(hnd);

   printf("needed %u seconds to extract\n",time(NULL)-starttime);

   return 0;
}
