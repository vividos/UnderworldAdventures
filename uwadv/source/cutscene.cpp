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
/*! \file cutscene.cpp

   cutscene frame extraction

*/

// needed includes
#include "common.hpp"
#include "cutscene.hpp"


// global functions

void ua_extract_data(Uint8 *src,Uint8 *dst,unsigned int maxpix)
{
   while(maxpix>0)
   {
      Sint8 cnt = (Sint8)*src++;

      if (cnt>0)
      {
         // dump
         while (cnt>0)
         {
            *dst++ = *src++;
            cnt--;
            maxpix--;
         }
      }
      else if (cnt==0)
      {
         // run
         Uint8 wordCnt = *src++;
         Uint8 pixel = *src++;
         maxpix -= wordCnt;
         while(wordCnt>0)
         {
            *dst++ = pixel;
            wordCnt--;
         }
      }
      else
      {
         cnt &= 0x7f;
         if (cnt != 0)
         {
            // shortSkip
            dst += cnt;
            maxpix -= cnt;
         }
         else
         {
            // longOp
            Uint16 wordCnt = *((Uint16*)src);
            src+=2;

            if ((Sint16)wordCnt <= 0)
            {
               // notLongSkip
               if (wordCnt == 0)
               {
                  break; // end loop
               }

               wordCnt &= 0x7fff; // Remove sign bit.
               if (wordCnt >= 0x4000)
               {
                  // longRun
                  wordCnt -= 0x4000; // Clear "longRun" bit
                  Uint8 pixel = *src++;
                  maxpix -= wordCnt;
                  while(wordCnt>0)
                  {  
                     *dst++ = pixel;
                     wordCnt--;
                  }
               }
               else
               {
                  // longDump
                  maxpix -= wordCnt;
                  while(wordCnt>0)
                  {
                     *dst++ = *src++;
                     wordCnt--;
                  }
               }
            }
            else
            {
               // longSkip
               maxpix -= wordCnt;
               dst += wordCnt;
            }
         }
      }
   }
}

const Uint8 *ua_cutscene::get_frame(unsigned int framenum)
{
   // first, search large page to use
   int i=0;
   for(;i<largepages; i++)
      if(lpdarray[i].base <= framenum && lpdarray[i].base + lpdarray[i].records > framenum)
         break;

   if (i>=largepages)
      throw ua_exception("could not find frame in large pages");

   // calculate large page pointer
   Uint16 *curlp16 = reinterpret_cast<Uint16*>(&lpages[0x10000*i]);
   Uint8 *curlp = reinterpret_cast<Uint8*>(curlp16);

   unsigned int destframe = framenum - lpdarray[i].base;

   // calculate offset to proper record
   unsigned int offset=0;
#if (SDL_BYTEORDER==SDL_BIG_ENDIAN)
   for(unsigned int j=0; j<destframe; j++)
      offset += ua_endian_convert16(curlp16[j+4]);
#else
   for(unsigned int j=0; j<destframe; j++)
      offset += curlp16[j+4];
#endif

   // calculate start of "record" struct
   Uint8 *src = curlp+8+lpdarray[i].records*2+offset;
   Uint16 *src16 = reinterpret_cast<Uint16*>(src);

   // add extra offset
   if(src[1])
      src += ( src16[1] + ( src16[1] & 1 ));

   ua_extract_data(&src[4],&outbuffer[0],width*height);

   return &outbuffer[0];
}
