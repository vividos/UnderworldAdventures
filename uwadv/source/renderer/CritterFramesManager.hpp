//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2022,2023 Underworld Adventures Team
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

#include "Critter.hpp"

namespace Base
{
   class Settings;
   class ResourceManager;
}
namespace Underworld
{
   class ObjectList;
}

/// critter frames manager class
class CritterFramesManager
{
public:
   /// ctor
   CritterFramesManager()
      :m_mapObjects(nullptr)
   {
   }

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

private:
   /// frames per second for critter animations
   static const double s_critterFramesPerSecond;

   /// vector with critter animations
   std::vector<Critter> m_allCritters;

   /// manages objects indices into object list which objects to control
   std::vector<Uint16> m_objectIndices;

   /// frame count for objects
   std::vector<double> m_objectFrameCount;

   /// currently managed map objects
   Underworld::ObjectList* m_mapObjects;
};
