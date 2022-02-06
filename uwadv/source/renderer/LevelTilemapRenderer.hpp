//
// Underworld Adventures - an Ultima Underworld remake project
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
/// \file LevelTilemapRenderer.hpp
/// \brief renderer for the tilemap of a level
//
#pragma once

#include "Quadtree.hpp"
#include "GeometryProvider.hpp"

namespace Underworld
{
   class Level;
}
class TextureManager;

/// level tile renderer class
class LevelTilemapRenderer
{
public:
   /// ctor
   LevelTilemapRenderer(const Underworld::Level& level,
      TextureManager& m_textureManager);

   /// renders a single tile
   void RenderTile(unsigned int xpos, unsigned int ypos);

private:
   /// ref to level
   const Underworld::Level& m_level;

   /// texture manager to use
   TextureManager& m_textureManager;

   /// geometry provider for level
   Physics::GeometryProvider m_geometryProvider;
};
