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

   image loading and handling, image lists

*/

// include guard
#ifndef __uwadv_image_hpp_
#define __uwadv_image_hpp_

// needed includes
#include "texture.hpp"


// classes

//! single image
class ua_image
{
public:
   ua_image(){}

   //! loads a single image; name must be without the extension ".gr"
   void load(ua_settings &settings, const char *name, unsigned int which=0,
      unsigned int palette=0);

   //! loads a raw image (*.byt)
   void load_raw(ua_settings &settings, const char *name, unsigned int palette=0);

   //! converts image to texture
   void to_texture(ua_texture &tex);

   // image manipulation

   //! creates a new bitmap
   void create(unsigned int width, unsigned int height, unsigned int initial=0,
      unsigned int palette=0);

   //! returns the pixel vector
   std::vector<Uint8> &get_pixels(){ return pixels; }

   //! pastes a image on a specific pos
   void paste_image(ua_image &img, unsigned int destx,unsigned int desty);

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
   ua_image_list();
   ~ua_image_list();

   //! loads a list of images
   void load(ua_settings &settings, const char *name, unsigned int from=0,
      unsigned int to=-1, unsigned int palette=0);

   //! returns size of list
   unsigned int size(){ return allimages.size(); };

   //! returns a single image
   const ua_image *get_image(unsigned int num);

   //! converts images to texture set
   void to_texture(ua_texture &tex, unsigned int from, unsigned int to);

protected:
   //! images in the list
   std::vector<ua_image*> allimages;
};

#endif
