//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2019 Underworld Adventures Team
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
#include "pch.hpp"
#include "LevelTilemapRenderer.hpp"
#include "Texture.hpp"
#include "UnderworldRenderer.hpp"

extern const double c_renderHeightScale;

LevelTilemapRenderer::LevelTilemapRenderer(const Underworld::Level& level, RendererImpl& rendererImpl,
   TextureManager& textureManager)
   :m_level(level),
   m_textureManager(textureManager),
   m_rendererImpl(rendererImpl),
   m_geometryProvider(level)
{
}

/// Callback function to render a visible tile; determined by Quad. The
/// function renders all triangles of that tile and all objects in it. The
/// function uses glPushName() to let the Renderer::SelectPick() method
/// know what triangles belong to what tile.
/// \param xpos tile x coordinate of visible tile
/// \param ypos tile y coordinate of visible tile
void LevelTilemapRenderer::RenderTile(unsigned int xpos, unsigned int ypos)
{
   glPushName((ypos << 8) + xpos);

   std::vector<Triangle3dTextured> allTriangles;
   m_geometryProvider.GetTileTriangles(xpos, ypos, allTriangles);

   unsigned int maxTriangles = allTriangles.size();
   for (unsigned int triangleIndex = 0; triangleIndex < maxTriangles; triangleIndex++)
   {
      Triangle3dTextured& triangle = allTriangles[triangleIndex];

      m_textureManager.Use(triangle.m_textureNumber);

      // set texture parameter
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

      // remember used texture number
      glPushName(triangle.m_textureNumber + 0x0400);

      glBegin(GL_TRIANGLES);
      for (int j = 0; j < 3; j++)
      {
         glTexCoord2d(triangle.m_vertices[j].u, triangle.m_vertices[j].v);
         glVertex3d(triangle.m_vertices[j].pos.x, triangle.m_vertices[j].pos.y,
            triangle.m_vertices[j].pos.z * c_renderHeightScale);
      }
      glEnd();
      glPopName();
   }

   m_rendererImpl.RenderObjects(m_level, xpos, ypos);

   glPopName();
}
