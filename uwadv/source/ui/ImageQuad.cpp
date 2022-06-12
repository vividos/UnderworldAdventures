//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2022 Underworld Adventures Team
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
/// \file ImageQuad.cpp
/// \brief image quad implementation
//
#include "pch.hpp"
#include "ImageQuad.hpp"
#include "Renderer.hpp"
#include "ImageManager.hpp"

/// \brief Does image quad initialisation; Window::create() is called and needed
/// texture(s) are init'ed. When there's no palette for the image yet, palette
/// 0 is retrieved and set.
/// The method is virtual and can be derived to do window/control specific
/// init'ing before or after calling this method.
/// \param game reference to game interface
/// \param xpos x position on Screen
/// \param ypos y position
void ImageQuad::Init(IBasicGame& game, unsigned int xpos,
   unsigned int ypos)
{
   Window::Create(xpos, ypos, m_image.GetXRes(), m_image.GetYRes());

   if (m_image.GetPalette().get() == NULL)
      m_image.SetPalette(game.GetImageManager().GetPalette(0));

   m_texture.Init(1);

   m_smoothUI = game.GetSettings().GetBool(Base::settingUISmooth);
}

/// The method adds a border around the current quad image, based on the
/// given image; the border pixels are taken from the window coordinates
/// set during Window::create() that is called in ImageQuad::Init().
/// The window is enlarged by 2 pixels in width and height and moved one
/// pixel left and up. To update the quad image, paste new image contents
/// to the coordinates destx = 1, desty = 1.
/// \param borderImage the background image where borders are copied from
void ImageQuad::AddBorder(IndexedImage& borderImage)
{
   IndexedImage tempImage = m_image;

   // enlarge the image
   m_image.Create(m_image.GetXRes() + 2, m_image.GetYRes() + 2);

   // left border
   m_image.PasteRect(borderImage, m_windowXPos - 1, m_windowYPos - 1, 1, m_windowHeight + 2, 0, 0);
   // right border
   m_image.PasteRect(borderImage, m_windowXPos + m_windowWidth, m_windowYPos - 1, 1, m_windowHeight + 2, m_windowWidth + 1, 0);

   // top border
   m_image.PasteRect(borderImage, m_windowXPos, m_windowYPos - 1, m_windowWidth, 1, 1, 0);
   // bottom border
   m_image.PasteRect(borderImage, m_windowXPos, m_windowYPos + m_windowHeight, m_windowWidth, 1, 1, m_windowHeight + 1);

   // image
   m_image.PasteRect(tempImage, 0, 0, tempImage.GetXRes(), tempImage.GetYRes(), 1, 1);

   m_windowXPos--;
   m_windowYPos--;

   m_hasBorder = true;
}

/// Updates image quad texture(s) with changes from the quad image available
/// through GetImage().
void ImageQuad::Update()
{
   m_windowWidth = m_image.GetXRes();
   m_windowHeight = m_image.GetYRes();

   m_texture.Convert(m_image);
   m_texture.Upload(0);
}

/// Cleans up texture(s) used for rendering the image quad.
void ImageQuad::Destroy()
{
   m_texture.Done();
}

/// Draws the image quad. The method takes into account if a border was added
/// with AddBorder().
void ImageQuad::Draw()
{
   double u = m_texture.GetTexU(), v = m_texture.GetTexV();
   m_texture.Use(0);

   double dx = 0.0, dy = 0.0;

   unsigned int x0, x1, y0, y1;
   x0 = m_windowXPos;
   x1 = m_windowXPos + m_windowWidth;

   y0 = 200 - m_windowYPos;
   y1 = 200 - m_windowYPos - m_windowHeight;

   if (m_hasBorder)
   {
      dx = 1.0 / m_windowWidth;
      dy = 1.0 / m_windowHeight;
      x0++; x1--; y0--; y1++;
   }

   double u0 = dx, u1 = u - dx;
   double v0 = dy, v1 = v - dy;

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_smoothUI ? GL_LINEAR : GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_smoothUI ? GL_LINEAR : GL_NEAREST);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   glBegin(GL_QUADS);
   glTexCoord2d(u0, v1); glVertex2d(x0, y1);
   glTexCoord2d(u1, v1); glVertex2d(x1, y1);
   glTexCoord2d(u1, v0); glVertex2d(x1, y0);
   glTexCoord2d(u0, v0); glVertex2d(x0, y0);
   glEnd();
}
