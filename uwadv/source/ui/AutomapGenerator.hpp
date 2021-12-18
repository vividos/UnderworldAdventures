//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2021 Underworld Adventures Team
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
/// \file AutomapGenerator.hpp
/// \brief auto-map generator
//
#pragma once

class IndexedImage;
class ImageManager;

namespace Underworld
{
   class Tilemap;
}

namespace UI
{
   /// generator for automap images from tilemap
   class AutomapGenerator
   {
   public:
      /// ctor
      AutomapGenerator(ImageManager& imageManager, const Underworld::Tilemap& tilemap);

   private:
      /// tilemap to use for generating automap
      const Underworld::Tilemap& m_tilemap;
   };

} // namespace UI
