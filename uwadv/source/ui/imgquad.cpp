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
#include "renderer.hpp"


// ua_image_quad methods

void ua_image_quad::init(ua_game_interface& game, unsigned int xpos,
   unsigned int ypos)
{
   ua_window::create(xpos,ypos,0,0);

   tex.init(&game.get_renderer().get_texture_manager(), 2);
}

void ua_image_quad::update()
{
   wnd_width = image.get_xres();
   wnd_height = image.get_yres();

   wnd_width += (wnd_width&1); // even width

   split_textures = wnd_width > 254;

   // upload image
   if (split_textures)
   {
      // split text image into two
      ua_image img_split1,img_split2;

      unsigned int texwidth = image.get_xres()/2;
      unsigned int texheight = image.get_yres();

      // create images
      img_split1.create(texwidth+1,texheight);
      img_split2.create(texwidth+1,texheight);

      img_split1.get_palette() = image.get_palette();
      img_split2.get_palette() = image.get_palette();

      // paste contents
      img_split1.paste_rect(image,0,0, texwidth+1,texheight, 0,0);
      img_split2.paste_rect(image,texwidth,0, texwidth,texheight, 0,0);

      img_split2.paste_rect(image,image.get_xres()-1,0, 1,texheight, texwidth,0); // copy border

      // upload it to the texture
      tex.convert(img_split1,0);
      tex.upload(0);

      tex.convert(img_split2,1);
      tex.upload(1);
   }
   else
   {
      // image is small enough
      tex.convert(image);
      tex.upload(0);
   }
}

void ua_image_quad::destroy()
{
   tex.done();
}

void ua_image_quad::draw()
{
   double u = tex.get_tex_u(), v = tex.get_tex_v();
   tex.use(0);

   unsigned int quadwidth = split_textures ? wnd_width/2 : wnd_width+2;
   unsigned int quadheight = wnd_height;
   double dx = split_textures ? 0.5/quadwidth : 0.0;

   // render (first) quad
   glBegin(GL_QUADS);
   glTexCoord2d(0.0,  v);   glVertex2i(wnd_xpos+0,        200-wnd_ypos-quadheight);
   glTexCoord2d(u-dx, v);   glVertex2i(wnd_xpos+quadwidth,200-wnd_ypos-quadheight);
   glTexCoord2d(u-dx, 0.0); glVertex2i(wnd_xpos+quadwidth,200-wnd_ypos);
   glTexCoord2d(0.0,  0.0); glVertex2i(wnd_xpos+0,        200-wnd_ypos);
   glEnd();

   if (split_textures)
   {
      tex.use(1);

      // render second quad
      glBegin(GL_QUADS);
      glTexCoord2d(0.0, v  ); glVertex2i(wnd_xpos+quadwidth,  200-wnd_ypos-quadheight);
      glTexCoord2d(u  , v  ); glVertex2i(wnd_xpos+quadwidth*2,200-wnd_ypos-quadheight);
      glTexCoord2d(u  , 0.0); glVertex2i(wnd_xpos+quadwidth*2,200-wnd_ypos);
      glTexCoord2d(0.0, 0.0); glVertex2i(wnd_xpos+quadwidth,  200-wnd_ypos);
      glEnd();
   }
}
