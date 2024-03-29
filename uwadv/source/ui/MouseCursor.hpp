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
/// \file MouseCursor.hpp
/// \brief mouse cursor
/// \details Lightweight mouse cursor class that wraps functionality needed for
/// every screen that shows a cursor, nothing more, nothing less. All
/// texture/graphic options must be set by the user of the class.
//
#pragma once

#include "ImageQuad.hpp"
#include "IndexedImage.hpp"

class IGame;

/// mouse cursor class
class MouseCursor : public ImageQuad
{
public:
   /// ctor
   MouseCursor() {}
   /// dtor
   virtual ~MouseCursor() {}

   /// initializes mouse cursor class
   void Init(IGameInstance& game, unsigned int initialType = 0,
      unsigned int paletteIndex = 0);

   /// show or hide the cursor
   void Show(bool show);

   /// set the type of cursor (the image used)
   void SetType(unsigned int type);

   /// sets custom image as cursor
   void SetCustom(IndexedImage& cursorImage);

   /// sets new mouse position x coordinate
   void SetXPos(unsigned int posX) { m_windowXPos = posX; }

   // virtual methods from Window
   virtual void Draw() override;
   virtual bool ProcessEvent(SDL_Event& event) override;

protected:
   /// is cursor visible
   bool m_isVisible = false;

   /// mouse cursor image list
   std::vector<IndexedImage> m_cursorImages;

   /// X offset from the center of the image to mouse position
   int m_offsetX = 0;

   /// Y offset from the center of the image to mouse position
   int m_offsetY = 0;
};
