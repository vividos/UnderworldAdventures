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
/*! \file io_endian.hpp

   \brief endian-aware 16- and 32-bit fread/fwrite routines

*/

// include guard
#ifndef uwadv_io_endian_hpp_
#define uwadv_io_endian_hpp_

// needed includes
#include "SDL.h"
#include <cstdio>


// inline functions


// fread/fwrite functions

//! reads a 16-bit int
inline Uint16 fread16(FILE* fd)
{
   Uint16 data;
   fread(&data,1,2,fd);
#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
   data = ua_endian_convert16(data);
#endif
   return data;
}

//! reads a 32-bit int
inline Uint32 fread32(FILE* fd)
{
   Uint32 data;
   fread(&data,1,4,fd);
#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
   data = ua_endian_convert32(data);
#endif
   return data;
}


//! writes a 16-bit int
inline void fwrite16(FILE* fd,Uint16 data)
{
#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
   data = ua_endian_convert16(data);
#endif
   fwrite(&data,1,2,fd);
}

//! writes a 32-bit int
inline void fwrite32(FILE* fd,Uint32 data)
{
#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
   data = ua_endian_convert32(data);
#endif
   fwrite(&data,1,4,fd);
}


#endif
