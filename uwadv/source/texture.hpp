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
/*! \file texture.hpp

   \brief texture management classes

   ua_texture can be used in several ways. either an image is converted to
   texture once and used from there on, or a texture is converted and uploaded
   for every rendered frame (e.g. for animations).

   ua_texture::init() allocates texture names from OpenGL
   ua_texture::convert() converts image pixels to 32-bit texture data
   ua_texture::use() activates a texture name for use in rendering
   ua_texture::upload() uploads a converted texture to the graphics card
   ua_texture::done() deletes the used texture names

   note: texture size for multiple textures is calculated when convert()ing
   the first texture. all other textures must have the same size.

*/

// include guard
#ifndef uwadv_texture_hpp_
#define uwadv_texture_hpp_

// needed includes
#include <vector>
#include "settings.hpp"
#include "image.hpp"


// constants

//! start of stock wall textures
const unsigned int ua_tex_stock_wall = 0x0000;
//! start of stock floor textures
const unsigned int ua_tex_stock_floor = 0x0100;
//! switch/lever/pull chain textures
const unsigned int ua_tex_stock_switches = 0x0200;
//! door textures
const unsigned int ua_tex_stock_door = 0x0210;
//! tmobj textures
const unsigned int ua_tex_stock_tmobj = 0x0220;


// typedefs

//! a GL_RGBA compatible palette
typedef Uint8 ua_onepalette[256][4];


// forward declaration
class ua_texture_manager;


// classes

//! texture class; represents one or more texture images
class ua_texture
{
public:
   //! ctor
   ua_texture():texmgr(NULL){}

   //! allocates and initializes OpenGL texture object
   void init(ua_texture_manager* texmgr, unsigned int numtex=1,
      GLenum min_filt=GL_LINEAR, GLenum max_filt=GL_LINEAR,
      GLenum wrap_s=GL_CLAMP_TO_EDGE, GLenum wrap_t=GL_CLAMP_TO_EDGE);

   //! converts image to texture
   void convert(ua_image& img, unsigned int numtex=0);

   //! converts image with custom palette to texture
   void convert(ua_onepalette& pal, ua_image& img, unsigned int numtex=0);

   //! convert image pixels to texture
   void convert(Uint8* pix, unsigned int origx, unsigned int origy,
      ua_onepalette& pal, unsigned int numtex);

   //! convert 32-bit RGBA values to texture
   void convert(unsigned int xres, unsigned int yres, Uint32* pix,
      unsigned int numtex=0);

   //! loads texture from (seekable) rwops stream
   void load(SDL_RWops* rwops);

   //! uses texture in OpenGL
   void use(unsigned int numtex=0);

   //! uploads a converted texture to OpenGL
   void upload(unsigned int numtex=0, bool mipmaps=false);

   //! returns u texture coordinate
   double get_tex_u() const;

   //! returns v texture coordinate
   double get_tex_v() const;

   //! cleans up texture name(s) after usage
   void done();

   // raw texture access

   //! returns array of texels
   const Uint32* get_texels(unsigned int numtex=0);

   //! returns x resolution
   unsigned int get_xres() const;

   //! returns y resolution
   unsigned int get_yres() const;

protected:
   //! pointer to texture manager, or NULL if none available
   ua_texture_manager* texmgr;

   //! upper left texture coordinates
   double u,v;

   //! x and y resolution of the texture
   unsigned int xres,yres;

   //! texture pixels for all images
   std::vector<Uint32> texels;

   //! texture name(s)
   std::vector<GLuint> texname;

   //! texture min./max. filter parameter
   GLenum min_filt, max_filt;

   //! texture wrapping parameter
   GLenum wrap_s, wrap_t;
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
   void init(ua_settings& settings);

   //! called every game tick
   void tick(double ticktime);

   //! resets usage of stock textures in OpenGL
   void reset();

   //! prepares a stock texture for usage in OpenGL
   void prepare(unsigned int idx);

   //! prepares an external texture for usage in OpenGL
   void prepare(ua_texture& tex);

   //! use a stock texture in OpenGL
   void use(unsigned int idx);

   //! prepares texture for object drawing
   void object_tex(Uint16 id,double& u1,double& v1,double& u2,double& v2);

   //! returns a specific palette
   ua_onepalette &get_palette(unsigned int pal);

   //! should be called when a new texname is about to be used
   bool using_new_texname(GLuint new_texname);

   //! converts stock texture to external one
   void stock_to_external(unsigned int idx, ua_texture& tex);

private:
   //! loads textures from file
   void load_textures(unsigned int startidx, const char* texfname);

   //! loads textures from image list
   void load_imgtextures(unsigned int startidx, ua_image_list& il);

   //! loads all palettes
   void load_palettes(const char* allpalname);

protected:
   //! last bound texture name
   GLuint last_texname;

   //! image list of all stock textures
   ua_image_list allstocktex_imgs;

   //! stock textures
   std::vector<ua_texture> stock_textures;

   //! infos about current/max texture frame for all stock textures
   std::vector<std::pair<unsigned int, unsigned int> > stock_animinfo;

   //! object sprite textures
   ua_texture obj_textures;

   //! all main palettes
   ua_onepalette allpals[8];

   //! time counter for animated textures
   double animcount;
};


// inline methods

inline double ua_texture::get_tex_u() const
{
   return u;
}

inline double ua_texture::get_tex_v() const
{
   return v;
}

inline unsigned int ua_texture::get_xres() const
{
   return xres;
}

inline unsigned int ua_texture::get_yres() const
{
   return yres;
}

inline ua_onepalette& ua_texture_manager::get_palette(unsigned int pal)
{
   return allpals[pal];
};


#endif
