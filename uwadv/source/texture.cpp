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


// ua_texture_manager methods

ua_texture_manager::ua_texture_manager()
{
   allwalltex = allfloortex = NULL;
   numwalltex = numfloortex = 0;
}

ua_texture_manager::~ua_texture_manager()
{
   delete_textures(false);
   delete_textures(true);
}

void ua_texture_manager::reset()
{
   glBindTexture(GL_TEXTURE_2D,0);

   int i,max = wall_texnums.size();
   for(i=0; i<max; i++)
   {
      if (wall_texnums[i]!=0)
         glDeleteTextures(1,&wall_texnums[i]);
   }

   max = floor_texnums.size();
   for(i=0; i<max; i++)
   {
      if (floor_texnums[i]!=0)
         glDeleteTextures(1,&floor_texnums[i]);
   }
}

void ua_texture_manager::prepare(bool wall, int idx)
{
   if ((wall && idx >= numwalltex) || (!wall && idx >= numfloortex))
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
   Uint8 xyres = wall ? wallxyres : floorxyres;
   Uint8 *indexed = wall ? allwalltex[idx] : allfloortex[idx];
   Uint32 *texels = new Uint32[xyres*xyres];

   ua_onepalette &pal = pals.get_palette(0);

   for(int i=0; i<xyres*xyres; i++)
   {
      texels[i] = *(Uint32*)(pal+indexed[i]);
   }

   // build texture and mipmaps
   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, xyres, xyres, GL_RGBA,
      GL_UNSIGNED_BYTE, texels);

   delete[] texels;

   if (wall)
      wall_texnums[idx] = texname;
   else
      floor_texnums[idx] = texname;
}

void ua_texture_manager::use(bool wall, int idx)
{
   GLuint texname=0;

   if (idx>=0)
      texname = wall ? wall_texnums[idx] : floor_texnums[idx];

   if (last_texname != texname)
   {
      // bind texture to use it
      glBindTexture(GL_TEXTURE_2D,texname);
      last_texname = texname;
   }
}

void ua_texture_manager::delete_textures(bool wall)
{
   int num=0;
   Uint8 **tex;

   if (wall){ num = numwalltex; tex = allwalltex; }
   else { num = numfloortex; tex = allfloortex; }

   for(int i=0; i<num; i++)
      delete[] tex[i];

   delete[] tex;

   if (wall){ numwalltex = 0; allwalltex = NULL; }
   else { numfloortex = 0; allfloortex = NULL; }
}
