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
/*! \file texloader.cpp

   \brief texture loading implementation

*/

// needed includes
#include "common.hpp"
#include "texture.hpp"
#include "../import/io_endian.hpp"


// extern texture import functions

extern void ua_import_tga(SDL_RWops* rwops, unsigned int& xres, unsigned int& yres,
   unsigned int& origx, unsigned int& origy, std::vector<Uint32>& texels);


// ua_texture methods

void ua_texture::load(SDL_RWops* rwops)
{
   unsigned int origx,origy;

   // currently we only have tga texture import
   // TODO: check for file type and load accordingly
   ua_import_tga(rwops,xres,yres,origx,origy,texels);

   // calculate max. u and v coordinates
   u = ((double)origx)/xres;
   v = ((double)origy)/yres;
}


// ua_texture_manager methods

void ua_texture_manager::init(ua_settings& settings)
{
   // load palettes
   {
      // load main palettes
      std::string allpalname(settings.get_string(ua_setting_uw_path));
      allpalname.append("data/pals.dat");

      load_palettes(allpalname.c_str());
   }

   // load stock textures
   if (settings.get_gametype() == ua_game_uw1 || settings.get_gametype() == ua_game_uw_demo)
   {
      // load all wall textures
      std::string walltexfname(settings.get_string(ua_setting_uw_path));
      walltexfname.append(
         settings.get_gametype() == ua_game_uw1 ? "data/w64.tr" : "data/dw64.tr");
      load_textures(ua_tex_stock_wall,walltexfname.c_str());

      // load all floor textures
      std::string floortexfname(settings.get_string(ua_setting_uw_path));
      floortexfname.append(
         settings.get_gametype() == ua_game_uw1 ? "data/f32.tr" : "data/df32.tr");
      load_textures(ua_tex_stock_floor,floortexfname.c_str());

      // set some animated textures
      {
         stock_animinfo[0x00ce].second = 8; // 206 lavafall
         stock_animinfo[0x0129].second = 8; // 469 rivulets of lava
         stock_animinfo[0x0117].second = 8; // 487 rivulets of lava
         stock_animinfo[0x0118].second = 8; // 486 lava
         stock_animinfo[0x0119].second = 8; // 485 lava

         stock_animinfo[0x0120].second = 4; // 478 water
         stock_animinfo[0x0121].second = 4; // 477 water
         stock_animinfo[0x0122].second = 4; // 476 water
         stock_animinfo[0x0110].second = 4; // 493 water
         stock_animinfo[0x0111].second = 4; // 494 water
      }

      // load switches/levers/pull chains
      {
         ua_image_list il;
         il.load(settings,"tmflat");

         load_imgtextures(ua_tex_stock_switches,il);
      }

      // load door textures
      {
         ua_image_list il;
         il.load(settings,"doors");

         load_imgtextures(ua_tex_stock_door,il);
      }

      // load tmobj textures
      {
         ua_image_list il;
         il.load(settings,"tmobj");

         load_imgtextures(ua_tex_stock_tmobj,il);
      }

      // init stock texture objects
      reset();
   }
   else
   if (settings.get_gametype() == ua_game_uw2)
   { 
      // load all textures
      std::string texfname(settings.get_string(ua_setting_uw_path));
      texfname.append("data/t64.tr");

      load_textures(0,texfname.c_str());

      // init stock texture objects
      reset();
   }

   // load object sprite textures
   {
      // load image list
      ua_image_list il;
      il.load(settings,"objects");

      // make sure we have at least have 460 images
      if (il.size()<460)
         throw ua_exception("expected 460 images in data/objects.gr");

      // copy images to object textures
      obj_textures.init(this,il.size());

      unsigned int max = il.size();
      for(unsigned int id=0; id<max; id++)
      {
         // objects [218..223] and 302 have different sizes
         if ((id>=218 && id<=223) || id==302 )
            continue;

         obj_textures.convert(il.get_image(id),id);
         obj_textures.upload(id);
      }
   }
}

void ua_texture_manager::load_textures(unsigned int startidx, const char* texfname)
{
   FILE* fd = fopen(texfname,"rb");
   if (fd==NULL)
   {
      std::string text("could not open texture file: ");
      text.append(texfname);
      throw ua_exception(text.c_str());
   }

   // get file length
   fseek(fd,0,SEEK_END);
   Uint32 flen = ftell(fd);
   fseek(fd,0,SEEK_SET);

   // get header values
   int val = fgetc(fd); // this always seems to be 2
   int xyres = fgetc(fd); // x and y resolution (square textures)

   Uint16 entries = fread16(fd);

   // reserve needed entries
   if (startidx+entries>stock_textures.size())
   {
      allstocktex_imgs.get_allimages().resize(startidx+entries);
      stock_textures.resize(startidx+entries);
      stock_animinfo.resize(startidx+entries,
         std::make_pair<unsigned int, unsigned int>(0,0));
   }

   // read in all offsets
   std::vector<Uint32> offsets(entries);
   for(unsigned int i=0; i<entries; i++)
      offsets[i] = fread32(fd);

   // read in all textures
   for(unsigned int tex=0; tex<entries; tex++)
   {
      if (offsets[tex]>=flen)
         continue;

      // seek to texture entry
      fseek(fd,offsets[tex],SEEK_SET);

      // alloc memory for texture
      unsigned int datalen = xyres*xyres;

      // create new image
      ua_image& teximg = allstocktex_imgs.get_image(startidx+tex);
      teximg.create(xyres,xyres);
      Uint8* pixels = &teximg.get_pixels()[0];

      stock_animinfo[startidx+tex].second = 1; // we have one texture

      unsigned int idx = 0;
      while(datalen>0)
      {
         unsigned int size = ua_min(datalen,1024);
         unsigned int read = fread(pixels+idx,1,size,fd);
         idx += read;
         datalen -= read;
      }
   }

   fclose(fd);
}

void ua_texture_manager::load_imgtextures(unsigned int startidx,
   ua_image_list& il)
{
   unsigned int max = il.size();

   // reserve needed entries
   if (startidx+max>stock_textures.size())
   {
      allstocktex_imgs.get_allimages().resize(startidx+max);
      stock_textures.resize(startidx+max);
      stock_animinfo.resize(startidx+max,
         std::make_pair<unsigned int, unsigned int>(0,1));
   }

   // copy images
   for(unsigned int i=0; i<max; i++)
      allstocktex_imgs.get_image(startidx+i) = il.get_image(i);
}

void ua_texture_manager::load_palettes(const char* allpalname)
{
   FILE* fd = fopen(allpalname,"rb");
   if (fd==NULL)
   {
      std::string text("could not open file ");
      text.append(allpalname);
      throw ua_exception(text.c_str());
   }

   // palettes are stored in ancient vga color format
   for(int pal=0; pal<8; pal++)
   {
      for(int color=0; color<256; color++)
      {
         allpals[pal][color][0] = fgetc(fd)<<2;
         allpals[pal][color][1] = fgetc(fd)<<2;
         allpals[pal][color][2] = fgetc(fd)<<2;
         allpals[pal][color][3] = color==0 ? 0 : 255;
      }
   }
   fclose(fd);
}
