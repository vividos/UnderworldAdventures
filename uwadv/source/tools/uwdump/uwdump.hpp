/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2003 Underworld Adventures Team

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
/*! \file uwdump.hpp

   \brief uw1 and uw2 dump program main header

*/

// include guard
#ifndef uwdump_uwdump_hpp_
#define uwdump_uwdump_hpp_


// functions

extern Uint16 fread16(FILE* fd);
extern Uint32 fread32(FILE* fd);

//! returns bits from a 16-bit value
inline Uint16 get_bits(Uint16 value, unsigned int start, unsigned numbits)
{
   return (value >> start) & ((1L<<numbits)-1);
}


// some global variables

extern bool is_uw2;

extern std::string basepath;
extern std::string param;

#endif
