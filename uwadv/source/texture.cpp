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
/*! \file texture.cpp

   \brief texture manager implementation

*/

// needed includes
#include "common.hpp"
#include "texture.hpp"


// ua_texture methods

void ua_texture::init(ua_texture_manager* the_texmgr,unsigned int numtex,
   GLenum the_min_filt, GLenum the_max_filt,
   GLenum the_wrap_s, GLenum the_wrap_t)
{
   done();

   xres = yres = 0;

   texmgr = the_texmgr;
   min_filt = the_min_filt;
   max_filt = the_max_filt;
   wrap_s = the_wrap_s;
   wrap_t = the_wrap_t;

   // create texture names
   texname.resize(numtex,0);
   if (numtex>0)
      glGenTextures(numtex,&texname[0]);
}

void ua_texture::convert(ua_image& img,unsigned int numtex)
{
   if (texmgr==NULL || img.get_palette()>=8) return;

   convert(texmgr->get_palette(img.get_palette()),img,numtex);
}

void ua_texture::convert(ua_onepalette& pal, ua_image& img,
   unsigned int numtex)
{
   Uint8* pix = &img.get_pixels()[0];

   unsigned int origx = img.get_xres();
   unsigned int origy = img.get_yres();

   convert(pix,origx,origy,pal,numtex);
}

void ua_texture::convert(Uint8* pix, unsigned int origx, unsigned int origy,
   ua_onepalette& pal, unsigned int numtex)
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
   Uint32* palptr = reinterpret_cast<Uint32*>(&pal);
   Uint32* texptr = &texels[numtex*xres*yres];

   for(unsigned int y=0; y<origy; y++)
   {
      Uint32* texptr2 = &texptr[y*xres];
      for(unsigned int x=0; x<origx; x++)
         *texptr2++ = palptr[pix[y*origx+x]];
   }
}

void ua_texture::convert(unsigned int origx, unsigned int origy, Uint32* pix,
   unsigned int numtex)
{
   // determine texture resolution (must be 2^n)
   xres = 16;
   while(xres<origx && xres<2048) xres<<=1;

   yres = 16;
   while(yres<origy && yres<2048) yres<<=1;

   u = ((double)origx)/xres;
   v = ((double)origy)/yres;

   texels.resize(texname.size()*xres*yres,0x00000000);

   // copy pixels
   Uint32* texptr = &texels[numtex*xres*yres];
   for(unsigned int y=0; y<origy; y++)
   {
      memcpy(texptr, pix, origx*sizeof(Uint32));
      pix += origx;
      texptr += xres;
   }
}

void ua_texture::use(unsigned int numtex)
{
   if (numtex>=texname.size())
      return; // invalid texture index

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

void ua_texture::upload(unsigned int numtex, bool mipmaps)
{
   use(numtex);

   Uint32* tex = &texels[numtex*xres*yres];

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
}

ua_texture_manager::~ua_texture_manager()
{
   reset();
}

void ua_texture_manager::reset()
{
   glBindTexture(GL_TEXTURE_2D,0);

   // call "done" for all stock textures
   unsigned int max = stock_textures.size();
   for(unsigned int i=0; i<max; i++)
      stock_textures[i].done();

   last_texname = (GLuint)-1;
}

void ua_texture_manager::prepare(unsigned int idx)
{
   if (idx>=allstocktex_imgs.size())
      return; // not a valid index

   unsigned int pal_max = stock_animinfo[idx].second > 0;
   if (pal_max<1)
      return; // not an available texture

   stock_textures[idx].init(this,pal_max,
      GL_LINEAR,GL_NEAREST_MIPMAP_LINEAR,GL_CLAMP,GL_REPEAT);

   if (pal_max==1)
   {
      // unanimated texture
      // convert to texture object
      stock_textures[idx].convert(allstocktex_imgs.get_image(idx),0);
      stock_textures[idx].upload(0,true); // upload texture with mipmaps
   }
   else
   {
      // animated texture
      // TODO upload
      ;
   }
}

void ua_texture_manager::use(unsigned int idx)
{
   if (idx>=stock_textures.size())
      return; // not a valid index

   stock_textures[idx].use();
}

void ua_texture_manager::object_tex(Uint16 id,double& u1,double& v1,double& u2,double& v2)
{
   obj_textures.use(id);
   u1 = v1 = 0.0;
   u2 = v2 = 1.0;
}

bool ua_texture_manager::using_new_texname(GLuint new_texname)
{
   if (last_texname == new_texname)
      return false; // no need to use texname again

   last_texname = new_texname;
   return true;
}

void ua_texture_manager::stock_to_external(unsigned int idx, ua_texture& tex)
{
   tex.convert(allstocktex_imgs.get_image(idx),0);
}
