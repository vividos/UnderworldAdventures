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

   // load all auxiliary palettes
   std::string allauxpalname(settings.uw1_path);
   allauxpalname.append("data/allpals.dat");

   fd = fopen(allauxpalname.c_str(),"rb");
   if (fd==NULL)
      throw ua_exception("could not open file allpals.dat");

   for(int entries=0; entries<32; entries++)
   {
      for(int color=0; color<16; color++)
      {
         allauxpals[entries][color] = fgetc(fd);
      }
   }
   fclose(fd);
}


// ua_texture_manager methods

void ua_texture_manager::load_textures(bool wall, const char *texfname)
{
   FILE *fd = fopen(texfname,"rb");
   if (fd==NULL)
      throw ua_exception("could not open texture file");

   // get file length
   fseek(fd,0,SEEK_END);
   Uint32 flen = ftell(fd);
   fseek(fd,0,SEEK_SET);

   // get header values
   int val = fgetc(fd); // this always seems to be 2
   int xyres = fgetc(fd); // x and y resolution (square textures)

   Uint16 entries = fread16(fd);

   // free memory for old textures
   delete_textures(wall);

   // allocate memory for all textures
   if (wall)
   {
      allwalltex = new Uint8*[entries];
      numwalltex = entries;
      wallxyres = xyres;

      wall_texnums.clear();
      wall_texnums.resize(entries,0);
   }
   else
   {
      allfloortex = new Uint8*[entries];
      numfloortex = entries;
      floorxyres = xyres;

      floor_texnums.clear();
      floor_texnums.resize(entries,0);
   }

   Uint8 **textures = wall ? allwalltex : allfloortex;

   // read in all offsets
   Uint32 *offsets = new Uint32[entries];
   for(int i=0; i<entries; i++) offsets[i] = fread32(fd);

   // read in all textures
   for(int tex=0; tex<entries; tex++)
   {
      // seek to texture entry
      fseek(fd,offsets[tex],SEEK_SET);

      // alloc memory for texture
      int datalen = xyres*xyres;

      textures[tex] = new Uint8[datalen];

      Uint8 *buffer = textures[tex];
      while(datalen>0)
      {
         int size = datalen > 1024 ? 1024 : datalen; // ua_min
         int read = fread(buffer,1,size,fd);
         buffer += read;
         datalen -= read;
      }
   }

   delete[] offsets;

   fclose(fd);
}

void ua_texture_manager::init(ua_settings &settings)
{
   // load palettes
   pals.load(settings);

   // load all wall textures
   std::string walltexfname(settings.uw1_path);
   walltexfname.append("data/w64.tr");
   load_textures(true,walltexfname.c_str());

   // load all floor textures
   std::string floortexfname(settings.uw1_path);
   floortexfname.append("data/f32.tr");
   load_textures(false,floortexfname.c_str());
}
