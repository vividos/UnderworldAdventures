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


//! forward references
class ua_object;
class ua_object_list;
class ua_texture_manager;


// classes

//! critter animation frames for one critter
class ua_critter
{
public:
   //! ctor
   ua_critter();

   //! prepares critter textures
   void prepare(ua_texture_manager& texmgr);

   //! returns critter texture index by frame
   inline unsigned int get_frame(Uint8 animstate, Uint8 animframe);

   //! returns texture object for a given frame
   inline ua_texture& get_texture(unsigned int frame);

   //! returns hotspot u coordinate
   inline double get_hotspot_u(unsigned int frame);

   //! returns hotspot v coordinate
   inline double get_hotspot_v(unsigned int frame);

   //! updates animation frame of object
   void update_frame(ua_object& obj);

   //! returns palette ptr
   ua_palette256_ptr& get_palette(){ return palette; }

protected:
   //! slot list with segment indices
   std::vector<Uint8> slotlist;

   //! segment list with list of used frames
   std::vector<std::vector<Uint8> > segmentlist;

   //! array with all frame bytes
   ua_smart_ptr<Uint8> allframe_bytes;

   //! frame resolution
   unsigned int xres, yres;

   //! max. number of frames stored in allframe_bytes
   unsigned int maxframes;

   //! width/height for all frames
   std::vector<unsigned int> imgsizes;

   //! hotspot x/y coordinates for all frames
   std::vector<unsigned int> hotxy_coords;

   //! texture with all critter images
   std::vector<ua_texture> tex;

   //! palette to use
   ua_palette256_ptr palette;

   friend class ua_uw_import;
};


//! critter frames manager class
class ua_critter_frames_manager
{
public:
   //! ctor
   ua_critter_frames_manager(){}

   //! initialize frames manager
   void init(ua_settings& settings, ua_image_manager& img_manager);

   //! resets controlled object frames and prepares new critter objects
   void prepare(ua_texture_manager& texmgr, ua_object_list* mapobjects);

   //! does tick processing
   void tick(double time);

   //! returns critter object
   ua_critter& get_critter(unsigned int idx){ return allcritters[idx]; }

protected:
   //! frames per second for critter animations
   static const double critter_fps;

protected:
   //! vector with critter animations
   std::vector<ua_critter> allcritters;

   //! manages objects indices into master object list which objects to control
   std::vector<Uint16> object_indices;

   //! frame count for objects
   std::vector<double> object_framecount;

   //! currently managed map objects
   ua_object_list* mapobjects;
};


// inline methods

unsigned int ua_critter::get_frame(Uint8 animstate, Uint8 animframe)
{
   return segmentlist[animstate][animframe];
}

ua_texture& ua_critter::get_texture(unsigned int frame)
{
   return tex[frame];
}

double ua_critter::get_hotspot_u(unsigned int frame)
{
   return double(hotxy_coords[frame*2+0])/tex[frame].get_xres();
}

double ua_critter::get_hotspot_v(unsigned int frame)
{
   return double(hotxy_coords[frame*2+1])/tex[frame].get_yres();
}


#endif
