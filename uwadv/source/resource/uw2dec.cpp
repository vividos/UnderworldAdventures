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
/*! \file uw2dec.cpp

   \brief ultima underworld 2 compression scheme decoding

*/

// needed includes
#include "common.hpp"
#include <vector>
#include "../import/io_endian.hpp"


// global functions

//! frees memory when SDL_RWclose is called
int ua_uw2dec_close(SDL_RWops* rwops)
{
   // free mem and reset pointers
   delete[] rwops->hidden.mem.base;
   rwops->hidden.mem.base = 0;
   rwops->hidden.mem.here = 0;
   rwops->hidden.mem.stop = 0;
   return 0;
}

//! creates SDL_RWops struct from compressed .ark file blocks (uw2 only)
/*! the function reads in compressed blocks from uw2 .ark files and creates
    a SDL_RWops struct to transparently access it.

    \param fd        an open .ark file
    \param blocknum  block number to read in
    \param destsize  number of bytes which should be read from the block
*/
SDL_RWops* ua_rwops_uw2dec(FILE* fd,unsigned int blocknum, unsigned int destsize)
{
   // read in number of blocks
   fseek(fd,0L,SEEK_SET);
   Uint16 nblocks = fread16(fd);

   // read in offset
   fseek(fd,blocknum*4+6,SEEK_SET);
   Uint32 offset = fread32(fd);

   if (offset == 0)
      return NULL; // no block

   // read in flags
   fseek(fd,(blocknum+nblocks)*4+6,SEEK_SET);
   Uint32 flags = fread32(fd);

   // read in real block size
   fseek(fd,(blocknum+nblocks*2)*4+6,SEEK_SET);
   Uint32 blocksize = fread32(fd);

   // read in available block size
   fseek(fd,(blocknum+nblocks*3)*4+6,SEEK_SET);
   Uint32 avail = fread32(fd);

   ua_trace("uw2dec: reading in block %u:\n offset=%06x, flags=%u, blocksize=%04x, avail=%04x, destsize=%04x\n",
      blocknum,offset,flags,blocksize,avail,destsize);

   // read in source data
   std::vector<Uint8> src;
   src.resize(blocksize,0);

   fseek(fd,offset,SEEK_SET);
   fread(&src[0],1,blocksize,fd);

   // allocate destination array
   Uint8* dest = new Uint8[destsize];
   memset(dest,0,destsize);

   // decode uw2 compression scheme
   if ((flags & 2) != 0)
   {
      // compressed data
      Uint8* ubuf = dest;
      Uint8* cp = &src[4];
      Uint8* ce = &src[blocksize];
      Uint8* up = ubuf;
      Uint8* ue = ubuf+destsize;

      while(up<ue && cp<ce)
      {
         unsigned char bits = *cp++;
         //ua_trace(" bits: %02x\n",bits);

         for(int i=0; i<8; i++, bits>>=1)
         {
            if(bits&1)
            {
               //ua_trace("copy byte: %02x (to %04x)\n",*cp,up-ubuf);
               *up++ = *cp++;
            }
            else
            {
               signed int m1 = *cp++; // m1: pos
               signed int m2 = *cp++; // m2: run

               m1 |= (m2&0xF0) << 4;

               // correct for sign bit
               if(m1&0x800)
                  m1 |= 0xFFFFF000;

               // add offsets
               m2 =  (m2&0x0F) + 3;
               m1 += 18;

               if(m1 > up-ubuf)
                  throw ua_exception("uw2dec: pos exceeds buffer!");

               // adjust pos to current 4k segment
               while(m1 < (up-ubuf-0x1000))
                  m1 += 0x1000;

               //ua_trace("copy from: pos: %03x, run: %02x\n",m1,m2);

               while(m2-- && up<ue)
                  *up++ = ubuf[m1++];
            }
            if(up>=ue || cp>=ce)
               break;
         }
      }

      ua_trace(" decoded %04x compressed bytes to %04x uncompressed data.\n",
         cp-&src[4],up-ubuf);
   }
   else
   {
      // uncompressed
      memcpy(dest,&src[0],blocksize);

      ua_trace(" block was uncompressed.\n");
   }

   // create rwops struct from decoded data
   SDL_RWops* rwops = SDL_RWFromMem(dest,destsize);

   // set new close function to free mem
   rwops->close = ua_uw2dec_close;

   return rwops;
}
