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

   texture loading implementation

*/

// needed includes
#include "common.hpp"
#include "texture.hpp"
#include "fread_endian.hpp"


// ua_palettes methods

void ua_palettes::load(ua_settings &settings)
{
   // load main palettes
   std::string allpalname(settings.uw1_path);
   allpalname.append("data/pals.dat");

   FILE *fd = fopen(allpalname.c_str(),"rb");
   if (fd==NULL)
      throw ua_exception("could not open file pals.dat");

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


// ua_texture_manager methods

void ua_texture_manager::init(ua_settings &settings)
{
   // load palettes
   pals.load(settings);

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
      objteximg[0].create(256,256);
      objteximg[1].create(256,256);

      // create first texture
      ua_image &part1 = objteximg[0];
      Uint16 id;
      for(id=0; id<256; id++)
      {
         // objects 218 thru 223 have different size
         if (id<218 || id>=224)
            part1.paste_image(il.get_image(id),(id&0x0f)<<4,id&0xf0);
      }

      // create first texture
      ua_image &part2 = objteximg[1];
      for(id=0; id<(460-256); id++)
      {
         if (id!=(302-256))
            part2.paste_image(il.get_image(id+256),(id&0x0f)<<4,id&0xf0);
      }
/*
      std::vector<Uint8> &pix1 = part1.get_pixels();
      std::vector<Uint8> &pix2 = part2.get_pixels();
      for(int i=0; i<256*256; i++)
      {
         if (pix1[i]==0) pix1[i]=11;
         if (pix2[i]==0) pix2[i]=11;
      }*/

      // TODO paste images 218 to 223 and 302 into remaining space

      // convert to textures
      objtexs[0].convert(*this,objteximg[0]);
      objtexs[1].convert(*this,objteximg[1]);

      // prepare textures
      objtexs[0].prepare(false);
      objtexs[1].prepare(false);
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
   allstocktex.resize(startidx+entries);

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

      ua_stock_texture &stex = allstocktex[startidx+tex];
      stex.pixels.resize(datalen);

      unsigned int idx = 0;
      while(datalen>0)
      {
         unsigned int size = ua_min(datalen,1024);
         unsigned int read = fread(&stex.pixels[idx],1,size,fd);
         idx += read;
         datalen -= read;
      }
   }

   fclose(fd);
}
