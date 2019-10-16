//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2019 Underworld Adventures Team
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
/// \brief tessellator class
//
#pragma once

#include "Triangle3d.hpp"
#include <SDL_opengl.h>
#include <gl/GLU.h>

// under win32, the callback function must have standard calling convention
#ifdef WIN32
#define GL_CALLBACK __stdcall
#else
#define GL_CALLBACK
#endif

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
   const std::vector<Triangle3dTextured>& Tessellate(Uint16 textureNumber);

private:
   /// deleted copy ctor
   PolygonTessellator(const PolygonTessellator&) = delete;
   /// deleted assignment operator
   PolygonTessellator& operator=(const PolygonTessellator&) = delete;

   // static callback functions

   /// called when triangle data begins
   static void GL_CALLBACK OnBeginData(GLenum type,
      PolygonTessellator* This);

   /// called when triangle data ends
   static void GL_CALLBACK OnEndData(PolygonTessellator* This);

   /// called when vertex data is created
   static void GL_CALLBACK OnVertexData(
      Vertex3d* vert, PolygonTessellator* This);

   /// called when new vertices are created, e.g. when subdividing triangles
   static void GL_CALLBACK OnCombinedData(GLdouble coords[3],
      Vertex3d* vertexData[4], GLfloat weight[4], Vertex3d** outputData,
      PolygonTessellator* This);

private:
   /// GLU tessellator object
   GLUtesselator* m_tessellator;

   /// current texture number
   Uint16 m_currentTextureNumber;

   /// current glBegin() parameter type
   GLenum m_currentType;

   /// indicates a fresh triangle start
   bool m_isTriangleStart;

   /// list of polygon vertices; only point 0 of triangle is valid
   std::vector<Vertex3d> m_polygonVertexList;

   /// temporary vertex cache to combine 3 vertices to a triangle
   std::vector<Vertex3d> m_vertexCache;

   /// list with new triangles
   std::vector<Triangle3dTextured> m_triangles;

   /// list of pointer to vertices created using combining
   std::vector<Vertex3d*> m_combinedVertices;
};
