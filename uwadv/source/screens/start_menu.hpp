//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
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
/// \file start_menu.hpp
/// \brief start menu screen
//
#pragma once

#include "screen.hpp"
#include "imgquad.hpp"
#include "mousecursor.hpp"
#include "fading.hpp"

/// \brief start menu screen class
/// The screen displays the main game menu. The user can select one of the
/// menu items "Introduction", "Create Character", "Acknowledgements" or
/// "Journey Onward" (only available when savegames are detected). Above
/// the menu there is a "warping" Ultima Underworld logo.
/// \todo implement the buttons using a generic ua_button class
class ua_start_menu_screen : public ua_screen
{
public:
   /// ctor
   ua_start_menu_screen(IGame& game);
   /// dtor
   virtual ~ua_start_menu_screen() {}

   // virtual functions from ua_screen
   virtual void init() override;
   virtual void destroy() override;
   virtual void draw() override;
   virtual bool process_event(SDL_Event& event) override;
   virtual void tick() override;

   virtual void mouse_event(bool button_clicked, bool left_button, bool button_down,
      unsigned int mousex, unsigned int mousey) override;

protected:
   /// called when resuming the screen
   void resume();

   /// does a button press
   void press_button();

   /// determines selected area by mouse coordinates
   int get_selected_area();

protected:
   // constants

   /// time to fade in/out
   static const double fade_time;

   /// palette shifts per second
   static const double palette_shifts_per_second;


   /// fading helper
   ua_fading_helper fader;

   /// mouse cursor
   ua_mousecursor mousecursor;

   /// current stage
   unsigned int stage;

   /// count for palette shifting
   double shiftcount;

   /// number of selected area, or -1 if none
   int selected_area;

   /// indicates if "journey onward" is available
   bool journey_avail;

   /// start screen image
   ua_image_quad img_screen;

   /// image list with buttons
   std::vector<IndexedImage> img_buttons;

   /// indicates if image quad texture should be reuploaded
   bool reupload_image;
};
