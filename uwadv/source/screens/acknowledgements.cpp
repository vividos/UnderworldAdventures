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

   stage 0: show animation frame
   stage 1: crossfade to next frame

*/

// needed includes
#include "common.hpp"
#include "acknowledgements.hpp"
#include "image.hpp"


// constants

//! time to show one credits image
const double ua_acknowledgements_screen::show_time = 3.0;

//! time to crossfade between two screens
const double ua_acknowledgements_screen::xfade_time = 0.5;


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
   tickcount = unsigned(show_time * core->get_tickrate()) - 3;
   curframe = unsigned(-1);

   ack.load(core->get_settings(),"cuts/cs012.n01");

   // init and fill the textures
   tex1.init(&core->get_texmgr(),2,GL_LINEAR,GL_LINEAR,GL_CLAMP,GL_CLAMP);
   tex2.init(&core->get_texmgr(),2,GL_LINEAR,GL_LINEAR,GL_CLAMP,GL_CLAMP);

   // tex 0 is a blank frame / fading-out frame
   ua_image img_clear1,img_clear2;
   img_clear1.create(256,200,1);
   tex1.convert(img_clear1,0);
   tex1.use(0);
   tex1.upload();

   img_clear2.create(64,200,1);
   tex2.convert(img_clear2,0);
   tex2.use(0);
   tex2.upload();

   // tex 1 is next fading-in frame
   tex1.convert(img_clear1,1);
   tex1.use(1);
   tex1.upload();

   tex2.convert(img_clear2,1);
   tex2.use(1);
   tex2.upload();
}

void ua_acknowledgements_screen::done()
{
   tex1.done();
   tex2.done();
}

void ua_acknowledgements_screen::handle_event(SDL_Event &event)
{
   switch(event.type)
   {
   case SDL_MOUSEBUTTONDOWN:
      // start crossfade immediately
      if (stage==0)
         tickcount = unsigned(core->get_tickrate()*show_time) + 1;
      break;

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

   // draw quad 1 (256 x 200)
   tex1.use(1-(curframe&1));
   double u = tex1.get_tex_u(), v = tex1.get_tex_v();

   glBegin(GL_QUADS);
   glTexCoord2d(0.0, v  ); glVertex2i(  0,  0);
   glTexCoord2d(u  , v  ); glVertex2i(256,  0);
   glTexCoord2d(u  , 0.0); glVertex2i(256,200);
   glTexCoord2d(0.0, 0.0); glVertex2i(  0,200);
   glEnd();

   // draw quad 2 (64 x 200)
   tex2.use(1-(curframe&1));
   u = tex2.get_tex_u(); v = tex2.get_tex_v();

   glBegin(GL_QUADS);
   glTexCoord2d(0.0, v  ); glVertex2i(256,  0);
   glTexCoord2d(u  , v  ); glVertex2i(320,  0);
   glTexCoord2d(u  , 0.0); glVertex2i(320,200);
   glTexCoord2d(0.0, 0.0); glVertex2i(256,200);
   glEnd();

   if (stage==1)
   {
      // render second quad, with alpha blended image

      glEnable(GL_BLEND);
      // render second quad; image to fade out

      // calculate alpha
      Uint8 alpha = 255-Uint8(255*(double(tickcount)/(core->get_tickrate()*xfade_time)));
      glColor4ub(255,255,255,alpha);

      // draw quad 1 (256 x 200)
      tex1.use(curframe&1);
      u = tex1.get_tex_u(); v = tex1.get_tex_v();

      glBegin(GL_QUADS);
      glTexCoord2d(0.0, v  ); glVertex2i(  0,  0);
      glTexCoord2d(u  , v  ); glVertex2i(256,  0);
      glTexCoord2d(u  , 0.0); glVertex2i(256,200);
      glTexCoord2d(0.0, 0.0); glVertex2i(  0,200);
      glEnd();

      // draw quad 2 (64 x 200)
      tex2.use(curframe&1);
      u = tex2.get_tex_u(); v = tex2.get_tex_v();

      glBegin(GL_QUADS);
      glTexCoord2d(0.0, v  ); glVertex2i(256,  0);
      glTexCoord2d(u  , v  ); glVertex2i(320,  0);
      glTexCoord2d(u  , 0.0); glVertex2i(320,200);
      glTexCoord2d(0.0, 0.0); glVertex2i(256,200);
      glEnd();
   }
}

void ua_acknowledgements_screen::tick()
{
   if (stage==0 && double(tickcount)/core->get_tickrate() >= show_time)
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
      ack.get_frame(curframe);

      // split animation in two images
      ua_image img1, img2;
      ack.copy_rect(img1,0,0, 256,200);
      ack.copy_rect(img2,256,0, 64,200);

      // upload textures
      unsigned int texnum = 1-(curframe&1);
      tex1.convert(ack.get_anim_palette(),img1,texnum);
      tex1.use(texnum);
      tex1.upload();

      tex2.convert(ack.get_anim_palette(),img2,texnum);
      tex2.use(texnum);
      tex2.upload();

      return;
   }

   if (stage==1 && double(tickcount)/core->get_tickrate() >= xfade_time)
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
      ua_image img_clear1,img_clear2;

      unsigned int texnum = (curframe&1);

      img_clear1.create(256,200,1);
      tex1.convert(img_clear1,texnum);
      tex1.use(texnum);
      tex1.upload();

      img_clear2.create(64,200,1);
      tex2.convert(img_clear2,texnum);
      tex2.use(texnum);
      tex2.upload();
   }
   ++curframe;
}
