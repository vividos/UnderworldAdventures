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
/*! \file critloader.cpp

   \brief critter images loading implementation

   for more infos about critters, see file docs/uw-formats.txt

*/

// needed includes
#include "common.hpp"
#include "critter.hpp"
#include "fread_endian.hpp"


// external functions

extern void ua_image_decode_rle(FILE *fd,std::vector<Uint8> &pixels,unsigned int bits,
   unsigned int datalen,unsigned int maxpix,unsigned char *auxpalidx);


// ua_critter methods

void ua_critter::load(const char* file, unsigned int used_auxpal)
{
   std::vector<ua_image>& allframes_list = allframes.get_allimages();

   unsigned int maxleft,maxright,maxtop,maxbottom;
   maxleft = maxright = maxtop = maxbottom = 0;

   ua_image img_frame;

   // do 2-pass loading:
   // pass 0:  determine max. frame image size
   // pass 1:  load frame images
   for(unsigned int pass=0; pass<2; pass++)
   {
      unsigned int curpage = 0;
      unsigned int segment_offset = 0;
      unsigned int frame_offset = 0;

      if (pass==1)
      {
         // pass 1: init frame image
         img_frame.create(maxleft+maxright,maxtop+maxbottom);
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
                  slotlist[i+slotbase] = fgetc(fd) + segment_offset;
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
                     cursegment[j] = fgetc(fd) + frame_offset;
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

            if (pass==1)
            {
               // resize image list, to gain a little performance
               allframes_list.resize(1/*noffsets+frame_offset*/);
            }

            // read in frame offsets
            std::vector<Uint16> alloffsets;
            alloffsets.resize(noffsets);

            for(unsigned int i=0; i<noffsets; i++)
               alloffsets[i] = fread16(fd);

            for(unsigned int n=0; n<noffsets; n++)
            {
               if (pass==1 && n+frame_offset>0)
                  break; // only load first frame - for now

               // seek to frame header
               fseek(fd,alloffsets[n],SEEK_SET);

               unsigned int width,height,hotx,hoty,type;
               width = fgetc(fd);
               height = fgetc(fd);
               hotx = fgetc(fd);
               hoty = fgetc(fd);
               type = fgetc(fd);

               // check if hotspot is in image
               if (hotx>width) hotx = width;
               if (hoty>height) hoty = height;

               if (pass==0)
               {
                  // pass 0: calculate maximum frame image width

                  unsigned int left,right,top,bottom;
                  right = width-hotx;
                  left = hotx;
                  top = hoty;
                  bottom = height-hoty;

                  // determine max width needed for this frame
                  if (right > maxright) maxright = right;
                  if (left > maxleft) maxleft = left;
                  if (top > maxtop) maxtop = top;
                  if (bottom > maxbottom) maxbottom = bottom;
               }
               else
               {
                  // pass 1: read in actual image

                  Uint16 datalen = fread16(fd);

                  ua_image img_temp;
                  img_temp.create(width,height);

                  // rle-decode image
                  ua_image_decode_rle(fd,img_temp.get_pixels(),
                     type==6 ? 5 : 4, datalen, width*height, auxpal);

                  // copy to frame
                  img_frame.clear();

                  // paste to frame
                  img_frame.paste_image(img_temp,
                     maxleft-hotx, maxtop-hoty);

                  allframes_list[frame_offset+n] = img_frame;
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

   // remember new hotspot
   hot_x = maxleft;
   hot_y = maxtop;
}

void ua_critter_pool::load(ua_settings& settings)
{
   ua_trace("loading all critter animations ... ");
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

   // read in critter anim and auxpal values
   for(int i=0; i<64; i++)
   {
      // get anim and auxpal values
      Uint8 anim = fgetc(assoc);
      Uint8 auxpal = fgetc(assoc);

      ua_critter crit;

      // construct critter filename base
      if (animnames[anim][0]!=0)
      {
         std::string critfile(settings.get_string(ua_setting_uw_path));

         char buffer[16];
         sprintf(buffer,"crit/cr%02opage",anim); // yeah, octal!
         critfile.append(buffer);

         // load it
         crit.load(critfile.c_str(),auxpal);
      }

      // add to rest of the critters
      allcritters.push_back(crit);
   }

   fclose(assoc);

   ua_trace("done, needed %u ms\n",SDL_GetTicks()-now);
}
