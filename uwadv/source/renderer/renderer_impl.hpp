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
/*! \file renderer_impl.hpp

   \brief renderer implementation class

*/

// include guard
#ifndef uwadv_renderer_impl_hpp_
#define uwadv_renderer_impl_hpp_

// needed includes
#include "uamath.hpp"
#include "level.hpp"


// classes

class ua_renderer_impl
{
public:
   //! ctor
   ua_renderer_impl();

   //! renders underworld level at given player pos and angles
   void render(const ua_level& level, ua_vector3d pos,
      double panangle, double rotangle, double fov);

protected:
   //! renders tile floor
   void render_floor(ua_levelmap_tile& tile, unsigned int x, unsigned int y);

   //! renders tile ceiling
   void render_ceiling(ua_levelmap_tile& tile, unsigned int x, unsigned int y);

   //! renders tile walls
   void render_walls(ua_levelmap_tile& tile, unsigned int x, unsigned int y);

   //! renders the objects of a tile
   void render_objects(unsigned int x, unsigned int y);


   //! renders a single object
   void render_object(ua_object& obj, unsigned int x, unsigned int y);

   //! renders decal
   void render_decal(ua_object& obj, unsigned int x, unsigned int y);

   //! renders tmap object
   void render_tmap_obj(ua_object& obj, unsigned int x, unsigned int y);

   //! draws a billboarded quad
   void draw_billboard_quad(ua_vector3d base,
      double quadwidth, double quadheight,
      double u1,double v1,double u2,double v2);

   //! retrieves tile coordinates
   static void get_tile_coords(unsigned int side, ua_levelmap_tiletype type,
      unsigned int basex, unsigned int basey, Uint16 basez, Uint16 slope, Uint16 ceiling,
      Uint16 &x1, Uint16 &y1, Uint16 &z1,
      Uint16 &x2, Uint16 &y2, Uint16 &z2);

   //! renders a wall of a tile, dependent on the neighbour
   static void render_wall(unsigned int side,
      Uint16 x1, Uint16 y1, Uint16 z1, Uint16 x2, Uint16 y2, Uint16 z2,
      Uint16 nz1, Uint16 nz2, Uint16 ceiling);

   //! helper function for get_tile_triangles()
   static void add_wall(ua_triangle3d_textured& tri1, ua_triangle3d_textured& tri2,
      unsigned int side,
      double x1, double y1, double z1,
      double x2, double y2, double z2,
      double nz1, double nz2, double ceiling);

protected:
   //! indicates if in selection (picking) mode
   bool selection_mode;

   //! billboard right and up vectors
   ua_vector3d bb_right, bb_up;
};


#endif
