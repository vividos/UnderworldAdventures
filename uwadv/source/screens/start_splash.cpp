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
/*! \file start_splash.cpp

   \brief start splash screens

*/

// needed includes
#include "common.hpp"
#include "start_splash.hpp"
#include "start_menu.hpp"
#include "ingame_orig.hpp"


// constants

//! fade in/out time in seconds
const double ua_start_splash_blend_time = 0.8;
//! animation frame rate, in frames per second
const double ua_splash_anim_framerate = 5.0;


// enums

//! action types for the splash screen
enum ua_start_splash_action
{
   ua_fadein,
   ua_showimg,
   ua_fadeout,
   ua_showanim,
   ua_endseq
};


// structs

//! list with splash sequence actions
struct ua_start_splash_sequence
{
   ua_start_splash_action type;
   const char *moreinfo;
} splash_seq[] =
{
   // first image
   { ua_fadein, NULL },
   { ua_showimg, "data/pres1.byt" },
   { ua_fadeout, NULL },

   // second image
   { ua_fadein, NULL },
   { ua_showimg, "data/pres2.byt" },
   { ua_fadeout, NULL },

   // title animation
   { ua_fadein, NULL },
   { ua_showanim, "cuts/cs011.n01" },
   { ua_fadeout, NULL },

   // finished
   { ua_endseq, NULL }
};


// ua_start_splash_screen methods

void ua_start_splash_screen::init()
{
   // start intro midi music
   core->get_audio().start_music(0,true);

   // setup orthogonal projection
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0,320,0,200);
   glMatrixMode(GL_MODELVIEW);

   // set OpenGL flags
   glEnable(GL_TEXTURE_2D);
   glDisable(GL_DEPTH_TEST);

   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

   // load first image
   if (core->get_settings().gtype == ua_game_uw_demo)
   {
      img.load_raw(core->get_settings(),"data/presd.byt",5);

      // write a string under the demo title
      ua_font font;
      ua_image img2;
      font.init(core->get_settings(),ua_font_big);
      font.create_string(img2,"Underworld Adventures",198);

      float scale = 0.9f;
      int xpos=int((320-img2.get_xres()*scale)/2);

      img.paste_image(img2,xpos,200-16);
   }
   else
   {
      img.load_raw(core->get_settings(),splash_seq[1].moreinfo,5);
   }

   img_loaded = true;
   is_animation = false;

   // convert to texture
   tex.convert(core->get_texmgr(),img);
   tex.prepare(false,GL_NEAREST,GL_NEAREST);

   stage=0;
   tickcount=0;
}

void ua_start_splash_screen::done()
{
}

void ua_start_splash_screen::handle_event(SDL_Event &event)
{
   switch(event.type)
   {
   case SDL_KEYDOWN:
   case SDL_MOUSEBUTTONDOWN:
      // when a key or mouse button was pressed, go to next stage
      if (splash_seq[stage].type==ua_fadein || splash_seq[stage].type==ua_showimg ||
          splash_seq[stage].type==ua_showanim)
      {
         stage++;
         tickcount=0;
      }
      break;
   }
}

void ua_start_splash_screen::render()
{
   glLoadIdentity();

   // calculate brightness of texture quad
   Uint8 light = 255;

   switch(splash_seq[stage].type)
   {
   case ua_fadein:
      light = Uint8(255*(float(tickcount) / (core->get_tickrate()*ua_start_splash_blend_time)));
      break;

   case ua_showimg:
   case ua_showanim:
      light=255;
      break;

   case ua_fadeout:
      light = Uint8(255-255*(float(tickcount) / (core->get_tickrate()*ua_start_splash_blend_time)));
      break;

   case ua_endseq:
      light=0;
   }

   glColor3ub(light,light,light);

   if (is_animation)
   {
      // prepare animation frame
      cuts.get_frame(tex,curframe);
      tex.upload(false);
   }
   else
   {
      // prepare image texture
      tex.use(core->get_texmgr());
   }

   double u = tex.get_tex_u(), v = tex.get_tex_v();

   // draw quad with image or animation
   glBegin(GL_QUADS);
   glTexCoord2d(0.0, v  ); glVertex2i(  0,  0);
   glTexCoord2d(u  , v  ); glVertex2i(320,  0);
   glTexCoord2d(u  , 0.0); glVertex2i(320,200);
   glTexCoord2d(0.0, 0.0); glVertex2i(  0,200);
   glEnd();
}

void ua_start_splash_screen::tick()
{
   ++tickcount;

   switch(splash_seq[stage].type)
   {
   case ua_fadein:
      if (!img_loaded)
      {
         if (splash_seq[stage+1].type==ua_showanim)
         {
            // load new animation
            cuts.load(core->get_settings(),splash_seq[stage+1].moreinfo);
            curframe = 0;
            animcount = 0;
            is_animation = true;
         }
         else
         {
            // load new image and texture
            img.load_raw(core->get_settings(),splash_seq[stage+1].moreinfo,5);
            tex.convert(core->get_texmgr(),img);
            tex.prepare(false,GL_NEAREST,GL_NEAREST);
            is_animation = false;
         }
         img_loaded = true;
         tickcount=0;
      }

      if (tickcount >= (core->get_tickrate()*ua_start_splash_blend_time))
      {
         stage++;
         tickcount=0;
      }
      break;

   case ua_fadeout:
      // do next stage when blend time is over
      if (tickcount >= (core->get_tickrate()*ua_start_splash_blend_time))
      {
         if (core->get_settings().gtype == ua_game_uw_demo)
         {
            // when we have the demo, we immediately go to the ingame
            core->replace_screen(new ua_ingame_orig_screen);
            return;
         }

         stage++;
         img_loaded = false;

         // do another tick to load the next one
         ua_start_splash_screen::tick();
      }
      break;

   case ua_showanim:
      break;

   case ua_endseq:
      // start next screen
      core->replace_screen(new ua_start_menu_screen);
      break;
   }

   if (is_animation)
   {
      // check if we have to do a new animation frame
      if (++animcount >= (core->get_tickrate()/ua_splash_anim_framerate))
      {
         // do next frame
         curframe++;
         animcount=0;
         if (curframe>=cuts.get_maxframes()-2)
            curframe=0;
      }
   }
}
