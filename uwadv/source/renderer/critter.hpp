//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file critter.hpp
/// \brief critter frames manager
//
#pragma once

#include "settings.hpp"
#include "indexedimage.hpp"
#include "texture.hpp"

namespace Underworld
{
   class Object;
   class ObjectList;
}
namespace Import
{
   class CrittersLoader;
}

/// critter animation frames for one critter
class ua_critter
{
public:
   /// ctor
   ua_critter();

   /// prepares critter textures
   void prepare();

   /// resets frame preparation
   void reset_prepare();

   /// returns critter texture index by frame
   unsigned int get_frame(Uint8 animstate, Uint8 animframe)
   {
      return segmentlist[animstate][animframe];
   }

   /// returns texture object for a given frame
   ua_texture& get_texture(unsigned int frame);

   /// returns hotspot u coordinate
   double get_hotspot_u(unsigned int frame) const
   {
      return double(hotxy_coords[frame * 2 + 0]) / tex[frame].get_xres();
   }

   /// returns hotspot v coordinate
   double get_hotspot_v(unsigned int frame) const
   {
      return double(hotxy_coords[frame * 2 + 1]) / tex[frame].get_yres();
   }

   /// updates animation frame of object
   void update_frame(Underworld::Object& obj);

   /// sets palette
   void set_palette(Palette256Ptr& pal) { palette = pal; }

protected:
   friend Import::CrittersLoader;

   /// slot list with segment indices
   std::vector<Uint8> slotlist;

   /// segment list with list of used frames
   std::vector<std::vector<Uint8> > segmentlist;

   /// array with all frame bytes
   std::vector<Uint8> allframe_bytes;

   /// indicates if a frame was already uploaded
   std::vector<bool> frame_uploaded;

   /// frame resolution
   unsigned int xres, yres;

   /// max. number of frames stored in allframe_bytes
   unsigned int maxframes;

   /// width/height for all frames
   std::vector<unsigned int> imgsizes;

   /// hotspot x/y coordinates for all frames
   std::vector<unsigned int> hotxy_coords;

   /// texture with all critter images
   std::vector<ua_texture> tex;

   /// palette to use
   Palette256Ptr palette;
};


/// critter frames manager class
class ua_critter_frames_manager
{
public:
   /// ctor
   ua_critter_frames_manager() {}

   /// initialize frames manager
   void init(Base::Settings& settings, ImageManager& img_manager);

   /// resets controlled object frames and prepares new critter objects
   void prepare(Underworld::ObjectList* mapobjects);

   /// does tick processing
   void tick(double tickrate);

   /// returns critter object
   ua_critter& get_critter(unsigned int idx)
   {
      return allcritters[idx];
   }

protected:
   /// frames per second for critter animations
   static const double critter_fps;

protected:
   /// vector with critter animations
   std::vector<ua_critter> allcritters;

   /// manages objects indices into master object list which objects to control
   std::vector<Uint16> object_indices;

   /// frame count for objects
   std::vector<double> object_framecount;

   /// currently managed map objects
   Underworld::ObjectList* mapobjects;
};
