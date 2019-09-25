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
/// \file IndexedImage.cpp
/// \brief indexed image creation and manipulation
//
#include "pch.hpp"
#include "ImageManager.hpp"
#include "ImageLoader.hpp"

void ImageManager::Init()
{
   Import::ImageLoader loader{ m_resourceManager };
   loader.LoadPalettes(m_allPalettes);
   loader.LoadAuxPalettes(m_allAuxPalettes);
}

/// when loading *.gr images, just specify the filename without the .gr and
/// without path; for *.byt images specify the complete relative path to the
/// file;
void ImageManager::Load(IndexedImage& image, const char* basename, unsigned int imgnum,
   unsigned int paletteIndex, ImageType type)
{
   switch (type)
   {
   case imageGr:
   {
      std::string filename("data/");
      filename.append(basename);
      filename.append(".gr");

      Import::ImageLoader loader{ m_resourceManager };
      loader.LoadImageGr(image, filename.c_str(), imgnum, m_allAuxPalettes);
   }
   break;

   case imageByt:
   {
      image.Create(320, 200);

      Import::ImageLoader loader{ m_resourceManager };
      loader.LoadImageByt(basename, &image.GetPixels()[0]);
   }
   break;

   default:
      break;
   }

   image.SetPalette(m_allPalettes[paletteIndex]);
}

/// just specify the filename without the .gr and without path.
/// imageTo is the image number after the last image loaded
void ImageManager::LoadList(std::vector<IndexedImage>& imageList, const char* basename,
   unsigned int imageFrom, unsigned int imageTo,
   unsigned int paletteIndex)
{
   std::string filename("data/");
   filename.append(basename);
   filename.append(".gr");

   // import the images
   Import::ImageLoader loader{ m_resourceManager };

   loader.LoadImageGrList(imageList, filename.c_str(), imageFrom, imageTo,
      m_allAuxPalettes);

   // set palette ptr for all images
   unsigned int max = imageList.size();
   for (unsigned int i = 0; i < max; i++)
      imageList[i].SetPalette(m_allPalettes[paletteIndex]);
}
