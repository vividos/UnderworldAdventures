//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2019,2022 Underworld Adventures Team
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
// critter graphics decoding
//
#include "hacking.h"

void decode_rle(FILE* fd, FILE* out, unsigned int bits, unsigned int datalen, unsigned char* auxpalidx)
{
   // bit extraction variables
   unsigned int bits_avail = 0;
   unsigned int rawbits;
   unsigned int bitmask = ((1 << bits) - 1) << (8 - bits);
   unsigned int nibble;

   // rle decoding vars
   int pixcount = 0;
   int stage = 0; // we start in stage 0
   int count;
   int record = 0; // we start with record 0=repeat (3=run)
   int repeatcount = 0;

   while (pixcount < int(datalen))
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

         rawbits = fgetc(fd);
         if (rawbits == EOF)
            return;

         //         printf("fgetc: %02x\n",rawbits);

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
      case 0: // repeat record stage 1
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

      case 1: // repeat record stage 2
      {
         // repeat 'nibble' color 'count' times
         for (int n = 0; n < count; n++)
         {
            fputc(auxpalidx[nibble], out);
            pixcount++;
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
         fputc(auxpalidx[nibble], out);
         pixcount++;

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

#ifndef HAVE_UW2
const char* cr_fmt = UWPATH "crit\\cr%02opage.n%02o";
const char* assocfilename = UWPATH "crit\\assoc.anm";
#else
const char* cr_fmt = UWPATH "crit\\cr%02o.%02o";
const char* assocfilename = UWPATH "crit\\as.an";
#endif


int critdec_main()
{
   // get 256 colors palette
   char palette[256 * 3];
   {
      FILE* pal = fopen(UWPATH "data\\pals.dat", "rb");

      fread(palette, 1, 256 * 3, pal);

      for (int j = 0; j < 256 * 3; j++) palette[j] <<= 2;

      for (int k = 0; k < 256; k++)
      {
         char h = palette[k * 3 + 0];
         palette[k * 3 + 0] = palette[k * 3 + 2];
         palette[k * 3 + 2] = h;
      }

      fclose(pal);
   }

   // read in game strings
   ua_gamestrings gs;
   gs.load(UWPATH "data\\strings.pak");

   // open log
   FILE* log = fopen("crit-log.txt", "w");
   fprintf(log, "critdec log file\n\n");

   // get animation and critter lists
   char critname[32][8] = {};
   {
      FILE* assoc = fopen(assocfilename, "rb");

#ifndef HAVE_UW2
      // read in animation names
      fread(critname, 32, 8, assoc);
#endif

      // read in critter infos
      for (int i = 0; i < 64; i++)
      {
         unsigned int anim = fgetc(assoc);
         unsigned int auxpal = fgetc(assoc);

#ifndef HAVE_UW2
         fprintf(log, "critter 0x%04x: anim = 0%02o (%-8.8s), auxpal = %02x, page file cr%02opage.nYY, critter = %s\n",
            i + 64, anim, critname[anim], auxpal,
            anim,
            gs.get_string(4, i + 64).c_str());
#else
         fprintf(log, "critter 0x%04x: anim = 0%02o (0x%02x), auxpal = %02x, page file cr%02o.YY, critter = %s\n",
            i + 64, anim, anim, auxpal,
            anim,
            gs.get_string(4, i + 64).c_str());
#endif
      }

      fprintf(log, "\n");

      fclose(assoc);
   }

#ifdef HAVE_UW2
   // read uw2 slots for all critters
   {
      FILE* fd = fopen(UWPATH "\\crit\\cr.an", "rb");

      unsigned char slotListUw2[32][512];

      fread(slotListUw2, 1, sizeof(slotListUw2), fd);
      for (unsigned int critterIndex = 0; critterIndex < 32; critterIndex++)
      {
         fprintf(log, "%s\n", gs.get_string(4, critterIndex + 64).c_str());

         for (unsigned int j = 0; j < 512; j++)
         {
            fprintf(log, "%02x%c",
               slotListUw2[critterIndex][j], (j & 7) == 7 ? '\n' : ' ');
         }

         fprintf(log, "\n");
      }

      fclose(fd);
   }

   FILE* pgmp = fopen(UWPATH "\\crit\\pg.mp", "rb");
#endif

   // page files go from cr00 to cr37 (octal, = 0x1F)
   for (int crit = 0; crit < 32; crit++)
   {
#ifndef HAVE_UW2
      // test first page file
      {
         char buffer[256];
         sprintf(buffer, cr_fmt, crit, 0);

         FILE* pfile = fopen(buffer, "rb");
         if (pfile == NULL)
            continue;

         fclose(pfile);
      }
#else
      unsigned char pageMap[8] = {};
      fread(pageMap, 1, 8, pgmp);

      // in uw2, we can check the page map
      if (pageMap[0] == 0xFF)
         continue;
#endif

#ifndef HAVE_UW2
      fprintf(log, "anim 0%02o: \"%-8.8s\"\n", crit, critname[crit]);
#else
      fprintf(log, "anim 0%02o:\n", crit);
#endif

      long offsetTableOffsetUw2 = 0x80;

      // read in all pages
      for (int page = 0;; page++)
      {
#ifdef HAVE_UW2
         // in uw2, we can check the page map
         if (pageMap[page] == 0xFF)
            break;
#endif

         char buffer[256];
         sprintf(buffer, cr_fmt, crit, page);
         FILE* pfile = fopen(buffer, "rb");
         if (pfile == NULL)
            break; // no more page files

#ifndef HAVE_UW2
         // read in slot list
         int slotbase = fgetc(pfile);
         int nslots = fgetc(pfile);

         if (nslots == 0)
         {
            fprintf(log, "%s: no slots in file\n", buffer);
            continue;
         }

         unsigned char* segoffsets = new unsigned char[nslots];
         fread(segoffsets, 1, nslots, pfile);

         int nsegs = fgetc(pfile);

         fprintf(log, "cr%02opage.n%02o\n slotbase = %2u, nslots = %3u, nsegs = %2u\n",
            crit, page, slotbase, nslots, nsegs);

         // print slot list
         {
            fprintf(log, " slot list:");

            for (int i = 0; i < nslots; i++)
            {
               if ((i & 15) == 0) fprintf(log, "\n  %04x:", i + slotbase);
               fprintf(log, " %02x", segoffsets[i]);
            }

            fprintf(log, "\n");
         }

         // print segment list
         for (int i = 0; i < nsegs; i++)
         {
            fprintf(log, " segment #%u:", i);

            for (int n = 0; n < 8; n++)
               fprintf(log, " %02x", fgetc(pfile));
            fprintf(log, "\n");
         }

         int nauxpals = fgetc(pfile);
         fprintf(log, " number of aux palettes: %u\n", nauxpals);

         // always choose first auxpal
         int useauxpal = 0;

         unsigned char* auxpals = new unsigned char[32 * nauxpals];
         fread(auxpals, 32, nauxpals, pfile);

         int noffsets = fgetc(pfile);
         int unknown1 = fgetc(pfile);
#else
         int noffsets = pageMap[page] + 1;
         if (page > 0)
            noffsets -= pageMap[page - 1] + 1;

         int nauxpals = 4; // all uw2 page files have 4 auxpals
         int useauxpal = 0; // always choose first auxpal

         unsigned char* auxpals = new unsigned char[32 * nauxpals];
         fread(auxpals, 32, nauxpals, pfile);

         fseek(pfile, offsetTableOffsetUw2, SEEK_SET);
#endif

         unsigned short* alloffsets = new unsigned short[noffsets];
         fread(alloffsets, 2, noffsets, pfile);

         // remember offset for next page
         offsetTableOffsetUw2 = ftell(pfile);

         // print all file offsets
         {
#ifndef HAVE_UW2
            fprintf(log, " noffsets = %u, unk1 = %02x", noffsets, unknown1);
#else
            fprintf(log, " noffsets = %u", noffsets);
#endif

            for (unsigned int i = 0; i < unsigned(noffsets); i++)
            {
               if ((i & 15) == 0) fprintf(log, "\n ");
               fprintf(log, " %04x", alloffsets[i]);
            }

            fprintf(log, "\n\n");
         }

         unsigned int maxleft, maxright, maxtop, maxbottom;
         maxleft = maxright = maxtop = maxbottom = 0;

         // decode all frames
         for (int frame = 0; frame < noffsets; frame++)
         {
            fseek(pfile, alloffsets[frame], SEEK_SET);
            int width, height, hotx, hoty, type;

            width = fgetc(pfile);
            height = fgetc(pfile);
            hotx = fgetc(pfile);
            hoty = fgetc(pfile);
            type = fgetc(pfile);

            int wsize = 5;
            if (type != 6)
               wsize = 4;

            fprintf(log, " frame #%02x, %ux%u, hot:(%u,%u), type=%u\n",
               frame, width, height, hotx, hoty, type);

            unsigned int left, right, top, bottom;

            right = width - hotx;
            left = hotx;
            top = hoty;
            bottom = height - hoty;

            if (right > maxright) maxright = right;
            if (left > maxleft) maxleft = left;
            if (top > maxtop) maxtop = top;
            if (bottom > maxbottom) maxbottom = bottom;

            //continue; // comment out to write all animation frames

            unsigned short datalen;
            fread(&datalen, 1, 2, pfile);

            // decode bitmap
            char buffer[256];
#ifndef HAVE_UW2
            sprintf(buffer, "cr%02o-%.8s-page%02o-frame%02x.tga", crit, critname[crit], page, frame);
#else
            sprintf(buffer, "cr%02o-page%02o-frame%02x.tga", crit, page, frame);
#endif

            FILE* tga = fopen(buffer, "wb");

            tga_writeheader(tga, width, height, 1, 1);

            // write palette
            fwrite(palette, 1, 256 * 3, tga);

            decode_rle(pfile, tga, wsize, width * height, auxpals + 32 * useauxpal);

            fclose(tga);
         }

         fprintf(log, " x/y range for all frames: %u x %u - "
            "new hot spot: (%u,%u)\n\n",
            maxleft + maxright, maxtop + maxbottom,
            maxleft, maxtop);

         delete[] alloffsets;
         delete[] auxpals;
#ifndef HAVE_UW2
         delete[] segoffsets;
#endif
         fclose(pfile);
      }

      printf(".");
      fprintf(log, "\n");
   }

#ifdef HAVE_UW2
   fclose(pgmp);
#endif

   fclose(log);

   return 0;
}
