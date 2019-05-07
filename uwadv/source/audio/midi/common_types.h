//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019 Michael Fink
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
/// \file common_types.h
/// \brief needed typedefs for Pentagram
//
#pragma once

#include <SDL_types.h>

// derive Pentagram types from SDL types
typedef Uint64 uint64;
typedef Uint32 uint32;
typedef Uint16 uint16;
typedef Uint8 uint8;

typedef Sint64 sint64;
typedef Sint32 sint32;
typedef Sint16 sint16;
typedef Sint8 sint8;

/// \todo mapping is static for now; fails on 64-bit targets; remove uintptr
/// instead
#define SIZEOF_INT 4
#define SIZEOF_INTP 4
#define SIZEOF_LONG 4
#define SIZEOF_LONG_LONG 8


#ifndef EX_TYPE_INTPTR
#  if (SIZEOF_INTP == SIZEOF_INT)
#    define EX_TYPE_INTPTR int
#  elif (SIZEOF_INTP == SIZEOF_LONG)
#    define EX_TYPE_INTPTR long
#  elif (SIZEOF_INTP == SIZEOF_LONG_LONG)
#    define EX_TYPE_INTPTR long long
#  else
#    error "Size of pointer type not equal to int, long or long long"
#  endif
#endif

typedef unsigned EX_TYPE_INTPTR uintptr;
typedef signed EX_TYPE_INTPTR sintptr;

#ifdef DEBUG
#  define COUT(x)       do { std::cout << x << std::endl; std::cout.flush(); } while (0)
#  define CERR(x)       do { std::cerr << x << std::endl; std::cerr.flush(); } while (0)
#else
#  define COUT(x)       do { } while(0)
#  define CERR(x)       do { } while(0)
#endif
