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
/// \file Critter.cpp
/// \brief critter frames manager implementation
//
#include "pch.hpp"
#include "Critter.hpp"
#include "Object.hpp"
#include "ObjectList.hpp"
#include "CrittersLoader.hpp"

const double CritterFramesManager::s_critterFramesPerSecond = 3.0;

Critter::Critter()
   :m_xres(0), m_yres(0), m_maxFrames(0)
{
}

/// Prepares textures for this critter.
void Critter::Prepare()
{
   ResetPrepare();

   m_allTextures.resize(m_maxFrames);

   for (unsigned int i = 0; i < m_maxFrames; i++)
   {
      Texture& currentTexture = m_allTextures[i];
      currentTexture.Init(1);
   }

   m_frameUploadedFlags.clear();
   m_frameUploadedFlags.resize(m_maxFrames, false);
}

void Critter::ResetPrepare()
{
   m_allTextures.clear();
}

Texture& Critter::GetTexture(unsigned int frame)
{
   //UaAssert(frame < m_frameUploadedFlags.size() && frame != 0xFF);
   if (frame == 0xff)
      frame = 0;

   if (!m_frameUploadedFlags[frame])
   {
      m_frameUploadedFlags[frame] = true;

      m_allTextures[frame].Convert(&m_allFrameBytes[frame * m_xres * m_yres],
         m_xres, m_yres, *m_palette.get(), 0);

      m_allTextures[frame].Upload(0, false);
      // using mipmapped textures (2nd param "true") disables the alpha
      // channel somehow; might be a driver problem
   }

   return m_allTextures[frame];
}

/// Updates animation frame for given object. When the end of the animation is
/// reached, it restarts with the first frame of the animation state.
/// \param obj object to modify animationFrame
void Critter::UpdateFrame(Underworld::Object& obj)
{
   Underworld::NpcInfo& info = obj.GetNpcObject().GetNpcInfo();
   Uint8& animationFrame = info.m_animationFrame;
   Uint8& animationState = info.m_animationState;

   ++animationFrame;

   if (m_segmentList[animationState].size() > animationFrame ||
      m_segmentList[animationState][animationFrame] == 0xff)
      animationFrame = 0;
}

/// Initializes critter frames manager. Imports all critter frames.
/// \param settings settings to use
/// \param resourceManager resource manager to use
/// \param imageManager image manager to load critters
void CritterFramesManager::Init(Base::Settings& settings, Base::ResourceManager& resourceManager, ImageManager& imageManager)
{
   // load all critters' frames
   Import::CrittersLoader loader{ settings, resourceManager };
   loader.LoadCritters(m_allCritters, imageManager.GetPalette(0));
}

/// Prepares all critter frames for all critters in given map.
/// \param new_mapobjects object list with new map objects to prepare
void CritterFramesManager::Prepare(Underworld::ObjectList* mapObjects)
{
   m_mapObjects = mapObjects;
   m_objectIndices.clear();
   m_objectFrameCount.clear();

   if (m_mapObjects == NULL)
      return;

   // go through all critters and reset them
   {
      unsigned int max = m_allCritters.size();
      for (unsigned int i = 0; i < max; i++)
         m_allCritters[i].ResetPrepare();
   }

   // go through master object list and check which object frames have to be managed
   Uint16 max = m_mapObjects->GetObjectListSize();

   for (Uint16 i = 1; i < max; i++)
   {
      const Underworld::ObjectPtr objPtr = m_mapObjects->GetObject(i);
      if (objPtr == NULL)
         continue;

      const Underworld::Object& obj = *objPtr;

      // NPC object?
      Uint16 item_id = obj.GetObjectInfo().m_itemID;
      if (item_id >= 0x0040 && item_id < 0x0080)
      {
         // remember object list pos
         m_objectIndices.push_back(i);
         m_objectFrameCount.push_back(0.0);

         // prepare texture
         m_allCritters[item_id - 0x0040].Prepare();
      }
   }
}

/// Does tick processing for critter frames. Goes through all managed objects
/// and checks if the frame has to be updated for an object.
/// \param tickRate tick rate in ticks/second
void CritterFramesManager::Tick(double tickRate)
{
   if (m_mapObjects == NULL)
      return;

   // check all objects if they have to be updated
   unsigned int max = m_objectIndices.size();
   for (unsigned int i = 0; i < max; i++)
   {
      double& framecount = m_objectFrameCount[i];
      framecount += 1.0 / tickRate;
      if (framecount > 1.0 / s_critterFramesPerSecond)
      {
         // next frame
         framecount -= 1.0 / s_critterFramesPerSecond;

         Uint16 index = m_objectIndices[i];
         Underworld::ObjectPtr obj = m_mapObjects->GetObject(index);

         // update
         m_allCritters[(obj->GetObjectInfo().m_itemID - 0x0040) % 0x003f].UpdateFrame(*obj);
      }
   }
}
