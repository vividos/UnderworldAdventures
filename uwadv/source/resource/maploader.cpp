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
/*! \file maploader.cpp

   game level map loading implementation

*/

// needed includes
#include "common.hpp"
#include "level.hpp"
#include "fread_endian.hpp"
#include <string>


// tables

ua_levelmap_tiletype ua_tile_type_mapping[16] =
{
  ua_tile_solid,
  ua_tile_open,
  ua_tile_diagonal_se,
  ua_tile_diagonal_sw,
  ua_tile_diagonal_ne,
  ua_tile_diagonal_nw,
  ua_tile_slope_n,
  ua_tile_slope_s,
  ua_tile_slope_e,
  ua_tile_slope_w,
  ua_tile_solid,
  ua_tile_solid,
  ua_tile_solid,
  ua_tile_solid,
  ua_tile_solid,
  ua_tile_solid
};


// ua_level methods

void ua_level::load(ua_settings &settings, unsigned int level)
{
   // construct map file name
   std::string mapfile(settings.uw1_path);

   if (settings.gtype == ua_game_uw1)
      mapfile.append("data/lev.ark");
   else if (settings.gtype == ua_game_uw_demo)
      mapfile.append("data/level13.st");

   FILE *fd = fopen(mapfile.c_str(),"rb");
   if (fd==NULL)
   {
      std::string text("could not open file ");
      text.append(mapfile);
      throw ua_exception(text.c_str());
   }

   Uint16 entries=0;
   std::vector<Uint32> offsets;

   if (settings.gtype == ua_game_uw1)
   {
      // read in all offsets
      entries = fread16(fd);

      offsets.resize(entries,0);
      for(Uint16 n=0; n<entries; n++)
         offsets[n] = fread32(fd);

      // for ultima underword 1, we have 9 level maps

      // read in texture usage table

      // seek to block
      fseek(fd,offsets[level+18],SEEK_SET);

      Uint16 tex;
      for(tex=0; tex<48; tex++) wall_textures[tex]  = fread16(fd)+ua_tex_stock_wall;
      for(tex=0; tex<10; tex++) floor_textures[tex] = fread16(fd)+ua_tex_stock_floor;
      for(tex=0; tex<6; tex++)  door_textures[tex]  = fread16(fd);
   }
   else
   {
      // we only have the demo
      std::string texmapfile(settings.uw1_path);
      texmapfile.append("data/level13.txm");

      FILE *fd2 = fopen(texmapfile.c_str(),"rb");
      if (fd2==NULL)
      {
         std::string text("could not open file ");
         text.append(texmapfile);
         throw ua_exception(text.c_str());
      }

      Uint16 tex;
      for(tex=0; tex<48; tex++) wall_textures[tex]  = fread16(fd2)+ua_tex_stock_wall;
      for(tex=0; tex<10; tex++) floor_textures[tex] = fread16(fd2)+ua_tex_stock_floor;
      for(tex=0; tex<6; tex++)  door_textures[tex]  = fread16(fd2);

      fclose(fd2);
   }

   // read in map info

   if (settings.gtype == ua_game_uw1)
   {
      // seek to proper level map
      fseek(fd,offsets[level],SEEK_SET);
   }
   if (settings.gtype == ua_game_uw_demo)
   {
      fseek(fd,0,SEEK_SET);
   }

   // alloc memory for tiles
   tiles.clear();
   tiles.resize(64*64);

   for(Uint16 tile=0; tile<64*64; tile++)
   {
      Uint32 tileword = fread32(fd);

      // extract infos from tile word
      tiles[tile].type = ua_tile_type_mapping[tileword & 0x0000000F];

      // all size values in height units
      tiles[tile].floor = (tileword & 0xF0) >> 3;
      tiles[tile].ceiling = 32;
      tiles[tile].slope = 2; // height units per tile

      // texture indices
      Uint8 wall_index = (tileword & 0x003F0000) >> 16; // 6 bit wide
      Uint8 floor_index = (tileword & 0x00003C00) >> 10; // 4 bit wide

      if (wall_index>48) wall_index=0;
      if (floor_index>10) floor_index=0;

      tiles[tile].texture_wall = wall_textures[wall_index];
      tiles[tile].texture_floor = floor_textures[floor_index];
   }

   fclose(fd);

   // load all objects
   allobjects.load(settings,level);
}
