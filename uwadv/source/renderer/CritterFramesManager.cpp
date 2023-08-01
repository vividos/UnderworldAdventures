//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2019,2023 Underworld Adventures Team
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
#include "CritterFramesManager.hpp"
#include "Object.hpp"
#include "ObjectList.hpp"
#include "CrittersLoader.hpp"
#include "ImageManager.hpp"

const double CritterFramesManager::s_critterFramesPerSecond = 3.0;

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
      size_t max = m_allCritters.size();
      for (size_t index = 0; index < max; index++)
         m_allCritters[index].ResetPrepare();
   }

   // go through object list and check which object frames have to be managed
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
   size_t max = m_objectIndices.size();
   for (size_t index = 0; index < max; index++)
   {
      double& framecount = m_objectFrameCount[index];
      framecount += 1.0 / tickRate;
      if (framecount > 1.0 / s_critterFramesPerSecond)
      {
         // next frame
         framecount -= 1.0 / s_critterFramesPerSecond;

         Uint16 objectIndex = m_objectIndices[index];
         Underworld::ObjectPtr obj = m_mapObjects->GetObject(objectIndex);

         // update
         m_allCritters[(obj->GetObjectInfo().m_itemID - 0x0040) % 0x003f].UpdateFrame(*obj);
      }
   }
}
