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
/*! \file quadtree.hpp

   \brief quadtree definitions

*/

// include guard
#ifndef uwadv_quadtree_hpp_
#define uwadv_quadtree_hpp_

// needed includes
#include <vector>
#include <utility>
#include "uamath.hpp"


// typedefs

//! coordinate of a tile
typedef std::pair<unsigned int, unsigned int> ua_quad_tile_coord;


// classes

//! view frustum
/*! as we only have a quadtree, we only need a 2d frustum. to simplify things
    more, we ignore the near clipping plane. this yields to a frustum
    triangle, which is described by its three corner points */
class ua_frustum2d
{
public:
   //! ctor
   ua_frustum2d(double xpos,double ypos,double xangle,double fov,double farplane);

   //! calculates if a point is in the view frustum
   bool is_in_frustum(double xpos,double ypos);

   //! returns triangle point
   ua_vector2d& get_point(unsigned int at){ return points[at]; }

protected:
   //! frustum triangle points
   ua_vector2d points[3];
};


//! represents a single quadtree quad
class ua_quad
{
public:
   //! ctor
   ua_quad(unsigned int my_xmin,unsigned int my_xmax,unsigned int my_ymin,unsigned int my_ymax)
      :xmin(my_xmin), xmax(my_xmax), ymin(my_ymin), ymax(my_ymax){}

   //! calculates all visible tiles from given view frustum
   void get_visible_tiles(ua_frustum2d& fr, std::vector<ua_quad_tile_coord>& tilelist);

   //! returns true when the quad intersects the frustum
   bool does_intersect(ua_frustum2d& fr);

protected:
   //! quad coordinates
   unsigned int xmin, xmax, ymin, ymax;
};

#endif
