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
// critter graphics decoding

#include "../hacking.h"


void decode_rle(FILE *fd,FILE *out,unsigned int bits,unsigned int datalen,unsigned char *auxpalidx)
{
   // bit extraction variables
   unsigned int bits_avail=0;
   unsigned int rawbits;
   unsigned int bitmask = ((1<<bits)-1) << (8-bits);
   unsigned int nibble;

   // rle decoding vars
   int pixcount=0;
   int stage=0; // we start in stage 0
   int count;
   int record=0; // we start with record 0=repeat (3=run)
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

//      printf("nibble: %02x\n",nibble);

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


const char *cr_fmt = UWPATH"crit\\cr%02opage.n%02o";

int main(int argc, char* argv[])
{
   const char *assocfilename = UWPATH"crit\\assoc.anm";

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

   FILE* log = fopen("crit-log.txt","w");

   for(int crit=0; crit<32; crit++)
   {
      // retrieve critter name
      char critname[8]; critname[0]=0;
      unsigned char anim=0;
      unsigned char variant=0;
      unsigned short unk1=0;

      FILE *assoc = fopen(assocfilename,"rb");
      fseek(assoc,crit*8,SEEK_SET);
      fread(critname,1,8,assoc);

      if (critname[0]==0)
         strcpy(critname,"*unkn*");

      fseek(assoc,32*8 + crit*2,SEEK_SET); // 0x0100 + x
      anim = fgetc(assoc);
      variant = fgetc(assoc);

      fseek(assoc,32*8 + 32*2 + crit*2,SEEK_SET); // 0x0140 + x
      fread(&unk1,2,1,assoc);

      fclose(assoc);

      {
         char buffer[256];
         sprintf(buffer,cr_fmt,anim,0);

         FILE *pfile = fopen(buffer,"rb");
         if (pfile==NULL)
            continue;

         fclose(pfile);
      }

      fprintf(log,"critter 0%02o: \"%-8.8s\", anim=%02u, variant=%02x unk1=%04x\n",crit,critname,anim,variant,unk1);

   continue;

      anim = crit;

      // read in all pages
      for(int page=0;;page++)
      {
         char buffer[256];
         sprintf(buffer,cr_fmt,anim,page);
         FILE *pfile = fopen(buffer,"rb");
         if (pfile==NULL)
            break; // no more page files

         // read in slot list
         int slotbase = fgetc(pfile);
         int nslots = fgetc(pfile);

         if (nslots==0)
         {
            fprintf(log,"no slots in file\n");
            continue;
         }

         unsigned char *segoffsets = new unsigned char[nslots];
         fread(segoffsets,1,nslots,pfile);

         int nsegs = fgetc(pfile);

         fprintf(log,"cr%02opage.n%02o: slotbase = %2u, nslots = %3u, nsegs = %2u\n",anim,page,slotbase,nslots,nsegs);

         // print slot list
         {
            fprintf(log," slot list:");

            for(int i=0; i<nslots; i++)
            {
               if ((i&15)==0) fprintf(log,"\n  %04x:",i+slotbase);
               fprintf(log," %02x",segoffsets[i]);
            }

            fprintf(log,"\n");
         }

         // print segment list
         for(int i=0; i<nsegs; i++)
         {
            fprintf(log," segment #%u:",i);

            for(int n=0; n<8; n++)
               fprintf(log," %02x",fgetc(pfile));
            fprintf(log,"\n");
         }

         int nauxpals = fgetc(pfile);


         fprintf(log," number of aux palettes: %u%s\n",nauxpals,
            variant>=nauxpals? " - warning! variant >= nauxpals" : "");

         // in case of wrong "variant" value, choose first auxpal
         if (variant>=nauxpals)
            variant = 0;

         unsigned char *auxpals = new unsigned char[32*nauxpals];
         fread(auxpals,32,nauxpals,pfile);

         int noffsets = fgetc(pfile);
         int unknown1 = fgetc(pfile);

         unsigned short *alloffsets = new unsigned short[noffsets];
         fread(alloffsets,2,noffsets,pfile);

         // print all file offsets
         {
            fprintf(log," noffsets = %u, unk1 = %02x", noffsets,unknown1);

            for(unsigned int i=0; i<noffsets; i++)
            {
               if ((i&15)==0) fprintf(log,"\n ");
               fprintf(log," %04x",alloffsets[i]);
            }

            fprintf(log,"\n\n");
         }

         unsigned int maxleft,maxright,maxtop,maxbottom;
         maxleft = maxright = maxtop = maxbottom = 0;

         // decode all frames
         for(int frame=0; frame<noffsets; frame++)
         {
            fseek(pfile,alloffsets[frame],SEEK_SET);
            int width,height,hotx,hoty,type;

            width = fgetc(pfile);
            height = fgetc(pfile);
            hotx = fgetc(pfile);
            hoty = fgetc(pfile);
            type = fgetc(pfile);

            int wsize=5;
            if (type!=6)
               wsize=4;

            fprintf(log," frame #%02x, %ux%u, hot:(%u,%u), type=%u\n",
               frame,width,height,hotx,hoty,type);

            unsigned int left,right,top,bottom;

            right = width-hotx;
            left = hotx;
            top = hoty;
            bottom = height-hoty;

            if (right > maxright) maxright = right;
            if (left > maxleft) maxleft = left;
            if (top > maxtop) maxtop = top;
            if (bottom > maxbottom) maxbottom = bottom;

      continue;

            unsigned short datalen;
            fread(&datalen,1,2,pfile);

            // decode bitmap
            char buffer[256];
            sprintf(buffer,"cr%02o-%.8s-page%02oframe%02x.tga",crit,critname,page,frame);

            FILE *tga = fopen(buffer,"wb");

            tga_writeheader(tga,width,height,1,1);

            // write palette
            fwrite(palette,1,256*3,tga);

            decode_rle(pfile,tga,wsize,width*height,auxpals+32*variant);

            fclose(tga);
         }

         fprintf(log," x/y range for all frames: %u x %u - "
            "new hot spot: (%u,%u)\n\n",
            maxleft+maxright,maxtop+maxbottom,
            maxleft,maxtop);

         delete[] alloffsets;
         delete[] auxpals;
         delete[] segoffsets;
         fclose(pfile);
      }

      fprintf(log,"\n");
   }

   fclose(log);

   return 0;
}
