//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2022 Underworld Adventures Team
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
/// \file ImageWindow.cpp
/// \brief window that manages an indexed image to draw to
//
#include "pch.hpp"
#include "ImageWindow.hpp"
#include "ImageScreen.hpp"
#include "ImageManager.hpp"

ImageWindow::ImageWindow(ImageScreen& screen, bool useTransparency)
   :m_screen(screen),
   m_useTransparency(useTransparency)
{
}

void ImageWindow::Init(unsigned int xpos, unsigned int ypos,
   unsigned int width, unsigned int height)
{
   Window::Create(xpos, ypos, width, height);

   m_image.Create(width, height);
   m_image.SetPalette(m_screen.GetImage().GetPalette());
}

IndexedImage& ImageWindow::GetImage()
{
   return m_image;
}

void ImageWindow::UpdateImage()
{
   m_screen.GetImage().PasteImage(
      m_image,
      m_windowXPos,
      m_windowYPos,
      m_useTransparency);

   m_screen.UpdateImage();
}
