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
/*! \file critter.hpp

   \brief critter animation loading

*/

// include guard
#ifndef __uwadv_critter_hpp_
#define __uwadv_critter_hpp_

// needed includes
#include "image.hpp"
#include "settings.hpp"


// classes

//! critter animations
class ua_critter
{
public:
   //! ctor
   ua_critter(){}

   //! loads critter animations
   void load(const char* file, unsigned int used_auxpal);

protected:
   //! slot list with segment indices
   std::vector<Uint8> slotlist;

   //! segment list with list of used frames
   std::vector<std::vector<Uint8> > segmentlist;

   //! all critter animation frames
   ua_image_list allframes;

   //! hotspot point in image
   unsigned int hotspot_x, hotspot_y;
};


//! critter pool with all critter animations
class ua_critter_pool
{
public:
   //! ctor
   ua_critter_pool(){}

   //! loads critter images
   void load(ua_settings& settings);

protected:
   //! vector with critter animation imagelists
   std::vector<ua_critter> allcritters;
};

#endif
