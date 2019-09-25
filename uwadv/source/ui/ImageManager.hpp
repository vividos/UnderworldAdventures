//
// Underworld Adventures - an Ultima Underworld remake project
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
/// \file ImageManager.hpp
/// \brief image manager
//
#pragma once

#include "IndexedImage.hpp"
#include "Palette256.hpp"

/// image manager class
class ImageManager
{
public:
   /// ctor
   ImageManager(Base::ResourceManager& resourceManager)
      :m_resourceManager(resourceManager)
   {
   }

   /// inits image manager
   void Init();

   /// loads a single image
   void Load(IndexedImage& image, const char* basename, unsigned int imageNumber = 0,
      unsigned int palette = 0, ImageType type = imageGr);

   /// loads an array of images; loads from "imageFrom" to (excluded) "imageTo"
   void LoadList(std::vector<IndexedImage>& imageList, const char* basename,
      unsigned int imageFrom = 0, unsigned int imageTo = 0,
      unsigned int palette = 0);

   /// returns ptr to palette
   Palette256Ptr GetPalette(unsigned int paletteIndex)
   {
      return m_allPalettes[paletteIndex >= 8 ? 0 : paletteIndex];
   }

protected:
   /// resource manager to use for loading
   Base::ResourceManager& m_resourceManager;

   /// smart ptr to all all palettes
   Palette256Ptr m_allPalettes[8];

   /// auxiliary palettes for 4-bit images
   Uint8 m_allAuxPalettes[32][16];
};
