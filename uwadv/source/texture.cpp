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


// ua_texture methods

void ua_texture::use()
{
}

void ua_texture::set_anim_step(unsigned int step)
{
}


// ua_texture_manager methods

ua_texture_manager::ua_texture_manager()
{
}

ua_texture_manager::~ua_texture_manager()
{
   allstocktex.clear();
}

void ua_texture_manager::reset()
{
   glBindTexture(GL_TEXTURE_2D,0);

   int max = allstocktex.size();
   for(int i=0; i<max; i++)
   {
      ua_stock_texture &stex = allstocktex.at(i);
      if (stex.texname!=0)
         glDeleteTextures(1,&stex.texname);
   }
}

void ua_texture_manager::prepare(unsigned int idx)
{
   if (idx>=allstocktex.size())
      return;

   ua_stock_texture &stex = allstocktex.at(idx);
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
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   // generate 32-bit truecolor texture
   Uint8 xyres = texsize==64*64 ? 64 : texsize==32*32 ? 32 : texsize/64;

//   Uint8 *indexed = wall ? allwalltex[idx] : allfloortex[idx];

   std::vector<Uint32> texels;
   texels.resize(stex.pixels.size()*4,0);

   // use texture #0, stock textures always use these
   ua_onepalette &pal = pals.get_palette(0);

   for(int i=0; i<texsize; i++)
   {
      texels[i] = *(Uint32*)(pal+stex.pixels.at(i));
   }

   // build texture and mipmaps
   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, xyres, xyres, GL_RGBA,
      GL_UNSIGNED_BYTE, &texels.at(0));

   // remember texture name
   stex.texname = texname;
}

void ua_texture_manager::use(unsigned int idx)
{
   if (idx>=allstocktex.size())
      return;

   ua_stock_texture &stex = allstocktex.at(idx);

   if (last_texname != stex.texname)
   {
      // bind texture to use it
      glBindTexture(GL_TEXTURE_2D,stex.texname);
      last_texname = stex.texname;
   }
}
