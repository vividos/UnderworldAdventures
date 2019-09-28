//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2019 Underworld Adventures Team
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
/// \file Quadtree.cpp
/// \brief quadtree and view frustum implementation
//
// There is a test main() function, just define TEST_QUADTREE and compile
// as a single project.
//
#include "pch.hpp"
#include "Quadtree.hpp"
#include "Math.hpp"

Frustum2d::Frustum2d(double xpos, double ypos,
   double angle, double fov, double farplane)
{
   m_points[0].Set(xpos - 3.0*cos(Deg2rad(angle)), ypos - 3.0*sin(Deg2rad(angle)));

   // move camera pos a bit behind
   farplane += 3.0;
   fov *= 1.3;

   // view vector
   double vx = farplane * cos(Deg2rad(angle)) + xpos;
   double vy = farplane * sin(Deg2rad(angle)) + ypos;

   // vector to second point
   double len = farplane * tan(Deg2rad(fov / 2.0));
   double wx = len * cos(Deg2rad(angle + 90.0));
   double wy = len * sin(Deg2rad(angle + 90.0));

   m_points[1].Set(vx + wx, vy + wy);
   m_points[2].Set(vx - wx, vy - wy);
}

/// Determines if the given point is inside the view frustum.
/// This function uses the barycentric coordinates of the view frustum
/// triangle to determine if a point is in the frustum. More info at:
/// http://research.microsoft.com/~hollasch/cgindex/math/barycentric.html
/// \param xpos x position
/// \param ypos y position
bool Frustum2d::IsInFrustum(double xpos, double ypos) const
{
   double b0 = (m_points[1].x - m_points[0].x) * (m_points[2].y - m_points[0].y) - (m_points[2].x - m_points[0].x) * (m_points[1].y - m_points[0].y);
   double b1 = ((m_points[1].x - xpos) * (m_points[2].y - ypos) - (m_points[2].x - xpos) * (m_points[1].y - ypos)) / b0;
   double b2 = ((m_points[2].x - xpos) * (m_points[0].y - ypos) - (m_points[0].x - xpos) * (m_points[2].y - ypos)) / b0;
   double b3 = ((m_points[0].x - xpos) * (m_points[1].y - ypos) - (m_points[1].x - xpos) * (m_points[0].y - ypos)) / b0;

   return (b1 > 0.0 && b2 > 0.0 && b3 > 0.0);
}

Quad::Quad(unsigned int xmin, unsigned int xmax,
   unsigned int ymin, unsigned int ymax)
   :m_xmin(xmin), m_xmax(xmax), m_ymin(ymin), m_ymax(ymax)
{
}

#ifdef TEST_QUADTREE
static int used_iter;
static unsigned int quad_type[64 * 64];
#endif

/// Finds visible tiles in given view frustum by determining if the quad is
/// completely contained in the quad and subdividing the quad in 4 subquads
/// if it's not contained. This function calls itself recursively, but not
/// more than log(max(xres,yres))/log(2).
/// \param frustum view frustum
/// \param callback callback interface to notify caller of visible tiles
void Quad::FindVisibleTiles(const Frustum2d& frustum,
   T_onVisibileTileFunc callback)
{
#ifdef TEST_QUADTREE
   used_iter++;
#endif
   bool res, all = true, one = false;

   // test quad corner coordinates
   res = frustum.IsInFrustum(double(m_xmin), double(m_ymin)); all &= res; one |= res;
   res = frustum.IsInFrustum(double(m_xmax), double(m_ymin)); all &= res; one |= res;
   res = frustum.IsInFrustum(double(m_xmax), double(m_ymax)); all &= res; one |= res;
   res = frustum.IsInFrustum(double(m_xmin), double(m_ymax)); all &= res; one |= res;

   if (all)
   {
      // all quad points are in the view frustum, and thus visible
      for (unsigned int x = m_xmin; x < m_xmax; x++)
         for (unsigned int y = m_ymin; y < m_ymax; y++)
         {
            callback(x, y);
#ifdef TEST_QUADTREE
            quad_type[x * 64 + y] = 1;
#endif
         }

      return;
   }

   // check quad size
   if (m_xmax - m_xmin >= 2)
   {
      // no quad coordinates were in the view frustum
      // check if frustum triangle does intersect otherwise
      if (!one && !CheckIntersection(frustum))
         return; // no, so don't subdivide further
   }
   else
   {
      // quad is single tile
      if (one)
      {
         // and is partly visible
         callback(m_xmin, m_ymin);
#ifdef TEST_QUADTREE
         quad_type[xmin * 64 + ymin] = 2;
#endif
      }
      return;
   }

   // divide the quad further
   unsigned int xnew = (m_xmax - m_xmin) >> 1;
   unsigned int ynew = (m_ymax - m_ymin) >> 1;

   static const unsigned int quad_coords[4][4] =
   {                  // +--+--+
      { 0, 1, 0, 1 }, // | 2| 3| y
      { 1, 2, 0, 1 }, // +--+--+ ^
      { 0, 1, 1, 2 }, // | 0| 1| |
      { 1, 2, 1, 2 }, // +--+--+-+-> x
   };

   // process all 4 sub-quads
   for (unsigned int i = 0; i < 4; i++)
   {
      Quad q(
         unsigned(m_xmin + xnew * quad_coords[i][0]),
         unsigned(m_xmin + xnew * quad_coords[i][1]),
         unsigned(m_ymin + ynew * quad_coords[i][2]),
         unsigned(m_ymin + ynew * quad_coords[i][3]));

      q.FindVisibleTiles(frustum, callback);
   }
}

/// Tests if the given frustum triangle intersects with the quad in any way.
/// first, it is tested if at least one point of the triangle is in the quad.
/// second test is to test quad lines against triangle lines for intersection.
/// also look at:
/// http://www.flipcode.com/cgi-bin/knowledge.cgi?showunit=0
/// http://www.flipcode.com/portal/issue07.shtml
/// \param frustum view frustum
bool Quad::CheckIntersection(const Frustum2d& frustum) const
{
   // check how much triangle points are in the quad
   unsigned int i, count = 0;
   for (i = 0; i < 3; i++)
   {
      const Vector2d& point = frustum.GetPoint(i);

      if (point.x >= m_xmin && point.x <= m_xmax &&
         point.y >= m_ymin && point.y <= m_ymax)
         count++;
   }

   // at least one? then we do intersect
   if (count > 0)
      return true;

   // check if triangle and quad lines intersect
   for (i = 0; i < 3; i++)
   {
      unsigned int j = (i + 1) % 3;

      const Vector2d& pt1 = frustum.GetPoint(i);
      const Vector2d& pt2 = frustum.GetPoint(j);

      for (unsigned int k = 0; k < 4; k++)
      {
         double dist1 = 0, dist2 = 0;

         switch (k)
         {
         case 0: dist1 = pt1.y - m_ymin; dist2 = pt2.y - m_ymin; break;
         case 1: dist1 = pt1.y - m_ymax; dist2 = pt2.y - m_ymax; break;
         case 2: dist1 = pt1.x - m_xmin; dist2 = pt2.x - m_xmin; break;
         case 3: dist1 = pt1.x - m_xmax; dist2 = pt2.x - m_xmax; break;
         }

         // equal sign?
         if (dist1*dist2 >= 0.0)
            continue; // lines don't cross

         double portion = dist2 / (dist2 - dist1);

         // calculate cross point
         double cx = pt2.x - portion * (pt2.x - pt1.x);
         double cy = pt2.y - portion * (pt2.y - pt1.y);

         bool cross = false;

         // check bounds of quad line
         switch (k)
         {
         case 0:
         case 1:
            if (cx >= m_xmin && cx <= m_xmax) cross = true;
            break;

         case 2:
         case 3:
            if (cy >= m_ymin && cy <= m_ymax) cross = true;
            break;
         }

         if (cross)
            return true;
      }
   }

   return false;
}

/// Collects visible tiles in the tilelist vector.
void QuadtreeTileCollector::OnVisibleTile(unsigned int xpos, unsigned int ypos)
{
   m_tileList.push_back(std::make_pair(xpos, ypos));
}

#ifdef TEST_QUADTREE

#include <SDL.h>

#ifdef _MSC_VER
#pragma comment(lib, "SDL2main.lib")
#endif


void runtime_test()
{
   const int loops = 5000;

   QuadtreeTileCollector coll;

   //   std::vector<QuadTileCoordinates> tilelist;

   //   Frustum2d frustum(5.0, 5.0, 30.0, 90.0, 48.0);
   Frustum2d frustum(33.0, 35.0, 290.0, 60.0, 32.0);
   Quad q(0, 64, 0, 64);

   Uint32 now = SDL_GetTicks();

   for (int i = 0; i < loops; i++)
   {
      coll.GetTileList().clear();
      q.FindVisibleTiles(frustum, coll);
   }

   Uint32 then = SDL_GetTicks();

   printf("needed: %2.2f ms for one call.\n", double(then - now) / double(loops));
}


#undef main

int main(int argc, char* argv[])
{
   double xpos = 33.0;
   double ypos = 33.0;
   //double angle = 20.0;
   double angle = 360.0 - 45.0;

   //   std::vector<QuadTileCoordinates> tilelist;
   QuadtreeTileCollector coll;

   //runtime_test();

   Quad q(0, 64, 0, 64);

   const unsigned int res = 128;

   SDL_Init(SDL_INIT_VIDEO);
   SDL_Surface *screen = SDL_SetVideoMode(res, res, 16, 0);

   SDL_ShowCursor(0);

   SDL_Event event;
   while (SDL_WaitEvent(&event) != 0)
   {
      switch (event.type)
      {
      case SDL_MOUSEBUTTONDOWN:
         angle += 15;

      case SDL_MOUSEMOTION:
      {
         SDL_LockSurface(screen);

         int x, y;
         SDL_GetMouseState(&x, &y);

         unsigned short *pix = (unsigned short *)screen->pixels;
         memset(pix, 0, res*res * 2);

         memset(quad_type, 0, sizeof(unsigned int) * 64 * 64);

         coll.GetTileList().clear();
         Frustum2d frustum(x / 2.0, 64.0 - y / 2.0, angle, 60.0, 16.0);
         //Frustum2d frustum(32, 32, 45.0, 69.2307692, 32.0);

         Uint32 now = SDL_GetTicks();

         used_iter = 0;
         q.FindVisibleTiles(frustum, coll);

         Uint32 then = SDL_GetTicks();

         std::vector<QuadTileCoordinates>& tilelist = coll.GetTileList();

         printf("pos:%2.1f/%2.1f\tused iter: %u\tquads: %u\tneeded %u ms\n",
            x / 2.0, 64.0 - y / 2.0,
            used_iter, tilelist.size(),
            then - now);

         int max = tilelist.size();
         for (int j = 0; j < max; j++)
         {
            const QuadTileCoordinates &co = tilelist[j];
            for (unsigned int a = 0; a < 2; a++)
               for (unsigned int b = 0; b < 2; b++)
                  pix[(res - co.second*(res / 64) - b)*res + co.first*(res / 64) + a] =
                  quad_type[co.first * 64 + co.second] == 1 ? 0xffff : 0xf00f;
         }

         for (unsigned int a = 0; a < 2; a++)
            for (unsigned int b = 0; b < 2; b++)
               pix[(unsigned(y) + b)*res + unsigned(x) + a] = 0;

         SDL_UnlockSurface(screen);
         SDL_UpdateRect(screen, 0, 0, 0, 0);
      }
      break;

      case SDL_QUIT:
         break;
      }

      if (event.type == SDL_QUIT)
         break;
   }

   SDL_Quit();

   return 0;
}

#endif
