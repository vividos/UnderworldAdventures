//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2004,2019 Underworld Adventures Team
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

extern bool DecodeBuiltInModels(const char* filename,
   std::vector<Model3DPtr>& allModels, bool dump = false);

void Model3DBuiltIn::Render(const Underworld::Object& object,
   TextureManager& textureManager, Vector3d& base)
{
   glDisable(GL_CULL_FACE);

   unsigned int maxTriangles = m_triangles.size();

   Vector3d origin;

   for (unsigned int triangleIndex = 0; triangleIndex < maxTriangles; triangleIndex++)
   {
#if 0
      //#ifdef HAVE_DEBUG
            // select debug triangle color
      switch (m_triangles[i].m_textureNumber)
      {
      case 1:
         glColor3ub(255, 0, 0); // red
         break;
      case 2:
         glColor3ub(0, 255, 0); // green
         break;
      case 3:
         glColor3ub(0, 0, 255); // blue
         break;
      case 4:
         glColor3ub(255, 0, 255); // violet
         break;
      }
#endif
      const Triangle3dTextured& tri = m_triangles[triangleIndex];

      glBegin(GL_TRIANGLES);

      // generate normal vector
      Vector3d normal, vec1(tri.m_vertices[1].pos), vec2(tri.m_vertices[2].pos);
      vec1 -= tri.m_vertices[0].pos;
      vec2 -= tri.m_vertices[0].pos;

      normal.cross(vec1, vec2);
      normal.normalize();
      normal *= -1;

      glNormal3d(normal.x, normal.y, normal.z);

      for (unsigned index = 0; index < 3; index++)
         glVertex3d(
            base.x - origin.x + tri.m_vertices[index].pos.x,
            base.y - origin.y + tri.m_vertices[index].pos.y,
            base.z - origin.z + tri.m_vertices[index].pos.z
         );

      glEnd();
   }

#if 0
   //#ifdef HAVE_DEBUG
      // draw extents box
   glDisable(GL_TEXTURE_2D);

   Vector3d ext(extents);
   ext.x *= 0.5;
   ext.y *= 0.5;

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
#endif

   glEnable(GL_CULL_FACE);
}

void Model3DBuiltIn::GetBoundingTriangles(const Underworld::Object& object,
   Vector3d& base, std::vector<Triangle3dTextured>& allTriangles)
{
   // just hand over the tessellated triangles
   allTriangles.insert(allTriangles.end(), m_triangles.begin(), m_triangles.end());
}

void Model3DSpecial::Render(const Underworld::Object& object,
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
      tri.set(0, base.x - ext.x, base.y - ext.y, base.z + ext.z, 0.0, 0.0);
      tri.set(1, base.x + ext.x, base.y - ext.y, base.z + ext.z, 0.0, 0.0);
      tri.set(2, base.x + ext.x, base.y + ext.y, base.z + ext.z, 0.0, 0.0);
      allTriangles.push_back(tri);

      tri.set(0, base.x - ext.x, base.y - ext.y, base.z + ext.z, 0.0, 0.0);
      tri.set(1, base.x + ext.x, base.y + ext.y, base.z + ext.z, 0.0, 0.0);
      tri.set(2, base.x - ext.x, base.y + ext.y, base.z + ext.z, 0.0, 0.0);
      allTriangles.push_back(tri);

      tri.set(0, base.x - ext.x, base.y - ext.y, base.z, 0.0, 0.0);
      tri.set(2, base.x + ext.x, base.y - ext.y, base.z, 0.0, 0.0);
      tri.set(1, base.x + ext.x, base.y + ext.y, base.z, 0.0, 0.0);
      allTriangles.push_back(tri);

      tri.set(0, base.x - ext.x, base.y - ext.y, base.z, 0.0, 0.0);
      tri.set(2, base.x + ext.x, base.y + ext.y, base.z, 0.0, 0.0);
      tri.set(1, base.x - ext.x, base.y + ext.y, base.z, 0.0, 0.0);
      allTriangles.push_back(tri);
   }
}
