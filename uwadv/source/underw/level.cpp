/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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
/*! \file level.cpp

   \brief level map and object implementation

*/

// needed includes
#include "common.hpp"
#include "level.hpp"
#include <set>
#include <cmath>


// ua_level methods

ua_level::ua_level()
:used(true)
{
   // do empty tile map
   tiles.resize(64*64);
}

double ua_level::get_floor_height(double xpos, double ypos)
{
   if (xpos<0.0 || ypos<0.0 || xpos>64.0 || ypos>64.0)
      return 0.0;

   double height = 0.0;

   ua_levelmap_tile& tile = get_tile(static_cast<Uint8>(xpos),static_cast<Uint8>(ypos));

   switch(tile.type)
   {
   case ua_tile_solid:
      height = tile.ceiling; // player shouldn't get there, though
      break;

      // sloped tiles
   case ua_tile_slope_n:
      height = tile.floor +
         double(tile.slope)*fmod(ypos,1.0);
      break;
   case ua_tile_slope_s:
      height = (tile.floor+tile.slope) -
         double(tile.slope)*fmod(ypos,1.0);
      break;
   case ua_tile_slope_e:
      height = tile.floor +
         double(tile.slope)*fmod(xpos,1.0);
      break;
   case ua_tile_slope_w:
      height = (tile.floor+tile.slope) -
         double(tile.slope)*fmod(xpos,1.0);
      break;

      // diagonal tiles
   case ua_tile_diagonal_se:
      if (fmod(xpos,1.0)-fmod(ypos,1.0)<0.0) height = tile.ceiling;
      else height = tile.floor;
      break;
   case ua_tile_diagonal_sw:
      if (fmod(xpos,1.0)+fmod(ypos,1.0)>1.0) height = tile.ceiling;
      else height = tile.floor;
      break;
   case ua_tile_diagonal_nw:
      if (fmod(xpos,1.0)-fmod(ypos,1.0)>0.0) height = tile.ceiling;
      else height = tile.floor;
      break;
   case ua_tile_diagonal_ne:
      if (fmod(xpos,1.0)+fmod(ypos,1.0)<1.0) height = tile.ceiling;
      else height = tile.floor;
      break;

   case ua_tile_open:
   default:
      height = tile.floor;
      break;
   };

   return height;
}

ua_levelmap_tile& ua_level::get_tile(unsigned int xpos, unsigned int ypos)
{
   xpos%=64; ypos%=64;
   return tiles[ypos*64 + xpos];
}

const ua_levelmap_tile& ua_level::get_tile(unsigned int xpos, unsigned int ypos) const
{
   xpos%=64; ypos%=64;
   return tiles[ypos*64 + xpos];
}

void ua_level::load_game(ua_savegame &sg)
{
   sg.begin_section("tilemap");

   used = sg.read8() != 0;

   if (!used)
      return; // don't read empty maps

   // read tilemap
   tiles.clear();
   tiles.resize(64*64);
   unsigned int n=0;

   std::set<Uint16> textures_used;

   for(n=0; n<64*64; n++)
   {
      ua_levelmap_tile &tile = tiles[n];
      tile.type = static_cast<ua_levelmap_tiletype>(sg.read8());
      tile.floor = sg.read16();
      tile.ceiling = sg.read16();
      tile.slope = sg.read8();
      tile.texture_wall = sg.read16();
      tile.texture_floor = sg.read16();
      tile.texture_ceiling = sg.read16();

      textures_used.insert(tile.texture_wall);
      textures_used.insert(tile.texture_floor);
      textures_used.insert(tile.texture_ceiling);
   }

   // add used textures to used_textures vector
   std::set<Uint16>::iterator iter,stop;
   iter = textures_used.begin();
   stop = textures_used.end();

   used_textures.clear();

   for(; iter != stop; iter++)
      used_textures.push_back(*iter);

   // read objects list
   allobjects.load_game(sg);

   // read map notes
   mapnotes.load_game(sg);

   sg.end_section();
}

void ua_level::save_game(ua_savegame& sg)
{
   sg.begin_section("tilemap");

   sg.write8(used ? 1 : 0);

   if (!used)
      return; // don't write empty maps

   // write tilemap
   unsigned int n=0;

   for(n=0; n<64*64; n++)
   {
      ua_levelmap_tile& tile = tiles[n];
      sg.write8(tile.type);
      sg.write16(tile.floor);
      sg.write16(tile.ceiling);
      sg.write8(tile.slope);
      sg.write16(tile.texture_wall);
      sg.write16(tile.texture_floor);
      sg.write16(tile.texture_ceiling);
   }

   // write objects list
   allobjects.save_game(sg);

   // write map notes list
   mapnotes.save_game(sg);

   sg.end_section();
}


// ua_levelmaps_list methods

ua_levelmaps_list::ua_levelmaps_list()
{
}

void ua_levelmaps_list::init()
{
   all_levels.clear();
}

void ua_levelmaps_list::load_game(ua_savegame &sg)
{
   // load all levels
   sg.begin_section("tilemaps");
   Uint32 max = sg.read32();

   all_levels.clear();
   all_levels.resize(max);

   for(Uint32 i=0; i<max; i++)
   {
      ua_level& newlevel = all_levels[i];

      // load tilemap / objects list / annotations
      newlevel.load_game(sg);
   }

   sg.end_section();
}

void ua_levelmaps_list::save_game(ua_savegame &sg)
{
   // save all levels
   sg.begin_section("tilemaps");

   unsigned int max = all_levels.size();
   sg.write32(max);

   for(unsigned int i=0; i<max; i++)
   {
      // save tilemap / objects list / annotations
      all_levels[i].save_game(sg);
   }

   sg.end_section();
}
