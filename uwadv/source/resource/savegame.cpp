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
/*! \file savegame.cpp

   \brief savegame reading/writing implementation

*/

// needed includes
#include "common.hpp"
#include "savegame.hpp"
#include "fread_endian.hpp"
#include <vector>


// constants

//! current version
const Uint32 ua_savegame::current_version = 0;


// ua_savegame methods

Uint32 ua_savegame::get_version()
{
   return save_version;
}

Uint8 ua_savegame::read8()
{
   return fgetc(sg);
}

Uint16 ua_savegame::read16()
{
   return fread16(sg);
}

Uint32 ua_savegame::read32()
{
   return fread32(sg);
}

void ua_savegame::write8(Uint8 value)
{
   fputc(value,sg);
}

void ua_savegame::write16(Uint16 value)
{
   fwrite16(sg,value);
}

void ua_savegame::write32(Uint32 value)
{
   fwrite32(sg,value);
}

void ua_savegame::begin_section(const char* section_name)
{
   if (saving)
      fputs(section_name,sg);
   else
   {
      unsigned int len = strlen(section_name);

      std::vector<char> buffer;
      buffer.resize(len+1);

      fgets(&buffer[0],len+1,sg);
      buffer[len]=0;

      // check if section names are the same
      if (strcmp(&buffer[0],section_name)!=0)
         throw ua_exception("savegame loading: section name mismatch");
   }
}

void ua_savegame::end_section()
{
}

void ua_savegame::close()
{
   fclose(sg);
}

void ua_savegame::open(const char* filename, bool issaving)
{
   saving = issaving;
   sg = fopen(filename, saving ? "wb" : "rb");

   // read version
   if (saving)
      write32(current_version);
   else
      save_version = read32();
}
