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
/*! \file quadtree.cpp

   quadtree and view frustum implementation

   there also is a test main() function, just define TEST_QUADTREE and compile
   as a single project.

*/

// needed includes
#include "common.hpp"
#include "quadtree.hpp"
#include <cmath>


// ua_frustum methods

ua_frustum::ua_frustum(double xpos,double ypos,double zpos,double myxangle,double myyangle,double fov,double farplane)
{
   xangle=myxangle;
   yangle=myyangle;
   pos[0]=xpos;
   pos[1]=ypos;
   pos[2]=zpos;

   // calculate all three triangle points forming the 2d frustum top view
   x[0] = xpos - 3.0*cos(ua_deg2rad(xangle));
   y[0] = ypos - 3.0*sin(ua_deg2rad(xangle));

   // move camera pos a bit behind
   farplane += 3.0;
   fov *= 1.3;

   // view vector
   double vx = farplane*cos(ua_deg2rad(xangle)) + xpos;
   double vy = farplane*sin(ua_deg2rad(xangle)) + ypos;

   // vector to second point
   double len = farplane*tan(ua_deg2rad(fov/2));
   double wx = len*cos(ua_deg2rad(xangle+90));
   double wy = len*sin(ua_deg2rad(xangle+90));

   x[1] = vx + wx;
   y[1] = vy + wy;

   x[2] = vx - wx;
   y[2] = vy - wy;
}

/*! this function uses the barycentric coordinates of the view frustum
    triangle to determine if a point is in the frustum

    more info at:
    http://research.microsoft.com/~hollasch/cgindex/math/barycentric.html
*/
bool ua_frustum::is_in_frustum(double xpos,double ypos)
{
   double b0 =  (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0]);
   double b1 = ((x[1] - xpos) * (y[2] - ypos) - (x[2] - xpos) * (y[1] - ypos)) / b0;
   double b2 = ((x[2] - xpos) * (y[0] - ypos) - (x[0] - xpos) * (y[2] - ypos)) / b0;
   double b3 = ((x[0] - xpos) * (y[1] - ypos) - (x[1] - xpos) * (y[0] - ypos)) / b0;

   return (b1>0.0 && b2>0.0 && b3>0.0);
}

#ifdef TEST_QUADTREE
static int used_iter;
#endif

void ua_quad::get_visible_tiles(ua_frustum &fr, std::vector<ua_quad_tile_coord> &tilelist)
{
#ifdef TEST_QUADTREE
   used_iter++;
#endif
   bool res,all=true,one=false;

   // test quads coordinates
   res = fr.is_in_frustum(x0,y0); all &= res; one |= res;
   res = fr.is_in_frustum(x1,y0); all &= res; one |= res;
   res = fr.is_in_frustum(x1,y1); all &= res; one |= res;
   res = fr.is_in_frustum(x0,y1); all &= res; one |= res;

   if (all)
   {
      // all quad coordinates were in the view frustum

      unsigned int x,y,xmax,ymax;
      xmax = unsigned(x1); ymax = unsigned(y1);

      // add all tiles in the quad
      for(x=unsigned(x0);x<xmax; x++)
      for(y=unsigned(y0);y<ymax; y++)
      {
         tilelist.push_back(
            std::make_pair<unsigned int,unsigned int>(x,y) );
      }
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

   // divide the quad further
   double xhalf = (x1-x0)/2, yhalf = (y1-y0)/2;

   if (xhalf<1.0 || yhalf<1.0)
   {
      if (one)
      // don't divide that one further, it's a single tile
      tilelist.push_back(
            std::make_pair<unsigned int,unsigned int>(unsigned(x0),unsigned(y0)) );
      return;
   }

   static const double quad_coords[4][4] =
   {
      { 0, 0, 1, 1 },
      { 1, 0, 2, 1 },
      { 0, 1, 1, 2 },
      { 1, 1, 2, 2 },
   };

   // process all 4 sub-quads
   for(unsigned int i=0; i<4; i++)
   {
      ua_quad q(
         x0+xhalf*quad_coords[i][0],
         y0+yhalf*quad_coords[i][1],
         x0+xhalf*quad_coords[i][2],
         y0+yhalf*quad_coords[i][3]);

      q.get_visible_tiles(fr,tilelist);
   }
}

/*! tests if the given frustum triangle intersects with the quad in any way
    first, it is tested if at least one point of the triangle is in the quad.
    second test is to test quad lines against triangle lines for intersection.

    also look at:
    http://www.flipcode.com/cgi-bin/knowledge.cgi?showunit=0
    http://www.flipcode.com/portal/issue07.shtml
*/
bool ua_quad::does_intersect(ua_frustum &fr)
{
   // check how much triangle points are in the quad
   int i,count=0;
   for(i=0; i<3; i++)
   {
      if ( (fr.get_x(i)>=x0 && fr.get_x(i)<=x1) &&
           (fr.get_y(i)>=y0 && fr.get_y(i)<=y1) )
         count++;
   }

   // at least one? then we do intersect
   if (count>0)
      return true;

   // check if triangle and quad lines intersect

   for(i=0; i<3; i++)
   {
      int j = (i+1)%3;

      double xt1=fr.get_x(i);
      double yt1=fr.get_y(i);

      double xt2=fr.get_x(j);
      double yt2=fr.get_y(j);

      for(int k=0; k<4; k++)
      {
         double dist1, dist2;

         switch(k)
         {
         case 0: dist1 = yt1-y0; dist2 = yt2-y0; break;
         case 1: dist1 = yt1-y1; dist2 = yt2-y1; break;
         case 2: dist1 = xt1-x0; dist2 = xt2-x0; break;
         case 3: dist1 = xt1-x1; dist2 = xt2-x1; break;
         }

         // equal sign?
         if (dist1*dist2 >= 0.0)
            continue; // lines don't cross

         double portion = dist2/(dist2-dist1);

         // calculate cross point
         double cx = xt2 - portion * (xt2-xt1);
         double cy = yt2 - portion * (yt2-yt1);

         bool cross = false;

         // check bounds of quad line
         switch(k)
         {
         case 0:
         case 1:
            if (cx>=x0 && cx<=x1) cross=true;
            break;

         case 2:
         case 3:
            if (cy>=y0 && cy<=y1) cross=true;
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

#include <time.h>
#include <windows.h>

void runtime_test()
{
   const int loops = 5000;

   std::vector<ua_quad_tile_coord> tilelist;

//   ua_frustum fr(5.0, 5.0, 30.0, 90.0, 48.0);
   ua_frustum fr(33.0, 35.0, 290.0, 60.0, 32.0);
   ua_quad q(0.0,0.0,64.0,64.0);

   unsigned int now = ::GetTickCount();

   for(int i=0; i<loops; i++)
   {
      tilelist.clear();
      q.get_visible_tiles(fr,tilelist);
   }

   unsigned int then = ::GetTickCount();

   printf("needed: %2.2f ms for one call.\n",double(then-now)/double(loops));
}


#include "SDL.h"

#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")

#undef SDL_main

int main(int argc, char* argv[])
{
   double xpos = 33.0;
   double ypos = 33.0;
   double angle = 20.0;

   std::vector<ua_quad_tile_coord> tilelist;

   runtime_test();

   ua_quad q(0.0,0.0,64.0,64.0);

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
            ua_frustum fr(x, 64-y, angle, 60.0, 32.0);

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
