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
/*! \file savegame.hpp

   \brief savegame class

*/

// include guard
#ifndef __uwadv_savegame_hpp_
#define __uwadv_savegame_hpp_

// needed includes


// classes

//! savegame class
class ua_savegame
{
public:
   // savegame loading functions

   //! returns version of savegame to load
   Uint32 get_version();

   //! reads a 8-bit value
   Uint8 read8();

   //! reads a 16-bit value
   Uint16 read16();

   //! reads a 32-bit value
   Uint32 read32();

   // savegame saving functions

   //! writes a 8-bit value
   void write8(Uint8 value);

   //! writes a 16-bit value
   void write16(Uint16 value);

   //! writes a 32-bit value
   void write32(Uint32 value);

   // common functions

   //! starts new section to read/write
   void begin_section(const char* section_name);

   //! ends current section
   void end_section();

   //! finally closes savegame file
   void close();

protected:
   //! ctor
   ua_savegame(){}
};

#endif
