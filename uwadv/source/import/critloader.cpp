/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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
/*! \file critloader.cpp

   \brief critter images loading implementation

   for more infos about critters, see file docs/uw-formats.txt

*/

// needed includes
#include "common.hpp"
#include "critter.hpp"
#include "import.hpp"
#include "io_endian.hpp"


// defines

// for optimizing, we omit the first pass in loading frames
// the xres, yres and maxframes members have to be set before calling
// ua_uw_import::load_critter()
//#define OMIT_1ST_PASS
#undef OMIT_1ST_PASS


// external functions

extern void ua_image_decode_rle(FILE *fd, Uint8* pixels, unsigned int bits,
   unsigned int datalen, unsigned int maxpix, unsigned char *auxpalidx,
   unsigned int padding, unsigned int linewidth);


// ua_uw_import methods

void ua_uw_import::load_critters(std::vector<ua_critter>& allcritters,
   ua_settings& settings, ua_palette256_ptr palette0)
{
   if (settings.get_gametype() != ua_game_uw2)
   {
      ua_uw1_import import;
      import.load_critters(allcritters,settings,palette0);
   }
   else
   {
//      ua_uw2_import import;
//      import.load_critters(allcritters,settings);
   }
}

#ifdef DO_CRITLOAD_DEBUG
extern unsigned int memory_used;
#endif

void ua_uw_import::load_critter(ua_critter& critter, const char* file,
   unsigned int used_auxpal)
{
   unsigned int& xres = critter.xres;
   unsigned int& yres = critter.yres;
   unsigned int& maxframes = critter.maxframes;
   std::vector<unsigned int>& hotxy_coords = critter.hotxy_coords;
   std::vector<unsigned int>& imgsizes = critter.imgsizes;
   std::vector<Uint8>& allframe_bytes = critter.allframe_bytes;
   std::vector<Uint8>& slotlist = critter.slotlist;
   std::vector<std::vector<Uint8> >& segmentlist = critter.segmentlist;

#ifndef OMIT_1ST_PASS
   xres = yres = 0;
   maxframes = 0;
#endif

   hotxy_coords.clear();
   imgsizes.clear();

   // do 2-pass loading:
   // pass 0: determine max. frame image size
   // pass 1: load frame images
#ifdef OMIT_1ST_PASS
   for(unsigned int pass=1; pass<2; pass++)
#else
   for(unsigned int pass=0; pass<2; pass++)
#endif
   {
      unsigned int curpage = 0;
      unsigned int segment_offset = 0;
      unsigned int frame_offset = 0;

      if (pass==1)
      {
         // pass 1: init frame image

#ifdef DO_CRITLOAD_DEBUG
         unsigned int used_bytes = maxframes * xres * yres;

         printf("allocating %u frames (%ux%u), %u bytes\n",maxframes,xres,yres,used_bytes);

         memory_used += used_bytes;
#endif

         // allocate needed number of bytes
         allframe_bytes.resize(maxframes * xres * yres);

#if defined(DO_CRITLOAD_DEBUG) && defined(HAVE_DEBUG)
         memset(&allframe_bytes[0],0xcc, maxframes * xres * yres);
#else
         memset(&allframe_bytes[0],0x00, maxframes * xres * yres);
#endif

         hotxy_coords.resize(maxframes*2,0);
         imgsizes.resize(maxframes*2,0);
      }

      while(true)
      {
         // construct next pagefile name
         std::string pagefile(file);

         char buffer[8];
         sprintf(buffer,".n%02u",curpage++);
         pagefile.append(buffer);

         // try to open pagefile
         FILE* fd = fopen(pagefile.c_str(),"rb");
         if (fd==NULL)
            break; // no more page files

         // load animation
         Uint8 auxpal[32];

         if (pass==0)
         {
            // pass 0: just skip over all tables

            // skip over slot and segment lists and all auxpals
            fgetc(fd);
            unsigned int nslots = fgetc(fd);
            fseek(fd,nslots,SEEK_CUR);

            unsigned int nsegs = fgetc(fd);
            fseek(fd,nsegs*8,SEEK_CUR);

            unsigned int nauxpals = fgetc(fd);
            fseek(fd,nauxpals*32,SEEK_CUR);
         }
         else
         {
            // pass 1: read in all tables

            // read in slot list
            {
               unsigned int slotbase = fgetc(fd);
               unsigned int nslots = fgetc(fd);

               slotlist.resize(nslots+slotbase,0xff);

               // read in segment indices
               for(unsigned int i=0; i<nslots; i++)
               {
                  Uint8 segment = fgetc(fd);
                  slotlist[i+slotbase] = segment == 0xff ? segment : (segment + segment_offset);
               }
            }

            // read in segment lists
            {
               unsigned int nsegs = fgetc(fd);
               segmentlist.resize(nsegs+segment_offset);

               for(unsigned int i=0; i<nsegs; i++)
               {
                  std::vector<Uint8>& cursegment = segmentlist[i+segment_offset];
                  cursegment.resize(8);

                  for(unsigned int j=0; j<8; j++)
                  {
                     Uint8 frame = fgetc(fd);

                     cursegment[j] = frame == 0xff ? frame : (frame + frame_offset);
                  }
               }

               segment_offset += nsegs;
            }

            // read auxiliary palette to use
            {
               unsigned int nauxpals = fgetc(fd);

               if (used_auxpal>nauxpals)
                  used_auxpal=0; // wrong palette given; should not happen

               // skip to used palette
               long nextdata = ftell(fd) + nauxpals*32;

               if (used_auxpal>0)
                  fseek(fd,used_auxpal*32,SEEK_CUR);

               fread(auxpal,32,1,fd);

               fseek(fd,nextdata,SEEK_SET);
            }
         }

         // read in all frames
         {
            unsigned int noffsets = fgetc(fd);
            unsigned int unknown1 = fgetc(fd);

            if (pass==0)
            {
               // determine number of frames in critter
               if (maxframes < noffsets+frame_offset)
                  maxframes = noffsets+frame_offset;
            }

            // read in frame offsets
            std::vector<Uint16> alloffsets;
            alloffsets.resize(noffsets);

            for(unsigned int i=0; i<noffsets; i++)
               alloffsets[i] = fread16(fd);

            for(unsigned int n=0; n<noffsets; n++)
            {
               // seek to frame header
               fseek(fd,alloffsets[n],SEEK_SET);
               unsigned int width,height,hotx,hoty,type;
               width = fgetc(fd);
               height = fgetc(fd);
               hotx = fgetc(fd);
               hoty = fgetc(fd);
               type = fgetc(fd);

#ifdef DO_CRITLOAD_DEBUG
               // check if hotspot is outside image
               if (hotx>width) hotx = width;
               if (hoty>height) hoty = height;
#endif

               if (pass==0)
               {
                  // pass 0: determine maximum frame image width

                  if (xres<width) xres = width;
                  if (yres<height) yres = height;
               }
               else
               {
                  // pass 1: read in actual image
                  unsigned int curframe = frame_offset+n;

                  imgsizes[curframe*2+0] = width;
                  imgsizes[curframe*2+1] = height;

                  hotxy_coords[curframe*2+0] = hotx;
                  hotxy_coords[curframe*2+1] = hoty;

                  Uint16 datalen = fread16(fd);

                  // rle-decode image
                  unsigned int bytes_offset = curframe*xres*yres;

                  ua_image_decode_rle(fd, &allframe_bytes[bytes_offset],
                     type==6 ? 5 : 4, datalen, width*height, auxpal,
                     xres-width, width);

#ifdef DO_CRITLOAD_DEBUG
                  // mark hotspot with a green and a red dot
                  allframe_bytes.get()[bytes_offset + hoty*xres + hotx] = 0xfd;
                  allframe_bytes.get()[bytes_offset + hoty*xres + hotx-1] = 0x14;
#endif
               }
               // end of current frame
            }
            // end of all frames

            frame_offset += noffsets;
         }

         // close pagefile
         fclose(fd);

         // end of page file
      }
      // end of all page files
   }
   // end of all passes
}


void ua_uw1_import::load_critters(std::vector<ua_critter>& allcritters,
   ua_settings& settings, ua_palette256_ptr palette0)
{
   allcritters.clear();
   allcritters.resize(0x0040);

   ua_trace("loading all uw1 critter animations ... ");
   unsigned int now = SDL_GetTicks();

   // load infos from "assoc.anm"
   char animnames[32][8];

   // open assoc.anm file
   std::string assocname(settings.get_string(ua_setting_uw_path));
   assocname.append(
      settings.get_bool(ua_setting_uw1_is_uw_demo) ? "crit/dassoc.anm" : "crit/assoc.anm");

   FILE* assoc = fopen(assocname.c_str(),"rb");
   if (assoc==NULL)
      throw ua_exception("could not find assoc.anm");

   // get animation names
   fread(animnames,32,8,assoc);

#ifdef OMIT_1ST_PASS
   unsigned int critres[64] =
   {
      68,58, 51,55, 70,51, 33,21, 39,54, 54,25, 60,46, 52,54, 39,48, 54,17,
      43,49, 49,47, 62,67, 44,52, 63,58, 60,56, 62,31, 67,50, 45,30, 43,60,
      44,56, 71,54, 37,19, 23,17, 95,73, 70,32, 67,50, 59,53, 69,54, 40,54,
      57,73, 90,79
   };

   unsigned int critframes[32] =
   {
      83, 65, 66, 34, 69, 49, 45, 63, 
      61, 38, 48, 70, 36, 38, 80, 47, 
      64, 63, 49, 53, 49, 63, 41, 8, 
      14, 81, 63, 68, 30, 69, 6, 8
   };
#endif

   // read in critter anim and auxpal values
   for(unsigned int i=0; i<64; i++)
   {
      // get anim and auxpal values
      Uint8 anim = fgetc(assoc);
      Uint8 auxpal = fgetc(assoc);

      ua_critter& critter = allcritters[i];

      // construct critter filename base
      if (animnames[anim][0]!=0)
      {
         std::string critfile(settings.get_string(ua_setting_uw_path));

         char buffer[16];
         sprintf(buffer,"crit/cr%02opage",anim); // yeah, octal!
         critfile.append(buffer);

//         ua_trace("loading critter %02x from cr%02opage.*, auxpal=%02x\n",
//            i,anim,auxpal);

#ifdef OMIT_1ST_PASS
         critter.xres = critres[anim*2+0];
         critter.yres = critres[anim*2+1];
         critter.maxframes = critframes[anim];
#endif
         // load it
         load_critter(critter,critfile.c_str(),auxpal);

         critter.set_palette(palette0);
      }
   }

   fclose(assoc);

   ua_trace("done, needed %u ms\n",SDL_GetTicks()-now);
}
