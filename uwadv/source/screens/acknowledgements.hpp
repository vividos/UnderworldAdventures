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
/// \file acknowledgements.hpp
/// \brief acknowledgements screen
//
#pragma once

#include "screen.hpp"
#include "imgquad.hpp"
#include "cutscene.hpp"
#include "fading.hpp"

/// cutscene view screen
class ua_acknowledgements_screen : public ua_screen
{
public:
   /// ctor
   ua_acknowledgements_screen(IGame& game) :ua_screen(game) {}

   // virtual functions from ua_screen
   virtual void init();
   virtual void destroy();
   virtual void draw();
   virtual bool process_event(SDL_Event& event);
   virtual void tick();

protected:
   /// inits fadeout at end
   void fadeout_end();

protected:
   /// time to show one credits page
   static const double show_time;

   /// time to crossfade between two screens
   static const double xfade_time;


   /// current view stage
   unsigned int stage;

   /// tickcount for every stage
   unsigned int tickcount;

   /// current visible frame
   unsigned int curframe;

   /// indicates if ended
   bool ended;

   /// fading helper
   ua_fading_helper fader;

   /// acknowledgements cutscene
   ua_cutscene cuts_ack;

   /// image to fade out
   ua_image_quad img_fadeout;
};
