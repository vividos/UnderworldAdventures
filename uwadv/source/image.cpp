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

   image creation and manipulation

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
   for(int i=0; i<pixels.size(); i++) pixels[i]=initial;
   palette = pal;
}

void ua_image::paste_image(const ua_image &img, unsigned int destx,unsigned int desty)
{
   unsigned int sxres = img.get_xres();
   unsigned int syres = img.get_yres();

   const std::vector<Uint8> &src = img.pixels;
   Uint8 *dest = &pixels[0];

   for(int y=0; y<syres; y++)
   for(int x=0; x<sxres; x++)
   {
      dest[(y+desty)*xres+(x+destx)] = src[y*sxres+x];
   }
}

// ua_image_list methods

ua_image_list::ua_image_list()
{
}

ua_image_list::~ua_image_list()
{
   // delete all images in list
   int max=allimages.size();
   for(int i=0; i<max; i++)
      delete allimages[i];
}

const ua_image *ua_image_list::get_image(unsigned int num)
{
   if (num>=allimages.size())
      return NULL;

   return allimages[num];
}
