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
/// \file renderer_impl.cpp
/// \brief renderer implementation
//
#include "common.hpp"
#include "renderer_impl.hpp"
#include "quadtree.hpp"
#include "geometry.hpp"

/// \brief height scale factor
/// This value scales down underworld z coordinates to coordinates in the
/// OpenGL world. It can be used to adjust the heightness of the underworld.
/// It should only appear in OpenGL function calls.
const double height_scale = 0.125*0.25;

/// Callback function to render a visible tile; determined by ua_quad. The
/// function renders all triangles of that tile and all objects in it. The
/// function uses glPushName() to let the ua_renderer::select_pick() method
/// know what triangles belong to what tile.
/// \param xpos tile x coordinate of visible tile
/// \param ypos tile y coordinate of visible tile
void ua_level_tile_renderer::visible_tile(unsigned int xpos, unsigned int ypos)
{
   glPushName((ypos << 8) + xpos);

   std::vector<ua_triangle3d_textured> alltriangles;
   geom.get_tile_triangles(xpos, ypos, alltriangles);

   unsigned int max = alltriangles.size();
   for (unsigned int i = 0; i < max; i++)
   {
      ua_triangle3d_textured& tri = alltriangles[i];

      renderer_impl.get_texture_manager().use(tri.texnum);

      // set texture parameter
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

      // remember used texture number
      glPushName(tri.texnum + 0x0400);

      glBegin(GL_TRIANGLES);
      for (int j = 0; j < 3; j++)
      {
         glTexCoord2d(tri.vertices[j].u, tri.vertices[j].v);
         glVertex3d(tri.vertices[j].pos.x, tri.vertices[j].pos.y,
            tri.vertices[j].pos.z*height_scale);
      }
      glEnd();
      glPopName();
   }

   // render objects
   renderer_impl.render_objects(level, xpos, ypos);

   glPopName();
}

ua_renderer_impl::ua_renderer_impl()
   :selection_mode(false)
{
}

/// Renders the visible parts of a level.
/// \param level the level to render
/// \param pos position of the viewer, e.g. the player
/// \param panangle angle to pan up/down the view
/// \param rotangle angle to rotate left/right the view
/// \param fov angle of field of view
void ua_renderer_impl::render(const Underworld::Level& level, ua_vector3d pos,
   double panangle, double rotangle, double fov)
{
   {
      // rotation
      glRotated(panangle + 270.0, 1.0, 0.0, 0.0);
      glRotated(-rotangle + 90.0, 0.0, 0.0, 1.0);

      // move to position on map
      glTranslated(-pos.x, -pos.y, -pos.z*height_scale);
   }

   // calculate billboard right and up vectors
   {
      float modelview[16];

      // get the current modelview matrix
      glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

      // retrieve right and up vectors
      bb_right.set(modelview[0], modelview[4], modelview[8]);
      bb_up.set(modelview[1], modelview[5], modelview[9]);

      bb_right.normalize();
      bb_up.normalize();
   }

   // draw all visible tiles
   ua_frustum2d fr(pos.x, pos.y, rotangle, fov, 8.0);

   ua_level_tile_renderer tilerenderer(level, *this);

   glColor3ub(192, 192, 192);

   // find tiles
   ua_quad q(0, 64, 0, 64);
   q.find_visible_tiles(fr, tilerenderer);
}

/// Renders all objects in a tile.
/// \param level the level in which the objects are
/// \param x tile x coordinate of tile which objects are to render
/// \param y tile y coordinate of tile which objects are to render
void ua_renderer_impl::render_objects(const Underworld::Level& level,
   unsigned int x, unsigned int y)
{
   // enable alpha blending
   glEnable(GL_BLEND);

   // prevent pixels with alpha < 0.1 to write to color and depth buffer
   glAlphaFunc(GL_GREATER, 0.1f);
   glEnable(GL_ALPHA_TEST);

   const Underworld::ObjectList& objlist = level.GetObjectList();

   // get first object link
   Uint16 link = objlist.GetListStart(x, y);

   while (link != 0)
   {
      const Underworld::Object& obj = *objlist.GetObject(link);

      // remember object list position for picking
      glPushName(link);

      // render object
      render_object(level, obj, x, y);

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
void ua_renderer_impl::render_object(const Underworld::Level& level,
   const Underworld::Object& obj, unsigned int x, unsigned int y)
{
   Uint16 item_id = obj.GetObjectInfo().m_itemID;

#ifndef HAVE_DEBUG
   // don't render invisible objects
   if (obj.GetObjectInfo().is_hidden)
      return;
#endif

   // hack: don't render some objects we currently don't support
   if ((item_id >= 0x00da && item_id <= 0x00df) || item_id == 0x012e)
      return;

   const Underworld::ObjectPositionInfo& posInfo = obj.GetPosInfo();
   const Underworld::NpcObject& npc = obj.GetNpcObject();
   const Underworld::NpcInfo& npcInfo = npc.GetNpcInfo();

   // get base coordinates
   ua_vector3d base = calc_object_pos(x, y, obj);

   // check if a 3d model is available for that item
   if (get_model3d_manager().model_avail(item_id))
   {
      base.z = posInfo.m_zpos * height_scale;

      get_model3d_manager().render(obj, texmgr, base);
   }
   else
      // critters
      if (item_id >= 0x0040 && item_id < 0x0080)
      {
         // critter object
         ua_critter& crit = get_critter_frames_manager().get_critter(item_id - 0x0040);
         unsigned int curframe = crit.get_frame(npcInfo.m_animationState, npcInfo.m_animationFrame);
         ua_texture& tex = crit.get_texture(curframe);

         tex.use(0);

         // adjust height for hotspot
         base.z -= 0.0;

         double u = crit.get_hotspot_u(curframe) / tex.get_tex_u();
         double v = crit.get_hotspot_v(curframe) / tex.get_tex_v();

         u = 1.0 - u * 2.0;
         v = v - 1.0;

         // fix for rotworm; hotspot always too high
         if (item_id == 0x0040) v += 0.25;

         render_sprite(base, 0.4, 0.88, true,
            tex.get_tex_u(), tex.get_tex_v(), u, v);
      }
      else
         /*
            // switches/levers/buttons/pull chains
            if ((item_id >= 0x0170 && item_id <= 0x017f) ||
                  item_id == 0x0161 || // a_lever
                  item_id == 0x0162 || // a_switch
                  item_id == 0x0166)   // some_writing
            {
               render_aligned_quad(object);
            }
            else
            // special tmap object
            if (item_id == 0x016e || item_id == 0x016f)
            {
               render_aligned_quad();
            }
            else
         */
      {
         // normal object
         double quadwidth = 0.25;

         // items that have to be drawn at the ceiling?
         if (item_id == 0x00d3 || item_id == 0x00d4) // a_stalactite / a_plant
         {
            // adjust height
            base.z = level.GetTilemap().GetTileInfo(x, y).m_ceiling - quadwidth;
         }

         // rune items?
         if (item_id >= 0x00e8 && item_id < 0x0100)
            item_id = 0x00e0; // generic rune-on-the-floor item

         // normal object
         texmgr.use(item_id + ua_tex_stock_objects);
         render_sprite(base, 0.5*quadwidth, quadwidth, false, 1.0, 1.0);
      }
}

/*
void ua_renderer_impl::render_decal(const Object& obj, unsigned int x, unsigned int y)
{
   // 0x0161 a_lever
   // 0x0162 a_switch
   // 0x0166 some writing
   // 0x017x buttons/switches/levers/pull chain

   Underworld::NpcObject& npc = obj.GetNpcObject();
   Underworld::NpcInfo& npcInfo = npc.GetNpcInfo();
   ua_vector3d base(static_cast<double>(x),static_cast<double>(y),
      extinfo.zpos*height_scale);

   ua_vector2d to_right;

   const double wall_offset = selection_mode ? 0.02 : 0.00;

   switch(extinfo.heading)
   {
   case 0: to_right.set(1.0,0.0);  base.x += extinfo.xpos/8.0; base.y += 1.0-wall_offset; break;
   case 2: to_right.set(0.0,-1.0); base.y += extinfo.ypos/8.0; base.x += 1.0-wall_offset; break;
   case 4: to_right.set(-1.0,0.0); base.x += extinfo.xpos/8.0; base.y += wall_offset; break;
   case 6: to_right.set(0.0,1.0);  base.y += extinfo.ypos/8.0; base.x += wall_offset; break;

   default:
      while(false);
      break; // should not occur
   }

   const double decalheight = 1.0/8.0;

   to_right.normalize();
   to_right *= decalheight;

   // select texture
   Uint16 item_id = obj.GetObjectInfo().m_itemID;
   unsigned int tex = 0;

   switch(item_id)
   {
   case 0x0161: // a_lever
      tex = obj.GetObjectInfo().flags + 4 + ua_tex_stock_tmobj;
      break;

   case 0x0162: // a_switch
      tex = obj.GetObjectInfo().flags + 12 + ua_tex_stock_tmobj;
      break;

   case 0x0166: // some_writing
      tex = (obj.GetObjectInfo().flags & 7) + 20 + ua_tex_stock_tmobj;
      break;

   default: // 0x017x
      tex = (item_id & 15) + ua_tex_stock_switches;
      break;
   }

   get_texture_manager().use(tex);

   double u1,v1,u2,v2;
   u1 = v1 = 0.0;
   u2 = v2 = 1.0;

   // enable polygon offset
   if (!selection_mode)
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

   if (!selection_mode)
      glDisable(GL_POLYGON_OFFSET_FILL);
}

void ua_renderer_impl::render_tmap_obj(const Object& obj, unsigned int x, unsigned int y)
{
   // 0x016e / 0x016f special tmap object
   Underworld::NpcObject& npc = obj.GetNpcObject();
   Underworld::NpcInfo& npcInfo = npc.GetNpcInfo();

   ua_vector3d pos(static_cast<double>(x),static_cast<double>(y),
      extinfo.zpos*height_scale);

   unsigned int x_fr = extinfo.xpos;
   unsigned int y_fr = extinfo.ypos;

   // hack: fixing some tmap decals
   if (x_fr>4) x_fr++;
   if (y_fr>4) y_fr++;

   // determine direction
   ua_vector3d dir;
   switch(extinfo.heading)
   {
   case 0: dir.set(1.0, 0.0, 0.0); break;
   case 2: dir.set(0.0, 1.0, 0.0); break;
   case 4: dir.set(-1.0, 0.0, 0.0); break;
   case 6: dir.set(0.0, -1.0, 0.0); break;

   case 1: dir.set(1.0, -1.0, 0.0); break;
   case 3: dir.set(-1.0, -1.0, 0.0); break;
   case 5: dir.set(-1.0, 1.0, 0.0); break;
   case 7: dir.set(1.0, 1.0, 0.0); break;
   }

   dir.normalize();
   dir *= 0.5;

   // add fractional position
   pos.x += x_fr / 8.0;
   pos.y += y_fr / 8.0;

   if (selection_mode)
   {
      // in selection mode, offset object pos
      ua_vector3d offset(dir);
      offset.rotate_z(-90.0);
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
   get_texture_manager().use(obj.GetObjectInfo().m_itemID+ua_tex_stock_objects);

   glBegin(GL_QUADS);
   glTexCoord2d(u2,v2); glVertex3d(pos.x+dir.x, pos.y+dir.y, pos.z);
   glTexCoord2d(u2,v1); glVertex3d(pos.x+dir.x, pos.y+dir.y, pos.z+1.0);
   glTexCoord2d(u1,v1); glVertex3d(pos.x-dir.x, pos.y-dir.y, pos.z+1.0);
   glTexCoord2d(u1,v2); glVertex3d(pos.x-dir.x, pos.y-dir.y, pos.z);
   glEnd();
#endif

   get_texture_manager().use(obj.GetObjectInfo().owner);
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
/// \param ignore_upvector ignores billboard up-vector when true; used for
///                        critters
/// \param u maximum u texture coordinate
/// \param v maximum v texture coordinate
/// \param move_u u-coordinate offset to move base, e.g. to hotspot
/// \param move_v v-coordinate offset to move base, e.g. to hotspot
void ua_renderer_impl::render_sprite(ua_vector3d base,
   double width, double height, bool ignore_upvector, double u, double v,
   double move_u, double move_v)
{
   // set texture parameter
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   // scale z axis before any calculation is done
   base.z *= height_scale;

   // move base to new location
   base += bb_right * move_u*width;
   base += bb_up * move_v*height;

   // calculate vectors for quad
   ua_vector3d base2(base);

   base -= bb_right * width;
   base2 += bb_right * width;

   ua_vector3d high1(base);
   ua_vector3d high2(base2);

   if (ignore_upvector)
   {
      high1.z += height;
      high2.z += height;
   }
   else
   {
      high1 += bb_up * height;
      high2 += bb_up * height;
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
   if (ignore_upvector)
   {
      glDisable(GL_TEXTURE_2D);

      GLfloat linewidth;
      glGetFloatv(GL_LINE_WIDTH, &linewidth);
      glLineWidth(5.0);

      glBegin(GL_LINE_LOOP);
      glVertex3d(base.x, base.y, base.z);
      glVertex3d(base2.x, base2.y, base2.z);
      glVertex3d(high2.x, high2.y, high2.z);
      glVertex3d(high1.x, high1.y, high1.z);
      glEnd();

      glLineWidth(linewidth);

      glEnable(GL_TEXTURE_2D);
   }
#endif

   glDisable(GL_POLYGON_OFFSET_FILL);
}

/*
/// Renders a wall-aligned textured quad.
/// \param width
/// \param height
void ua_renderer_impl::render_aligned_quad(const Object& obj, //const ua_vector3d& base,
   double width, double height, Uint16 texnum, bool ignore_upvector)
{
}
*/

/// calculates object position in 3d world
ua_vector3d ua_renderer_impl::calc_object_pos(unsigned int x, unsigned int y,
   const Underworld::Object& obj) const
{
   const Underworld::ObjectPositionInfo& posInfo = obj.GetPosInfo();

   double objxpos = static_cast<double>(x) + (posInfo.m_xpos) / 8.0 + 1.0 / 8.0;
   double objypos = static_cast<double>(y) + (posInfo.m_ypos) / 8.0 + 1.0 / 8.0;
   double height = posInfo.m_zpos;

   return ua_vector3d(objxpos, objypos, height);
}
