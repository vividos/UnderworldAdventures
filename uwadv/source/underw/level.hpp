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
/*! \file level.hpp

   \brief game level map class

*/
//! \ingroup underworld

//@{

// include guard
#ifndef uwadv_level_hpp_
#define uwadv_level_hpp_

// needed includes
#include "settings.hpp"
#include "texture.hpp"
#include "objects.hpp"
#include "savegame.hpp"


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

   //! stock texture id for ceiling
   Uint16 texture_ceiling;
};


// classes

//! level map and object representation
class ua_level
{
public:
   //! ctor
   ua_level();

   //! returns floor height on specific position
   double get_floor_height(double xpos, double ypos);

   //! returns a tile info struct
   ua_levelmap_tile &get_tile(unsigned int xpos, unsigned int ypos);

   //! returns map object list ref
   ua_object_list &get_mapobjects(){ return allobjects; }

   //! returns vector of used stock texture ids
   const std::vector<Uint16>& get_used_textures(){ return used_textures; }

   // loading / saving / importing

   //! loads a savegame
   void load_game(ua_savegame &sg);

   //! saves to a savegame
   void save_game(ua_savegame &sg);

   //! imports a level map
   void import_map(SDL_RWops* rwops, Uint16 textures[64],
      bool uw2_mode=false);

   //! imports texture usage info
   void import_texinfo(SDL_RWops* rwops, Uint16 textures[64],
      bool uw2_mode=false);

protected:
   //! all levelmap tiles; 64x64 tiles assumed
   std::vector<ua_levelmap_tile> tiles;

   //! all objects in level
   ua_object_list allobjects;

   //! numbers of all used stock textures
   std::vector<Uint16> used_textures;

   //! indicates if level map is in use
   bool used;
};


#endif
//@}
