/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Underworld Adventures Team

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
/*! \file imgquad.hpp

   \brief image quad that can be drawn onto screen

   the image quad supports resolutions up to 512 x 256

*/

// include guard
#ifndef __uwadv_imgquad_hpp_
#define __uwadv_imgquad_hpp_

// needed includes
#include "image.hpp"
#include "texture.hpp"


class ua_image_quad: public ua_image
{
public:
   //! ctor
   ua_image_quad(){}

   //! initializes image quad
   void init(ua_texture_manager* texmgr, unsigned int xpos, unsigned int ypos,
      unsigned int width, unsigned int height, bool copy_pal=true);

   //! converts and uploads the image to texture
   void convert_upload();

   //! renders image quad
   void render();

   //! cleans up image quad
   void done();

protected:
   //! image quad texture
   ua_texture tex;

   //! starting pos on screen
   unsigned int xpos, ypos;

   //! indicates if we use two textures to draw the quad
   bool split_textures;

   //! image quad palette
   ua_onepalette quadpalette;
};

#endif
