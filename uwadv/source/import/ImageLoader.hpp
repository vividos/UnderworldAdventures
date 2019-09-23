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
/// \file ImageLoader.hpp
/// \brief image loader
//
#pragma once

#include "IndexedImage.hpp"

namespace Base
{
   class Settings;
   class ResourceManager;
   class File;
}

class IndexedImage;

namespace Import
{
   /// image loader
   class ImageLoader
   {
   public:
      /// ctor
      ImageLoader(Base::ResourceManager& resourceManager)
         :m_resourceManager(resourceManager)
      {
      }

      /// loads 8 main palettes
      void LoadPalettes(Palette256Ptr allPalettes[8]);

      /// loads all 32 auxiliary palettes with 16 indices each
      void LoadAuxPalettes(Uint8 allAuxPalettes[32][16]);

      /// loads a *.gr image
      void LoadImageGr(IndexedImage& img, const char* imageName, unsigned int imageNumber,
         Uint8 allAuxPalettes[32][16]);

      /// loads a list of images from a *.gr file
      void LoadImageGrList(std::vector<IndexedImage>& imageList,
         const char* imageName, unsigned int imageFrom, unsigned int imageTo,
         Uint8 allAuxPalettes[32][16]);

      /// loads a *.byt image
      void LoadImageByt(const char* imageName, Uint8* pixels);

   private:
      /// loads *.gr image into pixels array
      void LoadImageGrImpl(IndexedImage& image, Base::File& file, Uint8 auxPalettes[32][16],
         bool special_panels);

   private:
      /// resource manager to use for loading
      Base::ResourceManager& m_resourceManager;
   };

} // namespace Import
