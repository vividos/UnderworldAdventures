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
/// \file IndexedImage.hpp
/// \brief image loading and handling
//
#pragma once

#include <vector>
#include <memory>
#include "Base.hpp"
#include "Palette256.hpp"

namespace Base
{
   class ResourceManager;
}

/// image loading type
enum ImageType
{
   imageGr = 0, ///< image from *.gr files
   imageByt,    ///< image from *.byt files
};

/// single image
class IndexedImage
{
public:
   /// ctor
   IndexedImage();
   /// dtor
   ~IndexedImage() {}

   /// returns image x resolution
   unsigned int GetXRes() const
   {
      return m_xres;
   }

   /// returns image y resolution
   unsigned int GetYRes() const
   {
      return m_yres;
   }

   // image manipulation

   /// creates a new bitmap
   void Create(unsigned int width, unsigned int height);

   /// pastes a rectangular area from given image
   void PasteRect(const IndexedImage& from_img, unsigned int fromx, unsigned int fromy,
      unsigned width, unsigned height, unsigned int destx, unsigned int desty,
      bool transparent = false);

   /// pastes whole given image to position
   void PasteImage(const IndexedImage& from_img, unsigned int destx,
      unsigned int desty, bool transparent = false)
   {
      PasteRect(from_img, 0, 0, from_img.GetXRes(), from_img.GetYRes(),
         destx, desty, transparent);
   }

   /// fills a specific rectangle with a color
   void FillRect(unsigned int startx, unsigned int starty,
      unsigned int width, unsigned int height, unsigned int color);

   /// clears bitmap with one palette index
   void Clear(Uint8 index = 0);


   /// returns the pixel vector
   std::vector<Uint8>& GetPixels() { return m_pixels; }

   /// returns a const pixel vector
   const std::vector<Uint8>& GetPixels() const { return m_pixels; }

   /// sets palette
   void SetPalette(Palette256Ptr palette) { m_palette = palette; }

   /// returns palette
   Palette256Ptr GetPalette() { return m_palette; };

   /// allocates memory for a new palette
   void CreateNewPalette();

   /// clones palette to get a freely modifiable one
   void ClonePalette();

protected:
   /// image resolution
   unsigned int m_xres, m_yres;

   /// pixels
   std::vector<Uint8> m_pixels;

   /// smart pointer to palette to use
   Palette256Ptr m_palette;
};
