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

   \brief level map and object implementation

*/

// needed includes
#include "common.hpp"
#include "level.hpp"
#include <string>
#include <set>
#include <algorithm>
#include <cmath>


#undef HAVE_ALTERNATE_RENDERER


// ua_level methods

void ua_level::prepare_textures(ua_texture_manager &texmgr)
{
   // prepare all used textures
   unsigned int max = used_textures.size();
   for(unsigned int n=0; n<max; n++)
      texmgr.prepare(used_textures[n]);

   // prepare ceiling texture
   texmgr.prepare(ceiling_texture);
}

double ua_level::get_floor_height(double xpos, double ypos)
{
   if (xpos<0.0 || ypos<0.0 || xpos>64.0 || ypos>64.0)
      return 0.0;

   double height = 0.0;

   ua_levelmap_tile &tile = tiles[static_cast<Uint8>(ypos)*64 + static_cast<Uint8>(xpos)];

   switch(tile.type)
   {
   case ua_tile_solid:
      height = tile.ceiling*height_scale; // player shouldn't get there, though
      break;

      // sloped tiles
   case ua_tile_slope_n:
      height = tile.floor*height_scale +
         double(tile.slope*height_scale)*fmod(ypos,1.0);
      break;
   case ua_tile_slope_s:
      height = (tile.floor+tile.slope)*height_scale -
         double(tile.slope*height_scale)*fmod(ypos,1.0);
      break;
   case ua_tile_slope_e:
      height = tile.floor*height_scale +
         double(tile.slope*height_scale)*fmod(xpos,1.0);
      break;
   case ua_tile_slope_w:
      height = (tile.floor+tile.slope)*height_scale -
         double(tile.slope*height_scale)*fmod(xpos,1.0);
      break;

      // diagonal tiles
   case ua_tile_diagonal_se:
      if (fmod(xpos,1.0)-fmod(ypos,1.0)<0.0) height = tile.ceiling*height_scale;
      else height = tile.floor*height_scale;
      break;
   case ua_tile_diagonal_sw:
      if (fmod(xpos,1.0)+fmod(ypos,1.0)>1.0) height = tile.ceiling*height_scale;
      else height = tile.floor*height_scale;
      break;
   case ua_tile_diagonal_nw:
      if (fmod(xpos,1.0)-fmod(ypos,1.0)>0.0) height = tile.ceiling*height_scale;
      else height = tile.floor*height_scale;
      break;
   case ua_tile_diagonal_ne:
      if (fmod(xpos,1.0)+fmod(ypos,1.0)<1.0) height = tile.ceiling*height_scale;
      else height = tile.floor*height_scale;
      break;

   case ua_tile_open:
   default:
      height = tile.floor*height_scale;
      break;
   };

   return height;
}

ua_levelmap_tile& ua_level::get_tile(unsigned int xpos, unsigned int ypos)
{
   xpos%=64; ypos%=64;
   return tiles[ypos*64 + xpos];
}

void add_wall(ua_triangle3d_textured& tri1, ua_triangle3d_textured& tri2,
   ua_levelmap_wall_render_side side,
   double x1, double y1, double z1,
   double x2, double y2, double z2,
   double nz1, double nz2, double ceiling)
{
   // calculate texture coordinates
   double v1,v2,v3,v4;
   v1 = ceiling-z1; // ceiling is the "base" for textures
   v2 = ceiling-z2;
   v3 = ceiling-nz2;
   v4 = ceiling-nz1;

   // add triangles with proper winding
   if (side == ua_left || side == ua_front)
   {
      tri1.set(0,x1,y1,z1, 0.0,v1);
      tri1.set(1,x2,y2,z2, 1.0,v2);
      tri1.set(2,x2,y2,nz2, 1.0,v3);

      tri2.set(0,x1,y1,z1, 0.0,v1);
      tri2.set(1,x2,y2,nz2, 1.0,v3);
      tri2.set(2,x1,y1,nz1, 0.0,v4);
   }
   else
   {
      tri1.set(0,x1,y1,z1, 1.0,v1);
      tri1.set(1,x1,y1,nz1, 1.0,v4);
      tri1.set(2,x2,y2,nz2, 0.0,v3);

      tri2.set(0,x1,y1,z1, 1.0,v1);
      tri2.set(1,x2,y2,nz2, 0.0,v3);
      tri2.set(2,x2,y2,z2, 0.0,v2);
   }
}


void ua_level::get_tile_triangles(unsigned int xpos, unsigned int ypos,
   std::vector<ua_triangle3d_textured>& alltriangles)
{
   ua_levelmap_tile& tile = get_tile(xpos,ypos);

   if (tile.type == ua_tile_solid)
      return; // no triangles to generate

   double x = xpos, y = ypos;
   double ceil_height = tile.ceiling*height_scale;
   double floor_height = tile.floor*height_scale;

   // tile walls
   {
      Uint16 walltex = tile.texture_wall;

      // diagonal walls
      {
         bool diag_used = true;
         ua_triangle3d_textured diag_tri1,diag_tri2;

         diag_tri1.texnum = diag_tri2.texnum = walltex;

         switch(tile.type)
         {
         case ua_tile_diagonal_se:
            add_wall(diag_tri1, diag_tri2,
               ua_left,x,y,floor_height,x+1,y+1,floor_height,
               ceil_height,ceil_height,ceil_height);
            break;

         case ua_tile_diagonal_sw:
            add_wall(diag_tri1, diag_tri2,
               ua_left,x,y+1,floor_height,x+1,y,floor_height,
               ceil_height,ceil_height,ceil_height);
            break;

         case ua_tile_diagonal_nw:
            add_wall(diag_tri1, diag_tri2,
               ua_left,x+1,y+1,floor_height,x,y,floor_height,
               ceil_height,ceil_height,ceil_height);
            break;

         case ua_tile_diagonal_ne:
            add_wall(diag_tri1, diag_tri2,
               ua_left,x+1,y,floor_height,x,y+1,floor_height,
               ceil_height,ceil_height,ceil_height);
            break;

         default:
            diag_used = false;
         }

         if (diag_used)
         {
            alltriangles.push_back(diag_tri1);
            alltriangles.push_back(diag_tri2);
         }
      }

      // draw every wall side
      for(unsigned int side=ua_left; side<=ua_back; side++)
      {
         // ignore some walls for diagonal wall tiles
         if ((tile.type==ua_tile_diagonal_se && (side==ua_left || side==ua_front)) ||
             (tile.type==ua_tile_diagonal_sw && (side==ua_right || side==ua_front)) ||
             (tile.type==ua_tile_diagonal_nw && (side==ua_right || side==ua_back)) ||
             (tile.type==ua_tile_diagonal_ne && (side==ua_left || side==ua_back)) )
            continue;

         Uint16 x1, y1, z1, x2, y2, z2;

         // get current tile coordinates
         get_tile_coords((ua_levelmap_wall_render_side)side,tile.type,
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

         ua_triangle3d_textured tri1,tri2;
         tri1.texnum = tri2.texnum = walltex;

         // now that we have all info, draw the tile wall
//         render_wall((ua_levelmap_wall_render_side)side,x1,y1,z1,x2,y2,z2,nz1,nz2,tile.ceiling);
         add_wall(tri1,tri2,
            (ua_levelmap_wall_render_side)side,
            x1,y1,z1*height_scale,
            x2,y2,z2*height_scale,
            nz1*height_scale,nz2*height_scale,ceil_height);

         alltriangles.push_back(tri1);
         alltriangles.push_back(tri2);

      } // end for
   }

   // tile floor / ceiling
   {
      double floor_slope_height = (tile.floor+tile.slope)*height_scale;

      ua_triangle3d_textured floor_tri1, floor_tri2;
      ua_triangle3d_textured ceil_tri1, ceil_tri2;
      bool tri2_used = true;

      // set texnums
      ceil_tri1.texnum = ceil_tri2.texnum = ceiling_texture;
      floor_tri1.texnum = floor_tri2.texnum = tile.texture_floor;

      // common ceiling quad
      ceil_tri1.set(0,x,y,ceil_height, 0.0,0.0);
      ceil_tri1.set(1,x+1,y+1,ceil_height, 1.0,1.0);
      ceil_tri1.set(2,x+1,y,ceil_height, 1.0,0.0);
      ceil_tri2.set(0,x,y,ceil_height, 0.0,0.0);
      ceil_tri2.set(1,x,y+1,ceil_height, 0.0,1.0);
      ceil_tri2.set(2,x+1,y+1,ceil_height, 1.0,1.0);

      switch(tile.type)
      {
      case ua_tile_open:
         floor_tri1.set(0,x,y,floor_height, 0.0,0.0);
         floor_tri1.set(1,x+1,y,floor_height, 1.0,0.0);
         floor_tri1.set(2,x+1,y+1,floor_height, 1.0,1.0);
         floor_tri2.set(0,x,y,floor_height, 0.0,0.0);
         floor_tri2.set(1,x+1,y+1,floor_height, 1.0,1.0);
         floor_tri2.set(2,x,y+1,floor_height, 0.0,1.0);
         break;

      case ua_tile_diagonal_se:
         floor_tri1.set(0,x,y,floor_height, 0.0,0.0);
         floor_tri1.set(1,x+1,y,floor_height, 1.0,0.0);
         floor_tri1.set(2,x+1,y+1,floor_height, 1.0,1.0);
         tri2_used = false;
         break;

      case ua_tile_diagonal_sw:
         floor_tri1.set(0,x,y,floor_height, 0.0,0.0);
         floor_tri1.set(1,x+1,y,floor_height, 1.0,0.0);
         floor_tri1.set(2,x,y+1,floor_height, 0.0,1.0);

         ceil_tri1.set(0,x,y,ceil_height, 0.0,0.0);
         ceil_tri1.set(1,x,y+1,ceil_height, 0.0,1.0);
         ceil_tri1.set(2,x+1,y,ceil_height, 1.0,0.0);
         tri2_used = false;
         break;

      case ua_tile_diagonal_nw:
         floor_tri1.set(0,x,y,floor_height, 0.0,0.0);
         floor_tri1.set(1,x+1,y+1,floor_height, 1.0,1.0);
         floor_tri1.set(2,x,y+1,floor_height, 0.0,1.0);
         ceil_tri1 = ceil_tri2;
         tri2_used = false;
         break;

      case ua_tile_diagonal_ne:
         floor_tri1.set(0,x,y+1,floor_height, 0.0,1.0);
         floor_tri1.set(1,x+1,y,floor_height, 1.0,0.0);
         floor_tri1.set(2,x+1,y+1,floor_height, 1.0,1.0);

         ceil_tri1.set(0,x,y+1,ceil_height, 0.0,1.0);
         ceil_tri1.set(1,x+1,y+1,ceil_height, 1.0,1.0);
         ceil_tri1.set(2,x+1,y,ceil_height, 1.0,0.0);

         tri2_used = false;
         break;

      case ua_tile_slope_n:
         floor_tri1.set(0,x,y,floor_height, 0.0,0.0);
         floor_tri1.set(1,x+1,y,floor_height, 1.0,0.0);
         floor_tri1.set(2,x+1,y+1,floor_slope_height, 1.0,1.0);
         floor_tri2.set(0,x,y,floor_height, 0.0,0.0);
         floor_tri2.set(1,x+1,y+1,floor_slope_height, 1.0,1.0);
         floor_tri2.set(2,x,y+1,floor_slope_height, 0.0,1.0);
         break;

      case ua_tile_slope_s:
         floor_tri1.set(0,x,y,floor_slope_height, 0.0,0.0);
         floor_tri1.set(1,x+1,y,floor_slope_height, 1.0,0.0);
         floor_tri1.set(2,x+1,y+1,floor_height, 1.0,1.0);
         floor_tri2.set(0,x,y,floor_slope_height, 0.0,0.0);
         floor_tri2.set(1,x+1,y+1,floor_height, 1.0,1.0);
         floor_tri2.set(2,x,y+1,floor_height, 0.0,1.0);
         break;

      case ua_tile_slope_e:
         floor_tri1.set(0,x,y,floor_height, 0.0,0.0);
         floor_tri1.set(1,x+1,y,floor_slope_height, 1.0,0.0);
         floor_tri1.set(2,x+1,y+1,floor_slope_height, 1.0,1.0);
         floor_tri2.set(0,x,y,floor_height, 0.0,0.0);
         floor_tri2.set(1,x+1,y+1,floor_slope_height, 1.0,1.0);
         floor_tri2.set(2,x,y+1,floor_height, 0.0,1.0);
         break;

      case ua_tile_slope_w:
         floor_tri1.set(0,x,y,floor_slope_height, 0.0,0.0);
         floor_tri1.set(1,x+1,y,floor_height, 1.0,0.0);
         floor_tri1.set(2,x+1,y+1,floor_height, 1.0,1.0);
         floor_tri2.set(0,x,y,floor_slope_height, 0.0,0.0);
         floor_tri2.set(1,x+1,y+1,floor_height, 1.0,1.0);
         floor_tri2.set(2,x,y+1,floor_slope_height, 0.0,1.0);
         break;

      } // end switch

      // insert triangles into list
      alltriangles.push_back(floor_tri1);
      if (tri2_used)
         alltriangles.push_back(floor_tri2);

      alltriangles.push_back(ceil_tri1);
      if (tri2_used)
         alltriangles.push_back(ceil_tri2);
   }
}

void ua_level::load_game(ua_savegame &sg)
{
   // read tilemap
   tiles.clear();
   tiles.resize(64*64);
   unsigned int n=0;

   std::set<Uint16> textures_used;

   for(n=0; n<64*64; n++)
   {
      ua_levelmap_tile &tile = tiles[n];
      tile.type = static_cast<ua_levelmap_tiletype>(sg.read8());
      tile.floor = sg.read16();
      tile.ceiling = sg.read16();
      tile.slope = sg.read8();
      tile.texture_wall = sg.read16();
      tile.texture_floor = sg.read16();

      textures_used.insert(tile.texture_wall);
      textures_used.insert(tile.texture_floor);
   }

   // add used textures to used_textures vector
   std::set<Uint16>::iterator iter,stop;
   iter = textures_used.begin();
   stop = textures_used.end();

   used_textures.clear();

   for(; iter != stop; iter++)
      used_textures.push_back(*iter);

   // read ceiling texture
   ceiling_texture = sg.read16();

   // read objects list
   allobjects.load_game(sg);

   // read annotations list
}

void ua_level::save_game(ua_savegame &sg)
{
   // write tilemap
   unsigned int n=0;

   for(n=0; n<64*64; n++)
   {
      ua_levelmap_tile& tile = tiles[n];
      sg.write8(tile.type);
      sg.write16(tile.floor);
      sg.write16(tile.ceiling);
      sg.write8(tile.slope);
      sg.write16(tile.texture_wall);
      sg.write16(tile.texture_floor);
   }

   // write ceiling texture
   sg.write16(ceiling_texture);

   // write objects list
   allobjects.save_game(sg);

   // write annotations list
}

void ua_level::render(ua_texture_manager &texmgr,ua_frustum &fr)
{
   // determine list of visible tiles
   std::vector<ua_quad_tile_coord> tilelist;
   ua_quad q(0, 0, 64, 64);

   q.get_visible_tiles(fr,tilelist);

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

   // draw floor tile polygons, for all visible tiles
   max = tilelist.size();
   for(i=0;i<max;i++)
   {
      const ua_quad_tile_coord &qtc = tilelist[i];
      render_floor(qtc.first,qtc.second,texmgr);
   }

   // draw visible tile ceilings
   for(i=0;i<max;i++)
   {
      const ua_quad_tile_coord &qtc = tilelist[i];
      render_ceiling(qtc.first,qtc.second,texmgr);
   }

   // draw all visible walls
   for(i=0;i<max;i++)
   {
      const ua_quad_tile_coord &qtc = tilelist[i];
      render_walls(qtc.first,qtc.second,texmgr);
   }

#endif

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
}

void ua_level::render(ua_texture_manager &texmgr)
{
   int x,y;

   for(y=0; y<64;y++) for(x=0; x<64;x++)
      render_floor(x,y,texmgr);

   for(y=0; y<64;y++) for(x=0; x<64;x++)
      render_ceiling(x,y,texmgr);

   for(y=0; y<64;y++) for(x=0; x<64;x++)
      render_walls(x,y,texmgr);
/*
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
}

void ua_level::render_floor(unsigned int x, unsigned int y, ua_texture_manager &texmgr)
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
}

void ua_level::render_ceiling(unsigned int x, unsigned int y, ua_texture_manager &texmgr)
{
   ua_levelmap_tile &tile = tiles[y*64 + x];
   if (tile.type == ua_tile_solid)
      return; // don't draw solid tiles

   // use ceiling texture
   texmgr.use(ceiling_texture);

   glColor3ub(192,192,192);

   // draw ceiling tile; for simplicity, we only draw a square
   glBegin(GL_QUADS);
   glTexCoord2d(0.0,0.0); glVertex3d(x,y,tile.ceiling*height_scale);
   glTexCoord2d(1.0,0.0); glVertex3d(x,y+1,tile.ceiling*height_scale);
   glTexCoord2d(1.0,1.0); glVertex3d(x+1,y+1,tile.ceiling*height_scale);
   glTexCoord2d(0.0,1.0); glVertex3d(x+1,y,tile.ceiling*height_scale);
   glEnd();
}

void ua_level::render_walls(unsigned int x, unsigned int y, ua_texture_manager &texmgr)
{
   ua_levelmap_tile &tile = tiles[y*64 + x];
   if (tile.type == ua_tile_solid)
      return; // don't draw solid tiles

   Uint16 x1, y1, z1, x2, y2, z2;

   // use wall texture
   texmgr.use(tile.texture_wall);

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
      render_wall((ua_levelmap_wall_render_side)side,x1,y1,z1,x2,y2,z2,nz1,nz2,tile.ceiling);
   }
}

void ua_level::render_objs(unsigned int x, unsigned int y,
   ua_texture_manager &texmgr, ua_frustum &fr)
{
   ua_object obj;

   if (allobjects.get_first_tile_object(x,y,obj))
   do
   {
      obj.render(x,y,texmgr,fr,*this);

   } while(allobjects.get_next_tile_object(obj));
}

void ua_level::get_tile_coords(
   ua_levelmap_wall_render_side side, ua_levelmap_tiletype type,
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

void ua_level::render_wall(ua_levelmap_wall_render_side side,
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
