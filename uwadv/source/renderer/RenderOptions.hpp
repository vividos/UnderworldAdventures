//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2019,2021 Underworld Adventures Team
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
/// \file RenderOptions.hpp
/// \brief render options
//
#pragma once

/// render options for the renderer
struct RenderOptions
{
   /// creates default render options object
   RenderOptions()
      :m_renderVisibleTilesUsingOctree(true),
      m_useFog(true),
      m_renderBoundingBoxes(false),
      m_renderHiddenObjects(false)
   {
   }

   /// indicates if the renderer should only render visibile tiles, using an
   /// octree and the view frustum to determine visible tiles; when false,
   /// renders all tiles (useful for mapdisp tool)
   bool m_renderVisibleTilesUsingOctree;

   /// indicates if fog should be used to hide distant tiles
   bool m_useFog;

   /// indicates if bounding boxes for 3D objects should be rendered
   bool m_renderBoundingBoxes;

   /// indicates if hidden objects, such as traps, should be rendered
   bool m_renderHiddenObjects;
};
