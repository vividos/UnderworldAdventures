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

   a

*/

// needed includes
#include "common.hpp"
#include "image.hpp"


// ua_image methods

void ua_image::create(unsigned int width, unsigned int height, unsigned int initial,
   unsigned int pal)
{
   xres = width; yres = height;
   pixels.resize(width*height,initial);
   for(int i=0; i<pixels.size(); i++) pixels.at(i)=initial;
   palette = pal;
}

void ua_image::paste_image(ua_image &img, unsigned int destx,unsigned int desty)
{
}

// ua_image_list methods

ua_image_list::ua_image_list()
{
}

ua_image_list::~ua_image_list()
{
}

const ua_image *ua_image_list::get_image(unsigned int num)
{
   if (num>=allimages.size())
      return NULL;

   return allimages.at(num);
}
