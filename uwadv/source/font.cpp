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
/*! \file font.cpp

   functions to create a image from a string, in a specific font. fonts are
   loaded in ua_font::init() (in resource/fontloader.cpp).

   font pixels are stored in an array, which can be accessed like this:

      fontdata[ch*charheight*maxwidth + y*maxwidth + x];

   where 'ch' is the character to access, 'y' is the y coordinate and 'x' is
   the x coord. pixels have the value 0 for transparent or 1 for solid. the
   function ua_font::create_string() matches the '1' to a specific color
   palette index.

*/

// needed includes
#include "common.hpp"
#include "font.hpp"
#include <cstring>


// ua_font methods

unsigned int ua_font::calc_length(const char *str)
{
   unsigned int width=0, len=strlen(str);
   for(unsigned int i=0; i<len; i++)
   {
      // count length for each char
      unsigned char ch = static_cast<unsigned char>(str[i]);

      if (ch>=nchars)
         width += maxwidth;
      else
         if (ch==0x20)
            width += spacewidth;
         else
            width += charlengths[ch];
   }
   return width;
}

void ua_font::create_string(ua_image &image, const char *str, Uint8 fg_idx)
{
   // create image with proper size
   unsigned int width=calc_length(str), len = strlen(str);

   image.create(width,charheight);

   std::vector<Uint8> &pixels = image.get_pixels();

   unsigned int pos=0;

   // store all chars in the image
   for(unsigned int i=0; i<len; i++)
   {
      unsigned char ch = static_cast<unsigned char>(str[i]);

      if (ch<nchars)
      {
         if (ch==0x20)
         {
            pos+=spacewidth;
            continue;
         }

         unsigned int clen = charlengths[ch];

         for(unsigned int y=0; y<charheight; y++)
         for(unsigned int x=0; x<clen; x++)
         {
            bool pixset =
               fontdata[ch*charheight*maxwidth + y*maxwidth + x] == 1;
            pixels[y*width+x+pos] = pixset ? fg_idx : 0;
         }

         pos += clen;
      }
   }
}
