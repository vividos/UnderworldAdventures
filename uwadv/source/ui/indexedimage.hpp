//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file indexedimage.hpp
/// \brief image loading and handling, image manager
//
#pragma once

#include <vector>
#include <memory>
#include "base.hpp"
#include "settings.hpp"
#include "palette256.hpp"

/// image loading type
enum ua_image_type
{
   ua_img_gr = 0, ///< image from *.gr files
   ua_img_byt,    ///< image from *.byt files
};


/// single image
class IndexedImage
{
   //friend class ua_image_list;
public:
   /// ctor
   IndexedImage();
   /// dtor
   ~IndexedImage() {}

   /// returns image x resolution
   unsigned int get_xres() const
   {
      return xres;
   }

   /// returns image y resolution
   unsigned int get_yres() const
   {
      return yres;
   }

   // image manipulation

   /// creates a new bitmap
   void create(unsigned int width, unsigned int height);

   /// copies a rectangular area to the given image
//   void copy_rect(IndexedImage& to_img, unsigned int startx, unsigned int starty,
//      unsigned int width, unsigned int height);

   /// pastes a rectangular area from given image
   void paste_rect(const IndexedImage& from_img, unsigned int fromx, unsigned int fromy,
      unsigned width, unsigned height, unsigned int destx, unsigned int desty,
      bool transparent = false);

   /// pastes whole given image to position
   void paste_image(const IndexedImage& from_img, unsigned int destx,
      unsigned int desty, bool transparent = false)
   {
      paste_rect(from_img, 0, 0, from_img.get_xres(), from_img.get_yres(),
         destx, desty, transparent);
   }

   /// fills a specific rectangle with a color
   void fill_rect(unsigned int startx, unsigned int starty,
      unsigned int width, unsigned int height, unsigned int color);

   /// clears bitmap with one palette index
   void clear(Uint8 index = 0);


   /// returns the pixel vector
   std::vector<Uint8>& get_pixels() { return pixels; }

   /// returns a const pixel vector
   const std::vector<Uint8>& get_pixels() const { return pixels; }

   /// sets palette
   void set_palette(Palette256Ptr pal) { palette = pal; }

   /// returns palette
   Palette256Ptr get_palette() { return palette; };

   /// allocates memory for a new palette
   void create_new_palette();

   /// clones palette to get a freely modifiable one
   void clone_palette();

protected:
   /// image resolution
   unsigned int xres, yres;

   /// pixels
   std::vector<Uint8> pixels;

   /// smart pointer to palette to use
   Palette256Ptr palette;
};


/// image manager class
class ImageManager
{
public:
   /// ctor
   ImageManager() {}

   /// inits image manager
   void init(Base::Settings& settings);

   /// loads a single image
   void load(IndexedImage& img, const char* basename, unsigned int imgnum = 0,
      unsigned int palette = 0, ua_image_type type = ua_img_gr);

   /// loads an array of images; loads from "img_from" to (excluded) "img_to"
   void load_list(std::vector<IndexedImage>& imgs, const char* basename,
      unsigned int img_from = 0, unsigned int img_to = 0,
      unsigned int palette = 0);

   /// returns ptr to palette
   Palette256Ptr get_palette(unsigned int pal)
   {
      return allpalettes[pal >= 8 ? 0 : pal];
   }

protected:
   /// path to uw folder
   std::string uw_path;

   /// smart ptr to all all palettes
   Palette256Ptr allpalettes[8];

   /// auxiliary palettes for 4-bit images
   Uint8 allauxpals[32][16];
};
