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
/*! \file quadtree.hpp

   quadtree definitions

*/

// include guard
#ifndef __uwadv_quadtree_hpp_
#define __uwadv_quadtree_hpp_

// needed includes
#include <vector>
#include <utility>


// typedefs

//! coordinate of a tile
typedef std::pair<unsigned int, unsigned int> ua_quad_tile_coord;


// classes

//! view frustum
/*! as we only have a quadtree, we only need a 2d frustum. to simplify things
    more, we ignore the near clipping plane. this yields to a frustum
    triangle, which is described by its three corner points */
class ua_frustum
{
public:
   //! ctor
   ua_frustum(double xpos,double ypos,double zpos,double xangle,double yangle,double fov,double farplane);

   //! calculates if a point is in the view frustum
   bool is_in_frustum(double xpos,double ypos);

   //! returns triangle x coordinate
   double get_x(unsigned int at){ return x[at]; }
   //! returns triangle y coordinate
   double get_y(unsigned int at){ return y[at]; }

   double get_pos(int i){ return pos[i]; }

   double get_xangle(){ return xangle; }
   double get_yangle(){ return yangle; }

protected:
   //! frustum triangle points
   double x[3],y[3];
   double pos[3];
   double xangle,yangle;
};


//! represents a single quadtree quad
class ua_quad
{
public:
   //! ctor
   ua_quad(unsigned int nx0,unsigned int ny0,unsigned int nx1,unsigned int ny1)
      :x0(nx0),y0(ny0),x1(nx1),y1(ny1){}

   //! calculates all visible tiles from given view frustum
   void get_visible_tiles(ua_frustum &fr, std::vector<ua_quad_tile_coord> &tilelist);

   //! returns true when the quad intersects the frustum
   bool does_intersect(ua_frustum &fr);

protected:
   //! quad coordinates
   unsigned int x0,y0,x1,y1;
};

#endif
