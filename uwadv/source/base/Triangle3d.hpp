//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019,2021 Underworld Adventures Team
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
/// \file Triangle3d.hpp
/// \brief triangle class
//
#pragma once

#include "Vertex3d.hpp"

/// \brief textured triangle
/// Represents a textured triangle made up with 3 Vertex3d objects. There
/// also is a m_textureNumber member to specify used texture.
struct Triangle3dTextured
{
   /// default ctor
   Triangle3dTextured()
      :m_textureNumber(0),
      m_colorIndex(0),
      m_flatShaded(false)
   {
   }

   /// ctor, setting vertex elements and properties
   Triangle3dTextured(const Vertex3d& pos1, const Vertex3d& pos2, const Vertex3d& pos3,
      Uint16 textureNumber = 0, Uint8 colorIndex = 0, bool flatShaded = false)
      :m_textureNumber(textureNumber),
      m_colorIndex(colorIndex),
      m_flatShaded(flatShaded)
   {
      m_vertices[0] = pos1;
      m_vertices[1] = pos2;
      m_vertices[2] = pos3;
   }

   /// vertices
   Vertex3d m_vertices[3];

   /// stock texture number used; 0 when not used
   Uint16 m_textureNumber;

   /// color palette index; when 0, use white
   Uint8 m_colorIndex;

   /// indicates if the triangle should be drawn flat-shaded or Gouraud shaded
   bool m_flatShaded;

   /// sets triangle point properties
   void Set(unsigned int point, double x, double y, double z, double u, double v)
   {
      m_vertices[point].pos.Set(x, y, z);
      m_vertices[point].u = u;
      m_vertices[point].v = v;
   }

   /// compare operator for std::sort
   bool operator<(const Triangle3dTextured& tri) const
   {
      return m_textureNumber > tri.m_textureNumber;
   }
};
