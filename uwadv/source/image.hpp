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
/*! \file image.hpp

   \brief image loading and handling, image lists

*/

// include guard
#ifndef __uwadv_image_hpp_
#define __uwadv_image_hpp_

// needed includes
#include <vector>
#include "settings.hpp"


// classes

//! single image
class ua_image
{
   friend class ua_image_list;
public:
   //! ctor
   ua_image();
   //! dtor
   virtual ~ua_image(){}

   //! loads a single image; name must be without the extension ".gr"
   void load(ua_settings &settings, const char *name, unsigned int which=0,
      unsigned int palette=0);

   //! loads a raw image (*.byt)
   void load_raw(ua_settings &settings, const char *name, unsigned int palette=0);

   //! returns image x resolution
   unsigned int get_xres() const { return xres; }
   //! returns image y resolution
   unsigned int get_yres() const { return yres; }
   //! returns palette number
   unsigned int get_palette() const { return palette; }

   // image manipulation

   //! creates a new bitmap
   void create(unsigned int width, unsigned int height, unsigned int initial=0,
      unsigned int palette=0);

   //! returns the pixel vector
   std::vector<Uint8> &get_pixels(){ return pixels; }

   //! pastes a image on a specific pos
   void paste_image(const ua_image &img, unsigned int destx,unsigned int desty,
      bool transparent=false);

   //! copies a rectangular area to the given image
   void copy_rect(ua_image &img, unsigned int startx, unsigned int starty,
      unsigned int width, unsigned int height);

   //! clears bitmap with one palette index
   void clear(Uint8 index=0);

protected:
   //! private image loader
   void load_image(FILE *fd,Uint8 auxpalidx[32][16]);

protected:
   //! used palette
   unsigned int palette;

   //! resolution
   unsigned int xres,yres;

   //! pixels
   std::vector<Uint8> pixels;
};


//! list of images
class ua_image_list
{
public:
   //! ctor
   ua_image_list(){}

   //! loads a list of images
   void load(ua_settings &settings, const char *name, unsigned int from=0,
      unsigned int to=0, unsigned int palette=0);

   //! returns size of list
   unsigned int size(){ return allimages.size(); };

   //! returns a single image
   ua_image &get_image(unsigned int num);

   //! returns the list of images
   std::vector<ua_image> &get_allimages(){ return allimages; }

protected:
   //! images in the list
   std::vector<ua_image> allimages;
};

#endif
