//
// Underworld Adventures - an Ultima Underworld hacking project
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
/// \file RendererImpl.cpp
/// \brief renderer implementation
//
#include "pch.hpp"
#include "RendererImpl.hpp"
#include "Quadtree.hpp"
#include "GeometryProvider.hpp"
#include "Constants.hpp"

/// \brief height scale factor
/// This value scales down underworld z coordinates to coordinates in the
/// OpenGL world. It can be used to adjust the heightness of the underworld.
/// It should only appear in OpenGL function calls.
const double c_heightScale = 0.125 * 0.25;

/// Callback function to render a visible tile; determined by Quad. The
/// function renders all triangles of that tile and all objects in it. The
/// function uses glPushName() to let the Renderer::SelectPick() method
/// know what triangles belong to what tile.
/// \param xpos tile x coordinate of visible tile
/// \param ypos tile y coordinate of visible tile
void LevelTilemapRenderer::OnVisibleTile(unsigned int xpos, unsigned int ypos)
{
   glPushName((ypos << 8) + xpos);

   std::vector<Triangle3dTextured> allTriangles;
   m_geometryProvider.GetTileTriangles(xpos, ypos, allTriangles);

   unsigned int maxTriangles = allTriangles.size();
   for (unsigned int triangleIndex = 0; triangleIndex < maxTriangles; triangleIndex++)
   {
      Triangle3dTextured& triangle = allTriangles[triangleIndex];

      m_rendererImpl.GetTextureManager().Use(triangle.m_textureNumber);

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
            triangle.m_vertices[j].pos.z * c_heightScale);
      }
      glEnd();
      glPopName();
   }

   m_rendererImpl.RenderObjects(m_level, xpos, ypos);

   glPopName();
}

RendererImpl::RendererImpl()
   :m_selectionMode(false)
{
}

/// Renders the visible parts of a level.
/// \param level the level to render
/// \param pos position of the viewer, e.g. the player
/// \param panAngle angle to pan up/down the view
/// \param rotateAngle angle to rotate left/right the view
/// \param fieldOfView angle of field of view
void RendererImpl::Render(const Underworld::Level& level, Vector3d pos,
   double panAngle, double rotateAngle, double fieldOfView)
{
   {
      // rotation
      glRotated(panAngle + 270.0, 1.0, 0.0, 0.0);
      glRotated(-rotateAngle + 90.0, 0.0, 0.0, 1.0);

      // move to position on map
      glTranslated(-pos.x, -pos.y, -pos.z*c_heightScale);
   }

   // calculate billboard right and up vectors
   {
      float modelview[16];

      // get the current modelview matrix
      glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

      // retrieve right and up vectors
      m_billboardRightVector.Set(modelview[0], modelview[4], modelview[8]);
      m_billboardUpVector.Set(modelview[1], modelview[5], modelview[9]);

      m_billboardRightVector.Normalize();
      m_billboardUpVector.Normalize();
   }

   // draw all visible tiles
   Frustum2d fr(pos.x, pos.y, rotateAngle, fieldOfView, 8.0);

   LevelTilemapRenderer tileRenderer(level, *this);

   glColor3ub(192, 192, 192);

   // find tiles
   Quad q(0, 64, 0, 64);
   q.FindVisibleTiles(fr, tileRenderer);
}

/// Renders all objects in a tile.
/// \param level the level in which the objects are
/// \param x tile x coordinate of tile which objects are to render
/// \param y tile y coordinate of tile which objects are to render
void RendererImpl::RenderObjects(const Underworld::Level& level,
   unsigned int x, unsigned int y)
{
   // enable alpha blending
   glEnable(GL_BLEND);

   // prevent pixels with alpha < 0.1 to write to color and depth buffer
   glAlphaFunc(GL_GREATER, 0.1f);
   glEnable(GL_ALPHA_TEST);

   const Underworld::ObjectList& objectList = level.GetObjectList();

   // get first object link
   Uint16 link = objectList.GetListStart(x, y);

   while (link != 0)
   {
      const Underworld::Object& obj = *objectList.GetObject(link);

      // remember object list position for picking
      glPushName(link);

      // render object
      RenderObject(level, obj, x, y);

      glPopName();

      // next object in link chain
      link = obj.GetObjectInfo().m_link;
   }

   // disable alpha blending again
   glDisable(GL_ALPHA_TEST);
   glDisable(GL_BLEND);
}

/// Renders an object at a time. When a 3d model for that object exists, the
/// model is drawn instead.
/// \param level level in which object is; const object
/// \param obj object to render; const object
/// \param x x tile coordinate of object
/// \param y y tile coordinate of object
void RendererImpl::RenderObject(const Underworld::Level& level,
   const Underworld::Object& obj, unsigned int x, unsigned int y)
{
   Uint16 itemId = obj.GetObjectInfo().m_itemID;

#ifndef HAVE_DEBUG
   // don't render invisible objects
   if (obj.GetObjectInfo().m_isHidden)
      return;
#endif

   // hack: don't render some objects we currently don't support
   if ((itemId >= 0x00da && itemId <= 0x00df) || itemId == 0x012e)
      return;

   const Underworld::ObjectPositionInfo& posInfo = obj.GetPosInfo();
   const Underworld::NpcObject& npc = obj.GetNpcObject();
   const Underworld::NpcInfo& npcInfo = npc.GetNpcInfo();

   // get base coordinates
   Vector3d base = CalcObjectPosition(x, y, obj);

   // check if a 3d model is available for that item
   if (GetModel3DManager().IsModelAvailable(itemId))
   {
      base.z = posInfo.m_zpos * c_heightScale;

      GetModel3DManager().Render(obj, m_textureManager, base);
   }
   else
      // critters
      if (itemId >= 0x0040 && itemId < 0x0080)
      {
         // critter object
         Critter& crit = GetCritterFramesManager().GetCritter(itemId - 0x0040);
         unsigned int curframe = crit.GetFrame(npcInfo.m_animationState, npcInfo.m_animationFrame);
         Texture& tex = crit.GetTexture(curframe);

         tex.Use(0);

         // adjust height for hotspot
         base.z -= 0.0;

         double u = crit.GetHotspotU(curframe) / tex.GetTexU();
         double v = crit.GetHotspotV(curframe) / tex.GetTexV();

         u = 1.0 - u * 2.0;
         v = v - 1.0;

         // fix for rotworm; hotspot always too high
         if (itemId == 0x0040) v += 0.25;

         RenderSprite(base, 0.4, 0.88, true,
            tex.GetTexU(), tex.GetTexV(), u, v);
      }
      else
         /* TODO reactivate
            // switches/levers/buttons/pull chains
            if ((itemId >= 0x0170 && itemId <= 0x017f) ||
                  itemId == 0x0161 || // a_lever
                  itemId == 0x0162 || // a_switch
                  itemId == 0x0166)   // some_writing
            {
               render_aligned_quad(object);
            }
            else
            // special tmap object
            if (itemId == 0x016e || itemId == 0x016f)
            {
               render_aligned_quad();
            }
            else
         */
      {
         // normal object
         double quadWidth = 0.25;

         // items that have to be drawn at the ceiling?
         if (itemId == 0x00d3 || itemId == 0x00d4) // a_stalactite / a_plant
         {
            // adjust height
            base.z = level.GetTilemap().GetTileInfo(x, y).m_ceiling - quadWidth;
         }

         // rune items?
         if (itemId >= 0x00e8 && itemId < 0x0100)
            itemId = 0x00e0; // generic rune-on-the-floor item

         // normal object
         m_textureManager.Use(itemId + Base::c_stockTexturesObjects);
         RenderSprite(base, 0.5*quadWidth, quadWidth, false, 1.0, 1.0);
      }
}

/*
void RendererImpl::RenderDecal(const Object& obj, unsigned int x, unsigned int y)
{
   // 0x0161 a_lever
   // 0x0162 a_switch
   // 0x0166 some writing
   // 0x017x buttons/switches/levers/pull chain

   Underworld::NpcObject& npc = obj.GetNpcObject();
   Underworld::NpcInfo& npcInfo = npc.GetNpcInfo();
   Vector3d base(static_cast<double>(x),static_cast<double>(y),
      extinfo.zpos*c_heightScale);

   Vector2d to_right;

   const double wall_offset = m_selectionMode ? 0.02 : 0.00;

   switch(extinfo.heading)
   {
   case 0: to_right.Set(1.0,0.0);  base.x += extinfo.xpos/8.0; base.y += 1.0-wall_offset; break;
   case 2: to_right.Set(0.0,-1.0); base.y += extinfo.ypos/8.0; base.x += 1.0-wall_offset; break;
   case 4: to_right.Set(-1.0,0.0); base.x += extinfo.xpos/8.0; base.y += wall_offset; break;
   case 6: to_right.Set(0.0,1.0);  base.y += extinfo.ypos/8.0; base.x += wall_offset; break;

   default:
      while(false);
      break; // should not occur
   }

   const double decalheight = 1.0/8.0;

   to_right.Normalize();
   to_right *= decalheight;

   // select texture
   Uint16 itemId = obj.GetObjectInfo().m_itemID;
   unsigned int tex = 0;

   switch(itemId)
   {
   case 0x0161: // a_lever
      tex = obj.GetObjectInfo().flags + 4 + c_stockTexturesTmobj;
      break;

   case 0x0162: // a_switch
      tex = obj.GetObjectInfo().flags + 12 + c_stockTexturesTmobj;
      break;

   case 0x0166: // some_writing
      tex = (obj.GetObjectInfo().flags & 7) + 20 + c_stockTexturesTmobj;
      break;

   default: // 0x017x
      tex = (itemId & 15) + c_stockTextureswitches;
      break;
   }

   GetTextureManager().use(tex);

   double u1,v1,u2,v2;
   u1 = v1 = 0.0;
   u2 = v2 = 1.0;

   // enable polygon offset
   if (!m_selectionMode)
   {
      glPolygonOffset(-2.0, -2.0);
      glEnable(GL_POLYGON_OFFSET_FILL);
   }

   // render quad
   glBegin(GL_QUADS);
   glTexCoord2d(u1,v2); glVertex3d(base.x-to_right.x,base.y-to_right.y,base.z-decalheight);
   glTexCoord2d(u2,v2); glVertex3d(base.x+to_right.x,base.y+to_right.y,base.z-decalheight);
   glTexCoord2d(u2,v1); glVertex3d(base.x+to_right.x,base.y+to_right.y,base.z+decalheight);
   glTexCoord2d(u1,v1); glVertex3d(base.x-to_right.x,base.y-to_right.y,base.z+decalheight);
   glEnd();

   if (!m_selectionMode)
      glDisable(GL_POLYGON_OFFSET_FILL);
}

void RendererImpl::RenderTmapObject(const Object& obj, unsigned int x, unsigned int y)
{
   // 0x016e / 0x016f special tmap object
   Underworld::NpcObject& npc = obj.GetNpcObject();
   Underworld::NpcInfo& npcInfo = npc.GetNpcInfo();

   Vector3d pos(static_cast<double>(x),static_cast<double>(y),
      extinfo.zpos*c_heightScale);

   unsigned int x_fr = extinfo.xpos;
   unsigned int y_fr = extinfo.ypos;

   // hack: fixing some tmap decals
   if (x_fr>4) x_fr++;
   if (y_fr>4) y_fr++;

   // determine direction
   Vector3d dir;
   switch(extinfo.heading)
   {
   case 0: dir.Set(1.0, 0.0, 0.0); break;
   case 2: dir.Set(0.0, 1.0, 0.0); break;
   case 4: dir.Set(-1.0, 0.0, 0.0); break;
   case 6: dir.Set(0.0, -1.0, 0.0); break;

   case 1: dir.Set(1.0, -1.0, 0.0); break;
   case 3: dir.Set(-1.0, -1.0, 0.0); break;
   case 5: dir.Set(-1.0, 1.0, 0.0); break;
   case 7: dir.Set(1.0, 1.0, 0.0); break;
   }

   dir.Normalize();
   dir *= 0.5;

   // add fractional position
   pos.x += x_fr / 8.0;
   pos.y += y_fr / 8.0;

   if (m_selectionMode)
   {
      // in selection mode, offset object pos
      Vector3d offset(dir);
      offset.RotateZ(-90.0);
      offset *= 0.01;

      pos += offset;
   }

   // enable polygon offset
   glPolygonOffset(-2.0, -2.0);
   glEnable(GL_POLYGON_OFFSET_FILL);

   double u1,v1,u2,v2;
   u1 = v1 = 0.0;
   u2 = v2 = 1.0;

#ifdef HAVE_DEBUG
   // render "tmap_c" or "tmap_s" overlay
   GetTextureManager().use(obj.GetObjectInfo().m_itemID+c_stockTexturesObjects);

   glBegin(GL_QUADS);
   glTexCoord2d(u2,v2); glVertex3d(pos.x+dir.x, pos.y+dir.y, pos.z);
   glTexCoord2d(u2,v1); glVertex3d(pos.x+dir.x, pos.y+dir.y, pos.z+1.0);
   glTexCoord2d(u1,v1); glVertex3d(pos.x-dir.x, pos.y-dir.y, pos.z+1.0);
   glTexCoord2d(u1,v2); glVertex3d(pos.x-dir.x, pos.y-dir.y, pos.z);
   glEnd();
#endif

   GetTextureManager().use(obj.GetObjectInfo().owner);
   u1 = v1 = 0.0;
   u2 = v2 = 1.0;

   // use wall color
   glColor3ub(128,128,128);

   glBegin(GL_QUADS);
   glTexCoord2d(u2,v2); glVertex3d(pos.x+dir.x, pos.y+dir.y, pos.z);
   glTexCoord2d(u2,v1); glVertex3d(pos.x+dir.x, pos.y+dir.y, pos.z+1.0);
   glTexCoord2d(u1,v1); glVertex3d(pos.x-dir.x, pos.y-dir.y, pos.z+1.0);
   glTexCoord2d(u1,v2); glVertex3d(pos.x-dir.x, pos.y-dir.y, pos.z);
   glEnd();

   // switch back to object color
   glColor3ub(192,192,192);

   glDisable(GL_POLYGON_OFFSET_FILL);
}
*/

/// Renders a billboarded drawn sprite; the texture has to be use()d before
/// calling, and the max u and v coordinates have to be passed
/// Objects are drawn using the method described in the billboarding tutorial,
/// "Cheating - Faster but not so easy". Billboarding tutorials:
/// http://www.lighthouse3d.com/opengl/billboarding/
/// http://nate.scuzzy.net/gltut/
/// \param base base coordinates of sprite
/// \param width relative width of object in relation to a tile
/// \param height relative height of object in relation to a tile
/// \param ignoreUpVector ignores billboard up-vector when true; used for
///                        critters
/// \param u maximum u texture coordinate
/// \param v maximum v texture coordinate
/// \param moveU u-coordinate offset to move base, e.g. to hotspot
/// \param moveV v-coordinate offset to move base, e.g. to hotspot
void RendererImpl::RenderSprite(Vector3d base,
   double width, double height, bool ignoreUpVector, double u, double v,
   double moveU, double moveV)
{
   // set texture parameter
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   // scale z axis before any calculation is done
   base.z *= c_heightScale;

   // move base to new location
   base += m_billboardRightVector * moveU*width;
   base += m_billboardUpVector * moveV*height;

   // calculate vectors for quad
   Vector3d base2(base);

   base -= m_billboardRightVector * width;
   base2 += m_billboardRightVector * width;

   Vector3d high1(base);
   Vector3d high2(base2);

   if (ignoreUpVector)
   {
      high1.z += height;
      high2.z += height;
   }
   else
   {
      high1 += m_billboardUpVector * height;
      high2 += m_billboardUpVector * height;
   }

   // enable polygon offset
   glPolygonOffset(-2.0, -2.0);
   glEnable(GL_POLYGON_OFFSET_FILL);

   // render quad
   glBegin(GL_QUADS);
   glTexCoord2d(0.0, v);   glVertex3d(base.x, base.y, base.z);
   glTexCoord2d(u, v);   glVertex3d(base2.x, base2.y, base2.z);
   glTexCoord2d(u, 0.0); glVertex3d(high2.x, high2.y, high2.z);
   glTexCoord2d(0.0, 0.0); glVertex3d(high1.x, high1.y, high1.z);
   glEnd();

#ifdef HAVE_DEBUG
   if (ignoreUpVector)
   {
      glDisable(GL_TEXTURE_2D);

      GLfloat lineWidth;
      glGetFloatv(GL_LINE_WIDTH, &lineWidth);
      glLineWidth(5.0);

      glBegin(GL_LINE_LOOP);
      glVertex3d(base.x, base.y, base.z);
      glVertex3d(base2.x, base2.y, base2.z);
      glVertex3d(high2.x, high2.y, high2.z);
      glVertex3d(high1.x, high1.y, high1.z);
      glEnd();

      glLineWidth(lineWidth);

      glEnable(GL_TEXTURE_2D);
   }
#endif

   glDisable(GL_POLYGON_OFFSET_FILL);
}

/* TODO reactivate
/// Renders a wall-aligned textured quad.
/// \param width
/// \param height
void RendererImpl::render_aligned_quad(const Object& obj, //const Vector3d& base,
   double width, double height, Uint16 textureNumber, bool ignoreUpVector)
{
}
*/

/// calculates object position in 3D world
Vector3d RendererImpl::CalcObjectPosition(unsigned int x, unsigned int y,
   const Underworld::Object& obj) const
{
   const Underworld::ObjectPositionInfo& posInfo = obj.GetPosInfo();

   double objectXPos = static_cast<double>(x) + (posInfo.m_xpos) / 8.0 + 1.0 / 8.0;
   double objectYPos = static_cast<double>(y) + (posInfo.m_ypos) / 8.0 + 1.0 / 8.0;
   double height = posInfo.m_zpos;

   return Vector3d(objectXPos, objectYPos, height);
}
