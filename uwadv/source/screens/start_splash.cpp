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

   \brief start splash screens implementation

   stages the ua_start_splash_screen can be in:

   stage 0: first opening screen
   stage 1: second opening screen (not in uw_demo)
   stage 2: fading in animation
   stage 3: showing animation
   stage 4: fading out animation
   stage 5: screen finished

*/

// needed includes
#include "common.hpp"
#include "start_splash.hpp"
#include "start_menu.hpp"
#include "ingame_orig.hpp"


// constants

//! number of seconds the splash screen images are shown
const double ua_start_splash_show_time = 4.0;

//! fade in/out time in seconds
const double ua_start_splash_blend_time = 0.8;

//! animation frame rate, in frames per second
const double ua_splash_anim_framerate = 5.0;


// ua_start_splash_screen methods

void ua_start_splash_screen::init()
{
   ua_trace("start splash screen started\n");

   // start intro midi music
   core->get_audio().start_music(0,false);

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

   // load first image
   ua_image img;

   const char *first_img_name = "data/pres1.byt";
   if (core->get_settings().get_gametype() == ua_game_uw_demo)
      first_img_name = "data/presd.byt";

   img_still.load_raw(core->get_settings(),first_img_name,5);
   img_still.init(&core->get_texmgr(),0,0,320,200);

   if (core->get_settings().get_gametype() == ua_game_uw_demo)
   {
      // write a string under the demo title
      ua_font font;
      ua_image img_temp;
      font.init(core->get_settings(),ua_font_big);
      font.create_string(img_temp,"Underworld Adventures",198);

      double scale = 0.9;
      unsigned int xpos = unsigned((320-img_temp.get_xres()*scale)/2);

      img_still.paste_image(img_temp,xpos,200-16);
   }

   img_still.convert_upload();

   // split image in two images
   stage = 0;
   tickcount = 0;
   curframe = 0;
   animcount = 0.0;

   // leave out first two screens when we have savegames
   if (core->get_settings().get_gametype() != ua_game_uw_demo &&
       core->get_savegames_mgr().get_savegames_count()>0)
   {
      stage=1;
      tickcount = unsigned(ua_start_splash_show_time * core->get_tickrate()) + 1;
      tick();
   }
}

void ua_start_splash_screen::done()
{
   img_still.done();
   cuts_anim.done();
}

void ua_start_splash_screen::handle_event(SDL_Event &event)
{
   switch(event.type)
   {
   case SDL_KEYDOWN:
   case SDL_MOUSEBUTTONDOWN:
      // when a key or mouse button was pressed, go to next stage
      switch(stage)
      {
      case 0:
      case 1:
         tickcount = unsigned(ua_start_splash_show_time * core->get_tickrate()) + 1;
         break;

      case 2:
         stage=4;
         tickcount = unsigned(ua_start_splash_blend_time * core->get_tickrate()) - tickcount;
         break;

      case 3:
         stage++;
         tickcount=0;

         // fade out music when we have the demo (ingame starts after this)
         if (core->get_settings().get_gametype() == ua_game_uw_demo)
            core->get_audio().fadeout_music(ua_start_splash_blend_time);

         break;
      }
      break;
   }
}

void ua_start_splash_screen::render()
{
   // calculate brightness of texture quad
   Uint8 light = 255;

   switch(stage)
   {
      // anim fade-in
   case 2:
      light = Uint8(255*(double(tickcount) / (core->get_tickrate()*ua_start_splash_blend_time)));
      break;

      // still image / anim
   case 0:
   case 1:
   case 3:
      light=255;
      break;

      // anim fade-out
   case 4:
      light = Uint8(255-255*(double(tickcount) / (core->get_tickrate()*ua_start_splash_blend_time)));
      break;

      // finished
   case 5:
      light=0;
      break;
   }

   glColor3ub(light,light,light);

   if (stage>=2)
   {
      // prepare and convert animation frame
      cuts_anim.get_frame(curframe);
      cuts_anim.convert_upload();

      // render quad
      cuts_anim.render();
   }
   else
   {
      img_still.render();
   }
}

void ua_start_splash_screen::tick()
{
   tickcount++;

   // check if animation should be loaded
   if ( (stage == 1 || (stage == 0 && core->get_settings().get_gametype() == ua_game_uw_demo)) &&
      tickcount >= ua_start_splash_show_time * core->get_tickrate())
   {
      // load animation
      cuts_anim.load(core->get_settings(),"cuts/cs011.n01");
      cuts_anim.init(&core->get_texmgr());

      curframe = 0;
      animcount = 0.0;
      stage = 2;
      tickcount = 0;
   }

   // check other stages
   switch(stage)
   {
   case 0:
      if (tickcount >= ua_start_splash_show_time * core->get_tickrate())
      {
         // load second image
         img_still.load_raw(core->get_settings(),"data/pres2.byt",5);
         img_still.convert_upload();

         stage++;
         tickcount=0;
      }
      break;

      // fade-in / out
   case 2:
   case 4:
      if (tickcount >= ua_start_splash_blend_time * core->get_tickrate())
      {
         stage++;
         tickcount=0;
         break;
      }
      // no break, fall through

      // animation
   case 3:
      // check if we have to do a new animation frame
      animcount += 1.0/core->get_tickrate();
      if (animcount >= 1.0/ua_splash_anim_framerate)
      {
         // do next frame
         curframe++;
         animcount -= 1.0/ua_splash_anim_framerate;
         if (curframe>=cuts_anim.get_maxframes()-2)
            curframe=0;
      }
      break;

      // finished
   case 5:
      // start next screen
      if (core->get_settings().get_gametype() == ua_game_uw_demo)
      {
         // when we have the demo, we immediately go to the ingame
         core->get_underworld().import_savegame(core->get_settings(),"data/",true);
         core->get_underworld().get_scripts().lua_started_newgame();
         core->replace_screen(new ua_ingame_orig_screen);
         return;
      }
      else
         core->replace_screen(new ua_start_menu_screen);
      break;
   }
}
