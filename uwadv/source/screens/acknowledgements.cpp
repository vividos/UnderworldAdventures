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
/*! \file acknowledgements.cpp

   \brief acknowledgements implementation

*/

// needed includes
#include "common.hpp"
#include "acknowledgements.hpp"
#include "image.hpp"


// constants

//! time to show one credits screen
const double ua_ack_showtime = 3.0;

//! time to crossfade between two screens
const double ua_ack_fadetime = 0.5;


// ua_acknowledgements_screen methods

void ua_acknowledgements_screen::init()
{
   ua_trace("acknowledgements screen started\n");

   // init OpenGL stuff

   // setup orthogonal projection
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0,320,0,200);
   glMatrixMode(GL_MODELVIEW);

   // set OpenGL flags
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,0);

   glDisable(GL_DEPTH_TEST);
   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

   // setup misc. variables
   ended = false;
   stage = 0; // crossfade
   tickcount = unsigned(ua_ack_showtime * core->get_tickrate()) - 3;
   curframe = unsigned(-1);

   ack.load(core->get_settings(),"cuts/cs012.n01");

   // init and fill the textures
   tex.init(2);

   // tex 0 is a blank frame / fading-out frame
   ua_image img_clear;
   img_clear.create(320,200,1);
   tex.convert(core->get_texmgr(),img_clear,0);
   tex.use(core->get_texmgr(),0);
   tex.upload();

   // tex 1 is next fading-in frame
   tex.convert(core->get_texmgr(),img_clear,1);
   tex.use(core->get_texmgr(),1);
   tex.upload();
}

void ua_acknowledgements_screen::done()
{
   tex.done();
}

void ua_acknowledgements_screen::handle_event(SDL_Event &event)
{
   switch(event.type)
   {
   case SDL_KEYDOWN:
      // handle key presses
      switch(event.key.keysym.sym)
      {
      case SDLK_RETURN:
      case SDLK_ESCAPE:
         ua_trace("acknowledgements ended by return/escape\n");
         ended = true;

         fadeout_end();
         break;
      }
   }
}

void ua_acknowledgements_screen::render()
{
   glClear(GL_COLOR_BUFFER_BIT);
   glDisable(GL_BLEND);

   // render first quad; image to show / to fade in
   glColor3ub(255,255,255);

   // prepare image texture
   tex.use(core->get_texmgr(),1-(curframe&1));

   double u = tex.get_tex_u(), v = tex.get_tex_v();

   // draw quad
   glBegin(GL_QUADS);
   glTexCoord2d(0.0, v  ); glVertex2i(  0,  0);
   glTexCoord2d(u  , v  ); glVertex2i(320,  0);
   glTexCoord2d(u  , 0.0); glVertex2i(320,200);
   glTexCoord2d(0.0, 0.0); glVertex2i(  0,200);
   glEnd();

   if (stage==1)
   {
      // render second quad, with alpha blended image

      glEnable(GL_BLEND);
      // render second quad; image to fade out

      // calculate alpha
      Uint8 alpha = 255-Uint8(255*(double(tickcount)/(core->get_tickrate()*ua_ack_fadetime)));
      glColor4ub(255,255,255,alpha);

      // prepare image texture
      tex.use(core->get_texmgr(),(curframe&1));

      u = tex.get_tex_u(); v = tex.get_tex_v();

      // draw quad
      glBegin(GL_QUADS);
      glTexCoord2d(0.0, v  ); glVertex2i(  0,  0);
      glTexCoord2d(u  , v  ); glVertex2i(320,  0);
      glTexCoord2d(u  , 0.0); glVertex2i(320,200);
      glTexCoord2d(0.0, 0.0); glVertex2i(  0,200);
      glEnd();
   }
}

void ua_acknowledgements_screen::tick()
{
   if (stage==0 && double(tickcount)/core->get_tickrate() >= ua_ack_showtime)
   {
      // switch to crossfade
      stage = 1;
      tickcount = 0;
      if (curframe==13)
      {
         ended=true;
         fadeout_end();
         return;
      }

      ++curframe;

      // load new animation frame
      ack.get_frame(tex,curframe,1-(curframe&1));
      tex.use(core->get_texmgr(),1-(curframe&1));
      tex.upload();

      return;
   }

   if (stage==1 && double(tickcount)/core->get_tickrate() >= ua_ack_fadetime)
   {
      if (ended)
         core->pop_screen();

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
   stage = 1;
   tickcount=0;

   // insert black frame as fadeout frame
   {
      ua_image img_clear;
      img_clear.create(320,200,1);
      tex.convert(core->get_texmgr(),img_clear,(curframe&1));
      tex.use(core->get_texmgr(),(curframe&1));
      tex.upload();
   }
   ++curframe;
}