//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2003,2004,2019 Underworld Adventures Team
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
/// \file importgfx.hpp
/// \brief user interface import functions
//
#pragma once

#include "indexedimage.hpp"

namespace Base
{
   class Settings;
   class File;
}

class IndexedImage;

namespace Import
{
   class ImageLoader
   {
   public:
      /// ctor
      ImageLoader() {}

      /// loads 8 main palettes
      void LoadPalettes(const char* allpalname, Palette256Ptr allpalettes[8]);

      /// loads all 32 auxiliary palettes with 16 indices each
      void LoadAuxPalettes(const char* auxpalname, Uint8 allauxpals[32][16]);

      /// loads a *.gr image
      void LoadImageGr(IndexedImage& img, const char* imgname, unsigned int imgnum,
         Uint8 auxpalettes[32][16]);

      /// loads a list of image from a *.gr file
      void LoadImageGrList(std::vector<IndexedImage>& imglist,
         const char* imgname, unsigned int img_from, unsigned int img_to,
         Uint8 auxpalettes[32][16]);

      /// loads a *.byt image
      void LoadImageByt(const char* imgname, Uint8* pixels);

   private:
      /// loads *.gr image into pixels array
      void LoadImageGrImpl(IndexedImage& image, Base::File& file, Uint8 auxpalidx[32][16],
         bool special_panels);
   };

} // namespace Import
