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
#include "import.hpp"
#include "underworld.hpp"
#include "level.hpp"
#include "io_endian.hpp"
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


// ua_uw1_import methods

void ua_uw1_import::load_levelmaps(std::vector<ua_level> &levels, ua_settings &settings,
   const char* folder)
{
   // load uw1 maps

   // determine number of levels
   unsigned int numlevels = settings.get_gametype() == ua_game_uw_demo ? 1 : 9;
   levels.resize(numlevels);

   ua_trace("importing %u uw1 level maps from %s\n",numlevels,folder);

   Uint16 textures[64];

   std::string mapfile(settings.get_string(ua_setting_uw_path));

   if (settings.get_gametype() == ua_game_uw_demo)
   {
      // load uw_demo maps
      SDL_RWops* rwops;
      ua_level& level = levels[0];

      // import texture usage table
      {
         mapfile.append("data/level13.txm");

         rwops = SDL_RWFromFile(mapfile.c_str(),"rb");
         if (rwops == NULL)
         {
            std::string text("could not open uw_demo texinfo file ");
            text.append(mapfile);
            throw ua_exception(text.c_str());
         }

         load_texinfo(level,rwops,textures,false);
         SDL_RWclose(rwops);
      }

      // import map
      mapfile.assign(settings.get_string(ua_setting_uw_path));
      mapfile.append("data/level13.st");

      rwops = SDL_RWFromFile(mapfile.c_str(),"rb");
      if (rwops==NULL)
      {
         std::string text("could not open uw_demo map file ");
         text.append(mapfile);
         throw ua_exception(text.c_str());
      }

      load_tilemap(level,rwops,textures,false);

      // import objects
      SDL_RWseek(rwops,0,SEEK_SET);
//      load_mapobjects(level.get_mapobjects(),rwops,textures);
      SDL_RWclose(rwops);
   }
   else
   if (settings.get_gametype() == ua_game_uw1)
   {
      // load uw1 maps

      // open map file
      mapfile.append(folder);
      mapfile.append("lev.ark");

      SDL_RWops* rwops = SDL_RWFromFile(mapfile.c_str(),"rb");
      if (rwops==NULL)
      {
         std::string text("could not open file ");
         text.append(mapfile);
         throw ua_exception(text.c_str());
      }

      // read in offsets
      std::vector<Uint32> offsets;
      Uint16 noffsets = SDL_RWread16(rwops);
      offsets.resize(noffsets,0);

      for(Uint16 n=0; n<noffsets; n++)
         offsets[n] = SDL_RWread32(rwops);

      // load all levels
      for(unsigned int i=0; i<numlevels; i++)
      {
         ua_level& level = levels[i];

         // load texture usage table
         SDL_RWseek(rwops,offsets[i+18],SEEK_SET);
         load_texinfo(level,rwops,textures,false);

         // load level map
         SDL_RWseek(rwops,offsets[i],SEEK_SET);
         load_tilemap(level,rwops,textures,false);

         // load object list
         SDL_RWseek(rwops,offsets[i],SEEK_SET);
//         load_mapobjects(level.get_mapobjects(),rwops,textures);

      }

      SDL_RWclose(rwops);
   }
}


// ua_uw2_import methods

void ua_uw2_import::load_levelmaps(std::vector<ua_level> &levels, ua_settings &settings,
   const char* folder)
{
   // load uw2 maps
   unsigned int numlevels = 80;
   levels.resize(numlevels);

   ua_trace("importing %u uw2 level maps from %s\n",numlevels,folder);

   Uint16 textures[64];

   // open map file
   std::string mapfile(settings.get_string(ua_setting_uw_path));

   mapfile.append(folder);
   mapfile.append("lev.ark");

   FILE* fd = fopen(mapfile.c_str(),"rb");
   if (fd==NULL)
   {
      std::string text("could not open uw2 map file ");
      text.append(mapfile);
      throw ua_exception(text.c_str());
   }

   // load all levels
   for(unsigned int i=0; i<numlevels; i++)
   {
      SDL_RWops* rwops;
      ua_level& level = levels[i];

      // load texture mapping
      rwops = get_rwops_uw2dec(fd,i+80,0x0086);
      if (rwops != NULL)
      {
         load_texinfo(level,rwops,textures,true);
         SDL_RWclose(rwops);
      }

      // load map / objects
      rwops = get_rwops_uw2dec(fd,i,0x7e08);
      if (rwops == NULL)
         continue;

      // load level map
      SDL_RWseek(rwops,0,SEEK_SET);
      load_tilemap(level,rwops,textures,true);

      // load object list
      SDL_RWseek(rwops,0,SEEK_SET);
//      load_mapobjects(level.get_mapobjects(),rwops,textures);

      SDL_RWclose(rwops);
   }

   fclose(fd);
}


// ua_uw_import methods

void ua_uw_import::load_levelmaps(std::vector<ua_level> &levels, ua_settings &settings,
   const char* folder)
{
   throw ua_exception("cannot call ua_uw_import::load_levelmaps() directly!");
}

void ua_uw_import::load_tilemap(ua_level& level, SDL_RWops* rwops, Uint16 textures[64], bool uw2_mode)
{
   std::vector<ua_levelmap_tile>& tiles = level.get_tileslist();

   // read in map info

   // alloc memory for tiles
   tiles.clear();
   tiles.resize(64*64);

   for(Uint16 tile=0; tile<64*64; tile++)
   {
      ua_levelmap_tile& curtile = tiles[tile];

      Uint32 tileword = SDL_RWread32(rwops);

      // extract infos from tile word
      curtile.type = ua_tile_type_mapping[tileword & 0x0000000F];

      // all size values in height units
      curtile.floor = (tileword & 0xF0) >> 1;
      curtile.ceiling = 128;
      curtile.slope = 8; // height units per tile

      // texture indices
      Uint8 wall_index = (tileword & 0x003F0000) >> 16; // 6 bit wide
      Uint8 floor_index = (tileword & 0x00003C00) >> 10; // 4 bit wide

      if (!uw2_mode)
      {
         if (wall_index>=48) wall_index=0;
         if (floor_index>=10) floor_index=0;
      }

      curtile.texture_wall = textures[wall_index];
      curtile.texture_floor = textures[floor_index + (uw2_mode ? 0 : 48)];
      curtile.texture_ceiling = textures[uw2_mode ? 32 : (9+48)];
   }

//   level.used = true;
}

/*! \todo load texture info for doors */
void ua_uw_import::load_texinfo(ua_level& level, SDL_RWops* rwops, Uint16 textures[64], bool uw2_mode)
{
   std::vector<Uint16>& used_textures = level.get_used_textures();

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
