//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2019,2022,2023 Underworld Adventures Team
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
/// \file ImageQuad.hpp
/// \brief image quad class
//
#pragma once

#include "Window.hpp"
#include "GameInterface.hpp"
#include "IndexedImage.hpp"
#include "Texture.hpp"

/// \brief image quad class
/// The ImageQuad class is a Window that draws an IndexedImage on an OpenGL
/// screen managed by Screen. It supports images up to 320x200 (the
/// standard screen size) and supports adding a border to properly render the
/// image quad on top of a background image in OpenGL.
/// The image quad can be registered using Screen::RegisterWindow() just
/// like any Window object. The underlying image can be retrieved via
/// GetImage().
/// The settings value in settingUISmooth determines if the image quad is
/// drawn using smooth (filtered) pixels.
class ImageQuad : public Window
{
public:
   /// ctor
   ImageQuad()
   {
   }

   /// initializes image quad window
   virtual void Init(IGameInstance& game, unsigned int xpos, unsigned int ypos);

   /// returns image to draw
   IndexedImage& GetImage()
   {
      return m_image;
   }

   /// adds border to image, taking border pixels from given image
   void AddBorder(IndexedImage& borderImage);

   /// updates internal texture when image was changed
   void Update();

   // virtual functions from Window
   virtual void Destroy() override;
   virtual void Draw() override;

private:
   /// the image to draw
   IndexedImage m_image;

   /// indicates if image has border
   bool m_hasBorder = false;

   /// image quad texture
   Texture m_texture;

   /// indicates if images are drawn using smooth (filtered) pixels
   bool m_smoothUI = false;
};
