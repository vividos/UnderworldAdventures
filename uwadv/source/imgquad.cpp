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
/*! \file imgquad.cpp

   \brief image quad implementation

*/

// needed includes
#include "common.hpp"
#include "imgquad.hpp"


// ua_image_quad methods

void ua_image_quad::init(ua_texture_manager* texmgr, unsigned int myxpos,
   unsigned int myypos, unsigned int mywidth, unsigned int myheight, bool copy_pal)
{
   xpos = myxpos;
   ypos = myypos;
   mywidth += (mywidth&1); // even width

   // init image (when needed)
   if (ua_image::xres == 0 || ua_image::yres == 0)
      ua_image::create(mywidth,myheight,0,ua_image::palette);

   // init texture
   split_textures = mywidth>254;

   tex.init(texmgr,split_textures ? 2 : 1,
      GL_LINEAR,GL_LINEAR,GL_CLAMP,GL_CLAMP);

   if (copy_pal && texmgr!=NULL)
      memcpy(quadpalette,texmgr->get_palette(ua_image::palette),sizeof(ua_onepalette));
}

void ua_image_quad::convert_upload()
{
   // upload image
   if (split_textures)
   {
      // split text image into two
      ua_image img_split1,img_split2;

      unsigned int texwidth = ua_image::xres/2;
      unsigned int texheight = ua_image::yres;

      // create images
      img_split1.create(texwidth+1,texheight);
      img_split2.create(texwidth+1,texheight);

      // paste contents
      img_split1.paste_rect(*this,0,0, texwidth+1,texheight, 0,0);
      img_split2.paste_rect(*this,texwidth,0, texwidth,texheight, 0,0);

      img_split2.paste_rect(*this,ua_image::xres-1,0, 1,texheight, texwidth,0); // copy border

      // upload it to the texture
      tex.convert(quadpalette,img_split1,0);
      tex.upload(0);

      tex.convert(quadpalette,img_split2,1);
      tex.upload(1);
   }
   else
   {
      // image is small enough
      tex.convert(*this);
      tex.upload(0);
   }
}

void ua_image_quad::render()
{
   double u = tex.get_tex_u(), v = tex.get_tex_v();
   tex.use(0);

   unsigned int quadwidth = split_textures ? ua_image::xres/2 : ua_image::xres+2;
   unsigned int quadheight = ua_image::yres;
   double dx = split_textures ? 0.5/quadwidth : 0.0;

   // render (first) quad
   glBegin(GL_QUADS);
   glTexCoord2d(0.0,  v);   glVertex2i(xpos+0,        200-ypos-quadheight);
   glTexCoord2d(u-dx, v);   glVertex2i(xpos+quadwidth,200-ypos-quadheight);
   glTexCoord2d(u-dx, 0.0); glVertex2i(xpos+quadwidth,200-ypos);
   glTexCoord2d(0.0,  0.0); glVertex2i(xpos+0,        200-ypos);
   glEnd();

   if (split_textures)
   {
      tex.use(1);

      // render second quad
      glBegin(GL_QUADS);
      glTexCoord2d(0.0, v  ); glVertex2i(xpos+quadwidth,  200-ypos-quadheight);
      glTexCoord2d(u  , v  ); glVertex2i(xpos+quadwidth*2,200-ypos-quadheight);
      glTexCoord2d(u  , 0.0); glVertex2i(xpos+quadwidth*2,200-ypos);
      glTexCoord2d(0.0, 0.0); glVertex2i(xpos+quadwidth,  200-ypos);
      glEnd();
   }
}

void ua_image_quad::done()
{
   tex.done();
}
