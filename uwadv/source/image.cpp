/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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
/*! \file image.cpp

   \brief image creation and manipulation

*/

// needed includes
#include "common.hpp"
#include "image.hpp"


// ua_image methods

ua_image::ua_image()
{
   xres=yres=0;
}

void ua_image::create(unsigned int width, unsigned int height, unsigned int initial,
   unsigned int pal)
{
   xres = width; yres = height;
   pixels.resize(width*height,initial);
   palette = pal;
}

/*! when image to paste and image that is pasted is the same one, then
    non-transparent pastes are only successful when the source and target
    areas don't overlap
*/
void ua_image::paste_image(const ua_image &img, unsigned int destx,unsigned int desty,
   bool transparent)
{
   // get resolution and pixel vectors
   unsigned int sxres = img.get_xres(), swidth=sxres;
   unsigned int syres = img.get_yres(), sheight=syres;

   // adjust source image if pasting would cross dest image borders
   if (destx+swidth>xres) swidth = xres-destx;
   if (desty+sheight>yres) sheight = yres-desty;

   const std::vector<Uint8> &src = img.pixels;
   Uint8 *dest = &pixels[0];

   if (!transparent)
   {
      // non-transparent paste
      for(unsigned int y=0; y<sheight; y++)
      {
         memcpy(&dest[(y+desty)*xres+destx], &src[y*sxres], swidth);
      }
   }
   else
   {
      // paste that omits transparent parts
      for(unsigned int y=0; y<sheight; y++)
      for(unsigned int x=0; x<swidth; x++)
      {
         Uint8 pixel = src[y*sxres+x];
         if (pixel!=0)
            dest[(y+desty)*xres+(x+destx)] = pixel;
      }
   }
}

void ua_image::copy_rect(ua_image &img, unsigned int startx, unsigned int starty,
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

void ua_image::fill_rect(unsigned int startx, unsigned int starty,
   unsigned int width, unsigned int height, unsigned int color)
{
   // get pixel and ptr
   Uint8* ptr = &pixels[0]+starty*xres+startx;

   // fill line by line
   for(unsigned int y=0; y<height; y++)
      memset(&ptr[y*xres],color,width);
}

void ua_image::clear(Uint8 index)
{
   unsigned int max = pixels.size();
   for(unsigned int i=0; i<max; i++) pixels[i]=index;
}


// ua_image_list methods

ua_image &ua_image_list::get_image(unsigned int num)
{
   if (num>=allimages.size())
      throw ua_exception("ua_image_list: index out of bounds");

   return allimages[num];
}
