/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file start_splash.hpp

   \brief start splash screen

   displays the first few splash screens seen at the start of ultima
   underworld

*/

// include guard
#ifndef __uwadv_start_splash_hpp_
#define __uwadv_start_splash_hpp_

// needed includes
#include "screen.hpp"
#include "image.hpp"
#include "font.hpp"
#include "cutscene.hpp"


// classes

//! start splash screen class
class ua_start_splash_screen: public ua_ui_screen_base
{
public:
   //! ctor
   ua_start_splash_screen(){}
   //! dtor
   virtual ~ua_start_splash_screen(){}

   // virtual functions from ua_ui_screen_base

   virtual void init();
   virtual void done();
   virtual void handle_event(SDL_Event &event);
   virtual void render();
   virtual void tick();

protected:
   //! current stage we are in
   unsigned int stage;

   //! count of ticks since last stage-start
   unsigned int tickcount;

   //! current still image
   ua_image img;

   //! texture object for current image / anim
   ua_texture tex;

   //! current cutscene
   ua_cutscene cuts;

   //! current animation frame
   unsigned int curframe;

   //! animation time count
   double animcount;
};

#endif
