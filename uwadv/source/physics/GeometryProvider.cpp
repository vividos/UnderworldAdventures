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
/// \file GeometryProvider.cpp
/// \brief level geometry provider implementation
//
#include "pch.hpp"
#include "GeometryProvider.hpp"

/// side of currently rendered wall; used internally
enum WallType
{
   left, right, front, back
};

/// Returns all triangles generated for tile with given coordinates.
/// \param xpos x tile position
/// \param ypos y tile position
/// \param allTriangles vector of triangles the new triangles are added to
void GeometryProvider::GetTileTriangles(unsigned int xpos,
   unsigned int ypos, std::vector<Triangle3dTextured>& allTriangles)
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
         Triangle3dTextured diag_tri1{ walltex }, diag_tri2{ walltex };

         switch (tile.m_type)
         {
         case Underworld::tileDiagonal_se:
            AddWall(diag_tri1, diag_tri2,
               left, x, y, floor_height, x + 1, y + 1, floor_height,
               ceil_height, ceil_height, ceil_height);
            break;

         case Underworld::tileDiagonal_sw:
            AddWall(diag_tri1, diag_tri2,
               left, x, y + 1, floor_height, x + 1, y, floor_height,
               ceil_height, ceil_height, ceil_height);
            break;

         case Underworld::tileDiagonal_nw:
            AddWall(diag_tri1, diag_tri2,
               left, x + 1, y + 1, floor_height, x, y, floor_height,
               ceil_height, ceil_height, ceil_height);
            break;

         case Underworld::tileDiagonal_ne:
            AddWall(diag_tri1, diag_tri2,
               left, x + 1, y, floor_height, x, y + 1, floor_height,
               ceil_height, ceil_height, ceil_height);
            break;

         default:
            diag_used = false;
         }

         if (diag_used)
         {
            allTriangles.push_back(diag_tri1);
            allTriangles.push_back(diag_tri2);
         }
      }

      // draw every wall side
      for (unsigned int side = left; side <= back; side++)
      {
         // ignore some walls for diagonal wall tiles
         if ((tile.m_type == Underworld::tileDiagonal_se && (side == left || side == front)) ||
            (tile.m_type == Underworld::tileDiagonal_sw && (side == right || side == front)) ||
            (tile.m_type == Underworld::tileDiagonal_nw && (side == right || side == back)) ||
            (tile.m_type == Underworld::tileDiagonal_ne && (side == left || side == back)))
            continue;

         Uint16 x1, y1, z1, x2, y2, z2;

         // get current tile coordinates
         GetTileCoords(side, tile.m_type,
            Uint16(x), Uint16(y), Uint16(tile.m_floor), Uint16(tile.m_slope), Uint16(tile.m_ceiling),
            x1, y1, z1, x2, y2, z2);

         // get adjacent tile coordinates
         Uint16 nx = 0, ny = 0, nz1, nz2;
         switch (side)
         {
         case left:  nx = Uint16(x) - 1; ny = Uint16(y); break;
         case right: nx = Uint16(x) + 1; ny = Uint16(y); break;
         case front: ny = Uint16(y) + 1; nx = Uint16(x); break;
         case back:  ny = Uint16(y) - 1; nx = Uint16(x); break;
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
               case left: adjside = right; break;
               case right: adjside = left; break;
               case front: adjside = back; break;
               default: adjside = front; break;
               }

               Uint16 dummy = 0;
               GetTileCoords(adjside, ntile.m_type, nx, ny,
                  ntile.m_floor, ntile.m_slope, ntile.m_ceiling,
                  dummy, dummy, nz1, dummy, dummy, nz2);

               // if the wall to the adjacent tile goes up (e.g. a stair),
               // we draw that wall. if it goes down, the adjacent tile has to
               // draw that wall. so we only draw walls that go up to another
               // tile or the ceiling.

               if (nz1 == nz2 && nz2 == ntile.m_ceiling)
               {
                  // GetTileCoords() returns this, when the adjacent wall is a
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

         Triangle3dTextured tri1{ walltex }, tri2{ walltex };

         // now that we have all info, draw the tile wall
         AddWall(tri1, tri2, side,
            x1, y1, z1,
            x2, y2, z2,
            nz1, nz2, ceil_height);

         allTriangles.push_back(tri1);
         allTriangles.push_back(tri2);

      } // end for
   }

   // tile floor / ceiling
   {
      double floor_slope_height = tile.m_floor + tile.m_slope;

      Triangle3dTextured floor_tri1{ tile.m_textureFloor }, floor_tri2{ tile.m_textureFloor };
      Triangle3dTextured ceil_tri1{ tile.m_textureCeiling }, ceil_tri2{ tile.m_textureCeiling };
      bool tri2_used = true;

      // common ceiling quad
      ceil_tri1.Set(0, x, y, ceil_height, 0.0, 0.0);
      ceil_tri1.Set(1, x + 1, y + 1, ceil_height, 1.0, 1.0);
      ceil_tri1.Set(2, x + 1, y, ceil_height, 1.0, 0.0);
      ceil_tri2.Set(0, x, y, ceil_height, 0.0, 0.0);
      ceil_tri2.Set(1, x, y + 1, ceil_height, 0.0, 1.0);
      ceil_tri2.Set(2, x + 1, y + 1, ceil_height, 1.0, 1.0);

      switch (tile.m_type)
      {
      case Underworld::tileOpen:
         floor_tri1.Set(0, x, y, floor_height, 0.0, 0.0);
         floor_tri1.Set(1, x + 1, y, floor_height, 1.0, 0.0);
         floor_tri1.Set(2, x + 1, y + 1, floor_height, 1.0, 1.0);
         floor_tri2.Set(0, x, y, floor_height, 0.0, 0.0);
         floor_tri2.Set(1, x + 1, y + 1, floor_height, 1.0, 1.0);
         floor_tri2.Set(2, x, y + 1, floor_height, 0.0, 1.0);
         break;

      case Underworld::tileDiagonal_se:
         floor_tri1.Set(0, x, y, floor_height, 0.0, 0.0);
         floor_tri1.Set(1, x + 1, y, floor_height, 1.0, 0.0);
         floor_tri1.Set(2, x + 1, y + 1, floor_height, 1.0, 1.0);
         tri2_used = false;
         break;

      case Underworld::tileDiagonal_sw:
         floor_tri1.Set(0, x, y, floor_height, 0.0, 0.0);
         floor_tri1.Set(1, x + 1, y, floor_height, 1.0, 0.0);
         floor_tri1.Set(2, x, y + 1, floor_height, 0.0, 1.0);

         ceil_tri1.Set(0, x, y, ceil_height, 0.0, 0.0);
         ceil_tri1.Set(1, x, y + 1, ceil_height, 0.0, 1.0);
         ceil_tri1.Set(2, x + 1, y, ceil_height, 1.0, 0.0);
         tri2_used = false;
         break;

      case Underworld::tileDiagonal_nw:
         floor_tri1.Set(0, x, y, floor_height, 0.0, 0.0);
         floor_tri1.Set(1, x + 1, y + 1, floor_height, 1.0, 1.0);
         floor_tri1.Set(2, x, y + 1, floor_height, 0.0, 1.0);
         ceil_tri1 = ceil_tri2;
         tri2_used = false;
         break;

      case Underworld::tileDiagonal_ne:
         floor_tri1.Set(0, x, y + 1, floor_height, 0.0, 1.0);
         floor_tri1.Set(1, x + 1, y, floor_height, 1.0, 0.0);
         floor_tri1.Set(2, x + 1, y + 1, floor_height, 1.0, 1.0);

         ceil_tri1.Set(0, x, y + 1, ceil_height, 0.0, 1.0);
         ceil_tri1.Set(1, x + 1, y + 1, ceil_height, 1.0, 1.0);
         ceil_tri1.Set(2, x + 1, y, ceil_height, 1.0, 0.0);

         tri2_used = false;
         break;
      case Underworld::tileSlope_n:

         floor_tri1.Set(0, x, y, floor_height, 0.0, 0.0);
         floor_tri1.Set(1, x + 1, y, floor_height, 1.0, 0.0);
         floor_tri1.Set(2, x + 1, y + 1, floor_slope_height, 1.0, 1.0);
         floor_tri2.Set(0, x, y, floor_height, 0.0, 0.0);
         floor_tri2.Set(1, x + 1, y + 1, floor_slope_height, 1.0, 1.0);
         floor_tri2.Set(2, x, y + 1, floor_slope_height, 0.0, 1.0);
         break;

      case Underworld::tileSlope_s:
         floor_tri1.Set(0, x, y, floor_slope_height, 0.0, 0.0);
         floor_tri1.Set(1, x + 1, y, floor_slope_height, 1.0, 0.0);
         floor_tri1.Set(2, x + 1, y + 1, floor_height, 1.0, 1.0);
         floor_tri2.Set(0, x, y, floor_slope_height, 0.0, 0.0);
         floor_tri2.Set(1, x + 1, y + 1, floor_height, 1.0, 1.0);
         floor_tri2.Set(2, x, y + 1, floor_height, 0.0, 1.0);
         break;

      case Underworld::tileSlope_e:
         floor_tri1.Set(0, x, y, floor_height, 0.0, 0.0);
         floor_tri1.Set(1, x + 1, y, floor_slope_height, 1.0, 0.0);
         floor_tri1.Set(2, x + 1, y + 1, floor_slope_height, 1.0, 1.0);
         floor_tri2.Set(0, x, y, floor_height, 0.0, 0.0);
         floor_tri2.Set(1, x + 1, y + 1, floor_slope_height, 1.0, 1.0);
         floor_tri2.Set(2, x, y + 1, floor_height, 0.0, 1.0);
         break;

      case Underworld::tileSlope_w:
         floor_tri1.Set(0, x, y, floor_slope_height, 0.0, 0.0);
         floor_tri1.Set(1, x + 1, y, floor_height, 1.0, 0.0);
         floor_tri1.Set(2, x + 1, y + 1, floor_height, 1.0, 1.0);
         floor_tri2.Set(0, x, y, floor_slope_height, 0.0, 0.0);
         floor_tri2.Set(1, x + 1, y + 1, floor_height, 1.0, 1.0);
         floor_tri2.Set(2, x, y + 1, floor_slope_height, 0.0, 1.0);
         break;

      default: break;
      } // end switch

      // insert triangles into list
      allTriangles.push_back(floor_tri1);
      if (tri2_used)
         allTriangles.push_back(floor_tri2);

      allTriangles.push_back(ceil_tri1);
      if (tri2_used)
         allTriangles.push_back(ceil_tri2);
   }
}

void GeometryProvider::AddWall(Triangle3dTextured& tri1, Triangle3dTextured& tri2,
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
   if (side == left || side == front)
   {
      tri1.Set(0, x1, y1, z1, 0.0, v1);
      tri1.Set(1, x2, y2, z2, 1.0, v2);
      tri1.Set(2, x2, y2, nz2, 1.0, v3);

      tri2.Set(0, x1, y1, z1, 0.0, v1);
      tri2.Set(1, x2, y2, nz2, 1.0, v3);
      tri2.Set(2, x1, y1, nz1, 0.0, v4);
   }
   else
   {
      tri1.Set(0, x1, y1, z1, 1.0, v1);
      tri1.Set(1, x1, y1, nz1, 1.0, v4);
      tri1.Set(2, x2, y2, nz2, 0.0, v3);

      tri2.Set(0, x1, y1, z1, 1.0, v1);
      tri2.Set(1, x2, y2, nz2, 0.0, v3);
      tri2.Set(2, x2, y2, z2, 0.0, v2);
   }
}

void GeometryProvider::GetTileCoords(
   unsigned int side, Underworld::TilemapTileType type,
   unsigned int basex, unsigned int basey, Uint16 basez, Uint16 slope, Uint16 ceiling,
   Uint16& x1, Uint16& y1, Uint16& z1,
   Uint16& x2, Uint16& y2, Uint16& z2)
{
   // determine x and y coordinates
   switch (side)
   {
   case left:
      x1 = basex; x2 = basex;
      y1 = basey; y2 = basey + 1;
      break;
   case right:
      x1 = basex + 1; x2 = basex + 1;
      y1 = basey; y2 = basey + 1;
      break;
   case front:
      x1 = basex; x2 = basex + 1;
      y1 = basey + 1; y2 = basey + 1;
      break;
   case back:
      x1 = basex; x2 = basex + 1;
      y1 = basey; y2 = basey;
      break;
   }

   // determine z coordinates
   z1 = z2 = basez;
   switch (side)
   {
   case left:
      if (type == Underworld::tileSlope_w || type == Underworld::tileSlope_s) z1 += slope;
      if (type == Underworld::tileSlope_w || type == Underworld::tileSlope_n) z2 += slope;
      // note: wall height set to ceiling
      // as this function is called for adjacent tile walls only
      if (type == Underworld::tileDiagonal_se || type == Underworld::tileDiagonal_ne)
         z1 = z2 = ceiling;
      break;

   case right:
      if (type == Underworld::tileSlope_e || type == Underworld::tileSlope_s) z1 += slope;
      if (type == Underworld::tileSlope_e || type == Underworld::tileSlope_n) z2 += slope;
      if (type == Underworld::tileDiagonal_sw || type == Underworld::tileDiagonal_nw)
         z1 = z2 = ceiling;
      break;

   case front:
      if (type == Underworld::tileSlope_n || type == Underworld::tileSlope_w) z1 += slope;
      if (type == Underworld::tileSlope_n || type == Underworld::tileSlope_e) z2 += slope;
      if (type == Underworld::tileDiagonal_se || type == Underworld::tileDiagonal_sw)
         z1 = z2 = ceiling;
      break;

   case back:
      if (type == Underworld::tileSlope_s || type == Underworld::tileSlope_w) z1 += slope;
      if (type == Underworld::tileSlope_s || type == Underworld::tileSlope_e) z2 += slope;
      if (type == Underworld::tileDiagonal_nw || type == Underworld::tileDiagonal_ne)
         z1 = z2 = ceiling;
      break;
   }
}
