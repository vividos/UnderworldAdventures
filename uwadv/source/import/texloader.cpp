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
#include "io_endian.hpp"
#include "import.hpp"


// extern texture import functions

extern void ua_import_tga(SDL_RWops* rwops, unsigned int& xres, unsigned int& yres,
   unsigned int& origx, unsigned int& origy, std::vector<Uint32>& texels);


// ua_texture methods
/*
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
*/

// ua_uw_import methods

void ua_uw_import::load_textures(std::vector<ua_image>& tex_images,
   unsigned int startidx, const char* texname,
   ua_smart_ptr<ua_palette256>& palette)
{
   FILE* fd = fopen(texname,"rb");
   if (fd==NULL)
   {
      std::string text("could not open texture file: ");
      text.append(texname);
      throw ua_exception(text.c_str());
   }

   // get file length
   fseek(fd,0,SEEK_END);
   Uint32 filelen = ftell(fd);
   fseek(fd,0,SEEK_SET);

   // get header values
   Uint8 val = fgetc(fd); // always 2 (.tr)
   unsigned int xyres = fgetc(fd); // x and y resolution (square textures)

   Uint16 entries = fread16(fd);

   // reserve needed entries
   if (startidx+entries > tex_images.size())
      tex_images.resize(startidx+entries);

   // read in all offsets
   std::vector<Uint32> offsets(entries);
   for(unsigned int i=0; i<entries; i++)
      offsets[i] = fread32(fd);

   // read in all textures
   for(unsigned int tex=0; tex<entries; tex++)
   {
      if (offsets[tex] >= filelen)
         continue;

      // seek to texture entry
      fseek(fd,offsets[tex],SEEK_SET);

      // alloc memory for texture
      unsigned int datalen = xyres*xyres;

      // create new image
      ua_image& teximg = tex_images[startidx+tex];
      teximg.create(xyres,xyres);
      Uint8* pixels = &teximg.get_pixels()[0];

      teximg.get_palette() = palette;

      unsigned int idx = 0;
      while(datalen>0)
      {
         unsigned int size = ua_min(datalen,4096);
         unsigned int read = fread(pixels+idx,1,size,fd);
         idx += read;
         datalen -= read;
      }
   }

   fclose(fd);
}
