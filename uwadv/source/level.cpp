/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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
/*! \file level.cpp

   level map and object implementation

*/

// needed includes
#include "common.hpp"
#include "level.hpp"
#include <string>
#include <cmath>


// ua_level methods

void ua_level::prepare_textures(ua_texture_manager &texmgr)
{
   int tex;
   for(tex=0;tex<48;tex++) texmgr.prepare(wall_textures[tex]);
   for(tex=0;tex<10;tex++) texmgr.prepare(floor_textures[tex]);

   // prepare door textures

   // ceiling texture, always #15
   texmgr.prepare(0x010f);
}

float ua_level::get_floor_height(float xpos, float ypos)
{
   if (xpos<0.f || ypos<0.f || xpos>64.f || ypos>64.f)
      return 0.f;

   float height = 0.f;

   ua_levelmap_tile &tile = tiles[Uint8(ypos)*64 + Uint8(xpos)];

   switch(tile.type)
   {
   case ua_tile_solid:
      height = 0.f; // player shouldn't get there, though
      break;

   case ua_tile_slope_n:
      height = tile.floor*height_scale + 
         float(tile.slope*height_scale)*fmod(ypos,1.0);
      break;
   case ua_tile_slope_s:
      height = (tile.floor+tile.slope)*height_scale -
         float(tile.slope*height_scale)*fmod(ypos,1.0);
      break;
   case ua_tile_slope_e:
      height = tile.floor*height_scale +
         float(tile.slope*height_scale)*fmod(xpos,1.0);
      break;
   case ua_tile_slope_w:
      height = (tile.floor+tile.slope)*height_scale -
         float(tile.slope*height_scale)*fmod(xpos,1.0);
      break;

   case ua_tile_open:
   case ua_tile_diagonal_se:
   case ua_tile_diagonal_sw:
   case ua_tile_diagonal_nw:
   case ua_tile_diagonal_ne:
   default:
      height = tile.floor*height_scale;
      break;
   };

   return height;
}

void ua_level::render(ua_texture_manager &texmgr)
{
   int x,y;
   // draw floor tile polygons, for all (or for all visible)
   for(y=0; y<64; y++)
   for(x=0; x<64; x++)
      render_floor(x,y,texmgr);

   // draw tile ceiling
   for(y=0; y<64; y++)
   for(x=0; x<64; x++)
      render_ceiling(x,y,texmgr);

   // draw all walls
   for(y=0; y<64; y++)
      for(x=0; x<64; x++)
         render_walls(x,y,texmgr);
}

void ua_level::render_floor(int x, int y, ua_texture_manager &texmgr)
{
   ua_levelmap_tile &tile = tiles[y*64 + x];
   if (tile.type == ua_tile_solid)
      return; // don't draw solid tiles

   // use texture
   texmgr.use(tile.texture_floor);
   glColor3ub(192,192,192);

   // draw floor tile
   switch(tile.type)
   {
   case ua_tile_open:
      glBegin(GL_QUADS);
      glTexCoord2d(0.0,0.0); glVertex3f(x,y,tile.floor*height_scale);
      glTexCoord2d(1.0,0.0); glVertex3f(x+1,y,tile.floor*height_scale);
      glTexCoord2d(1.0,1.0); glVertex3f(x+1,y+1,tile.floor*height_scale);
      glTexCoord2d(0.0,1.0); glVertex3f(x,y+1,tile.floor*height_scale);
      glEnd();
      break;

   case ua_tile_diagonal_se:
      glBegin(GL_TRIANGLES);
      glTexCoord2d(0.0,0.0); glVertex3f(x,y,tile.floor*height_scale);
      glTexCoord2d(1.0,0.0); glVertex3f(x+1,y,tile.floor*height_scale);
      glTexCoord2d(1.0,1.0); glVertex3f(x+1,y+1,tile.floor*height_scale);
      glEnd();
      break;

   case ua_tile_diagonal_sw:
      glBegin(GL_TRIANGLES);
      glTexCoord2d(0.0,0.0); glVertex3f(x,y,tile.floor*height_scale);
      glTexCoord2d(1.0,0.0); glVertex3f(x+1,y,tile.floor*height_scale);
      glTexCoord2d(0.0,1.0); glVertex3f(x,y+1,tile.floor*height_scale);
      glEnd();
      break;

   case ua_tile_diagonal_nw:
      glBegin(GL_TRIANGLES);
      glTexCoord2d(0.0,0.0); glVertex3f(x,y,tile.floor*height_scale);
      glTexCoord2d(1.0,1.0); glVertex3f(x+1,y+1,tile.floor*height_scale);
      glTexCoord2d(0.0,1.0); glVertex3f(x,y+1,tile.floor*height_scale);
      glEnd();
      break;

   case ua_tile_diagonal_ne:
      glBegin(GL_TRIANGLES);
      glTexCoord2d(0.0,1.0); glVertex3f(x,y+1,tile.floor*height_scale);
      glTexCoord2d(1.0,0.0); glVertex3f(x+1,y,tile.floor*height_scale);
      glTexCoord2d(1.0,1.0); glVertex3f(x+1,y+1,tile.floor*height_scale);
      glEnd();
      break;

   case ua_tile_slope_n:
      glBegin(GL_QUADS);
      glTexCoord2d(0.0,0.0); glVertex3f(x,y,tile.floor*height_scale);
      glTexCoord2d(1.0,0.0); glVertex3f(x+1,y,tile.floor*height_scale);
      glTexCoord2d(1.0,1.0); glVertex3f(x+1,y+1,(tile.floor+tile.slope)*height_scale);
      glTexCoord2d(0.0,1.0); glVertex3f(x,y+1,(tile.floor+tile.slope)*height_scale);
      glEnd();
      break;

   case ua_tile_slope_s:
      glBegin(GL_QUADS);
      glTexCoord2d(0.0,0.0); glVertex3f(x,y,(tile.floor+tile.slope)*height_scale);
      glTexCoord2d(1.0,0.0); glVertex3f(x+1,y,(tile.floor+tile.slope)*height_scale);
      glTexCoord2d(1.0,1.0); glVertex3f(x+1,y+1,tile.floor*height_scale);
      glTexCoord2d(0.0,1.0); glVertex3f(x,y+1,tile.floor*height_scale);
      glEnd();
      break;

   case ua_tile_slope_e:
      glBegin(GL_QUADS);
      glTexCoord2d(0.0,0.0); glVertex3f(x,y,tile.floor*height_scale);
      glTexCoord2d(1.0,0.0); glVertex3f(x+1,y,(tile.floor+tile.slope)*height_scale);
      glTexCoord2d(1.0,1.0); glVertex3f(x+1,y+1,(tile.floor+tile.slope)*height_scale);
      glTexCoord2d(0.0,1.0); glVertex3f(x,y+1,tile.floor*height_scale);
      glEnd();
      break;

   case ua_tile_slope_w:
      glBegin(GL_QUADS);
      glTexCoord2d(0.0,0.0); glVertex3f(x,y,(tile.floor+tile.slope)*height_scale);
      glTexCoord2d(1.0,0.0); glVertex3f(x+1,y,tile.floor*height_scale);
      glTexCoord2d(1.0,1.0); glVertex3f(x+1,y+1,tile.floor*height_scale);
      glTexCoord2d(0.0,1.0); glVertex3f(x,y+1,(tile.floor+tile.slope)*height_scale);
      glEnd();
      break;
   }
}

void ua_level::render_ceiling(int x, int y, ua_texture_manager &texmgr)
{
   ua_levelmap_tile &tile = tiles[y*64 + x];
   if (tile.type == ua_tile_solid)
      return; // don't draw solid tiles

   // use texture
   texmgr.use(0x010f);
   glColor3ub(192,192,192);

   // draw ceiling tile; for simplicity, we only draw a square
   glBegin(GL_QUADS);
   glTexCoord2d(0.0,0.0); glVertex3f(x,y,tile.ceiling*height_scale);
   glTexCoord2d(1.0,0.0); glVertex3f(x,y+1,tile.ceiling*height_scale);
   glTexCoord2d(1.0,1.0); glVertex3f(x+1,y+1,tile.ceiling*height_scale);
   glTexCoord2d(0.0,1.0); glVertex3f(x+1,y,tile.ceiling*height_scale);
   glEnd();
}

void ua_level::render_walls(int x, int y, ua_texture_manager &texmgr)
{
   ua_levelmap_tile &tile = tiles[y*64 + x];
   if (tile.type == ua_tile_solid)
      return; // don't draw solid tiles

   Uint8 x1, y1, z1, x2, y2, z2, dummy, nz1, nz2;

   // use wall texture
   texmgr.use(tile.texture_wall);

   // draw diagonal walls
   switch(tile.type)
   {
   case ua_tile_diagonal_se:
      render_wall(ua_left,x,y,tile.floor,x+1,y+1,tile.floor,
         tile.ceiling,tile.ceiling);
      break;

   case ua_tile_diagonal_sw:
      render_wall(ua_left,x,y+1,tile.floor,x+1,y,tile.floor,
         tile.ceiling,tile.ceiling);
      break;

   case ua_tile_diagonal_nw:
      render_wall(ua_left,x+1,y+1,tile.floor,x,y,tile.floor,
         tile.ceiling,tile.ceiling);
      break;

   case ua_tile_diagonal_ne:
      render_wall(ua_left,x+1,y,tile.floor,x,y+1,tile.floor,
         tile.ceiling,tile.ceiling);
      break;
   }

   // draw every side
   for(int side=ua_left; side<=ua_back; side++)
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
      get_tile_coords((ua_levelmap_wall_render_side)side,tile.type,
         x,y,tile.floor,tile.slope,tile.ceiling,
         x1,y1,z1, x2,y2,z2);

      // get adjacent tile coordinates
      Uint8 nx, ny;
      switch(side)
      {
      case ua_left:  nx = x-1; ny=y; break;
      case ua_right: nx = x+1; ny=y; break;
      case ua_front: ny = y+1; nx=x; break;
      case ua_back:  ny = y-1; nx=x; break;
      }

      if (nx<64 && ny<64)
      {
         ua_levelmap_tile &ntile = tiles[ny*64 + nx];

         if (ntile.type == ua_tile_solid)
         {
            // wall goes up to the ceiling
            nz1 = nz2 = ntile.ceiling;
         }
         else
         {
            // get z coordinates for the adjacent tile
            ua_levelmap_wall_render_side adjside;
            if (side==ua_left) adjside = ua_right;
            else if (side==ua_right) adjside = ua_left;
            else if (side==ua_front) adjside = ua_back;
            else adjside = ua_front;

            get_tile_coords(adjside,ntile.type,nx,ny,
               ntile.floor,ntile.slope,ntile.ceiling,
               dummy,dummy,nz1, dummy,dummy,nz2);

            if (side==ua_right || side==ua_back)
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
      render_wall((ua_levelmap_wall_render_side)side,x1,y1,z1,x2,y2,z2,nz1,nz2);
   }
}

void ua_level::get_tile_coords(
   ua_levelmap_wall_render_side side, ua_levelmap_tiletype type,
   Uint8 basex, Uint8 basey, Uint8 basez, Uint8 slope, Uint8 ceiling,
   Uint8 &x1, Uint8 &y1, Uint8 &z1,
   Uint8 &x2, Uint8 &y2, Uint8 &z2)
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

void ua_level::render_wall(ua_levelmap_wall_render_side side,
   Uint8 x1, Uint8 y1, Uint8 z1, Uint8 x2, Uint8 y2, Uint8 z2,
   Uint8 nz1, Uint8 nz2)
{
   if (z1 == nz1 && z2 == nz2)
      return;

   glColor4ub(128,128,128,128);

   double v1,v2,v3,v4;
   v1=0.0;
   v2=(z2-z1)*height_scale;
   v3=(nz1-z1-(nz1-nz2))*height_scale;
   v4=(nz1-z1)*height_scale;

   glBegin(GL_QUADS);

   // draw with proper winding
   if (side == ua_left || side == ua_front)
   {
      glTexCoord2d(1.0,v1); glVertex3f(x1,y1,z1*height_scale);
      glTexCoord2d(0.0,v2); glVertex3f(x2,y2,z2*height_scale);
      glTexCoord2d(0.0,v3); glVertex3f(x2,y2,nz2*height_scale);
      glTexCoord2d(1.0,v4); glVertex3f(x1,y1,nz1*height_scale);
   }
   else
   {
      glTexCoord2d(1.0,v1); glVertex3f(x1,y1,z1*height_scale);
      glTexCoord2d(1.0,v4); glVertex3f(x1,y1,nz1*height_scale);
      glTexCoord2d(0.0,v3); glVertex3f(x2,y2,nz2*height_scale);
      glTexCoord2d(0.0,v2); glVertex3f(x2,y2,z2*height_scale);
   }

   glEnd();
}
