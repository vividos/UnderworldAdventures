//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2019,2021,2022,2023 Underworld Adventures Team
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
/// \file PolygonTessellator.hpp
/// \brief polygon tessellator class
//
#pragma once

#include "Triangle3d.hpp"

/// polygon tessellator class
class PolygonTessellator
{
public:
   /// ctor
   PolygonTessellator();
   /// dtor
   ~PolygonTessellator();

   /// adds polygon vertex
   void AddPolygonVertex(const Vertex3d& vertex)
   {
      m_polygonVertexList.push_back(vertex);
   }

   /// tessellates the polygon and returns triangles
   const std::vector<Triangle3dTextured>& Tessellate(Uint16 textureNumber,
      Uint8 colorIndex = 0, bool flatShaded = false);

private:
   /// deleted copy ctor
   PolygonTessellator(const PolygonTessellator&) = delete;
   /// deleted assignment operator
   PolygonTessellator& operator=(const PolygonTessellator&) = delete;

private:
   class Impl;

   /// tessellator implementation
   std::unique_ptr<Impl> m_impl;

   /// list of polygon vertices
   std::vector<Vertex3d> m_polygonVertexList;
};
