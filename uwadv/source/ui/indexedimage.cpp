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
/// \file indexedimage.cpp
/// \brief indexed image creation and manipulation
//
#include "common.hpp"
#include "indexedimage.hpp"
#include "imageloader.hpp"

IndexedImage::IndexedImage()
   : xres(0), yres(0)
{
}

void IndexedImage::create(unsigned int width, unsigned int height)
{
   xres = width; yres = height;
   pixels.resize(width*height);
}

/*
// currently commented out; look if the function is used at all and remove it
// when not
void IndexedImage::copy_rect(IndexedImage &img, unsigned int startx, unsigned int starty,
   unsigned int width, unsigned int height)
{
   img.create(width,height,0,palette);

   // get source and destination ptr
   Uint8* src = &pixels[0]+starty*xres+startx;
   Uint8* dest = &img.get_pixels()[0];

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
void IndexedImage::paste_rect(const IndexedImage& from_img, unsigned int fromx, unsigned int fromy,
   unsigned width, unsigned height, unsigned int destx, unsigned int desty, bool transparent)
{
   // get resolution and pixel vectors
   unsigned int sxres = from_img.get_xres();
   unsigned int syres = from_img.get_yres();

   // check if width and height is in range
   if (width > sxres) width = sxres;
   if (height > syres) height = syres;

   // adjust source image if pasting would cross dest image borders
   if (destx + width > xres) width = xres - destx;
   if (desty + height > yres) height = yres - desty;

   // get source and dest pointer
   const Uint8* src = &from_img.get_pixels()[fromx + fromy * sxres];
   Uint8* dest = &pixels[destx + desty * xres];

   if (!transparent)
   {
      // non-transparent paste
      for (unsigned int y = 0; y < height; y++)
         memcpy(&dest[y*xres], &src[y*sxres], width);
   }
   else
   {
      // paste that omits transparent parts
      for (unsigned int y = 0; y < height; y++)
      {
         const Uint8* src_line = &src[y*sxres];
         Uint8* dest_line = &dest[y*xres];

         for (unsigned int x = 0; x < width; x++)
         {
            Uint8 pixel = src_line[x];
            if (pixel != 0)
               dest_line[x] = pixel;
         }
      }
   }
}

void IndexedImage::fill_rect(unsigned int startx, unsigned int starty,
   unsigned int width, unsigned int height, unsigned int color)
{
   // get pixel and ptr
   Uint8* ptr = &pixels[0] + starty * xres + startx;

   // fill line by line
   for (unsigned int y = 0; y < height; y++)
      memset(&ptr[y*xres], color, width);
}

void IndexedImage::clear(Uint8 index)
{
   if (pixels.size() > 0)
      memset(&pixels[0], index, pixels.size());
}

void IndexedImage::create_new_palette()
{
   palette = Palette256Ptr(new Palette256);
}

void IndexedImage::clone_palette()
{
   if (palette.get() == NULL)
      return;

   palette = Palette256Ptr(new Palette256(*palette));
}


void ImageManager::init(Base::Settings& settings)
{
   uw_path = settings.GetString(Base::settingUnderworldPath);

   Import::ImageLoader loader;

   // load all palettes
   std::string filename(uw_path);
   filename.append("data/pals.dat");

   loader.LoadPalettes(filename.c_str(), allpalettes);

   // load all auxiliary palettes
   filename.assign(uw_path);
   filename.append("data/allpals.dat");

   loader.LoadAuxPalettes(filename.c_str(), allauxpals);
}

/// when loading *.gr images, just specify the filename without the .gr and
/// without path; for *.byt images specify the complete relative path to the
/// file;
void ImageManager::load(IndexedImage& img, const char* basename, unsigned int imgnum,
   unsigned int palette, ua_image_type type)
{
   switch (type)
   {
   case ua_img_gr:
   {
      std::string filename(uw_path);
      filename.append("data/");
      filename.append(basename);
      filename.append(".gr");

      Import::ImageLoader loader;
      loader.LoadImageGr(img, filename.c_str(), imgnum, allauxpals);
   }
   break;

   case ua_img_byt:
   {
      std::string filename(uw_path);
      filename.append(basename);

      img.create(320, 200);

      Import::ImageLoader loader;
      loader.LoadImageByt(filename.c_str(), &img.get_pixels()[0]);
   }
   break;

   default:
      break;
   }

   img.set_palette(allpalettes[palette]);
}

/// just specify the filename without the .gr and without path.
/// img_to is the image number after the last image loaded
void ImageManager::load_list(std::vector<IndexedImage>& imgs, const char* basename,
   unsigned int img_from, unsigned int img_to,
   unsigned int palette)
{
   std::string filename(uw_path);
   filename.append("data/");
   filename.append(basename);
   filename.append(".gr");

   // import the images
   Import::ImageLoader loader;

   loader.LoadImageGrList(imgs, filename.c_str(), img_from, img_to,
      allauxpals);

   // set palette ptr for all images
   unsigned int max = imgs.size();
   for (unsigned int i = 0; i < max; i++)
      imgs[i].set_palette(allpalettes[palette]);
}
