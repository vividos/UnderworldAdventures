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
:xres(0), yres(0), palette(0)
{
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
void ua_image::paste_image(const ua_image &from_img, unsigned int destx,unsigned int desty,
   bool transparent)
{
   paste_rect(from_img,0,0,
      from_img.get_xres(), from_img.get_yres(),
      destx,desty,transparent);
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

void ua_image::paste_rect(const ua_image& from_img, unsigned int fromx, unsigned int fromy,
   unsigned width, unsigned height, unsigned int destx,unsigned int desty, bool transparent)
{
   // get resolution and pixel vectors
   unsigned int sxres = from_img.get_xres();
   unsigned int syres = from_img.get_yres();

   // adjust source image if pasting would cross dest image borders
   if (destx+width>xres) width = xres-destx;
   if (desty+height>yres) height = yres-desty;

   // get source and dest pointer
   const Uint8* src = &from_img.get_pixels()[fromx+fromy*sxres];
   Uint8* dest = &pixels[destx+desty*xres];

   if (!transparent)
   {
      // non-transparent paste
      for(unsigned int y=0; y<height; y++)
         memcpy(&dest[y*xres], &src[y*sxres], width);
   }
   else
   {
      // paste that omits transparent parts
      for(unsigned int y=0; y<height; y++)
      {
         const Uint8* src_line = &src[y*sxres];
         Uint8* dest_line = &dest[y*xres];

         for(unsigned int x=0; x<width; x++)
         {
            Uint8 pixel = src_line[x];
            if (pixel!=0)
               dest_line[x] = pixel;
         }
      }
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
