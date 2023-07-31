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
/// \file PolygonTessellator.cpp
/// \brief polygon tessellator implementation
//
#include "pch.hpp"
#include "PolygonTessellator.hpp"
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

PolygonTessellator::PolygonTessellator()
:m_impl(std::make_unique<Impl>())
{
}

PolygonTessellator::~PolygonTessellator()
{
}

/// Tessellates the polygon into triangles with the polygon vertices passed
/// with AddPolygonVertex().
/// \param textureNumber texture number to use when storing triangles
/// \param colorIndex color index to use when storing triangles
/// \param flatShaded
const std::vector<Triangle3dTextured>& PolygonTessellator::Tessellate(
   Uint16 textureNumber, Uint8 colorIndex, bool flatShaded)
{
   return m_impl->Tessellate(m_polygonVertexList, textureNumber, colorIndex, flatShaded);
}

/// Tessellates the polygon into triangles with the polygon vertices passed
/// with AddPolygonVertex().
/// \param textureNumber texture number to use when storing triangles
/// \param colorIndex color index to use when storing triangles
/// \param flatShaded
const std::vector<Triangle3dTextured>& PolygonTessellator::Impl::Tessellate(
   const std::vector<Vertex3d>& polygonVertexList,
   Uint16 textureNumber, Uint8 colorIndex, bool flatShaded)
{
   if (polygonVertexList.size() == 3)
   {
      Triangle3dTextured tri{
         polygonVertexList[0], polygonVertexList[1], polygonVertexList[2],
         textureNumber, colorIndex, flatShaded };

      m_triangles.push_back(tri);

      return m_triangles;
   }

   m_textureNumber = textureNumber;
   m_colorIndex = colorIndex;

   m_vertexCache.clear();

   gluTessProperty(m_tessellator, GLU_TESS_BOUNDARY_ONLY, GL_FALSE);

   // glu callback function typedef
   typedef void (GL_CALLBACK* GLU_CALLBACK)();

   // register callbacks
   gluTessCallback(m_tessellator, GLU_TESS_BEGIN, NULL);
   gluTessCallback(m_tessellator, GLU_TESS_BEGIN_DATA,
      reinterpret_cast<GLU_CALLBACK>(OnBeginData));

   gluTessCallback(m_tessellator, GLU_TESS_END, NULL);
   gluTessCallback(m_tessellator, GLU_TESS_END_DATA,
      reinterpret_cast<GLU_CALLBACK>(OnEndData));

   gluTessCallback(m_tessellator, GLU_TESS_VERTEX, NULL);
   gluTessCallback(m_tessellator, GLU_TESS_VERTEX_DATA,
      reinterpret_cast<GLU_CALLBACK>(OnVertexData));

   gluTessCallback(m_tessellator, GLU_TESS_COMBINE, NULL);
   gluTessCallback(m_tessellator, GLU_TESS_COMBINE_DATA,
      reinterpret_cast<GLU_CALLBACK>(OnCombinedData));

   gluTessBeginPolygon(m_tessellator, this);
   gluTessBeginContour(m_tessellator);

   // put all polygon vertices into tesselator
   size_t max = polygonVertexList.size();
   for (size_t index = 0; index < max; index++)
   {
      const Vertex3d& vertex = polygonVertexList[index];
      GLdouble coords[3] = { vertex.pos.x, vertex.pos.y, vertex.pos.z };

      gluTessVertex(m_tessellator, coords,
         const_cast<Vertex3d*>(&polygonVertexList[index]));
   }

   gluTessEndContour(m_tessellator);
   gluTessEndPolygon(m_tessellator);

   return m_triangles;
}

void GL_CALLBACK PolygonTessellator::Impl::OnBeginData(GLenum type, PolygonTessellator::Impl* This)
{
   This->m_currentType = type;
   This->m_isTriangleStart = true;
}

void GL_CALLBACK PolygonTessellator::Impl::OnEndData(PolygonTessellator::Impl* This)
{
   This->m_currentType = 0;
   This->m_vertexCache.clear();
}

void GL_CALLBACK PolygonTessellator::Impl::OnVertexData(Vertex3d* vert, PolygonTessellator::Impl* This)
{
   if (This->m_vertexCache.size() < 2)
   {
      // collect more vertices
      This->m_vertexCache.push_back(*vert);
      return;
   }

   // construct triangle
   Triangle3dTextured tri{
      This->m_vertexCache[0],
      This->m_vertexCache[1],
      *vert,
      This->m_textureNumber,
      This->m_colorIndex,
      This->m_flatShaded };

   This->m_triangles.push_back(tri);

   // check which vertices to throw away
   switch (This->m_currentType)
   {
   case GL_TRIANGLES: // clear all; new set of vertices
      This->m_vertexCache.clear();
      break;

   case GL_TRIANGLE_STRIP: // erase first vertex, insert second
      This->m_vertexCache.erase(This->m_vertexCache.begin());
      This->m_vertexCache.push_back(*vert);
      break;

   case GL_TRIANGLE_FAN: // delete second vertex, insert second
      This->m_vertexCache.pop_back();
      This->m_vertexCache.push_back(*vert);
      break;
   default:
      break; // should not happen
   }
}

void GL_CALLBACK PolygonTessellator::Impl::OnCombinedData(GLdouble coords[3],
   Vertex3d* vertexData[4], GLfloat weight[4], Vertex3d** out_data,
   PolygonTessellator::Impl* This)
{
   Vertex3d* vert = new Vertex3d;

   vert->pos.Set(coords[0], coords[1], coords[2]);
   vert->u =
      weight[0] * vertexData[0]->u +
      weight[1] * vertexData[1]->u +
      weight[2] * vertexData[2]->u +
      weight[3] * vertexData[3]->u;

   vert->v =
      weight[0] * vertexData[0]->v +
      weight[1] * vertexData[1]->v +
      weight[2] * vertexData[2]->v +
      weight[3] * vertexData[3]->v;

   This->m_combinedVertices.push_back(vert);
   *out_data = vert;
}
