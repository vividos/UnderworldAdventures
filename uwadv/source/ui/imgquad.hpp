//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2019 Underworld Adventures Team
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
/// \file imgquad.hpp
/// \brief image quad class
//
#pragma once

#include "window.hpp"
#include "game_interface.hpp"
#include "indexedimage.hpp"
#include "texture.hpp"

/// \brief image quad class
/// The ua_image_quad class is a ua_window that draws an IndexedImage on an OpenGL
/// screen managed by ua_screen. It supports images up to 320x200 (the
/// standard screen size) and supports adding a border to properly render the
/// image quad on top of a background image in OpenGL.
/// The image quad can be registered using ua_screen::register_window() just
/// like any ua_window object. The underlying image can be retrieved via
/// get_image().
/// The settings value in ua_setting_ui_smooth determines if the image quad is
/// drawn using smooth (filtered) pixels.
class ua_image_quad : public ua_window
{
public:
   /// ctor
   ua_image_quad() :split_textures(false), has_border(false) {}

   /// initializes image quad window
   virtual void init(IGame& game, unsigned int xpos, unsigned int ypos);

   /// returns image to draw
   IndexedImage& get_image()
   {
      return image;
   }

   /// adds border to image, taking border pixels from given image
   void add_border(IndexedImage& border_img);

   /// updates internal texture when image was changed
   void update();

   // virtual functions from ua_window
   virtual void destroy() override;
   virtual void draw() override;

protected:
   /// the image to draw
   IndexedImage image;

   /// indicates if image has border
   bool has_border;

   /// image quad texture
   ua_texture tex;

   /// indicates if we use two textures to draw the quad
   bool split_textures;

   /// indicates if images are drawn using smooth (filtered) pixels
   bool smooth_ui;
};
