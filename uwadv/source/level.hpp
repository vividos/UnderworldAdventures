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
/*! \file level.hpp

   \brief game level map class

*/

// include guard
#ifndef __uwadv_level_hpp_
#define __uwadv_level_hpp_

// needed includes
#include "settings.hpp"
#include "texture.hpp"
#include "quadtree.hpp"
#include "objects.hpp"
#include "savegame.hpp"
#include "uamath.hpp"


// enums

//! levelmap tile types
typedef enum
{
   ua_tile_solid = 0x00,
   ua_tile_open  = 0x01,
   ua_tile_diagonal_se = 0x02,
   ua_tile_diagonal_sw = 0x03,
   ua_tile_diagonal_nw = 0x04,
   ua_tile_diagonal_ne = 0x05,
   ua_tile_slope_n = 0x06,
   ua_tile_slope_e = 0x07,
   ua_tile_slope_s = 0x08,
   ua_tile_slope_w = 0x09

} ua_levelmap_tiletype;

//! side of currently rendered wall; used internally
typedef enum
{
   ua_left, ua_right, ua_front, ua_back
} ua_levelmap_wall_render_side;


// structs

//! levelmap tile description
struct ua_levelmap_tile
{
   //! ctor
   ua_levelmap_tile():type(ua_tile_solid){}

   //! tile type
   ua_levelmap_tiletype type;

   //! floor height
   Uint16 floor;

   //! ceiling height
   Uint16 ceiling;

   //! slope from this tile to next
   Uint8 slope;

   //! stock texture id for wall
   Uint16 texture_wall;

   //! stock texture id for floor
   Uint16 texture_floor;
};

//! textured triangle
struct ua_triangle3d_textured: public ua_triangle3d
{
   //! stock texture number used
   Uint16 texnum;

   //! u/v texture coordinates
   double tex_u[3];
   double tex_v[3];

   void set(unsigned int point, double x, double y, double z, double u, double v)
   {
      points[point].set(x,y,z); tex_u[point] = u; tex_v[point] = v;
   }

   bool operator<(const ua_triangle3d_textured &tri)
   {
      return texnum>tri.texnum;
   }
};


// classes

//! level map and object representation
class ua_level
{
public:
   //! ctor
   ua_level(){ height_scale=0.125; }
   //! dtor
   ~ua_level(){}

   //! prepares textures used in map for OpenGL
   void prepare_textures(ua_texture_manager &texmgr);

   //! renders map to OpenGL, using a view frustum
   void render(ua_texture_manager &texmgr,ua_frustum &fr);

   //! renders whole map to OpenGL
   void render(ua_texture_manager &texmgr);

   //! returns floor height on specific position
   double get_floor_height(double xpos, double ypos);

   //! returns a tile info struct
   ua_levelmap_tile &get_tile(unsigned int xpos, unsigned int ypos);

   //! returns map object list ref
   ua_object_list &get_mapobjects(){ return allobjects; }

   //! returns the list of all triangles for a given tile
   void get_tile_triangles(unsigned int xpos, unsigned int ypos,
      std::vector<ua_triangle3d_textured> &alltriangles);

   // loading / saving / importing

   //! loads a savegame
   void load_game(ua_savegame &sg);

   //! saves to a savegame
   void save_game(ua_savegame &sg);

   //! imports a level map
   void import_map(FILE *fd, Uint16 ceil_tex, Uint16 wall_textures[48],
      Uint16 floor_textures[10]);

   //! imports texture usage info
   void import_texinfo(FILE *fd, Uint16 wall_textures[48],
      Uint16 floor_textures[10]);

protected:
   //! renders the floor of a tile
   void render_floor(unsigned int x, unsigned int y, ua_texture_manager &texmgr);

   //! renders the ceiling of a tile
   void render_ceiling(unsigned int x, unsigned int y, ua_texture_manager &texmgr);

   //! renders the walls of a tile
   void render_walls(unsigned int x, unsigned int y, ua_texture_manager &texmgr);

   //! renders the objects of a tile
   void render_objs(unsigned int x, unsigned int y,
      ua_texture_manager &texmgr, ua_frustum &fr);

private:
   //! retrieves tile coordinates
   void get_tile_coords(ua_levelmap_wall_render_side side, ua_levelmap_tiletype type,
      unsigned int basex, unsigned int basey, Uint16 basez, Uint16 slope, Uint16 ceiling,
      Uint16 &x1, Uint16 &y1, Uint16 &z1,
      Uint16 &x2, Uint16 &y2, Uint16 &z2);

   //! renders a wall of a tile, dependent on the neighbour
   void render_wall(ua_levelmap_wall_render_side side,
      Uint16 x1, Uint16 y1, Uint16 z1, Uint16 x2, Uint16 y2, Uint16 z2,
      Uint16 nz1, Uint16 nz2, Uint16 ceiling);

protected:
   //! all levelmap tiles; 64x64 tiles assumed
   std::vector<ua_levelmap_tile> tiles;

   //! all objects in level
   ua_object_list allobjects;

   //! height scale in units per tile height
   double height_scale;

   //! numbers of all used stock textures
   std::vector<Uint16> used_textures;

   //! number of ceiling stock texture
   Uint16 ceiling_texture;
};

#endif
