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
/*! \file image.hpp

   \brief image loading and handling, image manager

*/

// include guard
#ifndef uwadv_image_hpp_
#define uwadv_image_hpp_

// needed includes
#include <vector>
#include "settings.hpp"


// typedefs

//! a GL_RGBA compatible palette
typedef Uint8 ua_palette256[256][4];


// enums

//! image loading type
enum ua_image_type
{
   ua_img_gr = 0, //!< image from *.gr files
   ua_img_byt,    //!< image from *.byt files
};


// classes

//! single image
class ua_image
{
//   friend class ua_image_list;
public:
   //! ctor
   ua_image();
   //! dtor
   ~ua_image(){}

   //! returns image x resolution
   unsigned int get_xres() const;
   //! returns image y resolution
   unsigned int get_yres() const;

   // image manipulation

   //! creates a new bitmap
   void create(unsigned int width, unsigned int height);

   //! copies a rectangular area to the given image
//   void copy_rect(ua_image& to_img, unsigned int startx, unsigned int starty,
//      unsigned int width, unsigned int height);

   //! pastes a rectangular area from given image
   void paste_rect(const ua_image& from_img, unsigned int fromx, unsigned int fromy,
      unsigned width, unsigned height, unsigned int destx,unsigned int desty,
      bool transparent=false);

   //! fills a specific rectangle with a color
   void fill_rect(unsigned int startx, unsigned int starty,
      unsigned int width, unsigned int height, unsigned int color);

   //! clears bitmap with one palette index
   void clear(Uint8 index=0);


   //! returns the pixel vector
   std::vector<Uint8>& get_pixels(){ return pixels; }

   //! returns a const pixel vector
   const std::vector<Uint8>& get_pixels() const { return pixels; }

   //! returns palette
   ua_smart_ptr<ua_palette256>& get_palette(){ return palette; };

protected:
   //! image resolution
   unsigned int xres,yres;

   //! pixels
   std::vector<Uint8> pixels;

   //! smart pointer to palette to use
   ua_smart_ptr<ua_palette256> palette;

//   friend class ua_image_manager;
};


//! image manager class
class ua_image_manager
{
public:
   //! ctor
   ua_image_manager(){}

   //! inits image manager
   void init(ua_settings& settings);

   //! loads a single image
   void load(ua_image& img, const char* basename, unsigned int imgnum = 0,
      unsigned int palette = 0, ua_image_type type = ua_img_gr);

   //! loads an array of images
   void load_list(std::vector<ua_image> imgs, const char* basename,
      unsigned int img_from = 0, unsigned int img_to = 0,
      unsigned int palette = 0);

   //! returns ptr to palette
   ua_smart_ptr<ua_palette256> get_palette(unsigned int pal);

protected:
   //! path to uw folder
   std::string uw_path;

   //! smart ptr to all all palettes
   ua_smart_ptr<ua_palette256> allpalettes[8];

   //! auxiliary palettes for 4-bit images
   Uint8 allauxpals[32][16];
};


// inline methods

inline unsigned int ua_image::get_xres() const
{
   return xres;
}

inline unsigned int ua_image::get_yres() const
{
   return yres;
}

inline ua_smart_ptr<ua_palette256> ua_image_manager::get_palette(
   unsigned int pal)
{
   return allpalettes[pal >= 8 ? 0 : pal];
}


#endif
