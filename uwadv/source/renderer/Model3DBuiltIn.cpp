//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2004,2019,2020,2021 Underworld Adventures Team
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
/// \file Model3DBuiltIn.cpp
/// \brief builtin models
//
#include "pch.hpp"
#include "Model3DBuiltIn.hpp"
#include "TextureManager.hpp"
#include "RenderOptions.hpp"

extern bool DecodeBuiltInModels(const char* filename,
   std::vector<Model3DPtr>& allModels, bool dump = false, bool isUw2 = false);

void Model3DBuiltIn::Render(const RenderOptions& renderOptions,
   const Vector3d& viewerPos, const Underworld::Object& object,
   TextureManager& textureManager, Vector3d& base)
{
   glDisable(GL_CULL_FACE);

   size_t maxTriangles = m_triangles.size();

   Vector3d origin;

   for (size_t triangleIndex = 0; triangleIndex < maxTriangles; triangleIndex++)
   {
      const Triangle3dTextured& tri = m_triangles[triangleIndex];

      Uint8 color[3] = { 255, 255, 255 };
      if (tri.m_colorIndex != 0)
      {
         textureManager.GetPaletteColor(tri.m_colorIndex, color[0], color[1], color[2]);
      }

      glBegin(GL_TRIANGLES);

      // generate normal vector
      Vector3d normal, vec1(tri.m_vertices[1].pos), vec2(tri.m_vertices[2].pos);
      vec1 -= tri.m_vertices[0].pos;
      vec2 -= tri.m_vertices[0].pos;

      normal = Vector3d::Cross(vec1, vec2);
      normal.Normalize();
      normal *= -1;

      glNormal3d(normal.x, normal.y, normal.z);

      if (tri.m_textureNumber == 0)
         glDisable(GL_TEXTURE_2D);
      else
      {
         glEnable(GL_TEXTURE_2D);
         textureManager.Use(tri.m_textureNumber);
      }

      for (unsigned index = 0; index < 3; index++)
      {
         Vector3d lightDirection = -viewerPos - (base + tri.m_vertices[index].pos);
         lightDirection.Normalize();

         double diffuseFactor = std::abs(normal.Dot(lightDirection));

         glColor3ub(
            color[0] * diffuseFactor,
            color[1] * diffuseFactor,
            color[2] * diffuseFactor);

         if (tri.m_textureNumber != 0)
            glTexCoord2d(tri.m_vertices[index].u, tri.m_vertices[index].v);

         glVertex3d(
            base.x - origin.x + tri.m_vertices[index].pos.x,
            base.y - origin.y + tri.m_vertices[index].pos.y,
            base.z - origin.z + tri.m_vertices[index].pos.z
         );
      }

      glEnd();
   }

   glEnable(GL_CULL_FACE);
   glColor3ub(192, 192, 192);

   if (renderOptions.m_renderBoundingBoxes)
      DrawExtentsBox(base);
}

void Model3DBuiltIn::DrawExtentsBox(const Vector3d& base)
{
   glColor3ub(255, 255, 255);
   glDisable(GL_TEXTURE_2D);

   Vector3d ext{ m_extents };

   glBegin(GL_LINE_LOOP);
   glVertex3d(base.x + ext.x, base.y + ext.y, base.z + ext.z);
   glVertex3d(base.x + ext.x, base.y + ext.y, base.z + 0);
   glVertex3d(base.x + -ext.x, base.y + ext.y, base.z + 0);
   glVertex3d(base.x + -ext.x, base.y + ext.y, base.z + ext.z);
   glEnd();

   glBegin(GL_LINE_LOOP);
   glVertex3d(base.x + ext.x, base.y + -ext.y, base.z + ext.z);
   glVertex3d(base.x + ext.x, base.y + -ext.y, base.z + 0);
   glVertex3d(base.x + -ext.x, base.y + -ext.y, base.z + 0);
   glVertex3d(base.x + -ext.x, base.y + -ext.y, base.z + ext.z);
   glEnd();

   glBegin(GL_LINES);
   glVertex3d(base.x + ext.x, base.y + ext.y, base.z + ext.z);
   glVertex3d(base.x + ext.x, base.y + -ext.y, base.z + ext.z);

   glVertex3d(base.x + -ext.x, base.y + ext.y, base.z + ext.z);
   glVertex3d(base.x + -ext.x, base.y + -ext.y, base.z + ext.z);

   glVertex3d(base.x + ext.x, base.y + ext.y, base.z + 0);
   glVertex3d(base.x + ext.x, base.y + -ext.y, base.z + 0);

   glVertex3d(base.x + -ext.x, base.y + ext.y, base.z + 0);
   glVertex3d(base.x + -ext.x, base.y + -ext.y, base.z + 0);
   glEnd();

   glEnable(GL_TEXTURE_2D);
}

void Model3DBuiltIn::GetBoundingTriangles(const Underworld::Object& object,
   Vector3d& base, std::vector<Triangle3dTextured>& allTriangles)
{
   // just hand over the tessellated triangles
   for (auto triangle : m_triangles)
   {
      Triangle3dTextured copyTriangle{ triangle };
      for (auto& vertex : copyTriangle.m_vertices)
         vertex.pos += base;

      allTriangles.push_back(copyTriangle);
   }
}

void Model3DSpecial::Render(const RenderOptions& renderOptions,
   const Vector3d& viewerPos, const Underworld::Object& object,
   TextureManager& textureManager, Vector3d& base)
{
   Uint16 itemId = object.GetObjectInfo().m_itemID;
   if (itemId == 0x0164)
   {
      // set texture for bridge
      textureManager.Use(object.GetObjectInfo().m_flags);

      // render bridge
      Vector3d ext(0.5, 0.5, 0.062);

      glBegin(GL_TRIANGLES);
      glTexCoord2d(0.0, 0.0); glVertex3d(base.x - ext.x, base.y - ext.y, base.z + ext.z);
      glTexCoord2d(1.0, 0.0); glVertex3d(base.x + ext.x, base.y - ext.y, base.z + ext.z);
      glTexCoord2d(1.0, 1.0); glVertex3d(base.x + ext.x, base.y + ext.y, base.z + ext.z);

      glTexCoord2d(0.0, 0.0); glVertex3d(base.x - ext.x, base.y - ext.y, base.z + ext.z);
      glTexCoord2d(1.0, 1.0); glVertex3d(base.x + ext.x, base.y + ext.y, base.z + ext.z);
      glTexCoord2d(0.0, 1.0); glVertex3d(base.x - ext.x, base.y + ext.y, base.z + ext.z);

      glTexCoord2d(0.0, 0.0); glVertex3d(base.x - ext.x, base.y - ext.y, base.z);
      glTexCoord2d(1.0, 1.0); glVertex3d(base.x + ext.x, base.y + ext.y, base.z);
      glTexCoord2d(1.0, 0.0); glVertex3d(base.x + ext.x, base.y - ext.y, base.z);

      glTexCoord2d(0.0, 0.0); glVertex3d(base.x - ext.x, base.y - ext.y, base.z);
      glTexCoord2d(0.0, 1.0); glVertex3d(base.x - ext.x, base.y + ext.y, base.z);
      glTexCoord2d(1.0, 1.0); glVertex3d(base.x + ext.x, base.y + ext.y, base.z);
      glEnd();
   }
   else if (itemId >= 0x0140 && itemId < 0x0150)
   {
      // TODO render door/open door/moving door
   }
}

void Model3DSpecial::GetBoundingTriangles(const Underworld::Object& object,
   Vector3d& base, std::vector<Triangle3dTextured>& allTriangles)
{
   Uint16 itemId = object.GetObjectInfo().m_itemID;
   if (itemId == 0x0164)
   {
      // bridge triangles
      Vector3d ext(0.5, 0.5, 0.062 + 1.6);

      Triangle3dTextured tri;
      tri.m_textureNumber = 0;// TODO set proper bridge texture
      tri.Set(0, base.x - ext.x, base.y - ext.y, base.z + ext.z, 0.0, 0.0);
      tri.Set(1, base.x + ext.x, base.y - ext.y, base.z + ext.z, 0.0, 0.0);
      tri.Set(2, base.x + ext.x, base.y + ext.y, base.z + ext.z, 0.0, 0.0);
      allTriangles.push_back(tri);

      tri.Set(0, base.x - ext.x, base.y - ext.y, base.z + ext.z, 0.0, 0.0);
      tri.Set(1, base.x + ext.x, base.y + ext.y, base.z + ext.z, 0.0, 0.0);
      tri.Set(2, base.x - ext.x, base.y + ext.y, base.z + ext.z, 0.0, 0.0);
      allTriangles.push_back(tri);

      tri.Set(0, base.x - ext.x, base.y - ext.y, base.z, 0.0, 0.0);
      tri.Set(2, base.x + ext.x, base.y - ext.y, base.z, 0.0, 0.0);
      tri.Set(1, base.x + ext.x, base.y + ext.y, base.z, 0.0, 0.0);
      allTriangles.push_back(tri);

      tri.Set(0, base.x - ext.x, base.y - ext.y, base.z, 0.0, 0.0);
      tri.Set(2, base.x + ext.x, base.y + ext.y, base.z, 0.0, 0.0);
      tri.Set(1, base.x - ext.x, base.y + ext.y, base.z, 0.0, 0.0);
      allTriangles.push_back(tri);
   }
}
