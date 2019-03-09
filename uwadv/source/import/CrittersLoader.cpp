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
/// \file critloader.cpp
/// \brief critter images loading implementation
/// for more infos about critters, see file docs/uw-formats.txt
//
#include "Import.hpp"
#include "Critter.hpp"
#include "CrittersLoader.hpp"
#include "ResourceManager.hpp"
#include "File.hpp"
#include <SDL.h>

// for optimizing, we omit the first pass in loading frames
// the xres, yres and maxframes members have to be set before calling
// CrittersLoader::LoadCritterFrames()
//#define OMIT_1ST_PASS
#undef OMIT_1ST_PASS

void ImageDecodeRLE(Base::File& file, Uint8* pixels, unsigned int bits,
   unsigned int datalen, unsigned int maxpix, unsigned char* auxPalettes,
   unsigned int padding = 0, unsigned int lineWidth = 0);

void Import::CrittersLoader::LoadCritters(std::vector<Critter>& allCritters,
   Palette256Ptr palette0)
{
   if (m_settings.GetGameType() == Base::gameUw1)
   {
      LoadCrittersUw1(allCritters, palette0);
   }
   else
   {
      LoadCrittersUw2(allCritters, palette0);
   }
}

#ifdef DO_CRITLOAD_DEBUG
extern unsigned int memory_used;
#endif

void Import::CrittersLoader::LoadCritterFrames(Critter& critter, const char* filename,
   unsigned int usedAuxPalette)
{
   unsigned int& xres = critter.m_xres;
   unsigned int& yres = critter.m_yres;
   unsigned int& maxframes = critter.m_maxFrames;
   std::vector<unsigned int>& m_hotspotXYCoordinates = critter.m_hotspotXYCoordinates;
   std::vector<unsigned int>& m_imageSizes = critter.m_imageSizes;
   std::vector<Uint8>& m_allFrameBytes = critter.m_allFrameBytes;
   std::vector<Uint8>& m_slotList = critter.m_slotList;
   std::vector<std::vector<Uint8> >& m_segmentList = critter.m_segmentList;

#ifndef OMIT_1ST_PASS
   xres = yres = 0;
   maxframes = 0;
#endif

   m_hotspotXYCoordinates.clear();
   m_imageSizes.clear();

   // do 2-pass loading:
   // pass 0: determine max. frame image size
   // pass 1: load frame images
#ifdef OMIT_1ST_PASS
   for (unsigned int pass = 1; pass < 2; pass++)
#else
   for (unsigned int pass = 0; pass < 2; pass++)
#endif
   {
      unsigned int curpage = 0;
      unsigned int segment_offset = 0;
      unsigned int frame_offset = 0;

      if (pass == 1)
      {
         // pass 1: init frame image

#ifdef DO_CRITLOAD_DEBUG
         unsigned int used_bytes = maxframes * xres * yres;

         printf("allocating %u frames (%ux%u), %u bytes\n", maxframes, xres, yres, used_bytes);

         memory_used += used_bytes;
#endif

         // allocate needed number of bytes
         m_allFrameBytes.resize(maxframes * xres * yres);
         UaAssert(!m_allFrameBytes.empty());

#if defined(DO_CRITLOAD_DEBUG) && defined(HAVE_DEBUG)
         memset(&m_allFrameBytes[0], 0xcc, maxframes * xres * yres);
#else
         memset(&m_allFrameBytes[0], 0x00, maxframes * xres * yres);
#endif

         m_hotspotXYCoordinates.resize(maxframes * 2, 0);
         m_imageSizes.resize(maxframes * 2, 0);
      }

      while (true)
      {
         // construct next pagefile name
         std::string pagefile = filename;

         char buffer[8];
         snprintf(buffer, sizeof(buffer), ".n%02u", curpage++);
         pagefile.append(buffer);

         // try to open pagefile
         if (!m_resourceManager.IsUnderworldFileAvailable(pagefile.c_str()))
            break; // no more page files

         Base::File file = m_resourceManager.GetUnderworldFile(Base::resourceGameUw, pagefile.c_str());

         // load animation
         Uint8 auxpal[32];

         if (pass == 0)
         {
            // pass 0: just skip over all tables

            // skip over slot and segment lists and all auxpals
            file.Read8();
            unsigned int nslots = file.Read8();
            file.Seek(nslots, Base::seekCurrent);

            unsigned int nsegs = file.Read8();
            file.Seek(nsegs * 8, Base::seekCurrent);

            unsigned int nauxpals = file.Read8();
            file.Seek(nauxpals * 32, Base::seekCurrent);
         }
         else
         {
            // pass 1: read in all tables

            // read in slot list
            {
               unsigned int slotbase = file.Read8();
               unsigned int nslots = file.Read8();

               m_slotList.resize(nslots + slotbase, 0xff);

               // read in segment indices
               for (unsigned int i = 0; i < nslots; i++)
               {
                  Uint8 segment = file.Read8();
                  m_slotList[i + slotbase] = segment == 0xff ? segment : Uint8(segment + segment_offset);
               }
            }

            // read in segment lists
            {
               unsigned int nsegs = file.Read8();
               m_segmentList.resize(nsegs + segment_offset);

               for (unsigned int i = 0; i < nsegs; i++)
               {
                  std::vector<Uint8>& cursegment = m_segmentList[i + segment_offset];
                  cursegment.resize(8);

                  for (unsigned int j = 0; j < 8; j++)
                  {
                     Uint8 frame = file.Read8();

                     cursegment[j] = frame == 0xff ? frame : Uint8(frame + frame_offset);
                  }
               }

               segment_offset += nsegs;
            }

            // read auxiliary palette to use
            {
               unsigned int nauxpals = file.Read8();

               if (usedAuxPalette > nauxpals)
                  usedAuxPalette = 0; // wrong palette given; should not happen

               // skip to used palette
               long nextdata = file.Tell() + nauxpals * 32;

               if (usedAuxPalette > 0)
                  file.Seek(usedAuxPalette * 32, Base::seekCurrent);

               file.ReadBuffer(auxpal, 32);

               file.Seek(nextdata, Base::seekBegin);
            }
         }

         // read in all frames
         {
            unsigned int noffsets = file.Read8();
            unsigned int unknown1 = file.Read8();
            unknown1;

            if (pass == 0)
            {
               // determine number of frames in critter
               if (maxframes < noffsets + frame_offset)
                  maxframes = noffsets + frame_offset;
            }

            // read in frame offsets
            std::vector<Uint16> alloffsets;
            alloffsets.resize(noffsets);

            for (unsigned int i = 0; i < noffsets; i++)
               alloffsets[i] = file.Read16();

            for (unsigned int n = 0; n < noffsets; n++)
            {
               // seek to frame header
               file.Seek(alloffsets[n], Base::seekBegin);
               unsigned int width, height, hotx, hoty, type;
               width = file.Read8();
               height = file.Read8();
               hotx = file.Read8();
               hoty = file.Read8();
               type = file.Read8();

#ifdef DO_CRITLOAD_DEBUG
               // check if hotspot is outside image
               if (hotx > width) hotx = width;
               if (hoty > height) hoty = height;
#endif

               if (pass == 0)
               {
                  // pass 0: determine maximum frame image width

                  if (xres < width) xres = width;
                  if (yres < height) yres = height;
               }
               else
               {
                  // pass 1: read in actual image
                  unsigned int curframe = frame_offset + n;

                  m_imageSizes[curframe * 2 + 0] = width;
                  m_imageSizes[curframe * 2 + 1] = height;

                  m_hotspotXYCoordinates[curframe * 2 + 0] = hotx;
                  m_hotspotXYCoordinates[curframe * 2 + 1] = hoty;

                  Uint16 datalen = file.Read16();

                  // rle-decode image
                  unsigned int bytes_offset = curframe * xres*yres;

                  ImageDecodeRLE(file, &m_allFrameBytes[bytes_offset],
                     type == 6 ? 5 : 4, datalen, width*height, auxpal,
                     xres - width, width);

#ifdef DO_CRITLOAD_DEBUG
                  // mark hotspot with a green and a red dot
                  m_allFrameBytes.get()[bytes_offset + hoty * xres + hotx] = 0xfd;
                  m_allFrameBytes.get()[bytes_offset + hoty * xres + hotx - 1] = 0x14;
#endif
               }
               // end of current frame
            }
            // end of all frames

            frame_offset += noffsets;
         }

         // close pagefile
         file.Close();

         // end of page file
      }
      // end of all page files
   }
   // end of all passes
}

void Import::CrittersLoader::LoadCrittersUw1(std::vector<Critter>& allCritters,
   Palette256Ptr palette0)
{
   allCritters.clear();
   allCritters.resize(0x0040);

   UaTrace("loading all uw1 critter animations ... ");
   unsigned int now = SDL_GetTicks();

   // load infos from "assoc.anm"
   Uint8 animnames[32][8];

   // open assoc.anm file
   const char* assocname =
      m_settings.GetBool(Base::settingUw1IsUwdemo) ? "crit/dassoc.anm" : "crit/assoc.anm";

   Base::File assoc = m_resourceManager.GetUnderworldFile(Base::resourceGameUw, assocname);
   if (!assoc.IsOpen())
      throw Base::Exception("could not find assoc.anm");

   // get animation names
   assoc.ReadBuffer(&animnames[0][0], 32 * 8);

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
   for (unsigned int i = 0; i < 64; i++)
   {
      // get anim and auxpal values
      Uint8 anim = assoc.Read8();
      Uint8 auxpal = assoc.Read8();

      Critter& critter = allCritters[i];

      // construct critter filename base
      if (animnames[anim][0] != 0)
      {

         char critterFile[16];
         snprintf(critterFile, sizeof(critterFile), "crit/cr%02opage", anim); // yeah, octal!

         //UaTrace("loading critter %02x from cr%02opage.*, auxpal=%02x\n",
         //   i,anim,auxpal);

#ifdef OMIT_1ST_PASS
         critter.xres = critres[anim * 2 + 0];
         critter.yres = critres[anim * 2 + 1];
         critter.maxframes = critframes[anim];
#endif
         // load it
         LoadCritterFrames(critter, critterFile, auxpal);

         critter.SetPalette(palette0);
      }
   }

   UaTrace("done, needed %u ms\n", SDL_GetTicks() - now);
}

void Import::CrittersLoader::LoadCrittersUw2(std::vector<Critter>& allCritters,
   Palette256Ptr palette0)
{
   // TODO implement
   allCritters;
   palette0;
}
