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
/*! \file texture.hpp

   a

   stock textures: wall textures start at index 0x0000, floor textures at
   0x0100

*/

// include guard
#ifndef __uwadv_texture_hpp_
#define __uwadv_texture_hpp_

// needed includes
#include <vector>
#include "settings.hpp"
#include "image.hpp"


// constants

const unsigned int ua_tex_stock_wall = 0x0000;
const unsigned int ua_tex_stock_floor = 0x0100;


// typedefs
typedef Uint8 ua_onepalette[256][4];


// classes

//! palette class
class ua_palettes
{
public:
   //! ctor
   ua_palettes(){}

   //! loads main palettes and all auxiliary palette tables
   void load(ua_settings &settings);

   //! returns an auxiliary palette table; type is Uint8 [256][4]
   ua_onepalette &get_palette(Uint8 pal){ return allpals[pal]; }

protected:
   //! all main palettes
   ua_onepalette allpals[8];
};


// forward declaration
class ua_texture_manager;


//! texture class; represents one or more texture images
class ua_texture
{
public:
   //! ctor
   ua_texture(){}

   //! converts image to texture
   void convert(ua_texture_manager &texmgr, ua_image &img);

   //! converts a list of images to textures
   void convert(ua_texture_manager &texmgr, ua_image_list &imglist,
      unsigned int start=0, unsigned int until=-1);

   //! prepares texture for usage
   void prepare(bool mipmaps=false, GLenum min_filt = GL_LINEAR,
      GLenum max_filt = GL_LINEAR);

   //! binds texture to use it in OpenGL
   void use(unsigned int animstep=0);

   //! returns u texture coordinate
   float get_tex_u(){ return u;}

   //! returns v texture coordinate
   float get_tex_v(){ return v; }

protected:
   //! upper left texture coordinates
   float u,v;

   //! x and y resolution of the texture
   unsigned int xres,yres;

   //! number of textures stored in 'texels'
   unsigned int texcount;

   //! texels for all images
   std::vector<Uint32> texels;

   //! texture name(s)
   std::vector<GLuint> texname;
};


//! stock texture
struct ua_stock_texture
{
   ua_stock_texture():texname(0){}

   std::vector<Uint8> pixels;
   GLuint texname;
};


//! texture manager
class ua_texture_manager
{
public:
   //! ctor
   ua_texture_manager();
   //! dtor
   ~ua_texture_manager();

   //! initializes texture manager; loads stock textures
   void init(ua_settings &settings);

   //! resets usage of stock textures in OpenGL
   void reset();

   //! prepares a stock texture for usage in OpenGL
   void prepare(unsigned int idx);

   //! prepares an external texture for usage in OpenGL
   void prepare(ua_texture &tex);

   //! use a stock texture in OpenGL
   void use(unsigned int idx);

   //! returns a specific palette
   ua_onepalette &get_palette(unsigned int pal){ return pals.get_palette(pal); };

private:
   //! loads all palettes
   void load_palettes(const char *allpalname);

   //! loads textures from file
   void load_textures(unsigned int startidx, const char *texfname);

protected:
   //! all palettes
   ua_palettes pals;

   //! last bound texture name
   GLuint last_texname;

   // all stock textures
   std::vector<ua_stock_texture> allstocktex;
};

#endif
