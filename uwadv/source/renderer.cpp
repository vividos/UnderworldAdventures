/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Underworld Adventures Team

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
/*! \file renderer.cpp

   \brief underworld renderer

   picking tutorial:
   http://www.lighthouse3d.com/opengl/picking/index.php3

*/

// needed includes
#include "common.hpp"
#include "renderer.hpp"
#include "quadtree.hpp"


// constants

const double ua_renderer::height_scale = 0.125;


// enums

//! side of currently rendered wall; used internally
enum
{
   ua_left, ua_right, ua_front, ua_back
};


// ua_renderer methods

ua_renderer::ua_renderer()
{
}

void ua_renderer::init(ua_underworld* uw, const ua_vector3d& view_offset)
{
   underw = uw;

   // culling
   glCullFace(GL_BACK);
   glFrontFace(GL_CCW);
   glEnable(GL_CULL_FACE);

   // z-buffer
   glEnable(GL_DEPTH_TEST);

   // enable texturing
   glEnable(GL_TEXTURE_2D);

   // smooth shading
   glShadeModel(GL_SMOOTH);

   // fog
   glEnable(GL_FOG);
   glFogi(GL_FOG_MODE,GL_EXP2);
   glFogf(GL_FOG_DENSITY,0.2f); // 0.65f
   glFogf(GL_FOG_START,0.0);
   glFogf(GL_FOG_END,1.0);
   GLint fog_color[4] = { 0,0,0,0 };
   glFogiv(GL_FOG_COLOR,fog_color);

   // give some hints
   glHint(GL_FOG_HINT,GL_DONT_CARE);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
   glHint(GL_POLYGON_SMOOTH_HINT,GL_DONT_CARE);
}

void ua_renderer::done()
{
   glDisable(GL_FOG);
}

void ua_renderer::setup_camera(double myfov, double myaspect, double myfarplane)
{
   fov = myfov;
   aspect = myaspect;
   farplane = myfarplane;

   setup_camera_priv(false,0,0);
}

void ua_renderer::setup_camera_priv(bool pick,unsigned int xpos, unsigned int ypos)
{
   // set projection matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   if (pick)
   {
      // calculate pick matrix
      GLint viewport[4];
      glGetIntegerv(GL_VIEWPORT, viewport);
      gluPickMatrix(GLdouble(xpos), GLdouble(viewport[3]-ypos), 5.0, 5.0, viewport);
   }

   gluPerspective(fov, aspect, 0.25, farplane);

   // switch back to modelview matrix
   glMatrixMode(GL_MODELVIEW);
}

void ua_renderer::render()
{
   ua_player &pl = underw->get_player();
   double plheight = 0.6+underw->get_player_height()*height_scale;
   double rotangle = pl.get_angle_rot();
   double panangle = pl.get_angle_pan();

   {
      // rotation
      glRotated(panangle+270.0, 1.0, 0.0, 0.0);
      glRotated(-rotangle+90.0, 0.0, 0.0, 1.0);

      // move to position on map
      glTranslated(-pl.get_xpos(),-pl.get_ypos(),-plheight);
   }

   // render level
   ua_frustum fr(pl.get_xpos(),pl.get_ypos(),plheight,rotangle,-panangle,fov,16.0);

   // determine list of visible tiles
   std::vector<ua_quad_tile_coord> tilelist;
   ua_quad q(0, 0, 64, 64);

   q.get_visible_tiles(fr,tilelist);

   ua_level &level = underw->get_current_level();
   ua_texture_manager& texmgr = underw->get_game_core()->get_texmgr();

   int i,max;

#ifdef HAVE_ALTERNATE_RENDERER

   // collect all triangles
   std::vector<ua_triangle3d_textured> alltriangles;
   max = tilelist.size();
   for(i=0; i<max; i++)
   {
      const ua_quad_tile_coord &qtc = tilelist[i];
      get_tile_triangles(qtc.first,qtc.second,alltriangles);
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
         glTexCoord2d(tri.tex_u[j],tri.tex_v[j]);
         glVertex3d(tri.points[j].x, tri.points[j].y, tri.points[j].z);
      }
      glEnd();
   }

#else

   glColor3ub(192,192,192);

   // draw floor tile polygons, for all visible tiles
   max = tilelist.size();
   for(i=0;i<max;i++)
   {
      const ua_quad_tile_coord &qtc = tilelist[i];
      ua_levelmap_tile& tile = level.get_tile(qtc.first,qtc.second);

      render_floor(tile,qtc.first,qtc.second,texmgr);
   }

   // draw visible tile ceilings
   for(i=0;i<max;i++)
   {
      const ua_quad_tile_coord &qtc = tilelist[i];
      ua_levelmap_tile& tile = level.get_tile(qtc.first,qtc.second);

      render_ceiling(tile,qtc.first,qtc.second,texmgr);
   }

   // draw all visible walls
   for(i=0;i<max;i++)
   {
      const ua_quad_tile_coord &qtc = tilelist[i];
      ua_levelmap_tile& tile = level.get_tile(qtc.first,qtc.second);

      render_walls(tile,qtc.first,qtc.second,texmgr);
   }

#endif
}

void ua_renderer::select_pick(unsigned int xpos, unsigned int ypos,
   unsigned int &tilex, unsigned int &tiley, bool &isobj, unsigned int &id)
{
   // set selection buffer
   GLuint select_buf[64];
   {
      glSelectBuffer(64,select_buf);

      // render objects in selection mode
      glRenderMode(GL_SELECT);

      // init name stack
      glInitNames();
   }

   // save projection matrix on stack
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();

   // sets projection matrix for selection rendering
   setup_camera_priv(true,xpos,ypos);

   // render scene
   render();

   // switch off selection mode
   GLint hits = glRenderMode(GL_RENDER);

   // restore previous projection matrix
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);

   // find out hit object
   GLint hitid = 0;
   if (hits>0)
   {
      GLuint min = 0xffffffff;
      unsigned int idx = 0;

      for(unsigned int i=0; i<hits && idx<64; i++)
      {
         // get count of names stored in this record
         GLuint namecount = select_buf[idx++];

         // check min. hit dist.
         if (select_buf[idx] < min)
         {
            // new min. hit dist.
            min = select_buf[idx++];
            idx++; // jump over max. hit dist.
            if (namecount>0)
               hitid = select_buf[idx]; // hit id (assumes we only have one)
         }
         else
            idx+=2; // jump over min./max. hit dist.

         // move idx to next record
         idx += namecount;
      }
   }
}

/*
void ua_level::render(ua_texture_manager &texmgr,ua_frustum &fr)
{

   // set up new viewpoint, "view coordinates" used in ua_object::render()
   glPushMatrix();
   glLoadIdentity();
   glRotated(-fr.get_yangle()+270.0, 1.0, 0.0, 0.0);

   // draw all objects in tile
   max = tilelist.size();
   for(i=0;i<max;i++)
   {
      const ua_quad_tile_coord &qtc = tilelist[i];
      render_objs(qtc.first,qtc.second,texmgr,fr);
   }

   // restore old viewpoint
   glPopMatrix();
}*/

/*
void ua_renderer::render(ua_texture_manager &texmgr)
{
   int x,y;

   glColor3ub(192,192,192);

   for(y=0; y<64;y++) for(x=0; x<64;x++)
      render_floor(x,y,texmgr);

   for(y=0; y<64;y++) for(x=0; x<64;x++)
      render_ceiling(x,y,texmgr);

   for(y=0; y<64;y++) for(x=0; x<64;x++)
      render_walls(x,y,texmgr);
*//*
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
*/
//}

void ua_renderer::render_floor(ua_levelmap_tile& tile, unsigned int x,
   unsigned int y, ua_texture_manager &texmgr)
{
   if (tile.type == ua_tile_solid)
      return; // don't draw solid tiles

   // use texture
   texmgr.use(tile.texture_floor);
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
   }

   glPopName();
   glPopName();
}

void ua_renderer::render_ceiling(ua_levelmap_tile& tile, unsigned int x,
   unsigned int y, ua_texture_manager& texmgr)
{
   if (tile.type == ua_tile_solid)
      return; // don't draw solid tiles

   // use ceiling texture
   texmgr.use(tile.texture_ceiling);
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

void ua_renderer::render_walls(ua_levelmap_tile& tile, unsigned int x,
   unsigned int y, ua_texture_manager& texmgr)
{
   if (tile.type == ua_tile_solid)
      return; // don't draw solid tiles

   Uint16 x1, y1, z1, x2, y2, z2;

   // use wall texture
   texmgr.use(tile.texture_wall);
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
      }

      // get current tile coordinates
      get_tile_coords(side,tile.type,
         x,y,tile.floor,tile.slope,tile.ceiling,
         x1,y1,z1, x2,y2,z2);

      // get adjacent tile coordinates
      Uint16 nx, ny, nz1, nz2;
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

/*
void ua_renderer::render_objs(unsigned int x, unsigned int y,
   ua_texture_manager &texmgr, ua_frustum &fr)
{
   ua_object obj;

   if (allobjects.get_first_tile_object(x,y,obj))
   do
   {
      obj.render(x,y,texmgr,fr,*this);

   } while(allobjects.get_next_tile_object(obj));
}
*/

void ua_renderer::get_tile_coords(
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

void ua_renderer::render_wall(unsigned int side,
   Uint16 x1, Uint16 y1, Uint16 z1, Uint16 x2, Uint16 y2, Uint16 z2,
   Uint16 nz1, Uint16 nz2, Uint16 ceiling)
{
   glColor4ub(128,128,128,128);

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
