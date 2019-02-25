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
/// \file IndexedImage.cpp
/// \brief indexed image creation and manipulation
//
#include "common.hpp"
#include "IndexedImage.hpp"
#include "ImageLoader.hpp"

IndexedImage::IndexedImage()
   :m_xres(0), m_yres(0)
{
}

void IndexedImage::Create(unsigned int width, unsigned int height)
{
   m_xres = width;
   m_yres = height;
   m_pixels.resize(width*height);
}

/*
// currently commented out; look if the function is used at all and remove it
// when not
void IndexedImage::CopyRect(IndexedImage &image, unsigned int startx, unsigned int starty,
   unsigned int width, unsigned int height)
{
   image.Create(width,height,0,palette);

   // get source and destination ptr
   Uint8* src = &m_pixels[0]+starty*xres+startx;
   Uint8* dest = &image.GetPixels()[0];

   // copy line by line
   for(unsigned int y=0; y<height; y++)
   {
      memcpy(dest,&src[y*xres],width);
      dest += width;
   }
}
*/

/// when image to paste and image that is pasted is the same one, then
/// non-transparent pastes are only successful when the source and target
/// areas don't overlap
void IndexedImage::PasteRect(const IndexedImage& from_img, unsigned int fromx, unsigned int fromy,
   unsigned width, unsigned height, unsigned int destx, unsigned int desty, bool transparent)
{
   // get resolution and pixel vectors
   unsigned int sxres = from_img.GetXRes();
   unsigned int syres = from_img.GetYRes();

   // check if width and height is in range
   if (width > sxres) width = sxres;
   if (height > syres) height = syres;

   // adjust source image if pasting would cross dest image borders
   if (destx + width > m_xres) width = m_xres - destx;
   if (desty + height > m_yres) height = m_yres - desty;

   // get source and dest pointer
   const Uint8* src = &from_img.GetPixels()[fromx + fromy * sxres];
   Uint8* dest = &m_pixels[destx + desty * m_xres];

   if (!transparent)
   {
      // non-transparent paste
      for (unsigned int y = 0; y < height; y++)
         memcpy(&dest[y * m_xres], &src[y*sxres], width);
   }
   else
   {
      // paste that omits transparent parts
      for (unsigned int y = 0; y < height; y++)
      {
         const Uint8* src_line = &src[y*sxres];
         Uint8* dest_line = &dest[y * m_xres];

         for (unsigned int x = 0; x < width; x++)
         {
            Uint8 pixel = src_line[x];
            if (pixel != 0)
               dest_line[x] = pixel;
         }
      }
   }
}

void IndexedImage::FillRect(unsigned int startx, unsigned int starty,
   unsigned int width, unsigned int height, unsigned int color)
{
   // get pixel and ptr
   Uint8* ptr = &m_pixels[0] + starty * m_xres + startx;

   // fill line by line
   for (unsigned int y = 0; y < height; y++)
      memset(&ptr[y * m_xres], color, width);
}

void IndexedImage::Clear(Uint8 index)
{
   if (m_pixels.size() > 0)
      memset(&m_pixels[0], index, m_pixels.size());
}

void IndexedImage::CreateNewPalette()
{
   m_palette = Palette256Ptr(new Palette256);
}

void IndexedImage::ClonePalette()
{
   if (m_palette.get() == NULL)
      return;

   m_palette = Palette256Ptr(new Palette256(*m_palette));
}


void ImageManager::Init(Base::Settings& settings)
{
   m_underworldPath = settings.GetString(Base::settingUnderworldPath);

   Import::ImageLoader loader;

   // load all palettes
   std::string filename(m_underworldPath);
   filename.append("data/pals.dat");

   loader.LoadPalettes(filename.c_str(), m_allPalettes);

   // load all auxiliary palettes
   filename.assign(m_underworldPath);
   filename.append("data/allpals.dat");

   loader.LoadAuxPalettes(filename.c_str(), m_allAuxPalettes);
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
      std::string filename(m_underworldPath);
      filename.append("data/");
      filename.append(basename);
      filename.append(".gr");

      Import::ImageLoader loader;
      loader.LoadImageGr(image, filename.c_str(), imgnum, m_allAuxPalettes);
   }
   break;

   case imageByt:
   {
      std::string filename(m_underworldPath);
      filename.append(basename);

      image.Create(320, 200);

      Import::ImageLoader loader;
      loader.LoadImageByt(filename.c_str(), &image.GetPixels()[0]);
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
   std::string filename(m_underworldPath);
   filename.append("data/");
   filename.append(basename);
   filename.append(".gr");

   // import the images
   Import::ImageLoader loader;

   loader.LoadImageGrList(imageList, filename.c_str(), imageFrom, imageTo,
      m_allAuxPalettes);

   // set palette ptr for all images
   unsigned int max = imageList.size();
   for (unsigned int i = 0; i < max; i++)
      imageList[i].SetPalette(m_allPalettes[paletteIndex]);
}
