/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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
#include "game_interface.hpp"
#include "importgfx.hpp"


// constants

const double ua_texture_manager::anim_fps = 1.5;


// ua_texture methods

ua_texture::ua_texture()
:xres(0), yres(0), u(0.0), v(0.0)
{
}

/*! Initializes texture and allocates OpenGL texture names. Previous textures
    are cleaned up with a call to done(). A pointer to a texture manager can
    be set to let the texture be managed from it.

    \param numtex number of texture images to allocate
*/
void ua_texture::init(unsigned int numtex)
{
   done();

   // create texture names
   texname.resize(numtex,0);
   if (numtex>0)
      glGenTextures(numtex,&texname[0]);
}

/*! Frees texture pixels and OpenGL texture names. */
void ua_texture::done()
{
   texels.clear();

   // delete all texture names
   if (texname.size()>0)
      glDeleteTextures(texname.size(),&texname[0]);
   texname.clear();
}

/*! Converts image data to texture.

    \param img image to convert to texture
    \param numtex index of texture to use for conversion
*/
void ua_texture::convert(ua_image& img,unsigned int numtex)
{
   convert(&img.get_pixels()[0], img.get_xres(), img.get_yres(),
      *img.get_palette(), numtex);
}

/*! Converts palette-indexed image data to texture. The resulting texture has
    a resolution of 2^n x 2^m. The texture resolution is determined when the
    first image is converted. There currently is a resolution limit of 2048 x
    2048.

    \param pix array with index values to convert
    \param origx x resolution of image stored in pix
    \param origy y resolution of image stored in pix
    \param pal 256 color palette to use in conversion.
    \param numtex number of texture image to convert to
*/
void ua_texture::convert(Uint8* pix, unsigned int origx, unsigned int origy,
   ua_palette256& pal, unsigned int numtex)
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
   Uint32* palptr = reinterpret_cast<Uint32*>(pal.get());
   Uint32* texptr = &texels[numtex*xres*yres];

   for(unsigned int y=0; y<origy; y++)
   {
      Uint32* texptr2 = &texptr[y*xres];
      for(unsigned int x=0; x<origx; x++)
         *texptr2++ = palptr[pix[y*origx+x]];
   }
}

/*! Converts 32-bit truecolor pixel data in GL_RGBA format to texture. Be sure
    to only convert images with exactly the same size with this method.

    \param origx x resolution of pixel data in pix
    \param origy y resolution of pixel data in pix
    \param pix 32-bit pixel data in GL_RGBA format (red byte, green byte, ...)
    \param numtex index of texture to convert
*/
void ua_texture::convert(unsigned int origx, unsigned int origy, Uint32* pix,
   unsigned int numtex)
{
   // determine texture resolution (must be 2^n)
   xres = 2;
   while(xres<origx && xres<2048) xres<<=1;

   yres = 2;
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

/*! Use the texture in OpenGL. All further triangle, quad, etc. definitions
    use the texture, until another texture is used or
    glBindTexture(GL_TEXTURE_2D, 0) is called.

    \param numtex index of texture to use
*/
void ua_texture::use(unsigned int numtex)
{
   if (numtex >= texname.size())
      return; // invalid texture index

   glBindTexture(GL_TEXTURE_2D,texname[numtex]);
}

/*! Uploads the convert()ed texture to the graphics card. The texture doesn't
    have to be use()d before uploading.

    \param numtex index of texture to upload
    \param mipmaps generates mipmaps when true
*/
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

   // check for errors
   GLenum error = glGetError();
   if (error != GL_NO_ERROR)
      ua_trace("ua_texture: error during uploading texture! (%u)\n",error);
}

/*! Returns 32-bit texture pixels in GL_RGBA format for specified texture.

    \param numtex index of texture to return texels
*/
const Uint32* ua_texture::get_texels(unsigned int numtex) const
{
   return &texels[numtex*xres*yres];
}


// ua_texture_manager methods

ua_texture_manager::ua_texture_manager()
:animcount(0.0)
{
}

ua_texture_manager::~ua_texture_manager()
{
   reset();
}

/*! Initializes texture manager. All stock textures are loaded and animation
    infos are generated for animated textures.

    \param game game interface
*/
void ua_texture_manager::init(ua_game_interface& game)
{
   palette0 = game.get_image_manager().get_palette(0);

   ua_settings& settings = game.get_settings();

   ua_uw_import_gfx import;

   // load stock textures
   if (settings.get_gametype() == ua_game_uw1)
   {
      // load all wall textures
      std::string walltexfname(settings.get_string(ua_setting_uw_path));
      walltexfname.append(
         settings.get_bool(ua_setting_uw1_is_uw_demo) ? "data/dw64.tr" : "data/w64.tr");

      import.load_textures(allstocktex_imgs, ua_tex_stock_wall,
         walltexfname.c_str(), palette0);

      // load all floor textures
      std::string floortexfname(settings.get_string(ua_setting_uw_path));
      floortexfname.append(
         settings.get_bool(ua_setting_uw1_is_uw_demo) ? "data/df32.tr" : "data/f32.tr");

      import.load_textures(allstocktex_imgs, ua_tex_stock_floor,
         floortexfname.c_str(), palette0);

      // load objects
      {
         allstocktex_imgs.resize(ua_tex_stock_objects);

         // add images to list; we can do this, since the list isn't clear()ed
         // before adding more images
         game.get_image_manager().load_list(allstocktex_imgs,"objects");
      }

      // load switches/levers/pull chains
      {
         allstocktex_imgs.resize(ua_tex_stock_switches);
         game.get_image_manager().load_list(allstocktex_imgs,"tmflat");
      }

      // load door textures
      {
         allstocktex_imgs.resize(ua_tex_stock_door);
         game.get_image_manager().load_list(allstocktex_imgs,"doors");
      }

      // load tmobj textures
      {
         allstocktex_imgs.resize(ua_tex_stock_tmobj);
         game.get_image_manager().load_list(allstocktex_imgs,"tmobj");
      }

      stock_animinfo.resize(allstocktex_imgs.size(),
         std::make_pair<unsigned int, unsigned int>(0,1));

      // set some animated textures
      {
         stock_animinfo[0x00ce].second = 8; // 206 lavafall
         stock_animinfo[0x0129].second = 8; // 469 rivulets of lava
         stock_animinfo[0x0117].second = 8; // 487 rivulets of lava
         stock_animinfo[0x0118].second = 8; // 486 lava
         stock_animinfo[0x0119].second = 8; // 485 lava

         stock_animinfo[0x0120].second = 4; // 478 water
         stock_animinfo[0x0121].second = 4; // 477 water
         stock_animinfo[0x0122].second = 4; // 476 water
         stock_animinfo[0x0110].second = 4; // 493 water
         stock_animinfo[0x0111].second = 4; // 494 water
      }
   }
   else
   if (settings.get_gametype() == ua_game_uw2)
   {
      // load all textures
      std::string texfname(settings.get_string(ua_setting_uw_path));
      texfname.append("data/t64.tr");

      import.load_textures(allstocktex_imgs, ua_tex_stock_wall,
         texfname.c_str(), palette0);

      stock_animinfo.resize(allstocktex_imgs.size(),
         std::make_pair<unsigned int, unsigned int>(0,1));
   }

   // now that all texture images are loaded, we can resize the texture array
   stock_textures.resize(allstocktex_imgs.size());

   // init stock texture objects
   reset();
}

/*! Does all tick processing for textures. Animates animated textures.

    \param tickrate tick rate in ticks/second
*/
void ua_texture_manager::tick(double tickrate)
{
   animcount += 1.0/tickrate;

   if (animcount > 1.0/anim_fps)
   {
      animcount -= 1.0/anim_fps;

      // next animation frame
      unsigned int max = stock_animinfo.size();
      for(unsigned int i=0; i<max; i++)
      {
         std::pair<unsigned int, unsigned int>& info = stock_animinfo[i];

         if (info.second > 1)
         {
            // this one has animation
            if ((++info.first) >= info.second)
               info.first = 0;
         }
      }
   }
}

/*! Resets all stock textures. */
void ua_texture_manager::reset()
{
   glBindTexture(GL_TEXTURE_2D,0);

   // call "done" for all stock textures
   unsigned int max = stock_textures.size();
   for(unsigned int i=0; i<max; i++)
      stock_textures[i].done();

   last_texname = 0;
}

/*! Prepares a stock texture for use in OpenGL.

    \param idx index of stock texture to prepare
*/
void ua_texture_manager::prepare(unsigned int idx)
{
   if (idx>=allstocktex_imgs.size())
      return; // not a valid index

   unsigned int pal_max = stock_animinfo[idx].second;
   if (pal_max<1)
      return; // not an available texture

   stock_textures[idx].init(pal_max);

   if (pal_max==1)
   {
      // only allow mipmaps for non-object images
      bool mipmap = (idx < ua_tex_stock_objects) || (idx > ua_tex_stock_objects+0x0200);

      // unanimated texture
      // convert to texture object
      stock_textures[idx].convert(allstocktex_imgs[idx],0);
      stock_textures[idx].upload(0,mipmap); // upload texture with mipmaps
   }
   else
   {
      ua_palette256 pal = *palette0;

      unsigned int xres = allstocktex_imgs[idx].get_xres();
      unsigned int yres = allstocktex_imgs[idx].get_xres();
      Uint8* pixels = &allstocktex_imgs[idx].get_pixels()[0];

      // animated texture
      if (pal_max == 8)
      {
         // lava texture: indices 16 through 23
         for(unsigned int i=0; i<8; i++)
         {
            stock_textures[idx].convert(pixels, xres, yres, pal, i);
            stock_textures[idx].upload(i,true); // upload texture with mipmaps

            // rotate entries
            pal.rotate(16,8,false);
         }
      }
      else
      if (pal_max == 4)
      {
         // water texture: indices 48 through 51
         for(unsigned int i=0; i<4; i++)
         {
            stock_textures[idx].convert(pixels, xres, yres, pal, i);
            stock_textures[idx].upload(i,true); // upload texture with mipmaps

            // rotate entries
            pal.rotate(48,4,true);
         }
      }
   }
}

/*! Uses a stock texture.

    \param idx index of stock texture to use
*/
void ua_texture_manager::use(unsigned int idx)
{
   if (idx>=stock_textures.size())
      return; // not a valid index

   stock_textures[idx].use(stock_animinfo[idx].first);
}

/*! Uses a new texture name. Returns false when the texture is already in use.

    \param new_texname new texture name to use
*/
bool ua_texture_manager::using_new_texname(GLuint new_texname)
{
   if (last_texname == new_texname)
      return false; // no need to use texname again

   last_texname = new_texname;
   return true;
}

/*! Converts a stock texture to an external one.

    \param idx index of stock texture to convert
    \param tex texture object to convert to
*/
void ua_texture_manager::stock_to_external(unsigned int idx, ua_texture& tex)
{
   tex.convert(allstocktex_imgs[idx],0);
}
