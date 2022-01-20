//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2003,2004,2019,2022 Underworld Adventures Team
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
/// \file CrittersLoader.hpp
/// \brief critters loader class
//
#pragma once

#include "IndexedImage.hpp"

class Font;
class IndexedImage;
struct LargePageDescriptor;
class Critter;

namespace Base
{
   class Settings;
   class ResourceManager;
}

namespace Import
{
   class CrittersLoader
   {
   public:
      /// ctor
      CrittersLoader(Base::Settings& settings, Base::ResourceManager& resourceManager)
         :m_settings(settings),
         m_resourceManager(resourceManager)
      {
      }

      /// loads all critters
      void LoadCritters(std::vector<Critter>& allCritters,
         Palette256Ptr palette0);

      /// loads all frames of a critter
      void LoadCritterFrames(Critter& critter, const char* filename,
         unsigned int anim, unsigned int usedAuxPalette, bool isUw2);

   private:
      /// loads all uw1 critters
      void LoadCrittersUw1(std::vector<Critter>& allCritters,
         Palette256Ptr palette0);

      /// loads all uw2 critters
      void LoadCrittersUw2(std::vector<Critter>& allCritters,
         Palette256Ptr palette0);

   private:
      /// settings to determine what game type we have
      Base::Settings& m_settings;

      /// resource manager to use for loading
      Base::ResourceManager& m_resourceManager;
   };

} // namespace Import
