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
/*! \file texture.cpp

   texture manager implementation

*/

// needed includes
#include "common.hpp"
#include "texture.hpp"
#include <cmath>


// ua_texture methods

void ua_texture::convert(ua_texture_manager &texmgr,ua_image &img)
{
   if (img.get_palette()>=8)
      return;

   Uint8 *pix = &img.get_pixels()[0];

   unsigned int origx = img.get_xres();
   unsigned int origy = img.get_yres();

   // determine texture resolution (must be 2^n)
   xres = (unsigned int)pow(2,int(log(origx)/log(2)+0.9999));
   yres = (unsigned int)pow(2,int(log(origy)/log(2)+0.9999));
   texcount = 1;

   u = float(origx)/xres;
   v = float(origy)/yres;

   texels.resize(xres*yres,0x00000000);

   ua_onepalette &pal = texmgr.get_palette(img.get_palette());

   for(unsigned int y=0; y<origy; y++)
   for(unsigned int x=0; x<origx; x++)
   {
      Uint8 idx = pix[y*origx+x];
      Uint32 texel = *((Uint32*)pal[idx]);
      texels[y*xres+x] = texel;
   }
}

void ua_texture::convert(ua_texture_manager &texmgr,ua_image_list &imglist,
   unsigned int start, unsigned int until)
{
   // TODO implement
//   if (until==-1)
//      until = imglist.size();

//   texcount = until-start;
}

void ua_texture::prepare(bool mipmaps, GLenum min_filt, GLenum max_filt)
{
   texname.resize(texcount,0);

   // create texture names
   glGenTextures(texcount,&texname[0]);

   for(unsigned int i=0; i<texcount; i++)
   {
      Uint32 *tex = &texels[i*xres*yres];

      glBindTexture(GL_TEXTURE_2D, texname[i]);

      // set texture parameters
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, min_filt);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, max_filt);

      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

      // build mipmaps/single texture
      if (mipmaps)
         gluBuild2DMipmaps(GL_TEXTURE_2D, 3, xres, yres, GL_RGBA,
            GL_UNSIGNED_BYTE, tex);
      else
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xres, yres, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, tex);
   }
}

void ua_texture::use(ua_texture_manager &texmgr, unsigned int animstep)
{
   if (animstep>=texname.size())
      return;

   // invalidates currently used texture
   texmgr.invalidate_tex();

   glBindTexture(GL_TEXTURE_2D,texname[animstep]);
}


// ua_texture_manager methods

ua_texture_manager::ua_texture_manager()
{
}

ua_texture_manager::~ua_texture_manager()
{
   reset();
   allstocktex.clear();
}

void ua_texture_manager::reset()
{
   glBindTexture(GL_TEXTURE_2D,0);

   int max = allstocktex.size();
   for(int i=0; i<max; i++)
   {
      ua_stock_texture &stex = allstocktex[i];
      if (stex.texname!=0)
         glDeleteTextures(1,&stex.texname);
   }
}

void ua_texture_manager::prepare(unsigned int idx)
{
   if (idx>=allstocktex.size())
      return;

   ua_stock_texture &stex = allstocktex[idx];
   unsigned int texsize = stex.pixels.size();

   if (texsize==0)
      return;

   // generate a texture object number
   GLuint texname=0;
   glGenTextures(1,&texname);

   glBindTexture(GL_TEXTURE_2D, texname);

   // set texture parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR/*GL_LINEAR*/);

   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   // generate 32-bit truecolor texture
   Uint8 xyres = texsize==64*64 ? 64 : texsize==32*32 ? 32 : texsize/64;

   std::vector<Uint32> texels;
   texels.resize(stex.pixels.size()*4,0);

   // use texture #0, stock textures always use these
   ua_onepalette &pal = pals.get_palette(0);

   for(int i=0; i<texsize; i++)
   {
      texels[i] = *(Uint32*)(pal+stex.pixels[i]);
   }

   // build texture and mipmaps
   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, xyres, xyres, GL_RGBA,
      GL_UNSIGNED_BYTE, &texels[0]);

   // remember texture name
   stex.texname = texname;
}

void ua_texture_manager::use(unsigned int idx)
{
   if (idx>=allstocktex.size())
      return;

   ua_stock_texture &stex = allstocktex[idx];

   if (last_texname != stex.texname)
   {
      // bind texture to use it
      glBindTexture(GL_TEXTURE_2D,stex.texname);
      last_texname = stex.texname;
   }
}

void ua_texture_manager::object_tex(Uint16 id,double &u1,double &v1,double &u2,double &v2)
{
   // select texture
   if (id<0x100) objtexs[0].use(*this);
   else objtexs[1].use(*this);

   // calculate texture coordinates
   double delta = 1.0/256;

   u1 = ((id&0x0f)<<4)*delta;
   v1 = (id&0xf0)*delta+0.3*delta;

   u2 = (((id&0x0f)+1)<<4)*delta-delta*0.4;
   v2 = ((id&0xf0)+15)*delta+0.8*delta;
}
