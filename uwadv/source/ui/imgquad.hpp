/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

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
//! \ingroup userinterface

//@{

// include guard
#ifndef uwadv_imgquad_hpp_
#define uwadv_imgquad_hpp_

// needed includes
#include "window.hpp"
#include "game_interface.hpp"
#include "image.hpp"
#include "texture.hpp"


// classes

//! image quad class
class ua_image_quad: public ua_window
{
public:
   //! ctor
   ua_image_quad(){}

   //! initializes image quad window
   void init(ua_game_interface& game, unsigned int xpos, unsigned int ypos);

   //! returns image to draw
   ua_image& get_image();

   //! updates internal texture when image was changed
   void update();

   // virtual functions from ua_window
   virtual void destroy();
   virtual void draw();

protected:
   //! the image to draw
   ua_image image;

   //! image quad texture
   ua_texture tex;

   //! indicates if we use two textures to draw the quad
   bool split_textures;
};


// inline methods

inline ua_image& ua_image_quad::get_image()
{
   return image;
}


#endif
//@}
