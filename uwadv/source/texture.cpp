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

   \brief texture manager implementation

*/

// needed includes
#include "common.hpp"
#include "texture.hpp"
#include <cmath>


// ua_texture methods

void ua_texture::init(ua_texture_manager *the_texmgr,unsigned int numtex,
   GLenum the_min_filt, GLenum the_max_filt, GLenum the_wrap_s, GLenum the_wrap_t)
{
   done();

   xres = yres = 0;

   texmgr = the_texmgr;
   min_filt = the_min_filt;
   max_filt = the_max_filt;
   wrap_s = the_wrap_s;
   wrap_t = the_wrap_t;

   last_used_tex_idx = (unsigned)-1;

   // create texture names
   texname.resize(numtex,0);
   if (numtex>0)
      glGenTextures(numtex,&texname[0]);
}

void ua_texture::convert(ua_image &img,unsigned int numtex)
{
   if (texmgr==NULL || img.get_palette()>=8) return;

   convert(texmgr->get_palette(img.get_palette()),img,numtex);
}

void ua_texture::convert(ua_onepalette &pal, ua_image &img,
   unsigned int numtex)
{
   Uint8 *pix = &img.get_pixels()[0];

   unsigned int origx = img.get_xres();
   unsigned int origy = img.get_yres();

   convert(pix,origx,origy,pal,numtex);
}

void ua_texture::convert(Uint8 *pix, unsigned int origx, unsigned int origy,
   ua_onepalette &pal, unsigned int numtex)
{
   // only do resolution determination for the first texture
   if (numtex==0 || xres==0 || yres==0)
   {
      // determine texture resolution (must be 2^n)
      xres = 16;
      while(xres<origx && xres<2048) xres<<=1;

      yres = 16;
      while(yres<origy && yres<2048) yres<<=1;

      u = ((double)origx)/xres;
      v = ((double)origy)/yres;

      texels.resize(texname.size()*xres*yres,0x00000000);
   }

   // convert color indices to 32-bit texture
   Uint32 *palptr = reinterpret_cast<Uint32*>(&pal);
   Uint32 *texptr = &texels[numtex*xres*yres];

   for(unsigned int y=0; y<origy; y++)
   {
      Uint32 *texptr2 = &texptr[y*xres];
      for(unsigned int x=0; x<origx; x++)
         *texptr2++ = palptr[pix[y*origx+x]];
   }
}

void ua_texture::use(unsigned int numtex)
{
   if (numtex>=texname.size())
      return; // invalid texture index

   last_used_tex_idx = numtex;

   // invalidates currently used texture
   if (texmgr == NULL || texmgr->using_new_texname(texname[numtex]))
   {
      glBindTexture(GL_TEXTURE_2D,texname[numtex]);

      // set texture parameter
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, min_filt);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, max_filt);
   }
}

void ua_texture::upload(bool mipmaps)
{
   Uint32 *tex = &texels[last_used_tex_idx*xres*yres];

   if (mipmaps)
   {
      // build mipmapped textures
      gluBuild2DMipmaps(GL_TEXTURE_2D, 3, xres, yres, GL_RGBA,
         GL_UNSIGNED_BYTE, tex);
   }
   else
   {
      // build texture
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xres, yres, 0, GL_RGBA,
         GL_UNSIGNED_BYTE, tex);
   }
}

void ua_texture::done()
{
   texels.clear();

   // delete all texture names
   if (texname.size()>0)
      glDeleteTextures(texname.size(),&texname[0]);
   texname.clear();

   // invalidate current texture
   if (texmgr != NULL)
      texmgr->using_new_texname(0);
}

const Uint32* ua_texture::get_texels(unsigned int numtex)
{
   return &texels[numtex*xres*yres];
}


// ua_texture_manager methods

ua_texture_manager::ua_texture_manager()
{
   stocktex_count[0]=stocktex_count[1]=0;
}

ua_texture_manager::~ua_texture_manager()
{
   reset();
}

void ua_texture_manager::reset()
{
   glBindTexture(GL_TEXTURE_2D,0);

   // delete all stock textures
   stocktex[0].done();
   stocktex[1].done();
   stocktex[0].init(this,stocktex_count[0],GL_LINEAR,GL_NEAREST_MIPMAP_LINEAR);
   stocktex[1].init(this,stocktex_count[1],GL_LINEAR,GL_NEAREST_MIPMAP_LINEAR);
   last_texname = 0;
}

void ua_texture_manager::prepare(unsigned int idx)
{
   if (idx>=allstocktex_imgs.size())
      return; // not a valid index

   unsigned int stock = 0;
   unsigned int stockidx = idx;
   if (idx>=ua_tex_stock_floor)
   {
      stockidx -= ua_tex_stock_floor;
      stock = 1;
   }

   // convert to texture object
   stocktex[stock].convert(allstocktex_imgs.get_image(idx),stockidx);
   stocktex[stock].use(stockidx);
   stocktex[stock].upload(true); // upload texture with mipmaps
}

void ua_texture_manager::use(unsigned int idx)
{
   if (idx>=allstocktex_imgs.size())
      return; // not a valid index

   unsigned int stock=0;
   if (idx>=ua_tex_stock_floor)
   {
      idx -= ua_tex_stock_floor;
      stock = 1;
   }

   stocktex[stock].use(idx);
}

void ua_texture_manager::object_tex(Uint16 id,double &u1,double &v1,double &u2,double &v2)
{
   // select texture
   objtex.use(id<0x100 ? 0 : 1);

   // calculate texture coordinates
   double delta = 1.0/256;

   u1 = ((id&0x0f)<<4)*delta;
   v1 = (id&0xf0)*delta;

   u2 = (((id&0x0f)<<4)+16)*delta;
   v2 = ((id&0xf0)+16)*delta;
}

bool ua_texture_manager::using_new_texname(GLuint new_texname)
{
   if (last_texname == new_texname)
      return false; // no need to use texname again

   last_texname = new_texname;
   return true;
}

void ua_texture_manager::stock_to_external(unsigned int idx, ua_texture &tex)
{
   tex.convert(allstocktex_imgs.get_image(idx),0);
}
