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
   bool is_in_frustum(double xpos,double ypos) const;

   //! returns triangle point
   const ua_vector2d& get_point(unsigned int at) const { return points[at]; }

protected:
   //! frustum triangle points
   ua_vector2d points[3];
};


//! callback class interface
class ua_quad_callback
{
public:
   //! callback method
   virtual void visible_tile(unsigned int xpos, unsigned int ypos)=0;
};


//! represents a single quadtree quad
class ua_quad
{
public:
   //! ctor
   ua_quad(unsigned int xmin,unsigned int xmax,
      unsigned int ymin,unsigned int ymax);

   //! finds all visible tiles in given view frustum
   void find_visible_tiles(const ua_frustum2d& fr, ua_quad_callback& callback);

   //! returns true when the quad intersects the frustum
   bool does_intersect(const ua_frustum2d& fr) const;

protected:
   //! quad coordinates
   unsigned int xmin, xmax, ymin, ymax;
};


//! coordinate of a tile
typedef std::pair<unsigned int, unsigned int> ua_quad_tile_coord;


//! visible tile collector helper class
class ua_quad_tile_collector: public ua_quad_callback
{
public:
   // method from ua_quad_callback
   virtual void visible_tile(unsigned int xpos, unsigned int ypos);

   //! returns tile list
   std::vector<ua_quad_tile_coord>& get_tilelist(){ return tilelist; }

protected:
   //! tile list
   std::vector<ua_quad_tile_coord> tilelist;
};


#endif
