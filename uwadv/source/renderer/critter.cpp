//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2005,2019 Underworld Adventures Team
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
/// \file critter.cpp
/// \brief critter frames manager implementation
//
#include "common.hpp"
#include "critter.hpp"
#include "object.hpp"
#include "objectlist.hpp"
#include "crittersloader.hpp"

const double ua_critter_frames_manager::critter_fps = 3.0;

ua_critter::ua_critter()
   :xres(0), yres(0), maxframes(0)
{
}

/// Prepares textures for this critter.
void ua_critter::prepare()
{
   reset_prepare();

   tex.resize(maxframes);

   for (unsigned int i = 0; i < maxframes; i++)
   {
      ua_texture& curtex = tex[i];
      curtex.init(1);
   }

   frame_uploaded.clear();
   frame_uploaded.resize(maxframes, false);
}

void ua_critter::reset_prepare()
{
   tex.clear();
}

ua_texture& ua_critter::get_texture(unsigned int frame)
{
   //UaAssert(frame < frame_uploaded.size() && frame != 0xFF);
   if (frame == 0xff)
      frame = 0;

   if (!frame_uploaded[frame])
   {
      frame_uploaded[frame] = true;

      tex[frame].convert(&allframe_bytes[frame*xres*yres],
         xres, yres, *palette.get(), 0);

      tex[frame].upload(0, false);
      // using mipmapped textures (2nd param "true") disables the alpha
      // channel somehow; might be a driver problem
   }
   return tex[frame];
}

/// Updates animation frame for given object. When the end of the animation is
/// reached, it restarts with the first frame of the animation state.
/// \param obj object to modify animframe
void ua_critter::update_frame(Underworld::Object& obj)
{
   Underworld::NpcInfo& info = obj.GetNpcObject().GetNpcInfo();
   Uint8& animframe = info.m_animationFrame;
   Uint8& animstate = info.m_animationState;

   ++animframe;

   if (segmentlist[animstate].size() > animframe ||
      segmentlist[animstate][animframe] == 0xff)
      animframe = 0;
}

/// Initializes critter frames manager. Imports all critter frames.
/// \param settings settings to use
/// \param img_manager image manager to load critters
void ua_critter_frames_manager::init(Base::Settings& settings, ImageManager& img_manager)
{
   // load all critters' frames
   Import::CrittersLoader::load_critters(allcritters, settings, img_manager.get_palette(0));
}

/// Prepares all critter frames for all critters in given map.
/// \param new_mapobjects object list with new map objects to prepare
void ua_critter_frames_manager::prepare(Underworld::ObjectList* new_mapobjects)
{
   mapobjects = new_mapobjects;
   object_indices.clear();
   object_framecount.clear();

   if (mapobjects == NULL)
      return;

   // go through all critters and reset them
   {
      unsigned int max = allcritters.size();
      for (unsigned int i = 0; i < max; i++)
         allcritters[i].reset_prepare();
   }

   // go through master object list and check which object frames have to be managed
   Uint16 max = mapobjects->GetObjectListSize();

   for (Uint16 i = 1; i < max; i++)
   {
      const Underworld::ObjectPtr objPtr = mapobjects->GetObject(i);
      if (objPtr == NULL)
         continue;

      const Underworld::Object& obj = *objPtr;

      // NPC object?
      Uint16 item_id = obj.GetObjectInfo().m_itemID;
      if (item_id >= 0x0040 && item_id < 0x0080)
      {
         // remember object list pos
         object_indices.push_back(i);
         object_framecount.push_back(0.0);

         // prepare texture
         allcritters[item_id - 0x0040].prepare();
      }
   }
}

/// Does tick processing for critter frames. Goes through all managed objects
/// and checks if the frame has to be updated for an object.
/// \param tickrate tick rate in ticks/second
void ua_critter_frames_manager::tick(double tickrate)
{
   if (mapobjects == NULL)
      return;

   // check all objects if they have to be updated
   unsigned int max = object_indices.size();
   for (unsigned int i = 0; i < max; i++)
   {
      double& framecount = object_framecount[i];
      framecount += 1.0 / tickrate;
      if (framecount > 1.0 / critter_fps)
      {
         // next frame
         framecount -= 1.0 / critter_fps;

         Uint16 index = object_indices[i];
         Underworld::ObjectPtr obj = mapobjects->GetObject(index);

         // update
         allcritters[(obj->GetObjectInfo().m_itemID - 0x0040) % 0x003f].update_frame(*obj);
      }
   }
}
