/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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
/*! \file convloader.cpp

   \brief conversation code and conv. globals loader

*/

// needed includes
#include "common.hpp"
#include "import.hpp"
#include "io_endian.hpp"
#include "conv/codevm.hpp"


//! Loads conversation globals from bglobals.dat or babglobs.dat
/*! When initial is set to true, the file to load only contains size entries,
    and no actual data; all globals are initalized with 0 then.
*/
void ua_uw_import::load_conv_globals(ua_conv_globals& globals,
   ua_settings& settings, const char* folder, bool initial)
{
   std::string bgname(settings.get_string(ua_setting_uw_path));
   bgname.append(folder);
   bgname.append(initial ? "babglobs.dat" : "bglobals.dat");

   // try to open file
   FILE *fd = fopen(bgname.c_str(),"rb");
   if (fd==NULL)
   {
      std::string text("error loading conv. globals file: ");
      text.append(bgname);
      throw ua_exception(text.c_str());
   }

   std::vector< std::vector<Uint16> >& allglobals = globals.get_allglobals();

   // read in all slot/size/[globals] infos
   while (!feof(fd))
   {
      Uint16 slot = fread16(fd);
      Uint16 size = fread16(fd);

      std::vector<Uint16> globals;
      globals.resize(size,0);

      if (!initial)
      {
         // read in globals
         for(Uint16 i=0; i<size; i++)
            globals.push_back(fread16(fd));
      }

      // put globals in allglobals list
      if (slot>allglobals.size())
         allglobals.resize(slot);

      allglobals.insert(allglobals.begin()+slot,globals);
   }

   fclose(fd);
}
