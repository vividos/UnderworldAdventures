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
/*! \file texloader.cpp

   \brief texture loading implementation

*/

// needed includes
#include "common.hpp"
#include "texture.hpp"
#include "fread_endian.hpp"


// ua_texture_manager methods

void ua_texture_manager::init(ua_settings &settings)
{
   // load palettes
   {
      // load main palettes
      std::string allpalname(settings.uw1_path);
      allpalname.append("data/pals.dat");

      load_palettes(allpalname.c_str());
   }

   // load stock textures
   if (settings.gtype == ua_game_uw1 || settings.gtype == ua_game_uw_demo)
   {
      // load all wall textures
      std::string walltexfname(settings.uw1_path);
      walltexfname.append(
         settings.gtype == ua_game_uw1 ? "data/w64.tr" : "data/dw64.tr");
      load_textures(ua_tex_stock_wall,walltexfname.c_str());

      // load all floor textures
      std::string floortexfname(settings.uw1_path);
      floortexfname.append(
         settings.gtype == ua_game_uw1 ? "data/f32.tr" : "data/df32.tr");
      load_textures(ua_tex_stock_floor,floortexfname.c_str());

      // init stock texture objects
      reset();
   }

   // load object texture graphics
   if (settings.gtype == ua_game_uw1 || settings.gtype == ua_game_uw_demo)
   {
      // load image list
      ua_image_list il;
      il.load(settings,"objects");

      // make sure we have at least have 460 images
      if (il.size()<460)
         throw ua_exception("expected 460 images in data/objects.gr");

      // create object textures
      ua_image part1,part2;
      part1.create(256,256);
      part2.create(256,256);

      // create first texture
      Uint16 id;
      for(id=0; id<256; id++)
      {
         // objects 218 thru 223 have different size
         if (id<218 || id>=224)
            part1.paste_image(il.get_image(id),(id&0x0f)<<4,id&0xf0);
      }

      // create first texture
      for(id=0; id<(460-256); id++)
      {
         if (id!=(302-256))
            part2.paste_image(il.get_image(id+256),(id&0x0f)<<4,id&0xf0);
      }

      // TODO paste images 218 to 223 and 302 into remaining space

      // convert to textures
      objtex.init(this,2);
      objtex.convert(part1,0);
      objtex.convert(part2,1);

      // prepare textures
      objtex.use(0); objtex.upload(false);
      objtex.use(1); objtex.upload(false);
   }
}

void ua_texture_manager::load_textures(unsigned int startidx, const char *texfname)
{
   FILE *fd = fopen(texfname,"rb");
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
   allstocktex_imgs.get_allimages().resize(startidx+entries);
   stocktex_count[startidx==ua_tex_stock_wall ? 0 : 1] = entries;

   // read in all offsets
   std::vector<Uint32> offsets(entries);
   for(int i=0; i<entries; i++) offsets[i] = fread32(fd);

   // read in all textures
   for(int tex=0; tex<entries; tex++)
   {
      // seek to texture entry
      fseek(fd,offsets[tex],SEEK_SET);

      // alloc memory for texture
      unsigned int datalen = xyres*xyres;

      // create new image
      ua_image &teximg = allstocktex_imgs.get_image(startidx+tex);
      teximg.create(xyres,xyres);
      Uint8 *pixels = &teximg.get_pixels()[0];

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

void ua_texture_manager::load_palettes(const char *allpalname)
{
   FILE *fd = fopen(allpalname,"rb");
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
