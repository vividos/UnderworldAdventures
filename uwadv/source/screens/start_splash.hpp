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
/// \file start_splash.hpp
/// \brief start splash screen
//
#pragma once

#include "screen.hpp"
#include "imgquad.hpp"
#include "font.hpp"
#include "cutscene.hpp"
#include "fading.hpp"

/// \brief start splash screen class
/// The screen displays the splash screen sequence at start of Ultima
/// Underworld. It shows two images (or just one when using the demo) and the
/// animated logo. The images are skipped when savegames are available.
/// The screen can be in one of these stages:
/// stage 0: first opening screen
/// stage 1: second opening screen (not in uw_demo)
/// stage 2: fading in animation
/// stage 3: showing animation
/// stage 4: fading out animation
/// stage 5: screen finished
class ua_start_splash_screen : public ua_screen
{
public:
   /// ctor
   ua_start_splash_screen(IGame& game) :ua_screen(game) {}
   /// dtor
   virtual ~ua_start_splash_screen() {}

   // virtual functions from ua_screen
   virtual void init() override;
   virtual void destroy() override;
   virtual void draw() override;
   virtual bool process_event(SDL_Event& event) override;
   virtual void tick() override;

protected:
   /// number of seconds the splash screen images are shown
   static const double show_time;

   /// fade in/out time in seconds
   static const double blend_time;

   /// animation frame rate, in frames per second
   static const double anim_framerate;

   /// fading helper
   ua_fading_helper fader;

   /// current stage we are in
   unsigned int stage;

   /// count of ticks since last stage-start
   unsigned int tickcount;

   /// current still image
   ua_image_quad img_still;

   /// current cutscene
   ua_cutscene cuts_anim;

   /// current animation frame
   unsigned int curframe;

   /// animation time count
   double animcount;
};
