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
/*! \file levelmap.hpp

   class to load the game map

*/

// include guard
#ifndef __uwadv_levelmap_hpp_
#define __uwadv_levelmap_hpp_

// needed includes
#include "settings.hpp"
#include "texture.hpp"


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
} ua_wall_render_side;


// structs

//! levelmap tile description
typedef struct
{
   ua_levelmap_tiletype type;

   Uint16 floor;
   Uint16 ceiling;
   Uint8 slope;

   Uint16 texture_wall;
   Uint16 texture_floor;

   Uint16 index;

} ua_levelmap_tile;


// classes

//! levelmap representation
class ua_levelmap
{
public:
   //! ctor
   ua_levelmap(){ tiles=NULL; height_scale=0.125f; }
   //! dtor
   ~ua_levelmap(){ delete[] tiles; tiles=NULL; };

   //! loads the map of a specific level
   void load(ua_settings &settings, int level);

   //! prepares textures used in map for OpenGL
   void prepare_textures(ua_texture_manager &texmgr);

   //! renders map to OpenGL
   void render(ua_texture_manager &texmgr);

   // returns floor height on specific position
   float get_floor_height(float xpos, float ypos);

protected:
   //! renders the floor of a tile
   void render_floor(int x, int y, ua_texture_manager &texmgr);

   //! renders the ceiling of a tile
   void render_ceiling(int x, int y, ua_texture_manager &texmgr);

   //! renders the walls of a tile
   void render_walls(int x, int y, ua_texture_manager &texmgr);

private:
   //! retrieves tile coordinates
   void get_tile_coords(ua_wall_render_side side, ua_levelmap_tiletype type,
      Uint8 basex, Uint8 basey, Uint8 basez, Uint8 slope, Uint8 ceiling,
      Uint8 &x1, Uint8 &y1, Uint8 &z1,
      Uint8 &x2, Uint8 &y2, Uint8 &z2);

   //! renders a wall of a tile, dependent on the neighbour
   void render_wall(ua_wall_render_side side,
      Uint8 x1, Uint8 y1, Uint8 z1, Uint8 x2, Uint8 y2, Uint8 z2,
      Uint8 nz1, Uint8 nz2);

protected:
   //! all levelmap tiles; 64x64 tiles assumed
   ua_levelmap_tile *tiles;

   //! height scale in units per tile height
   float height_scale;

   // texture usage tables
   Uint16 wall_textures[48];
   Uint16 floor_textures[10];
   Uint16 door_textures[6];
};

#endif
