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
/*! \file quadtree.cpp

   \brief quadtree and view frustum implementation

   there also is a test main() function, just define TEST_QUADTREE and compile
   as a single project.

*/

// needed includes
#include "common.hpp"
#include "quadtree.hpp"
#include "uamath.hpp"


// ua_frustum2d methods

ua_frustum2d::ua_frustum2d(double xpos, double ypos,
   double angle, double fov, double farplane)
{
   points[0].set(xpos - 3.0*cos(ua_deg2rad(angle)),ypos - 3.0*sin(ua_deg2rad(angle)));

   // move camera pos a bit behind
   farplane += 3.0;
   fov *= 1.3;

   // view vector
   double vx = farplane*cos(ua_deg2rad(angle)) + xpos;
   double vy = farplane*sin(ua_deg2rad(angle)) + ypos;

   // vector to second point
   double len = farplane*tan(ua_deg2rad(fov/2.0));
   double wx = len*cos(ua_deg2rad(angle+90.0));
   double wy = len*sin(ua_deg2rad(angle+90.0));

   points[1].set(vx + wx, vy + wy);
   points[2].set(vx - wx, vy - wy);
}

/*! this function uses the barycentric coordinates of the view frustum
    triangle to determine if a point is in the frustum

    more info at:
    http://research.microsoft.com/~hollasch/cgindex/math/barycentric.html
*/
bool ua_frustum2d::is_in_frustum(double xpos,double ypos)
{
   double b0 =  (points[1].x - points[0].x) * (points[2].y - points[0].y) - (points[2].x - points[0].x) * (points[1].y - points[0].y);
   double b1 = ((points[1].x - xpos) * (points[2].y - ypos) - (points[2].x - xpos) * (points[1].y - ypos)) / b0;
   double b2 = ((points[2].x - xpos) * (points[0].y - ypos) - (points[0].x - xpos) * (points[2].y - ypos)) / b0;
   double b3 = ((points[0].x - xpos) * (points[1].y - ypos) - (points[1].x - xpos) * (points[0].y - ypos)) / b0;

   return (b1>0.0 && b2>0.0 && b3>0.0);
}


// ua_quad methods

#ifdef TEST_QUADTREE
static int used_iter;
#endif

void ua_quad::get_visible_tiles(ua_frustum2d& fr, std::vector<ua_quad_tile_coord>& tilelist)
{
#ifdef TEST_QUADTREE
   used_iter++;
#endif
   bool res, all=true, one=false;

   // test quad corner coordinates
   res = fr.is_in_frustum(double(xmin),double(ymin)); all &= res; one |= res;
   res = fr.is_in_frustum(double(xmax),double(ymin)); all &= res; one |= res;
   res = fr.is_in_frustum(double(xmax),double(ymax)); all &= res; one |= res;
   res = fr.is_in_frustum(double(xmin),double(ymax)); all &= res; one |= res;

   if (all)
   {
      // all quad points are in the view frustum

      // add all tiles in the quad
      for(unsigned int x=xmin; x<xmax; x++)
         for(unsigned int y=ymin; y<ymax; y++)
            tilelist.push_back(
               std::make_pair<unsigned int,unsigned int>(x,y) );

      return;
   }

   if (!one)
   {
      // no quad coordinates were in the view frustum

      // check if frustum triangle does intersect otherwise
      if (!does_intersect(fr))
         return;
   }

   // quad is partly visible

   // check if quad is one tile wide
   if (!one && xmax-xmin <= 4)
   {
      tilelist.push_back(
         std::make_pair<unsigned int,unsigned int>(xmin,ymin) );
      return;
   }

   // divide the quad further
   unsigned int xnew = (xmax-xmin)>>1;
   unsigned int ynew = (ymax-ymin)>>1;

   static const unsigned int quad_coords[4][4] =
   {                  // +--+--+
      { 0, 1, 0, 1 }, // | 2| 3| y
      { 1, 2, 0, 1 }, // +--+--+ ^
      { 0, 1, 1, 2 }, // | 0| 1| |
      { 1, 2, 1, 2 }, // +--+--+-+-> x
   };

   // process all 4 sub-quads
   for(unsigned int i=0; i<4; i++)
   {
      ua_quad q(
         unsigned(xmin+xnew*quad_coords[i][0]),
         unsigned(xmin+xnew*quad_coords[i][1]),
         unsigned(ymin+ynew*quad_coords[i][2]),
         unsigned(ymin+ynew*quad_coords[i][3]));

      q.get_visible_tiles(fr,tilelist);
   }
}

/*! tests if the given frustum triangle intersects with the quad in any way.
    first, it is tested if at least one point of the triangle is in the quad.
    second test is to test quad lines against triangle lines for intersection.

    also look at:
    http://www.flipcode.com/cgi-bin/knowledge.cgi?showunit=0
    http://www.flipcode.com/portal/issue07.shtml
*/
bool ua_quad::does_intersect(ua_frustum2d& fr)
{
   // check how much triangle points are in the quad
   int i,count=0;
   for(i=0; i<3; i++)
   {
      ua_vector2d& point = fr.get_point(i);

      if (point.x >= xmin && point.x <= xmax &&
          point.y >= ymin && point.y <= ymax)
            count++;
   }

   // at least one? then we do intersect
   if (count>0)
      return true;

   // check if triangle and quad lines intersect

   for(i=0; i<3; i++)
   {
      int j = (i+1)%3;

      ua_vector2d& pt1 = fr.get_point(i);
      ua_vector2d& pt2 = fr.get_point(j);

      for(int k=0; k<4; k++)
      {
         double dist1=0, dist2=0;

         switch(k)
         {
         case 0: dist1 = pt1.y-ymin; dist2 = pt2.y-ymin; break;
         case 1: dist1 = pt1.y-ymax; dist2 = pt2.y-ymax; break;
         case 2: dist1 = pt1.x-xmin; dist2 = pt2.x-xmin; break;
         case 3: dist1 = pt1.x-xmax; dist2 = pt2.x-xmax; break;
         }

         // equal sign?
         if (dist1*dist2 >= 0.0)
            continue; // lines don't cross

         double portion = dist2/(dist2-dist1);

         // calculate cross point
         double cx = pt2.x - portion * (pt2.x-pt1.x);
         double cy = pt2.y - portion * (pt2.y-pt1.y);

         bool cross = false;

         // check bounds of quad line
         switch(k)
         {
         case 0:
         case 1:
            if (cx >= xmin && cx <= xmax) cross=true;
            break;

         case 2:
         case 3:
            if (cy >= ymin && cy <= ymax) cross=true;
            break;
         }

         if (cross)
            return true;
      }
   }

   return false;
}


// quadtree test case

#ifdef TEST_QUADTREE

#include "SDL.h"

#ifdef _MSC_VER
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#endif


void runtime_test()
{
   const int loops = 5000;

   std::vector<ua_quad_tile_coord> tilelist;

//   ua_frustum2d fr(5.0, 5.0, 30.0, 90.0, 48.0);
   ua_frustum2d fr(33.0, 35.0, 290.0, 60.0, 32.0);
   ua_quad q(0,64,0,64);

   Uint32 now = SDL_GetTicks();

   for(int i=0; i<loops; i++)
   {
      tilelist.clear();
      q.get_visible_tiles(fr,tilelist);
   }

   Uint32 then = SDL_GetTicks();

   printf("needed: %2.2f ms for one call.\n",double(then-now)/double(loops));
}


#undef SDL_main

int main(int argc, char* argv[])
{
   double xpos = 33.0;
   double ypos = 33.0;
   //double angle = 20.0;
   double angle = 45.0;

   std::vector<ua_quad_tile_coord> tilelist;

   //runtime_test();

   ua_quad q(0,64,0,64);

   SDL_Init(SDL_INIT_VIDEO);
   SDL_Surface *screen = SDL_SetVideoMode(64, 64, 16 , 0);

   SDL_Event event;
   while(SDL_WaitEvent(&event) != 0)
   {
      switch(event.type)
      {
      case SDL_MOUSEBUTTONDOWN:
         angle+=15;

      case SDL_MOUSEMOTION:
         {
            SDL_LockSurface(screen);

            int x,y;
            SDL_GetMouseState(&x,&y);

            unsigned short *pix = (unsigned short *)screen->pixels;
            memset(pix,0,64*64*2);

            tilelist.clear();
            ua_frustum2d fr(x, 64-y, angle, 60.0, 16.0);
            //ua_frustum2d fr(32, 32, 45.0, 69.2307692, 32.0);

            used_iter = 0;
            q.get_visible_tiles(fr,tilelist);

            printf("used iter: %u\tquads: %u\n",used_iter,tilelist.size());

            int max = tilelist.size();
            for(int j=0; j<max; j++)
            {
               const ua_quad_tile_coord &co = tilelist[j];
               pix[(64-co.second)*64 + co.first] = 0xffff;
            }

            SDL_UnlockSurface(screen);
            SDL_UpdateRect(screen , 0 , 0 , 0 , 0 );
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
