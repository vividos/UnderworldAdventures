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
#include "audio.hpp"
#include "savegame.hpp"
#include "renderer.hpp"
#include "start_menu.hpp"
//#include "ingame_orig.hpp"


// constants

//! number of seconds the splash screen images are shown
const double ua_start_splash_screen::show_time = 4.0;

//! fade in/out time in seconds
const double ua_start_splash_screen::blend_time = 0.8;

//! animation frame rate, in frames per second
const double ua_start_splash_screen::anim_framerate = 5.0;


// ua_start_splash_screen methods

void ua_start_splash_screen::init()
{
   ua_screen::init();

   ua_trace("start splash screen started\n");

   // start intro midi music
   game->get_audio_manager().start_music(0,false);

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
   ua_trace("loading first image\n");
   ua_image img;

   const char* first_img_name = "data/pres1.byt";
   if (game->get_settings().get_gametype() == ua_game_uw_demo)
      first_img_name = "data/presd.byt";

   img_still.load_raw(game->get_settings(),first_img_name,5);
   img_still.init(&game->get_renderer().get_texture_manager(),0,0,320,200);

   if (game->get_settings().get_gametype() == ua_game_uw_demo)
   {
      // write a string under the demo title
      ua_font font;
      ua_image img_temp;
      font.load(game->get_settings(),ua_font_big);
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
   if (game->get_settings().get_gametype() != ua_game_uw_demo &&
       game->get_savegames_manager().get_savegames_count()>0)
   {
      ua_trace("skipping images (savegames available)\n");

      stage=1;
      tickcount = unsigned(show_time * game->get_tickrate()) + 1;
      tick();
   }

   // switch on cursor for this screen
   SDL_ShowCursor(1);
}

void ua_start_splash_screen::destroy()
{
   ua_screen::destroy();

   img_still.done();
   cuts_anim.done();

   SDL_ShowCursor(0);

   ua_trace("start splash screen ended\n\n");
}

void ua_start_splash_screen::draw()
{
   ua_screen::draw();

   // calculate brightness of texture quad
   Uint8 light = 255;

   switch(stage)
   {
      // anim fade-in/fade-out
   case 2:
   case 4:
      light = fader.get_fade_value();
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
      // render still image
      img_still.render();
   }
}

bool ua_start_splash_screen::process_event(SDL_Event& event)
{
   bool ret = false;

   switch(event.type)
   {
   case SDL_KEYDOWN:
   case SDL_MOUSEBUTTONDOWN:
      // when a key or mouse button was pressed, go to next stage
      switch(stage)
      {
      case 0: // first or second image
      case 1:
         tickcount = unsigned(show_time * game->get_tickrate()) + 1;
         ret = true;
         break;

      case 2: // fading in animation
         {
            stage=4;

            // init fadeout
            fader.init(false,game->get_tickrate(),blend_time);

            // "simulate" the number of ticks we already did
            for(unsigned int i=0; i<tickcount; i++)
               fader.tick();

            tickcount = unsigned(blend_time * game->get_tickrate()) - tickcount;

            ret = true;
         }
         break;

      case 3: // showing animation
         stage++;
         tickcount=0;

         // fade out music when we have the demo (ingame starts after this)
         if (game->get_settings().get_gametype() == ua_game_uw_demo)
            game->get_audio_manager().fadeout_music(blend_time);

         // init fadeout
         fader.init(false,game->get_tickrate(),blend_time);

         ret = true;
         break;
      }
      break;
   }

   return ret;
}

void ua_start_splash_screen::tick()
{
   ua_screen::tick();

   tickcount++;

   // check if animation should be loaded
   if ( (stage == 1 || (stage == 0 && game->get_settings().get_gametype() == ua_game_uw_demo)) &&
      tickcount >= show_time * game->get_tickrate())
   {
      ua_trace("loading animation\n");

      // load animation
      cuts_anim.load(game->get_settings(),"cuts/cs011.n01");
      cuts_anim.init(&game->get_renderer().get_texture_manager());

      curframe = 0;
      animcount = 0.0;
      stage = 2;
      tickcount = 0;

      // init fadein
      fader.init(true,game->get_tickrate(),blend_time);
   }

   // check other stages
   switch(stage)
   {
   case 0:
      if (tickcount >= show_time * game->get_tickrate())
      {
         ua_trace("loading second image\n");

         // load second image
         img_still.load_raw(game->get_settings(),"data/pres2.byt",5);
         img_still.convert_upload();

         stage++;
         tickcount=0;
      }
      break;

      // fade-in / out
   case 2:
   case 4:
      if (fader.tick())
      {
         stage++;
         tickcount=0;
         break;
      }
      // no break, fall through

      // animation
   case 3:
      // check if we have to do a new animation frame
      animcount += 1.0/game->get_tickrate();
      if (animcount >= 1.0/anim_framerate)
      {
         // do next frame
         curframe++;
         animcount -= 1.0/anim_framerate;
         if (curframe>=cuts_anim.get_maxframes()-2)
            curframe=0;
      }
      break;

      // finished
   case 5:
      // start next screen
      if (game->get_settings().get_gametype() == ua_game_uw_demo)
      {
         // when we have the demo, we immediately go to the ingame screen
         //game->get_underworld().import_savegame(core->get_settings(),"data/",true);
         //game->get_underworld().get_scripts().lua_started_newgame();
         //game->replace_screen(new ua_ingame_orig_screen);
         return;
      }
      else
         game->replace_screen(new ua_start_menu_screen,false);
      break;
   }
}
