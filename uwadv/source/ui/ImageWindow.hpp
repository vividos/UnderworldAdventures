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
/// \file ImageWindow.hpp
/// \brief window that manages an indexed image to draw to
//
#pragma once

class ImageScreen;

/// Image window that uses an IndexedImage located on a specific position in
/// an ImageScreen. The image uses the same palette as the screen.
class ImageWindow : public Window
{
public:
   /// ctor
   ImageWindow(ImageScreen& screen);

   /// initializes image window
   void Init(unsigned int xpos, unsigned int ypos,
      unsigned int width, unsigned int height);

protected:
   // methods for ImageWindow derived classes

   /// returns the indexed image that is used in the window
   IndexedImage& GetImage();

   /// updates the current image by pasting into the screen's image
   void UpdateImage();

private:
   /// reference to the screen holding the base image
   ImageScreen& m_screen;

   /// the image for this image window
   IndexedImage m_image;
};
