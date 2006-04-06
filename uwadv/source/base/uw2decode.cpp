/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2006 Michael Fink

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
/*! \file uw2decode.cpp

   \brief Ultima Underworld 2 .ark compression format decoder

*/

// needed includes
#include "base.hpp"
#include "uw2decode.hpp"

using Base::File;
using Base::SDL_RWopsPtr;

namespace Detail
{

//! Data decoding for uw2 compression format
/*! See uw-formats.txt for a detailed description how the data is compressed.
*/
Uint32 Uw2DecodeData(const std::vector<Uint8>& vecSourceData, Uint8* pDestBuffer, Uint32 uiDestSize)
{
   Uint32 uiDestCount = 0;

   if (pDestBuffer == NULL)
      uiDestSize = 1; // when not passing a buffer, just set dest size to 1 to keep loop running

   // compressed data
   Uint8* ubuf = pDestBuffer;
   const Uint8* cp = &vecSourceData[4]; // for some reason the first 4 bytes are not used
   const Uint8* ce = &vecSourceData[vecSourceData.size()];
   Uint8* up = ubuf;
   Uint8* ue = ubuf+uiDestSize;

   while (up < ue && cp < ce)
   {
      unsigned char bits = *cp++;
      //UaTrace(" bits: %02x\n",bits);

      for (int i=0; i<8; i++, bits >>= 1)
      {
         if (bits&1)
         {
            //UaTrace("copy byte: %02x (to %04x)\n",*cp,up-ubuf);
            if (up != NULL)
               *up++ = *cp++;
            else
               cp++;

            uiDestCount++;
         }
         else
         {
            signed int m1 = *cp++; // m1: pos
            signed int m2 = *cp++; // m2: run

            m1 |= (m2&0xF0) << 4;

            // correct for sign bit
            if (m1&0x800)
               m1 |= 0xFFFFF000;

            // add offsets
            m2 =  (m2&0x0F) + 3;
            m1 += 18;

            uiDestCount += m2; // add count

            if (up != NULL)
            {
               if (m1 > up-ubuf)
                  throw Base::RuntimeException("Uw2DecodeData: pos exceeds buffer!");

               // adjust pos to current 4k segment
               while (m1 < (up-ubuf-0x1000))
                  m1 += 0x1000;

               //UaTrace("copy from: pos: %03x, run: %02x\n",m1,m2);

               while (m2-- && up<ue)
                  *up++ = ubuf[m1++];
            }
         }

         // stop if source or destination buffer pointer overrun
         if (up >= ue || cp >= ce)
            break;
      }
   }

//   UaTrace(" decoded %04x compressed bytes to %04x uncompressed data.\n",
//      cp - &vecSourceData[4], uiDestCount);
   return uiDestCount;
}

//! Close callback for SDL_RWops struct; used when reading decoded uw2 data
/*! Frees memory when SDL_RWclose is called
    \param rwops pointer to the rwops struct to be freed
*/
int Uw2DecodeClose(SDL_RWops* rwops)
{
   // free mem and reset pointers
   delete[] rwops->hidden.mem.base;
   rwops->hidden.mem.base = 0;
   rwops->hidden.mem.here = 0;
   rwops->hidden.mem.stop = 0;
   return 0;
}

}

/*! Reads in compressed blocks from uw2 .ark files and creates a file with
    decoded data. Note that the given file must already be at the proper block
    start position.

    The code was adapted from the LoW project: http://low.sourceforge.net/

    \param file file in .ark format, at the block start position
    \param bCompressed indicates if block is actually compressed
    \param uiDataSize number of source bytes in block (either compressed or uncompressed)
*/
Base::File Base::Uw2Decode(const Base::File& file, bool bCompressed, Uint32 uiDataSize)
{
   UaAssert(uiDataSize > 0); // trying to load entry that has size 0?

   // read in source data
   std::vector<Uint8> vecSourceData;
   vecSourceData.resize(uiDataSize);

   Uint8* pDestBuffer = NULL;
   Uint32 uiDestSize = uiDataSize;

   if (bCompressed)
   {
      // read in data
      file.ReadBuffer(&vecSourceData[0], uiDataSize);

      // find out decoded length and allocate memory
      uiDestSize = Detail::Uw2DecodeData(vecSourceData, NULL, 0);

      pDestBuffer = new Uint8[uiDestSize];

      // decode it
      UaVerify(uiDestSize == Detail::Uw2DecodeData(vecSourceData, pDestBuffer, uiDestSize));
   }
   else
   {
      // just copy the data
      pDestBuffer = new Uint8[uiDataSize];
      file.ReadBuffer(pDestBuffer, uiDataSize);
   }

   // create rwops struct from decoded data
   SDL_RWopsPtr rwops = SDL_RWopsPtr(SDL_RWFromMem(pDestBuffer, uiDestSize));

   rwops->close = Detail::Uw2DecodeClose;

   return Base::File(rwops);
}
