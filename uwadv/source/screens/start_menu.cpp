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
/*! \file start_menu.cpp

   \brief menu at game start

*/

// needed includes
#include "common.hpp"
#include "start_menu.hpp"
#include "ingame_orig.hpp"
#include "cutscene_view.hpp"
#include "acknowledgements.hpp"
#include "create_character.hpp"


// constants

//! time to fade in/out
const double ua_start_menu_screen::fade_time = 0.5;

//! palette shifts per second
const double ua_start_menu_screen::palette_shifts_per_second = 20.0;


// ua_start_menu_screen methods

void ua_start_menu_screen::init()
{
   ua_trace("start menu screen started\n");

   // load background image
   img.load_raw(core->get_settings(),"data/opscr.byt",2);

   // get palette #2 (needed for palette shifting)
   memcpy(palette,core->get_texmgr().get_palette(2),sizeof(ua_onepalette));

   // load button graphics
   img_buttons.load(core->get_settings(),"opbtn",0,0,2);

   // init mouse cursor
   mousecursor = new ua_mousecursor(core);

   resume();
}

void ua_start_menu_screen::suspend()
{
   tex.done();
}

void ua_start_menu_screen::resume()
{
   // setup orthogonal projection
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0,320,0,200);
   glMatrixMode(GL_MODELVIEW);

   // set OpenGL flags
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,0);

   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

   // init texture
   tex.init(&core->get_texmgr());
   tex.convert(img);
   tex.use();
   tex.upload();

   mousecursor->show(true);

   stage = 0;
   tickcount = 0;
   journey_avail = core->get_savegames_mgr().get_savegames_count()>0;
   buttondown = false;
   selected_area = -1;
   shiftcount=0.0;
}

void ua_start_menu_screen::done()
{
   delete mousecursor;

   suspend();

   // clear screen
   glClearColor(0,0,0,0);
   glClear(GL_COLOR_BUFFER_BIT);
   SDL_GL_SwapBuffers();
}

void ua_start_menu_screen::handle_event(SDL_Event &event)
{
   switch(event.type)
   {
   case SDL_KEYDOWN:
      // handle key presses
      switch(event.key.keysym.sym)
      {
      case SDLK_UP:
         // select the area above, if possible
         if (selected_area==-1) selected_area=0;
         if (selected_area>0) selected_area--;
         break;

      case SDLK_DOWN:
         // select the area below, if possible
         if (selected_area+1<(journey_avail?4:3))
            selected_area++;
         break;

      case SDLK_RETURN:
         // simulate clicking on that area
         if (stage==1)
         {
            stage++;
            tickcount=0;
         }
         break;
      }
      break;

   case SDL_MOUSEBUTTONDOWN:
      // select the area where the mouse button is pressed
      buttondown=true;
      if (stage==1)
         selected_area = get_selected_area();
      break;

   case SDL_MOUSEMOTION:
      mousecursor->updatepos();
      if (stage==1 && buttondown)
      {
         int ret = get_selected_area();
         if (ret!=-1)
            selected_area = ret;
      }
      break;

   case SDL_MOUSEBUTTONUP:
      buttondown=false;
      if (stage==1)
      {
         // determine if user released the mouse button over the same area
         int ret = get_selected_area();
         if (ret != -1 && ret == selected_area)
         {
            stage++;
            tickcount=0;
         }
      }
      break;
   }
}

void ua_start_menu_screen::render()
{
   glClear(GL_COLOR_BUFFER_BIT);
   glLoadIdentity();

   // combine button graphics with background image
   const unsigned int btn_coords[] =
   {
      98,81, 81,104, 72,128, 85,153
   };

   unsigned int max = journey_avail? 4 : 3;
   for(unsigned int i=0; i<max; i++)
   {
      unsigned int btnnr = i*2;
      if (int(i)==selected_area) btnnr++;

      img.paste_image(img_buttons.get_image(btnnr),
         btn_coords[i*2],btn_coords[i*2+1]);
   }

   // calculate brightness of texture quad
   Uint8 light = 255;

   switch(stage)
   {
   case 0:
      light = Uint8(255*(double(tickcount) / (core->get_tickrate()*fade_time)));
      break;

   case 2:
      light = Uint8(255-255*(double(tickcount) / (core->get_tickrate()*fade_time)));
      break;

   case 3:
      light=0;
      break;
   }

   glColor3ub(light,light,light);

   // prepare image texture
   tex.convert(palette,img);
   tex.use();
   tex.upload();

   double u = tex.get_tex_u(), v = tex.get_tex_v();

   // draw background quad
   glBegin(GL_QUADS);
   glTexCoord2d(0.0, v  ); glVertex2i(  0,  0);
   glTexCoord2d(u  , v  ); glVertex2i(320,  0);
   glTexCoord2d(u  , 0.0); glVertex2i(320,200);
   glTexCoord2d(0.0, 0.0); glVertex2i(  0,200);
   glEnd();

   mousecursor->draw();
   tex.use();
}

void ua_start_menu_screen::tick()
{
   // when fading in or out, check if blend time is over
   if ((stage==0 || stage==2) &&
      ++tickcount >= (core->get_tickrate()*fade_time))
   {
      // do next stage
      stage++;
      tickcount=0;
   }

   // do palette shifting
   shiftcount += 1.0/core->get_tickrate();
   if (shiftcount >= 1.0/palette_shifts_per_second)
   {
      shiftcount -= 1.0/palette_shifts_per_second;

      // shift palette
      Uint8 saved[4];
      memcpy(saved,palette[127],4);
      memmove(palette[65],palette[64],(127-64)*4);
      memcpy(palette[64],saved,4);
   }

   // in stage 3, we really press the selected button
   if (stage==3)
   {
      press_button();
      stage=0;
      tickcount=0;
   }
}

void ua_start_menu_screen::press_button()
{
   // clear screen
   glClearColor(0,0,0,0);
   glClear(GL_COLOR_BUFFER_BIT);
   SDL_GL_SwapBuffers();

   switch(selected_area)
   {
   case 0: // "introduction"
      core->push_screen(new ua_cutscene_view_screen(0));
      break;

   case 1: // "create character"
      core->push_screen(new ua_create_character_screen);
      break;

   case 2: // "acknowledgements"
      core->push_screen(new ua_acknowledgements_screen);
      break;

   case 3: // "journey onward"
      if (journey_avail)
      {
         // ask for game to open
         // load game
         core->push_screen(new ua_ingame_orig_screen);
      }
      break;
   }
}

int ua_start_menu_screen::get_selected_area()
{
   // get mouse coordinates
   int x,y;
   SDL_GetMouseState(&x,&y);

   unsigned int xpos,ypos;
   xpos = unsigned(double(x)/core->get_screen_width()*320);
   ypos = unsigned(double(y)/core->get_screen_height()*200);

   // check for whole area
   int ret;
   if ((ypos<81 || ypos > 180) || (xpos<64 || xpos>248))
      ret=-1;
   else if (ypos<104) ret=0;
   else if (ypos<128) ret=1;
   else if (ypos<153) ret=2;
   else
      if (journey_avail) ret=3; else ret=-1;

   return ret;
}
