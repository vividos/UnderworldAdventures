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
#include "game_interface.hpp"
#include "import.hpp"


// constants

const double ua_texture_anim_fps = 1.5;


// ua_texture methods

ua_texture::ua_texture()
:texmgr(NULL), xres(0), yres(0), u(0.0), v(0.0)
{
}

void ua_texture::init(ua_texture_manager* the_texmgr,unsigned int numtex)
{
   done();

   texmgr = the_texmgr;

   // create texture names
   texname.resize(numtex,0);
   if (numtex>0)
      glGenTextures(numtex,&texname[0]);
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

void ua_texture::convert(ua_image& img,unsigned int numtex)
{
   convert(&img.get_pixels()[0], img.get_xres(), img.get_yres(),
      *img.get_palette(), numtex);
}

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

void ua_texture::use(unsigned int numtex)
{
   if (numtex>=texname.size())
      return; // invalid texture index

   // invalidates currently used texture
   if (texmgr == NULL || texmgr->using_new_texname(texname[numtex]))
      glBindTexture(GL_TEXTURE_2D,texname[numtex]);
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

void ua_texture_manager::init(ua_game_interface& game)
{
   palette0 = game.get_image_manager().get_palette(0);

   ua_settings& settings = game.get_settings();

   ua_uw_import import;

   // load stock textures
   if (settings.get_gametype() == ua_game_uw1 || settings.get_gametype() == ua_game_uw_demo)
   {
      // load all wall textures
      std::string walltexfname(settings.get_string(ua_setting_uw_path));
      walltexfname.append(
         settings.get_gametype() == ua_game_uw1 ? "data/w64.tr" : "data/dw64.tr");

      import.load_textures(allstocktex_imgs, ua_tex_stock_wall,
         walltexfname.c_str(), palette0);

      // load all floor textures
      std::string floortexfname(settings.get_string(ua_setting_uw_path));
      floortexfname.append(
         settings.get_gametype() == ua_game_uw1 ? "data/f32.tr" : "data/df32.tr");

      import.load_textures(allstocktex_imgs, ua_tex_stock_floor,
         walltexfname.c_str(), palette0);

      // load objects
      {
         allstocktex_imgs.resize(ua_tex_stock_objects-1);

         // add images to list; we can do this, since the list isn't clear()ed
         // before adding more images
         game.get_image_manager().load_list(allstocktex_imgs,"objects");
      }

      // load switches/levers/pull chains
      {
         allstocktex_imgs.resize(ua_tex_stock_switches-1);
         game.get_image_manager().load_list(allstocktex_imgs,"tmflat");
      }

      // load door textures
      {
         allstocktex_imgs.resize(ua_tex_stock_door-1);
         game.get_image_manager().load_list(allstocktex_imgs,"doors");
      }

      // load tmobj textures
      {
         allstocktex_imgs.resize(ua_tex_stock_tmobj-1);
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

void ua_texture_manager::tick(double tickrate)
{
   animcount += tickrate;

   if (animcount > 1.0/ua_texture_anim_fps)
   {
      animcount -= 1.0/ua_texture_anim_fps;

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

void ua_texture_manager::reset()
{
   glBindTexture(GL_TEXTURE_2D,0);

   // call "done" for all stock textures
   unsigned int max = stock_textures.size();
   for(unsigned int i=0; i<max; i++)
      stock_textures[i].done();

   last_texname = 0;
}

//! rotates palette indices
void ua_palette_rotate(ua_palette256& pal,Uint8 start, Uint8 len, bool forward)
{
   Uint8 save[4];
   if (forward)
   {
      memcpy(save,&pal[start],sizeof(pal[0]));
      memmove(&pal[start],&pal[start+1],(len-1)*sizeof(pal[0]));
      memcpy(pal[start+len-1],save,sizeof(pal[0]));
   }
   else
   {
      memcpy(save,&pal[start+len-1],sizeof(pal[0]));
      memmove(&pal[start+1],&pal[start],(len-1)*sizeof(pal[0]));
      memcpy(pal[start],save,sizeof(pal[0]));
   }
}

void ua_texture_manager::prepare(unsigned int idx)
{
   if (idx>=allstocktex_imgs.size())
      return; // not a valid index

   unsigned int pal_max = stock_animinfo[idx].second;
   if (pal_max<1)
      return; // not an available texture

   stock_textures[idx].init(this,pal_max);

   if (pal_max==1)
   {
      // unanimated texture
      // convert to texture object
      stock_textures[idx].convert(allstocktex_imgs[idx],0);
      stock_textures[idx].upload(0,true); // upload texture with mipmaps
   }
   else
   {
      ua_palette256 pal;
      memcpy(pal,*palette0,sizeof(ua_palette256));

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
            ua_palette_rotate(pal,16,8,false);
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
            ua_palette_rotate(pal,48,4,true);
         }
      }
   }
}

void ua_texture_manager::use(unsigned int idx)
{
   if (idx>=stock_textures.size())
      return; // not a valid index

   stock_textures[idx].use(stock_animinfo[idx].first);
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
   tex.convert(allstocktex_imgs[idx],0);
}
