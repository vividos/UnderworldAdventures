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
/// \file CutsceneLoader.hpp
/// \brief cutscene loader
//
#pragma once

class IndexedImage;
struct LargePageDescriptor;

namespace Base
{
   class ResourceManager;
}

namespace Import
{
   /// cutscene loader
   class CutsceneLoader
   {
   public:
      /// loads cutscene data
      static void LoadCutscene(Base::ResourceManager& resourceManager,
         const char* filename,
         IndexedImage& image,
         std::vector<LargePageDescriptor>& largePageDescriptorList,
         std::vector<Uint8>& largePages,
         unsigned int& numRecords);

      /// extracts cutscene data from source data
      static void ExtractCutsceneData(Uint8* source, Uint8* dest, unsigned int maxPixel);
   };

} // namespace Import
