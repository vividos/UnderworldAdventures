//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2021,2022,2023 Underworld Adventures Team
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
/// \file GluPolygonTessellatorImpl.hpp
/// \brief polygon tessellator implementation using GLU library
//
#include "pch.hpp"
#include "../PolygonTessellator.hpp"
#include <SDL2/SDL_opengl.h>
#include <gl/GLU.h>

// under win32, the callback function must have standard calling convention
#ifdef WIN32
#define GL_CALLBACK __stdcall
#else
#define GL_CALLBACK
#endif

/// polygon tesselator implementation that uses the GLU library
class PolygonTessellator::Impl
{
public:
   Impl()
   {
      m_tessellator = gluNewTess();
   }

   ~Impl()
   {
      gluDeleteTess(m_tessellator);

      // free vertices created through combining
      size_t max = m_combinedVertices.size();
      for (size_t index = 0; index < max; index++)
         delete m_combinedVertices[index];

      m_combinedVertices.clear();
   }

   /// tesselates given polygon vertex list and returns triangles
   const std::vector<Triangle3dTextured>& Tessellate(
      const std::vector<Vertex3d>& polygonVertexList,
      Uint16 textureNumber, Uint8 colorIndex, bool flatShaded);

private:
   // static callback functions

   /// called when triangle data begins
   static void GL_CALLBACK OnBeginData(GLenum type,
      PolygonTessellator::Impl* This);

   /// called when triangle data ends
   static void GL_CALLBACK OnEndData(PolygonTessellator::Impl* This);

   /// called when vertex data is created
   static void GL_CALLBACK OnVertexData(
      Vertex3d* vert, PolygonTessellator::Impl* This);

   /// called when new vertices are created, e.g. when subdividing triangles
   static void GL_CALLBACK OnCombinedData(GLdouble coords[3],
      Vertex3d* vertexData[4], GLfloat weight[4], Vertex3d** outputData,
      PolygonTessellator::Impl* This);

private:
   /// GLU tessellator object
   GLUtesselator* m_tessellator;

   /// texture number to use for triangles
   Uint16 m_textureNumber = 0;

   /// color index to use for triangles
   Uint8 m_colorIndex = 0;

   /// flat shaded flag, to use for triangles
   bool m_flatShaded = false;

   /// current glBegin() parameter type
   GLenum m_currentType = 0;

   /// indicates a fresh triangle start
   bool m_isTriangleStart = false;

   /// temporary vertex cache to combine 3 vertices to a triangle
   std::vector<Vertex3d> m_vertexCache;

   /// list with new triangles
   std::vector<Triangle3dTextured> m_triangles;

   /// list of pointer to vertices created using combining
   std::vector<Vertex3d*> m_combinedVertices;
};
