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
/*! \file critter.cpp

   \brief critter image handling implementation

*/

// needed includes
#include "common.hpp"
#include "critter.hpp"


// ua_critter methods

void ua_critter::prepare(ua_texture_manager& texmgr)
{
   unsigned int max = allframes.size();

   tex.init(&texmgr,max);

   for(unsigned int i=0; i<max; i++)
   {
      tex.convert(allframes.get_image(i),i);
      tex.use(i);
      tex.upload(false);
      // using true for mipmapped textures disabled the alpha channel somehow
      // might be a driver problem
   }
}


// ua_critter_pool methods

void ua_critter_pool::prepare(ua_texture_manager& texmgr)
{
   unsigned int max = allcritters.size();
   for(unsigned int i=0; i<max; i++)
      allcritters[i].prepare(texmgr);
}
