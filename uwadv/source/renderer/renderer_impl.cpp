/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file renderer_impl.cpp

   \brief renderer implementation

*/

// not using the alt. renderer
#undef HAVE_ALTERNATE_RENDERER
#define HAVE_ALTERNATE_RENDERER

// needed includes
#include "common.hpp"
#include "renderer_impl.hpp"
#include "quadtree.hpp"
#include "geometry.hpp"

#ifdef HAVE_ALTERNATE_RENDERER
#include <algorithm>
#endif


//! TODO remove, deprecated!
const double height_scale = 0.125;


// enums
/*
//! side of currently rendered wall; used internally
enum
{
   ua_left, ua_right, ua_front, ua_back
};
*/

// ua_renderer_impl methods

ua_renderer_impl::ua_renderer_impl()
:selection_mode(false)
{
}

void ua_renderer_impl::render(const ua_level& level, ua_vector3d pos,
   double panangle, double rotangle, double fov)
{
   // retrieve tiles to draw
   ua_frustum2d fr(pos.x,pos.y,rotangle,fov,8.0);

   // determine list of visible tiles
   std::vector<ua_quad_tile_coord> tilelist;
   ua_quad q(0,64, 0,64);

   q.get_visible_tiles(fr,tilelist);

   {
      // rotation
      glRotated(panangle+270.0, 1.0, 0.0, 0.0);
      glRotated(-rotangle+90.0, 0.0, 0.0, 1.0);

      // move to position on map
      glTranslated(-pos.x, -pos.y, -pos.z*0.25*height_scale);
   }

   int i,max;

#ifdef HAVE_ALTERNATE_RENDERER

   ua_geometry_provider geom(level);

   // collect all triangles
   std::vector<ua_triangle3d_textured> alltriangles;
   max = tilelist.size();
   for(i=0; i<max; i++)
   {
      const ua_quad_tile_coord& qtc = tilelist[i];
      geom.get_tile_triangles(qtc.first,qtc.second,alltriangles);
   }

   // sort triangles by texnum
   std::sort(alltriangles.begin(), alltriangles.end());

   // render all triangles
   glColor3ub(192,192,192);
   max = alltriangles.size();
   for(i=0; i<max; i++)
   {
      ua_triangle3d_textured &tri = alltriangles[i];

      texmgr.use(tri.texnum);

      glBegin(GL_TRIANGLES);
      for(int j=0; j<3; j++)
      {
         glTexCoord2d(tri.vertices[j].u,tri.vertices[j].v);
         glVertex3d(tri.vertices[j].pos.x, tri.vertices[j].pos.y,
            tri.vertices[j].pos.z*0.25*height_scale);
      }
      glEnd();
   }

#else

   glColor3ub(192,192,192);
   //glColor4ub(192,192,192,192);

/*TODO
   // draw floor tile polygons, for all visible tiles
   max = tilelist.size();
   for(i=0;i<max;i++)
   {
      const ua_quad_tile_coord &qtc = tilelist[i];
      const ua_levelmap_tile& tile = const_cast<ua_level&>(level).get_tile(qtc.first,qtc.second);

      render_floor(const_cast<ua_levelmap_tile&>(tile),qtc.first,qtc.second);
   }

   // draw visible tile ceilings
   for(i=0;i<max;i++)
   {
      const ua_quad_tile_coord &qtc = tilelist[i];
      ua_levelmap_tile& tile = level.get_tile(qtc.first,qtc.second);

      render_ceiling(tile,qtc.first,qtc.second);
   }

   //glColor4ub(128,128,128,128);
   glColor3ub(128,128,128);

   // draw all visible walls
   for(i=0;i<max;i++)
   {
      const ua_quad_tile_coord &qtc = tilelist[i];
      ua_levelmap_tile& tile = level.get_tile(qtc.first,qtc.second);

      render_walls(tile,qtc.first,qtc.second);
   }

   //glColor4ub(192,192,192,255);
   glColor3ub(192,192,192);

   // draw all visible objects
   for(i=0;i<max;i++)
   {
      const ua_quad_tile_coord &qtc = tilelist[i];
      render_objects(qtc.first,qtc.second);
   }
*/
#endif
}
/*
void ua_renderer_impl::render(ua_texture_manager &texmgr)
{
   int x,y;

   glColor3ub(192,192,192);

   for(y=0; y<64;y++) for(x=0; x<64;x++)
      render_floor(x,y,texmgr);

   for(y=0; y<64;y++) for(x=0; x<64;x++)
      render_ceiling(x,y,texmgr);

   for(y=0; y<64;y++) for(x=0; x<64;x++)
      render_walls(x,y,texmgr);

   {
      // construct proper frustum for viewing in mapdisp
      ua_frustum fr(0.0,0.0,0.0,45.0,0.0,90.0,16.0);

      // set up new viewpoint, "view coordinates" used in ua_object::render()
      glPushMatrix();
      glLoadIdentity();
      glRotatef(-fr.get_yangle()+270, 1.0, 0.0, 0.0);

      for(y=0; y<64;y++) for(x=0; x<64;x++)
         render_objs(x,y,texmgr,fr);

      // restore old viewpoint
      glPopMatrix();
   }
}
*/

#if 0
void ua_renderer_impl::render_floor(ua_levelmap_tile& tile, unsigned int x,
   unsigned int y)
{
   if (tile.type == ua_tile_solid)
      return; // don't draw solid tiles

   // use texture
   texmgr->use(tile.texture_floor);
   glPushName((y<<8) + x);
   glPushName(tile.texture_floor+0x0400);

   // draw floor tile
   switch(tile.type)
   {
   case ua_tile_open:
      glBegin(GL_QUADS);
      glTexCoord2d(0.0,0.0); glVertex3d(x,y,tile.floor*height_scale);
      glTexCoord2d(1.0,0.0); glVertex3d(x+1,y,tile.floor*height_scale);
      glTexCoord2d(1.0,1.0); glVertex3d(x+1,y+1,tile.floor*height_scale);
      glTexCoord2d(0.0,1.0); glVertex3d(x,y+1,tile.floor*height_scale);
      glEnd();
      break;

   case ua_tile_diagonal_se:
      glBegin(GL_TRIANGLES);
      glTexCoord2d(0.0,0.0); glVertex3d(x,y,tile.floor*height_scale);
      glTexCoord2d(1.0,0.0); glVertex3d(x+1,y,tile.floor*height_scale);
      glTexCoord2d(1.0,1.0); glVertex3d(x+1,y+1,tile.floor*height_scale);
      glEnd();
      break;

   case ua_tile_diagonal_sw:
      glBegin(GL_TRIANGLES);
      glTexCoord2d(0.0,0.0); glVertex3d(x,y,tile.floor*height_scale);
      glTexCoord2d(1.0,0.0); glVertex3d(x+1,y,tile.floor*height_scale);
      glTexCoord2d(0.0,1.0); glVertex3d(x,y+1,tile.floor*height_scale);
      glEnd();
      break;

   case ua_tile_diagonal_nw:
      glBegin(GL_TRIANGLES);
      glTexCoord2d(0.0,0.0); glVertex3d(x,y,tile.floor*height_scale);
      glTexCoord2d(1.0,1.0); glVertex3d(x+1,y+1,tile.floor*height_scale);
      glTexCoord2d(0.0,1.0); glVertex3d(x,y+1,tile.floor*height_scale);
      glEnd();
      break;

   case ua_tile_diagonal_ne:
      glBegin(GL_TRIANGLES);
      glTexCoord2d(0.0,1.0); glVertex3d(x,y+1,tile.floor*height_scale);
      glTexCoord2d(1.0,0.0); glVertex3d(x+1,y,tile.floor*height_scale);
      glTexCoord2d(1.0,1.0); glVertex3d(x+1,y+1,tile.floor*height_scale);
      glEnd();
      break;

   case ua_tile_slope_n:
      glBegin(GL_QUADS);
      glTexCoord2d(0.0,0.0); glVertex3d(x,y,tile.floor*height_scale);
      glTexCoord2d(1.0,0.0); glVertex3d(x+1,y,tile.floor*height_scale);
      glTexCoord2d(1.0,1.0); glVertex3d(x+1,y+1,(tile.floor+tile.slope)*height_scale);
      glTexCoord2d(0.0,1.0); glVertex3d(x,y+1,(tile.floor+tile.slope)*height_scale);
      glEnd();
      break;

   case ua_tile_slope_s:
      glBegin(GL_QUADS);
      glTexCoord2d(0.0,0.0); glVertex3d(x,y,(tile.floor+tile.slope)*height_scale);
      glTexCoord2d(1.0,0.0); glVertex3d(x+1,y,(tile.floor+tile.slope)*height_scale);
      glTexCoord2d(1.0,1.0); glVertex3d(x+1,y+1,tile.floor*height_scale);
      glTexCoord2d(0.0,1.0); glVertex3d(x,y+1,tile.floor*height_scale);
      glEnd();
      break;

   case ua_tile_slope_e:
      glBegin(GL_QUADS);
      glTexCoord2d(0.0,0.0); glVertex3d(x,y,tile.floor*height_scale);
      glTexCoord2d(1.0,0.0); glVertex3d(x+1,y,(tile.floor+tile.slope)*height_scale);
      glTexCoord2d(1.0,1.0); glVertex3d(x+1,y+1,(tile.floor+tile.slope)*height_scale);
      glTexCoord2d(0.0,1.0); glVertex3d(x,y+1,tile.floor*height_scale);
      glEnd();
      break;

   case ua_tile_slope_w:
      glBegin(GL_QUADS);
      glTexCoord2d(0.0,0.0); glVertex3d(x,y,(tile.floor+tile.slope)*height_scale);
      glTexCoord2d(1.0,0.0); glVertex3d(x+1,y,tile.floor*height_scale);
      glTexCoord2d(1.0,1.0); glVertex3d(x+1,y+1,tile.floor*height_scale);
      glTexCoord2d(0.0,1.0); glVertex3d(x,y+1,(tile.floor+tile.slope)*height_scale);
      glEnd();
      break;

   default: break;
   }

   glPopName();
   glPopName();
}

void ua_renderer_impl::render_ceiling(ua_levelmap_tile& tile, unsigned int x,
   unsigned int y)
{
   if (tile.type == ua_tile_solid)
      return; // don't draw solid tiles

   // use ceiling texture
   texmgr->use(tile.texture_ceiling);
   glPushName((y<<8) + x);
   glPushName(tile.texture_ceiling+0x0400);

   // draw ceiling tile; for simplicity, we only draw a square
   glBegin(GL_QUADS);
   glTexCoord2d(0.0,0.0); glVertex3d(x,y,tile.ceiling*height_scale);
   glTexCoord2d(1.0,0.0); glVertex3d(x,y+1,tile.ceiling*height_scale);
   glTexCoord2d(1.0,1.0); glVertex3d(x+1,y+1,tile.ceiling*height_scale);
   glTexCoord2d(0.0,1.0); glVertex3d(x+1,y,tile.ceiling*height_scale);
   glEnd();

   glPopName();
   glPopName();
}

void ua_renderer_impl::render_walls(ua_levelmap_tile& tile, unsigned int x,
   unsigned int y)
{
   if (tile.type == ua_tile_solid)
      return; // don't draw solid tiles

   Uint16 x1, y1, z1, x2, y2, z2;

   // use wall texture
   texmgr->use(tile.texture_wall);
   glPushName((y<<8) + x);
   glPushName(tile.texture_wall+0x0400);

   // draw diagonal walls
   switch(tile.type)
   {
   case ua_tile_diagonal_se:
      render_wall(ua_left,x,y,tile.floor,x+1,y+1,tile.floor,
         tile.ceiling,tile.ceiling,tile.ceiling);
      break;

   case ua_tile_diagonal_sw:
      render_wall(ua_left,x,y+1,tile.floor,x+1,y,tile.floor,
         tile.ceiling,tile.ceiling,tile.ceiling);
      break;

   case ua_tile_diagonal_nw:
      render_wall(ua_left,x+1,y+1,tile.floor,x,y,tile.floor,
         tile.ceiling,tile.ceiling,tile.ceiling);
      break;

   case ua_tile_diagonal_ne:
      render_wall(ua_left,x+1,y,tile.floor,x,y+1,tile.floor,
         tile.ceiling,tile.ceiling,tile.ceiling);
      break;

   default: break;
   }

   ua_level& level = underw->get_current_level();

   // draw every side
   for(unsigned int side=ua_left; side<=ua_back; side++)
   {
      // ignore some walls for diagonal wall tiles
      switch(tile.type)
      {
      case ua_tile_diagonal_se:
         if (side==ua_left || side==ua_front) continue;
         break;

      case ua_tile_diagonal_sw:
         if (side==ua_right || side==ua_front) continue;
         break;

      case ua_tile_diagonal_nw:
         if (side==ua_right || side==ua_back) continue;
         break;

      case ua_tile_diagonal_ne:
         if (side==ua_left || side==ua_back) continue;
         break;

      default: break;
      }

      // get current tile coordinates
      get_tile_coords(side,tile.type,
         x,y,tile.floor,tile.slope,tile.ceiling,
         x1,y1,z1, x2,y2,z2);

      // get adjacent tile coordinates
      Uint16 nx=0, ny=0, nz1, nz2;
      switch(side)
      {
      case ua_left:  nx=x-1; ny=y; break;
      case ua_right: nx=x+1; ny=y; break;
      case ua_front: ny=y+1; nx=x; break;
      case ua_back:  ny=y-1; nx=x; break;
      }

      if (nx<64 && ny<64)
      {
         // tile inside map
         ua_levelmap_tile &ntile = level.get_tile(nx,ny);

         if (ntile.type == ua_tile_solid)
         {
            // wall goes up to the ceiling
            nz1 = nz2 = ntile.ceiling;
         }
         else
         {
            // get z coordinates for the adjacent tile
            unsigned int adjside;
            switch(side)
            {
            case ua_left: adjside=ua_right; break;
            case ua_right: adjside=ua_left; break;
            case ua_front: adjside=ua_back; break;
            default: adjside=ua_front; break;
            }

            Uint16 dummy=0;
            get_tile_coords(adjside,ntile.type,nx,ny,
               ntile.floor,ntile.slope,ntile.ceiling,
               dummy,dummy,nz1, dummy,dummy,nz2);

            // if the wall to the adjacent tile goes up (e.g. a stair),
            // we draw that wall. if it goes down, the adjacent tile has to
            // draw that wall. so we only draw walls that go up to another
            // tile or the ceiling.

            if (nz1 == nz2 && nz2 == ntile.ceiling)
            {
               // get_tile_coords() returns this, when the adjacent wall is a
               // diagonal wall. we assume the diagonal tile has the same
               // height as our current tile to render.

               nz1 = nz2 = tile.ceiling;
            }

            // determine if we should draw the wall
            if (nz1 < z1 || nz2 < z2)
               continue;
         }
      }
      else
      {
         // tile outside map
         // seems to never happen, but only to be at the safe side
         nz1 = nz2 = tile.ceiling;
      }

      // special case: no wall to draw
      if (z1 == nz1 && z2 == nz2)
         continue;

      // now that we have all info, draw the tile wall
      render_wall(side,x1,y1,z1,x2,y2,z2,nz1,nz2,tile.ceiling);
   }

   glPopName();
   glPopName();
}

void ua_renderer_impl::render_objects(unsigned int x, unsigned int y)
{
   glPushName((y<<8) + x);

   // find out billboard right and up vectors
   {
      float modelview[16];

      // get the current modelview matrix
      glGetFloatv(GL_MODELVIEW_MATRIX , modelview);

      // retrieve right and up vectors
      bb_right.set(modelview[0],modelview[4],modelview[8]);
      bb_up.set(modelview[1],modelview[5],modelview[9]);

      bb_right.normalize();
      bb_up.normalize();
   }

   // enable alpha blending
   glEnable(GL_BLEND);

   // prevent pixels with alpha < 0.1 to write to color and depth buffer
   glAlphaFunc(GL_GREATER, 0.1f);
   glEnable(GL_ALPHA_TEST);

   ua_object_list& objlist = underw->get_current_level().get_mapobjects();

   // get first object link
   Uint16 link = objlist.get_tile_list_start(x,y);

   while(link != 0)
   {
      ua_object& obj = objlist.get_object(link);

      // remember object list position for picking
      glPushName(link);

      // render object
      render_object(obj,x,y);

      glPopName();

      // next object in link chain
      link = obj.get_object_info().link;
   }

   // disable alpha blending again
   glDisable(GL_BLEND);
   glDisable(GL_ALPHA_TEST);

   glPopName();
}

/*! objects are drawn using the method described in the billboarding tutorial,
    "Cheating - Faster but not so easy" */
void ua_renderer_impl::render_object(ua_object& obj, unsigned int x, unsigned int y)
{
   Uint16 item_id = obj.get_object_info().item_id;

#ifndef HAVE_DEBUG
   // don't render invisible objects
   if (obj.get_object_info().is_hidden)
      return;
#endif

   // hack: don't render some objects we currently don't support
   if ((item_id >= 0x00da && item_id <= 0x00df) || item_id == 0x012e)
      return;

   ua_level& level = underw->get_current_level();
   ua_object_info_ext& extinfo = obj.get_ext_object_info();

   double objxpos = static_cast<double>(x) + (extinfo.xpos)/7.0;
   double objypos = static_cast<double>(y) + (extinfo.ypos)/7.0;
   double height = extinfo.zpos*height_scale;

   ua_vector3d base(objxpos, objypos, height);

   // check if a 3d model is available for that item
   if (modelmgr->model_avail(item_id))
   {
      base.z = extinfo.zpos*height_scale;

      // hack: set texture for bridge
      if (item_id==0x0164)
      {
         // TODO move this to bridge rendering
         texmgr->use(obj.get_object_info().flags);
      }

      modelmgr->render(obj,base);
      return;
   }
   if (item_id >= 0x0040 && item_id < 0x0080)
   {
      // critter object
      ua_critter& crit = critpool->get_critter(item_id-0x0040);
      ua_texture& tex = crit.get_texture();

      double u = tex.get_tex_u(), v = tex.get_tex_v();
      double hot_u = crit.get_hotspot_u(), hot_v = crit.get_hotspot_v();

      double scale_x = 0.4*u*tex.get_xres()/64.0;
      double scale_z = 0.9*v*tex.get_yres()/64.0;

      // modify base point
      base.z -= (v-hot_v)/v*scale_z*bb_up.length();
      // TODO: modify x/y coords according to hot_u

      ua_vector3d bb_up_save(bb_up);
      bb_up.x = bb_up.y = 0.0; // NPC's up vector 
      bb_up.normalize();

      //Johnm - use the texture for the current frame of critter animation
      tex.use(crit.get_currentframe());

      draw_billboard_quad(base,
         scale_x,
         scale_z,
         0.0, 0.0, u,v);

      bb_up = bb_up_save;
   }
   else
   {
      // switches/levers/buttons/pull chains
      if ((item_id >= 0x0170 && item_id <= 0x017f) ||
          item_id == 0x0161 || // a_lever
          item_id == 0x0162 || // a_switch
          item_id == 0x0166)   // some_writing
      {
         render_decal(obj,x,y);
         return;
      }

      // special tmap object
      if (item_id == 0x016e || item_id == 0x016f)
      {
         render_tmap_obj(obj,x,y);
#ifndef HAVE_DEBUG
         return;
#endif
      }

      // normal object
      double quadwidth = 0.25;

      // items that have to be drawn at the ceiling?
      if (item_id == 0x00d3 || item_id == 0x00d4) // a_stalactite / a_plant
      {
         // adjust height
         base.z = level.get_tile(x,y).ceiling*height_scale - quadwidth;
      }

      // rune items?
      if (item_id>=0x00e8 && item_id<0x0100)
         item_id = 0x00e0; // generic rune-on-the-floor item

      // get object texture coords
      double u1,v1,u2,v2;
      texmgr->object_tex(item_id,u1,v1,u2,v2);

      draw_billboard_quad(base, 0.5*quadwidth, quadwidth,
         u1,v1,u2,v2);
   }
}

void ua_renderer_impl::render_decal(ua_object& obj, unsigned int x, unsigned int y)
{
   // 0x0161 a_lever
   // 0x0162 a_switch
   // 0x0166 some writing
   // 0x017x buttons/switches/levers/pull chain

   ua_object_info_ext& extinfo = obj.get_ext_object_info();
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
   Uint16 item_id = obj.get_object_info().item_id;
   unsigned int tex = 0;

   switch(item_id)
   {
   case 0x0161: // a_lever
      tex = obj.get_object_info().flags + 4 + ua_tex_stock_tmobj;
      break;

   case 0x0162: // a_switch
      tex = obj.get_object_info().flags + 12 + ua_tex_stock_tmobj;
      break;

   case 0x0166: // some_writing
      tex = (obj.get_object_info().flags & 7) + 20 + ua_tex_stock_tmobj;
      break;

   default: // 0x017x
      tex = (item_id & 15) + ua_tex_stock_switches;
      break;
   }

   texmgr->use(tex);

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

void ua_renderer_impl::render_tmap_obj(ua_object& obj, unsigned int x, unsigned int y)
{
   // 0x016e / 0x016f special tmap object
   ua_object_info_ext& extinfo = obj.get_ext_object_info();

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

#ifdef HAVE_DEBUG
   // render "tmap_c" or "tmap_s" overlay
   texmgr->object_tex(obj.get_object_info().item_id,u1,v1,u2,v2);

   glBegin(GL_QUADS);
   glTexCoord2d(u2,v2); glVertex3d(pos.x+dir.x, pos.y+dir.y, pos.z);
   glTexCoord2d(u2,v1); glVertex3d(pos.x+dir.x, pos.y+dir.y, pos.z+1.0);
   glTexCoord2d(u1,v1); glVertex3d(pos.x-dir.x, pos.y-dir.y, pos.z+1.0);
   glTexCoord2d(u1,v2); glVertex3d(pos.x-dir.x, pos.y-dir.y, pos.z);
   glEnd();
#endif

   texmgr->use(obj.get_object_info().owner);
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

void ua_renderer_impl::draw_billboard_quad(
   ua_vector3d base,
   double quadwidth, double quadheight,
   double u1,double v1,double u2,double v2)
{
   // calculate vectors for that quad
   ua_vector3d base2(base);

   base  -= bb_right*quadwidth;
   base2 += bb_right*quadwidth;

   ua_vector3d high1(base);
   ua_vector3d high2(base2);

   high1 += bb_up*quadheight;
   high2 += bb_up*quadheight;

   // render quad
   glBegin(GL_QUADS);
   glTexCoord2d(u1,v2); glVertex3d(base .x,base .y,base .z);
   glTexCoord2d(u2,v2); glVertex3d(base2.x,base2.y,base2.z);
   glTexCoord2d(u2,v1); glVertex3d(high2.x,high2.y,high2.z);
   glTexCoord2d(u1,v1); glVertex3d(high1.x,high1.y,high1.z);
   glEnd();
}
#endif
/*
void ua_renderer_impl::get_tile_coords(
   unsigned int side, ua_levelmap_tiletype type,
   unsigned int basex, unsigned int basey, Uint16 basez, Uint16 slope, Uint16 ceiling,
   Uint16 &x1, Uint16 &y1, Uint16 &z1,
   Uint16 &x2, Uint16 &y2, Uint16 &z2)
{
   // determine x and y coordinates
   switch(side)
   {
   case ua_left:
      x1 = basex; x2 = basex;
      y1 = basey; y2 = basey+1;
      break;
   case ua_right:
      x1 = basex+1; x2 = basex+1;
      y1 = basey; y2 = basey+1;
      break;
   case ua_front:
      x1 = basex; x2 = basex+1;
      y1 = basey+1; y2 = basey+1;
      break;
   case ua_back:
      x1 = basex; x2 = basex+1;
      y1 = basey; y2 = basey;
      break;
   }

   // determine z coordinates
   z1 = z2 = basez;
   switch(side)
   {
   case ua_left:
      if (type == ua_tile_slope_w || type == ua_tile_slope_s) z1 += slope;
      if (type == ua_tile_slope_w || type == ua_tile_slope_n) z2 += slope;
      // note: wall height set to ceiling
      // as this function is called for adjacent tile walls only
      if (type == ua_tile_diagonal_se || type == ua_tile_diagonal_ne)
         z1 = z2 = ceiling;
      break;

   case ua_right:
      if (type == ua_tile_slope_e || type == ua_tile_slope_s) z1 += slope;
      if (type == ua_tile_slope_e || type == ua_tile_slope_n) z2 += slope;
      if (type == ua_tile_diagonal_sw || type == ua_tile_diagonal_nw)
         z1 = z2 = ceiling;
      break;

   case ua_front:
      if (type == ua_tile_slope_n || type == ua_tile_slope_w) z1 += slope;
      if (type == ua_tile_slope_n || type == ua_tile_slope_e) z2 += slope;
      if (type == ua_tile_diagonal_se || type == ua_tile_diagonal_sw)
         z1 = z2 = ceiling;
      break;

   case ua_back:
      if (type == ua_tile_slope_s || type == ua_tile_slope_w) z1 += slope;
      if (type == ua_tile_slope_s || type == ua_tile_slope_e) z2 += slope;
      if (type == ua_tile_diagonal_nw || type == ua_tile_diagonal_ne)
         z1 = z2 = ceiling;
      break;
   }
}
*/
#if 0
void ua_renderer_impl::render_wall(unsigned int side,
   Uint16 x1, Uint16 y1, Uint16 z1, Uint16 x2, Uint16 y2, Uint16 z2,
   Uint16 nz1, Uint16 nz2, Uint16 ceiling)
{
   // calculate texture coordinates
   double v1,v2,v3,v4;
   v1=(ceiling-z1)*height_scale;
   v2=(ceiling-z2)*height_scale;
   v3=(ceiling-nz2)*height_scale;
   v4=(ceiling-nz1)*height_scale;

   glBegin(GL_QUADS);

   // draw with proper winding
   if (side == ua_left || side == ua_front)
   {
      glTexCoord2d(0.0,v1); glVertex3d(x1,y1,z1*height_scale);
      glTexCoord2d(1.0,v2); glVertex3d(x2,y2,z2*height_scale);
      glTexCoord2d(1.0,v3); glVertex3d(x2,y2,nz2*height_scale);
      glTexCoord2d(0.0,v4); glVertex3d(x1,y1,nz1*height_scale);
   }
   else
   {
      glTexCoord2d(1.0,v1); glVertex3d(x1,y1,z1*height_scale);
      glTexCoord2d(1.0,v4); glVertex3d(x1,y1,nz1*height_scale);
      glTexCoord2d(0.0,v3); glVertex3d(x2,y2,nz2*height_scale);
      glTexCoord2d(0.0,v2); glVertex3d(x2,y2,z2*height_scale);
   }

   glEnd();
}
#endif
