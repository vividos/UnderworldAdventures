/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

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
/*! \file acknowledgements.cpp

   \brief acknowledgements implementation

   stage 0: show animation frame
   stage 1: crossfade to next frame

*/

// needed includes
#include "common.hpp"
#include "acknowledgements.hpp"
#include "image.hpp"
#include "renderer.hpp"


// constants

//! time to show one credits image
const double ua_acknowledgements_screen::show_time = 3.0;

//! time to crossfade between two screens
const double ua_acknowledgements_screen::xfade_time = 0.5;


// ua_acknowledgements_screen methods

void ua_acknowledgements_screen::init()
{
   ua_screen::init();

   ua_trace("acknowledgements screen started\n");

   game->get_renderer().setup_camera2d();

   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

   // set up variables
   ended = false;
   stage = 1; // crossfade
   tickcount = 0;
   curframe = 0;

   // init cutscene quad
   cuts_ack.load(game->get_settings(),"cuts/cs012.n01");
   cuts_ack.init(*game, 0,0);
   cuts_ack.update_frame(curframe);

   // init fadeout image
   img_fadeout.init(*game, 0,0);
   {
      ua_image& img = img_fadeout.get_image();

      img.create(320,200);
      img.clear(1);
   }
   img_fadeout.update();

   // init fadeout
   fader.init(false,game->get_tickrate(),xfade_time);

   SDL_ShowCursor(1);
}

void ua_acknowledgements_screen::destroy()
{
   SDL_ShowCursor(0);

   cuts_ack.destroy();
   img_fadeout.destroy();

   ua_trace("acknowledgements screen ended\n\n");
}

void ua_acknowledgements_screen::draw()
{
   glClear(GL_COLOR_BUFFER_BIT);
   glDisable(GL_BLEND);

   // render first quad; image to show / to fade in
   glColor3ub(255,255,255);

   // draw first quad
   cuts_ack.draw();

   if (stage==1)
   {
      // render second quad using alpha blending
      glEnable(GL_BLEND);

      Uint8 alpha = fader.get_fade_value();
      glColor4ub(255,255,255,alpha);

      // draw second quad
      img_fadeout.draw();
   }
}

bool ua_acknowledgements_screen::process_event(SDL_Event& event)
{
   switch(event.type)
   {
   case SDL_MOUSEBUTTONDOWN:
      // start crossfade immediately
      if (stage==0)
         tickcount = unsigned(game->get_tickrate()*show_time) + 1;
      break;

   case SDL_KEYDOWN:
      // handle key presses
      switch(event.key.keysym.sym)
      {
      case SDLK_SPACE:
         tickcount = unsigned(show_time * game->get_tickrate()) + 1;
         break;

      case SDLK_RETURN:
      case SDLK_ESCAPE:
         ua_trace("acknowledgements ended by return/escape\n");
         ended = true;

         fadeout_end();
         break;

      default:
         break;
      }
   default:
      break;
   }

   return true;
}

void ua_acknowledgements_screen::tick()
{
   if (stage==0 && double(tickcount)/game->get_tickrate() >= show_time)
   {
      // last frame? fade out and end
      if (curframe==13)
      {
         ended = true;
         fadeout_end();
         return;
      }

      // switch to crossfade
      stage = 1;
      tickcount = 0;

      // reinit fader
      fader.init(false,game->get_tickrate(),xfade_time);

      // copy old frame to fadeout image
      img_fadeout.get_image() = cuts_ack.get_image();
      img_fadeout.update();

      // load new animation frame
      cuts_ack.update_frame(++curframe);
      img_fadeout.update();

      //ua_trace("crossfading to frame %u\n",curframe);
      return;
   }

   if (stage==1 && fader.tick())
   {
      // fading complete; switch to showing image
      //ua_trace("showing frame %u\n",curframe);

      if (ended)
         game->remove_screen();

      // switch to show mode
      stage = 0;
      tickcount = 0;

      return;
   }

   ++tickcount;
}

void ua_acknowledgements_screen::fadeout_end()
{
   // initiate fadeout
   if (stage == 1)
      tickcount = unsigned(xfade_time * game->get_tickrate()) - tickcount;
   else
      tickcount = 0;
   stage = 1;

   // reinit fader
   fader.init(false,game->get_tickrate(),xfade_time, tickcount);

   // copy last frame to fadeout image
   img_fadeout.get_image() = cuts_ack.get_image();
   img_fadeout.update();

   // insert black frame as fadeout frame
   cuts_ack.get_image().clear(1);
   cuts_ack.update();
}
