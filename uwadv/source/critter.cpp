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
/*! \file critter.cpp

   \brief critter image handling implementation

*/

// needed includes
#include "common.hpp"
#include "critter.hpp"


// constants

// should make move this to a header
const double critter_fps = 1.0;


// ua_critter methods

ua_critter::ua_critter()
:allframe_bytes(NULL)
{
   animcount = 0.0;
   currentframe = 0;
}

void ua_critter::prepare(ua_texture_manager& texmgr)
{
   tex.init(&texmgr,maxframes);

   for(unsigned int i=0; i<maxframes; i++)
   {
      tex.convert(&allframe_bytes.get()[i*xres*yres],         
         xres,yres,texmgr.get_palette(0),i);

      tex.upload(i,false);
      // using mipmapped textures (2nd param "true") disables the alpha
      // channel somehow; might be a driver problem
   }

   // start with 1st frame of idle animation
   currentanim = slotlist[0x24];
}

unsigned int ua_critter::get_currentframe()
{
   return segmentlist[currentanim][currentframe];
}

void ua_critter::tick(double ticktime)
{
   animcount += ticktime;

   if (animcount > 1.0/critter_fps)
   {
      animcount -= 1.0/critter_fps;
      currentframe++;

      // check if current frame needs to be reset
      if( currentframe >= segmentlist[currentanim].size() ||
          segmentlist[currentanim][currentframe] == 0xff)
      {
         currentframe = 0;

         // do new animation
         Uint8 newslot = 0x23 + unsigned( (rand()*3.0)/double(RAND_MAX) );
         currentanim = slotlist[newslot];
      }
   }
}


// ua_critter_pool methods

void ua_critter_pool::tick(double ticktime)
{
   unsigned int max = allcritters.size();
   for(unsigned int i=0; i<max; i++)
      allcritters[i].tick(ticktime);
}

void ua_critter_pool::prepare(ua_texture_manager& texmgr)
{
   unsigned int max = allcritters.size();
   for(unsigned int i=0; i<max; i++)
      allcritters[i].prepare(texmgr);
}
