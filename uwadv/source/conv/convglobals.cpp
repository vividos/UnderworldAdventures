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
/*! \file codevm.cpp

   \brief conv globals

*/

// needed includes
#include "common.hpp"
#include "convglobals.hpp"
#include "savegame.hpp"
#include <sstream>


// ua_conv_globals methods

void ua_conv_globals::load_game(ua_savegame &sg)
{
   sg.begin_section("conv.globals");

   allglobals.clear();

   // get number of slots
   Uint16 max1 = sg.read16();

   for(Uint16 i=0; i<max1; i++)
   {
      // get number of slot globals
      Uint16 max2 = sg.read16();

      std::vector<Uint16> slotglobals;

      // get all slot globals
      for(Uint16 j=0; j<max2; j++)
         slotglobals.push_back(sg.read16());

      allglobals.push_back(slotglobals);
   }

   sg.end_section();
}

void ua_conv_globals::save_game(ua_savegame &sg)
{
   sg.begin_section("conv.globals");

   // write number of conv slots
   unsigned int max1 = allglobals.size();
   sg.write16(max1);

   // for each slot ...
   for(unsigned int i=0; i<max1; i++)
   {
      // write number of slot globals
      unsigned int max2 = allglobals[i].size();
      sg.write16(max2);

      // write all globals
      for(unsigned int j=0; j<max2; j++)
         sg.write16(allglobals[i][j]);
   }

   sg.end_section();
}
