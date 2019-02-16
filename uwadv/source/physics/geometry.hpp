//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
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
//
/// \file geometry.hpp
/// \brief level geometry provider class
//
#pragma once

#include "level.hpp"
#include "triangle3d.hpp"

/// level geometry provider class
class ua_geometry_provider
{
public:
   /// ctor
   ua_geometry_provider(const Underworld::Level& level)
      :m_level(level)
   {
   }

   /// returns the list of all triangles for a given tile
   void get_tile_triangles(unsigned int xpos, unsigned int ypos,
      std::vector<ua_triangle3d_textured>& allTriangles);

protected:
   /// helper function for get_tile_triangles()
   void add_wall(ua_triangle3d_textured& tri1, ua_triangle3d_textured& tri2,
      unsigned int side,
      double x1, double y1, double z1,
      double x2, double y2, double z2,
      double nz1, double nz2, double ceiling);

   /// retrieves tile coordinates
   void get_tile_coords(unsigned int side, Underworld::TilemapTileType type,
      unsigned int basex, unsigned int basey, Uint16 basez, Uint16 slope, Uint16 ceiling,
      Uint16& x1, Uint16& y1, Uint16& z1,
      Uint16& x2, Uint16& y2, Uint16& z2);

protected:
   /// level to work with
   const Underworld::Level& m_level;
};
