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
/*! \file critter.cpp

   \brief critter frames manager implementation

*/

// needed includes
#include "common.hpp"
#include "critter.hpp"
#include "objects.hpp"
#include "import.hpp"


// constants

const double ua_critter_frames_manager::critter_fps = 3.0;


// ua_critter methods

ua_critter::ua_critter()
:xres(0), yres(0), maxframes(0), allframe_bytes(NULL)
{
}

/*! Prepares textures for this critter. */
void ua_critter::prepare()
{
   tex.resize(maxframes);

   for(unsigned int i=0; i<maxframes; i++)
   {
      ua_texture& curtex = tex[i];

      curtex.init(1);

      unsigned int offset = 0;
      curtex.convert(&allframe_bytes.get()[i*xres*yres],
         xres,yres, *palette.get(), 0);

      curtex.upload(0,false);
      // using mipmapped textures (2nd param "true") disables the alpha
      // channel somehow; might be a driver problem
   }
}

/*! Updates animation frame for given object. When the end of the animation is
    reached, it restarts with the first frame of the animation state.

    \param obj object to modify animframe
*/
void ua_critter::update_frame(ua_object& obj)
{
   Uint8& animframe = obj.get_ext_object_info().animframe;
   Uint8& animstate = obj.get_ext_object_info().animstate;

   ++animframe;

   if (segmentlist[animstate].size() > animframe ||
       segmentlist[animstate][animframe] == 0xff)
      animframe = 0;
}


// ua_critter_frames_manager methods

/*! Initializes critter frames manager. Imports all critter frames.

    \param settings settings to use
    \param img_manager image manager to load critters
*/
void ua_critter_frames_manager::init(ua_settings& settings, ua_image_manager& img_manager)
{
   // load all critters' frames
   ua_uw_import import;
   import.load_critters(allcritters,settings,img_manager.get_palette(0));
}

/*! Prepares all critter frames for all critters in given map.

    \param new_mapobjects object list with new map objects to prepare
*/
void ua_critter_frames_manager::prepare(ua_object_list* new_mapobjects)
{
   mapobjects = new_mapobjects;
   object_indices.clear();
   object_framecount.clear();

   if (mapobjects == NULL)
      return;

   // go through master object list and check which object frames have to be managed
   const std::vector<ua_object>& objlist = mapobjects->get_master_obj_list();

   unsigned int max = objlist.size();
   for(unsigned int i=0; i<max; i++)
   {
      const ua_object& obj = objlist[i];

      // NPC object?
      Uint16 item_id = obj.get_object_info().item_id;
      if (item_id >= 0x0040 && item_id < 0x0080)
      {
         // remember object list pos
         object_indices.push_back(i);
         object_framecount.push_back(0.0);

         // prepare texture
         allcritters[item_id-0x0040].prepare();
      }
   }
}

/*! Does tick processing for critter frames. Goes through all managed objects
    and checks if the frame has to be updated for an object.

    \param tickrate tick rate in ticks/second
*/
void ua_critter_frames_manager::tick(double tickrate)
{
   if (mapobjects == NULL)
      return;

   // check all objects if they have to be updated
   unsigned int max = object_indices.size();
   for(unsigned int i=0; i<max; i++)
   {
      double& framecount = object_framecount[i];
      framecount += 1.0/tickrate;
      if (framecount > 1.0/critter_fps)
      {
         // next frame
         framecount -= 1.0/critter_fps;

         Uint16 index = object_indices[i];
         ua_object& obj = mapobjects->get_object(index);

         // update
         allcritters[(obj.get_object_info().item_id-0x0040)%0x003f].update_frame(obj);
      }
   }
}
