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
/*! \file maploader.cpp

   \brief game level map loading implementation

*/

// needed includes
#include "common.hpp"
#include "underworld.hpp"
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


// external function
extern SDL_RWops* ua_rwops_uw2dec(FILE* fd,unsigned int blocknum,
   unsigned int destsize);


// global functions

void ua_import_levelmaps(ua_settings& settings, const char* folder,
   std::vector<ua_level>& levels)
{
   // determine number of levels
   unsigned int numlevels = 0;
   switch(settings.get_gametype())
   {
   case ua_game_uw_demo: numlevels = 1; break;
   case ua_game_uw1: numlevels = 9; break;
   case ua_game_uw2: numlevels = 80; break;
   }

   levels.reserve(numlevels);

   ua_trace("importing %u level maps from %s\n",numlevels,folder);

   // open map file
   std::string mapfile(settings.get_string(ua_setting_uw_path));
   if (settings.get_gametype() == ua_game_uw_demo)
      mapfile.append("data/level13.st");
   else
   {
      mapfile.append(folder);
      mapfile.append("lev.ark");
   }

   FILE *fd = fopen(mapfile.c_str(),"rb");
   if (fd==NULL)
   {
      std::string text("could not open file ");
      text.append(mapfile);
      throw ua_exception(text.c_str());
   }

   Uint16 textures[64];

   // uw_demo is treated specially
   if (settings.get_gametype() == ua_game_uw_demo)
   {
      // load uw_demo maps

      ua_level level;

      // import texture usage table
      {
         mapfile.assign(settings.get_string(ua_setting_uw_path));
         mapfile.append("data/level13.txm");

         SDL_RWops* rwops = SDL_RWFromFile(mapfile.c_str(),"rb");
         if (rwops == NULL)
         {
            std::string text("could not open file ");
            text.append(mapfile);
            throw ua_exception(text.c_str());
         }
         level.import_texinfo(rwops,textures);
         SDL_RWclose(rwops);
      }

      // import map
      SDL_RWops* rwops = SDL_RWFromFP(fd,0);
      level.import_map(rwops,textures);

      // import objects
      SDL_RWseek(rwops,0,SEEK_SET);
      level.get_mapobjects().import_objs(rwops);
      SDL_RWclose(rwops);

      levels.push_back(level);
   }
   else
   if (settings.get_gametype() == ua_game_uw1)
   {
      // load uw1 maps

      // read in offsets
      std::vector<Uint32> offsets;
      Uint16 noffsets = fread16(fd);
      offsets.resize(noffsets,0);

      for(Uint16 n=0; n<noffsets; n++)
         offsets[n] = fread32(fd);

      SDL_RWops* rwops = SDL_RWFromFP(fd,0);

      // load all levels
      for(unsigned int i=0; i<numlevels; i++)
      {
         ua_level level;

         // load texture usage table
         SDL_RWseek(rwops,offsets[i+18],SEEK_SET);
         level.import_texinfo(rwops,textures);

         // load level map
         SDL_RWseek(rwops,offsets[i],SEEK_SET);
         level.import_map(rwops,textures);

         // load object list
         SDL_RWseek(rwops,offsets[i],SEEK_SET);
         level.get_mapobjects().import_objs(rwops);

         levels.push_back(level);
      }

      SDL_RWclose(rwops);
   }
   else
   if (settings.get_gametype() == ua_game_uw2)
   {
      // load uw2 maps

      // resize; uw2 has some unused level blocks
      levels.resize(numlevels);

      // load all levels
      for(unsigned int i=0; i<numlevels; i++)
      {
         SDL_RWops* rwops;
         ua_level level;

         // load texture mapping
         rwops = ua_rwops_uw2dec(fd,i+80,0x0086);
         if (rwops != NULL)
         {
            level.import_texinfo(rwops,textures,true);
            SDL_RWclose(rwops);
         }

         // load map / objects
         rwops = ua_rwops_uw2dec(fd,i,0x7e08);
         if (rwops == NULL)
            continue;

         // load level map
         SDL_RWseek(rwops,0,SEEK_SET);
         level.import_map(rwops,textures,true);

         // load object list
         SDL_RWseek(rwops,0,SEEK_SET);
         level.get_mapobjects().import_objs(rwops);

         SDL_RWclose(rwops);

         // put into vector
         levels[i] = level;
      }
   }

   fclose(fd);
}


// ua_level methods

void ua_level::import_map(SDL_RWops* rwops, Uint16 textures[64], bool uw2_mode)
{
   // read in map info

   // alloc memory for tiles
   tiles.clear();
   tiles.resize(64*64);

   for(Uint16 tile=0; tile<64*64; tile++)
   {
      Uint32 tileword = SDL_RWread32(rwops);

      // extract infos from tile word
      tiles[tile].type = ua_tile_type_mapping[tileword & 0x0000000F];

      // all size values in height units
      tiles[tile].floor = (tileword & 0xF0) >> 3;
      tiles[tile].ceiling = 32;
      tiles[tile].slope = 2; // height units per tile

      // texture indices
      Uint8 wall_index = (tileword & 0x003F0000) >> 16; // 6 bit wide
      Uint8 floor_index = (tileword & 0x00003C00) >> 10; // 4 bit wide

      if (!uw2_mode)
      {
         if (wall_index>=48) wall_index=0;
         if (floor_index>=10) floor_index=0;
      }

      tiles[tile].texture_wall = textures[wall_index];
      tiles[tile].texture_floor = textures[floor_index + (uw2_mode ? 0 : 48)];
      tiles[tile].texture_ceiling = textures[uw2_mode ? 32 : (9+48)];
   }

   used = true;
}

void ua_level::import_texinfo(SDL_RWops* rwops, Uint16 textures[64], bool uw2_mode)
{
   used_textures.clear();

   if (!uw2_mode)
   {
      Uint16 tex;
      for(tex=0; tex<48; tex++)
      {
         textures[tex]  = SDL_RWread16(rwops)+ua_tex_stock_wall;
         used_textures.push_back(textures[tex]);
      }
      for(tex=48; tex<48+10; tex++)
      {
         textures[tex] = SDL_RWread16(rwops)+ua_tex_stock_floor;
         used_textures.push_back(textures[tex]);
      }
      for(tex=48+10; tex<64; textures[tex++]=0);
   }
   else
   {
      Uint16 tex;
      for(tex=0; tex<64; tex++)
      {
         textures[tex] = SDL_RWread16(rwops);
         used_textures.push_back(textures[tex]);
      }
   }

//   for(tex=0; tex<6; tex++)  door_textures[tex]  = fread16(fd);
}
