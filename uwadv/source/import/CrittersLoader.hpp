//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2003,2004,2019 Underworld Adventures Team
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

class Settings;
class Font;
class IndexedImage;
struct LargePageDescriptor;
class Critter;

namespace Import
{
   class CrittersLoader
   {
   public:
      /// loads all critters
      static void LoadCritters(std::vector<Critter>& allCritters,
         Base::Settings& settings, Palette256Ptr palette0);

      /// loads all frames of a critter
      static void LoadCritterFrames(Critter& critter, const char* filename,
         unsigned int usedAuxPalette);

   private:
      /// loads all uw1 critters
      static void LoadCrittersUw1(std::vector<Critter>& allCritters,
         Base::Settings& settings, Palette256Ptr palette0);

      /// loads all uw2 critters
      static void LoadCrittersUw2(std::vector<Critter>& allCritters,
         Base::Settings& settings, Palette256Ptr palette0);
   };

} // namespace Import
