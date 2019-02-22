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
/// \file mousecursor.hpp
/// \brief mouse cursor
/// Lightweight mouse cursor class that wraps functionality needed for
/// every screen that shows a cursor, nothing more, nothing less. All
/// texture/graphic options must be set by the user of the class.
//
#pragma once

#include "window.hpp"
#include "indexedimage.hpp"
#include "texture.hpp"

class IGame;

/// mouse cursor class
class ua_mousecursor : public ua_window
{
public:
   /// ctor
   ua_mousecursor() {}
   /// dtor
   virtual ~ua_mousecursor() {}

   /// initializes mouse cursor class
   void init(IGame& game, unsigned int initialtype = 0);

   /// show or hide the cursor
   void show(bool show);

   /// set the type of cursor (the image used)
   void set_type(unsigned int type);

   /// sets custom image as cursor
   void set_custom(IndexedImage& cursorimg);

   // virtual methods from ua_window

   virtual void destroy() override;
   virtual void draw() override;
   bool process_event(SDL_Event& event) override;

protected:
   /// cursor visible
   bool isvisible;

   /// mouse cursor image list
   std::vector<IndexedImage> img_cursors;

   /// texture object for mouse
   ua_texture mousetex;

   /// indicates if mouse cursor is drawn using smooth (filtered) pixels
   bool smooth_ui;
};
