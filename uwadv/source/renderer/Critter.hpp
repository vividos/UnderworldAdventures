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
/// \file Critter.hpp
/// \brief critter frames manager
//
#pragma once

#include "IndexedImage.hpp"
#include "Texture.hpp"

namespace Base
{
   class Settings;
   class ResourceManager;
}

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
class Critter
{
public:
   /// ctor
   Critter();

   /// prepares critter textures
   void Prepare();

   /// resets frame preparation
   void ResetPrepare();

   /// returns critter texture index by frame
   unsigned int GetFrame(Uint8 animationState, Uint8 animationFrame)
   {
      return m_segmentList[animationState][animationFrame];
   }

   /// returns texture object for a given frame
   Texture& GetTexture(unsigned int frame);

   /// returns hotspot u coordinate
   double GetHotspotU(unsigned int frame) const
   {
      return double(m_hotspotXYCoordinates[frame * 2 + 0]) / m_allTextures[frame].GetXRes();
   }

   /// returns hotspot v coordinate
   double GetHotspotV(unsigned int frame) const
   {
      return double(m_hotspotXYCoordinates[frame * 2 + 1]) / m_allTextures[frame].GetYRes();
   }

   /// updates animation frame of object
   void UpdateFrame(Underworld::Object& obj);

   /// sets palette
   void SetPalette(Palette256Ptr& palette) { m_palette = palette; }

protected:
   friend Import::CrittersLoader;

   /// slot list with segment indices
   std::vector<Uint8> m_slotList;

   /// segment list with list of used frames
   std::vector<std::vector<Uint8> > m_segmentList;

   /// array with all frame bytes
   std::vector<Uint8> m_allFrameBytes;

   /// indicates if a frame was already uploaded
   std::vector<bool> m_frameUploadedFlags;

   /// frame resolution
   unsigned int m_xres, m_yres;

   /// max. number of frames stored in m_allFrameBytes
   unsigned int m_maxFrames;

   /// width/height for all frames
   std::vector<unsigned int> m_imageSizes;

   /// hotspot x/y coordinates for all frames
   std::vector<unsigned int> m_hotspotXYCoordinates;

   /// texture with all critter images
   std::vector<Texture> m_allTextures;

   /// palette to use
   Palette256Ptr m_palette;
};


/// critter frames manager class
class CritterFramesManager
{
public:
   /// ctor
   CritterFramesManager() {}

   /// initialize frames manager
   void Init(Base::Settings& settings, Base::ResourceManager& resourceManager, ImageManager& imageManager);

   /// resets controlled object frames and prepares new critter objects
   void Prepare(Underworld::ObjectList* mapObjects);

   /// does tick processing
   void Tick(double tickRate);

   /// returns critter object
   Critter& GetCritter(unsigned int index)
   {
      return m_allCritters[index];
   }

protected:
   /// frames per second for critter animations
   static const double s_critterFramesPerSecond;

protected:
   /// vector with critter animations
   std::vector<Critter> m_allCritters;

   /// manages objects indices into master object list which objects to control
   std::vector<Uint16> m_objectIndices;

   /// frame count for objects
   std::vector<double> m_objectFrameCount;

   /// currently managed map objects
   Underworld::ObjectList* m_mapObjects;
};
