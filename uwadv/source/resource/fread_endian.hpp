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
/*! \file fread_endian.hpp

   endian-aware 16-bit and 32-bit fread routines

*/

// include guard
#ifndef __uwadv_fread_endian_hpp_
#define __uwadv_fread_endian_hpp_

// needed includes
#include "SDL.h"
#include <cstdio>


// endian conversion

inline Uint16 ua_endian_convert16(Uint16 x)
{
   return (((x)&0x00ff)<<8) | (((x)&0xff00)>>8);
}

inline Uint32 ua_endian_convert32(Uint32 x)
{
   return ua_endian_convert16(((x)&0xffff0000)>>16) |
      ua_endian_convert16((x)&0x0000ffff)<<16;
}


// inline functions

inline Uint16 fread16(FILE *fd)
{
   Uint16 data;
   fread(&data,1,2,fd);
#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
   data = ua_endian_convert16(data);
#endif
   return data;
}

inline Uint32 fread32(FILE *fd)
{
   Uint32 data;
   fread(&data,1,4,fd);
#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
   data = ua_endian_convert32(data);
#endif
   return data;
}

#endif
