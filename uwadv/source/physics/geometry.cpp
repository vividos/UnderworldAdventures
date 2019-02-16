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
/// \file geometry.cpp
/// \brief level geometry provider implementation
//
#include "common.hpp"
#include "geometry.hpp"

/// side of currently rendered wall; used internally
enum ua_wall_type
{
   ua_left, ua_right, ua_front, ua_back
};

/// Returns all triangles generated for tile with given coordinates.
/// \param xpos x tile position
/// \param ypos y tile position
/// \param alltriangles vector of triangles the new triangles are added to
void ua_geometry_provider::get_tile_triangles(unsigned int xpos,
   unsigned int ypos, std::vector<ua_triangle3d_textured>& alltriangles)
{
   const Underworld::TileInfo& tile = m_level.GetTilemap().GetTileInfo(xpos, ypos);

   if (tile.m_type == Underworld::tileSolid)
      return; // no triangles to generate

   double x = xpos, y = ypos;
   double ceil_height = tile.m_ceiling;
   double floor_height = tile.m_floor;

   // tile walls
   {
      Uint16 walltex = tile.m_textureWall;

      // diagonal walls
      {
         bool diag_used = true;
         ua_triangle3d_textured diag_tri1, diag_tri2;

         diag_tri1.texnum = diag_tri2.texnum = walltex;

         switch (tile.m_type)
         {
         case Underworld::tileDiagonal_se:
            add_wall(diag_tri1, diag_tri2,
               ua_left, x, y, floor_height, x + 1, y + 1, floor_height,
               ceil_height, ceil_height, ceil_height);
            break;

         case Underworld::tileDiagonal_sw:
            add_wall(diag_tri1, diag_tri2,
               ua_left, x, y + 1, floor_height, x + 1, y, floor_height,
               ceil_height, ceil_height, ceil_height);
            break;

         case Underworld::tileDiagonal_nw:
            add_wall(diag_tri1, diag_tri2,
               ua_left, x + 1, y + 1, floor_height, x, y, floor_height,
               ceil_height, ceil_height, ceil_height);
            break;

         case Underworld::tileDiagonal_ne:
            add_wall(diag_tri1, diag_tri2,
               ua_left, x + 1, y, floor_height, x, y + 1, floor_height,
               ceil_height, ceil_height, ceil_height);
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
      for (unsigned int side = ua_left; side <= ua_back; side++)
      {
         // ignore some walls for diagonal wall tiles
         if ((tile.m_type == Underworld::tileDiagonal_se && (side == ua_left || side == ua_front)) ||
            (tile.m_type == Underworld::tileDiagonal_sw && (side == ua_right || side == ua_front)) ||
            (tile.m_type == Underworld::tileDiagonal_nw && (side == ua_right || side == ua_back)) ||
            (tile.m_type == Underworld::tileDiagonal_ne && (side == ua_left || side == ua_back)))
            continue;

         Uint16 x1, y1, z1, x2, y2, z2;

         // get current tile coordinates
         get_tile_coords(side, tile.m_type,
            Uint16(x), Uint16(y), Uint16(tile.m_floor), Uint16(tile.m_slope), Uint16(tile.m_ceiling),
            x1, y1, z1, x2, y2, z2);

         // get adjacent tile coordinates
         Uint16 nx = 0, ny = 0, nz1, nz2;
         switch (side)
         {
         case ua_left:  nx = Uint16(x) - 1; ny = Uint16(y); break;
         case ua_right: nx = Uint16(x) + 1; ny = Uint16(y); break;
         case ua_front: ny = Uint16(y) + 1; nx = Uint16(x); break;
         case ua_back:  ny = Uint16(y) - 1; nx = Uint16(x); break;
         }

         if (nx < 64 && ny < 64)
         {
            // tile inside map

            const Underworld::TileInfo& ntile = m_level.GetTilemap().GetTileInfo(nx, ny);

            if (ntile.m_type == Underworld::tileSolid)
            {
               // wall goes up to the ceiling
               nz1 = nz2 = ntile.m_ceiling;
            }
            else
            {
               // get z coordinates for the adjacent tile
               unsigned int adjside;
               switch (side)
               {
               case ua_left: adjside = ua_right; break;
               case ua_right: adjside = ua_left; break;
               case ua_front: adjside = ua_back; break;
               default: adjside = ua_front; break;
               }

               Uint16 dummy = 0;
               get_tile_coords(adjside, ntile.m_type, nx, ny,
                  ntile.m_floor, ntile.m_slope, ntile.m_ceiling,
                  dummy, dummy, nz1, dummy, dummy, nz2);

               // if the wall to the adjacent tile goes up (e.g. a stair),
               // we draw that wall. if it goes down, the adjacent tile has to
               // draw that wall. so we only draw walls that go up to another
               // tile or the ceiling.

               if (nz1 == nz2 && nz2 == ntile.m_ceiling)
               {
                  // get_tile_coords() returns this, when the adjacent wall is a
                  // diagonal wall. we assume the diagonal tile has the same
                  // height as our current tile to render.

                  nz1 = nz2 = tile.m_ceiling;
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
            nz1 = nz2 = tile.m_ceiling;
         }

         // special case: no wall to draw
         if (z1 == nz1 && z2 == nz2)
            continue;

         ua_triangle3d_textured tri1, tri2;
         tri1.texnum = tri2.texnum = walltex;

         // now that we have all info, draw the tile wall
         add_wall(tri1, tri2, side,
            x1, y1, z1,
            x2, y2, z2,
            nz1, nz2, ceil_height);

         alltriangles.push_back(tri1);
         alltriangles.push_back(tri2);

      } // end for
   }

   // tile floor / ceiling
   {
      double floor_slope_height = tile.m_floor + tile.m_slope;

      ua_triangle3d_textured floor_tri1, floor_tri2;
      ua_triangle3d_textured ceil_tri1, ceil_tri2;
      bool tri2_used = true;

      // set texnums
      ceil_tri1.texnum = ceil_tri2.texnum = tile.m_textureCeiling;
      floor_tri1.texnum = floor_tri2.texnum = tile.m_textureFloor;

      // common ceiling quad
      ceil_tri1.set(0, x, y, ceil_height, 0.0, 0.0);
      ceil_tri1.set(1, x + 1, y + 1, ceil_height, 1.0, 1.0);
      ceil_tri1.set(2, x + 1, y, ceil_height, 1.0, 0.0);
      ceil_tri2.set(0, x, y, ceil_height, 0.0, 0.0);
      ceil_tri2.set(1, x, y + 1, ceil_height, 0.0, 1.0);
      ceil_tri2.set(2, x + 1, y + 1, ceil_height, 1.0, 1.0);

      switch (tile.m_type)
      {
      case Underworld::tileOpen:
         floor_tri1.set(0, x, y, floor_height, 0.0, 0.0);
         floor_tri1.set(1, x + 1, y, floor_height, 1.0, 0.0);
         floor_tri1.set(2, x + 1, y + 1, floor_height, 1.0, 1.0);
         floor_tri2.set(0, x, y, floor_height, 0.0, 0.0);
         floor_tri2.set(1, x + 1, y + 1, floor_height, 1.0, 1.0);
         floor_tri2.set(2, x, y + 1, floor_height, 0.0, 1.0);
         break;

      case Underworld::tileDiagonal_se:
         floor_tri1.set(0, x, y, floor_height, 0.0, 0.0);
         floor_tri1.set(1, x + 1, y, floor_height, 1.0, 0.0);
         floor_tri1.set(2, x + 1, y + 1, floor_height, 1.0, 1.0);
         tri2_used = false;
         break;

      case Underworld::tileDiagonal_sw:
         floor_tri1.set(0, x, y, floor_height, 0.0, 0.0);
         floor_tri1.set(1, x + 1, y, floor_height, 1.0, 0.0);
         floor_tri1.set(2, x, y + 1, floor_height, 0.0, 1.0);

         ceil_tri1.set(0, x, y, ceil_height, 0.0, 0.0);
         ceil_tri1.set(1, x, y + 1, ceil_height, 0.0, 1.0);
         ceil_tri1.set(2, x + 1, y, ceil_height, 1.0, 0.0);
         tri2_used = false;
         break;

      case Underworld::tileDiagonal_nw:
         floor_tri1.set(0, x, y, floor_height, 0.0, 0.0);
         floor_tri1.set(1, x + 1, y + 1, floor_height, 1.0, 1.0);
         floor_tri1.set(2, x, y + 1, floor_height, 0.0, 1.0);
         ceil_tri1 = ceil_tri2;
         tri2_used = false;
         break;

      case Underworld::tileDiagonal_ne:
         floor_tri1.set(0, x, y + 1, floor_height, 0.0, 1.0);
         floor_tri1.set(1, x + 1, y, floor_height, 1.0, 0.0);
         floor_tri1.set(2, x + 1, y + 1, floor_height, 1.0, 1.0);

         ceil_tri1.set(0, x, y + 1, ceil_height, 0.0, 1.0);
         ceil_tri1.set(1, x + 1, y + 1, ceil_height, 1.0, 1.0);
         ceil_tri1.set(2, x + 1, y, ceil_height, 1.0, 0.0);

         tri2_used = false;
         break;
      case Underworld::tileSlope_n:

         floor_tri1.set(0, x, y, floor_height, 0.0, 0.0);
         floor_tri1.set(1, x + 1, y, floor_height, 1.0, 0.0);
         floor_tri1.set(2, x + 1, y + 1, floor_slope_height, 1.0, 1.0);
         floor_tri2.set(0, x, y, floor_height, 0.0, 0.0);
         floor_tri2.set(1, x + 1, y + 1, floor_slope_height, 1.0, 1.0);
         floor_tri2.set(2, x, y + 1, floor_slope_height, 0.0, 1.0);
         break;

      case Underworld::tileSlope_s:
         floor_tri1.set(0, x, y, floor_slope_height, 0.0, 0.0);
         floor_tri1.set(1, x + 1, y, floor_slope_height, 1.0, 0.0);
         floor_tri1.set(2, x + 1, y + 1, floor_height, 1.0, 1.0);
         floor_tri2.set(0, x, y, floor_slope_height, 0.0, 0.0);
         floor_tri2.set(1, x + 1, y + 1, floor_height, 1.0, 1.0);
         floor_tri2.set(2, x, y + 1, floor_height, 0.0, 1.0);
         break;

      case Underworld::tileSlope_e:
         floor_tri1.set(0, x, y, floor_height, 0.0, 0.0);
         floor_tri1.set(1, x + 1, y, floor_slope_height, 1.0, 0.0);
         floor_tri1.set(2, x + 1, y + 1, floor_slope_height, 1.0, 1.0);
         floor_tri2.set(0, x, y, floor_height, 0.0, 0.0);
         floor_tri2.set(1, x + 1, y + 1, floor_slope_height, 1.0, 1.0);
         floor_tri2.set(2, x, y + 1, floor_height, 0.0, 1.0);
         break;

      case Underworld::tileSlope_w:
         floor_tri1.set(0, x, y, floor_slope_height, 0.0, 0.0);
         floor_tri1.set(1, x + 1, y, floor_height, 1.0, 0.0);
         floor_tri1.set(2, x + 1, y + 1, floor_height, 1.0, 1.0);
         floor_tri2.set(0, x, y, floor_slope_height, 0.0, 0.0);
         floor_tri2.set(1, x + 1, y + 1, floor_height, 1.0, 1.0);
         floor_tri2.set(2, x, y + 1, floor_slope_height, 0.0, 1.0);
         break;

      default: break;
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

void ua_geometry_provider::add_wall(ua_triangle3d_textured& tri1, ua_triangle3d_textured& tri2,
   unsigned int side,
   double x1, double y1, double z1,
   double x2, double y2, double z2,
   double nz1, double nz2, double ceiling)
{
   // calculate texture coordinates
   double v1, v2, v3, v4;

   // ceiling is the "base" for textures; for wall textures a texture is
   // mapped to 32.0 height units
   v1 = (ceiling - z1) / 32.0;
   v2 = (ceiling - z2) / 32.0;
   v3 = (ceiling - nz2) / 32.0;
   v4 = (ceiling - nz1) / 32.0;

   // add triangles with proper winding
   if (side == ua_left || side == ua_front)
   {
      tri1.set(0, x1, y1, z1, 0.0, v1);
      tri1.set(1, x2, y2, z2, 1.0, v2);
      tri1.set(2, x2, y2, nz2, 1.0, v3);

      tri2.set(0, x1, y1, z1, 0.0, v1);
      tri2.set(1, x2, y2, nz2, 1.0, v3);
      tri2.set(2, x1, y1, nz1, 0.0, v4);
   }
   else
   {
      tri1.set(0, x1, y1, z1, 1.0, v1);
      tri1.set(1, x1, y1, nz1, 1.0, v4);
      tri1.set(2, x2, y2, nz2, 0.0, v3);

      tri2.set(0, x1, y1, z1, 1.0, v1);
      tri2.set(1, x2, y2, nz2, 0.0, v3);
      tri2.set(2, x2, y2, z2, 0.0, v2);
   }
}

void ua_geometry_provider::get_tile_coords(
   unsigned int side, Underworld::TilemapTileType type,
   unsigned int basex, unsigned int basey, Uint16 basez, Uint16 slope, Uint16 ceiling,
   Uint16& x1, Uint16& y1, Uint16& z1,
   Uint16& x2, Uint16& y2, Uint16& z2)
{
   // determine x and y coordinates
   switch (side)
   {
   case ua_left:
      x1 = basex; x2 = basex;
      y1 = basey; y2 = basey + 1;
      break;
   case ua_right:
      x1 = basex + 1; x2 = basex + 1;
      y1 = basey; y2 = basey + 1;
      break;
   case ua_front:
      x1 = basex; x2 = basex + 1;
      y1 = basey + 1; y2 = basey + 1;
      break;
   case ua_back:
      x1 = basex; x2 = basex + 1;
      y1 = basey; y2 = basey;
      break;
   }

   // determine z coordinates
   z1 = z2 = basez;
   switch (side)
   {
   case ua_left:
      if (type == Underworld::tileSlope_w || type == Underworld::tileSlope_s) z1 += slope;
      if (type == Underworld::tileSlope_w || type == Underworld::tileSlope_n) z2 += slope;
      // note: wall height set to ceiling
      // as this function is called for adjacent tile walls only
      if (type == Underworld::tileDiagonal_se || type == Underworld::tileDiagonal_ne)
         z1 = z2 = ceiling;
      break;

   case ua_right:
      if (type == Underworld::tileSlope_e || type == Underworld::tileSlope_s) z1 += slope;
      if (type == Underworld::tileSlope_e || type == Underworld::tileSlope_n) z2 += slope;
      if (type == Underworld::tileDiagonal_sw || type == Underworld::tileDiagonal_nw)
         z1 = z2 = ceiling;
      break;

   case ua_front:
      if (type == Underworld::tileSlope_n || type == Underworld::tileSlope_w) z1 += slope;
      if (type == Underworld::tileSlope_n || type == Underworld::tileSlope_e) z2 += slope;
      if (type == Underworld::tileDiagonal_se || type == Underworld::tileDiagonal_sw)
         z1 = z2 = ceiling;
      break;

   case ua_back:
      if (type == Underworld::tileSlope_s || type == Underworld::tileSlope_w) z1 += slope;
      if (type == Underworld::tileSlope_s || type == Underworld::tileSlope_e) z2 += slope;
      if (type == Underworld::tileDiagonal_nw || type == Underworld::tileDiagonal_ne)
         z1 = z2 = ceiling;
      break;
   }
}
