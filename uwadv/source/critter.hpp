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
/*! \file critter.hpp

   \brief critter animation loading

*/

// include guard
#ifndef uwadv_critter_hpp_
#define uwadv_critter_hpp_

// needed includes
#include "settings.hpp"
#include "image.hpp"
#include "texture.hpp"


// classes

//! critter animations
class ua_critter
{
public:
   //! ctor
   ua_critter();

   //! loads critter animations
   void load(const char* file, unsigned int used_auxpal);

   //! prepares critter textures
   void prepare(ua_texture_manager& texmgr);

   //! returns critter texture
   inline ua_texture& get_texture();

   //! returns hotspot u coordinate
   inline double get_hotspot_u();

   //! returns hotspot v coordinate
   inline double get_hotspot_v();

   // get current frame of animation
   unsigned int get_currentframe();

   //! called every game tick
   void tick(double ticktime);

protected:
   //! slot list with segment indices
   std::vector<Uint8> slotlist;

   //! segment list with list of used frames
   std::vector<std::vector<Uint8> > segmentlist;

   //! all critter animation frames
   ua_image_list allframes;

   //! hotspot point in image
   unsigned int hot_x, hot_y;

   //! texture with all critter images
   ua_texture tex;

   //! current animation segment
   unsigned int currentanim;

   //! time counter for animated textures
   double animcount;

   //! current frame of animation being played for critter
   unsigned int currentframe;
};


//! critter pool with all critter animations
class ua_critter_pool
{
public:
   //! ctor
   ua_critter_pool(){}

   //! loads critter images
   void load(ua_settings& settings);

   //! prepares all critter textures
   void prepare(ua_texture_manager& texmgr);

   //! returns critter object
   ua_critter& get_critter(unsigned int idx){ return allcritters[idx]; }

   //! called every game tick
   void tick(double ticktime);

protected:
   //! vector with critter animation imagelists
   std::vector<ua_critter> allcritters;
};


// inline methods

ua_texture& ua_critter::get_texture()
{
   return tex;
}

double ua_critter::get_hotspot_u()
{
   return double(hot_x)/tex.get_xres();
}

double ua_critter::get_hotspot_v()
{
   return double(hot_y)/tex.get_yres();
}


#endif
