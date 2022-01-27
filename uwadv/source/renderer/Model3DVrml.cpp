//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2020 Underworld Adventures Team
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
/// \file Model3DVrml.cpp
/// \brief VRML97 model
//
#include "pch.hpp"
#include "Model3DVrml.hpp"
#include "Texture.hpp"

void Model3DVrml::Render(const RenderOptions& renderOptions,
   const Vector3d& viewerPos, const Underworld::Object& object,
   TextureManager& textureManager, Vector3d& base)
{
   m_texture.Use();

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

   // render all triangles in list
   size_t max = m_coordIndex.size();
   for (size_t i = 0; i < max; i += 3)
   {
      glBegin(GL_TRIANGLES);

      for (size_t n = 0; n < 3; n++)
      {
         int c_idx = m_coordIndex[i + n];
         int t_idx = m_texCoordIndex[i + n];

         Vector3d vec3d{ m_coords[c_idx] };
         Vector2d vec2d{ m_texCoords[t_idx] };

         //vec3d *= 0.008;
         vec2d.x *= m_texture.GetTexU();
         vec2d.y *= m_texture.GetTexV();

         glTexCoord2d(vec2d.x, vec2d.y);
         glVertex3d(vec3d.x + base.x, vec3d.y + base.y, vec3d.z + base.z);
      }

      glEnd();
   }
}

void Model3DVrml::GetBoundingTriangles(const Underworld::Object& object,
   Vector3d& base, std::vector<Triangle3dTextured>& allTriangles)
{
   // TODO implement
}
