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


// constants

//! time for fade in/out
const double ua_start_menu_blend_time = 0.5;

//! palette shifts per second
const double ua_start_menu_palette_shifts_per_second = 20.0;


// ua_start_menu_screen methods

void ua_start_menu_screen::init()
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
   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

   // load background image
   img.load_raw(core->get_settings(),"data/opscr.byt",2);

   // get palette #2 (needed for palette shifting)
   memcpy(palette,core->get_texmgr().get_palette(2),sizeof(ua_onepalette));

   // load button graphics
   img_buttons.load(core->get_settings(),"opbtn",0,0,2);

   stage=0;
   tickcount=0;
   shiftcount=0;
   journey_avail=true;
   selected_area=-1;
}

void ua_start_menu_screen::done()
{
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
      if (stage==1)
         get_selected_area();
      break;

   case SDL_MOUSEBUTTONUP:
      if (stage==1)
      {
         // determine if user released the mouse button over the same area
         int save = selected_area;
         get_selected_area();
         if (selected_area!=-1 && selected_area == save)
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
      if (i==selected_area) btnnr++;

      img.paste_image(img_buttons.get_image(btnnr),
         btn_coords[i*2],btn_coords[i*2+1]);
   }

   // calculate brightness of texture quad
   Uint8 light = 255;

   switch(stage)
   {
   case 0:
      light = Uint8(255*(float(tickcount) / (core->get_tickrate()*ua_start_menu_blend_time)));
      break;

   case 2:
      light = Uint8(255-255*(float(tickcount) / (core->get_tickrate()*ua_start_menu_blend_time)));
      break;

   case 3:
      light=0;
      break;
   }

   glColor3ub(light,light,light);

   // prepare image texture
   ua_texture_manager &texmgr = core->get_texmgr();
   tex.convert(img,palette);
   tex.upload();

   double u = tex.get_tex_u(), v = tex.get_tex_v();

   // draw background quad
   glBegin(GL_QUADS);
   glTexCoord2d(0.0, v  ); glVertex2i(  0,  0);
   glTexCoord2d(u  , v  ); glVertex2i(320,  0);
   glTexCoord2d(u  , 0.0); glVertex2i(320,200);
   glTexCoord2d(0.0, 0.0); glVertex2i(  0,200);
   glEnd();
}

void ua_start_menu_screen::tick()
{
   // when fading in or out, check if blend time is over
   if ((stage==0 || stage==2) &&
      ++tickcount >= (core->get_tickrate()*ua_start_menu_blend_time))
   {
      // do next stage
      stage++;
      tickcount=0;
   }

   // do palette shifting
   if (++shiftcount>=core->get_tickrate()/ua_start_menu_palette_shifts_per_second)
   {
      shiftcount=0;
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
   switch(selected_area)
   {
   case 0:
      //core->push_screen(new ua_introduction_screen);
      break;

   case 1: // "create character"
      //core->push_screen(new ua_create_char_screen);
      break;

   case 2: // "acknowledgements"
      //core->push_screen(new ua_acknowledgements_screen);
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

void ua_start_menu_screen::get_selected_area()
{
   // get mouse coordinates
   int x,y;
   SDL_GetMouseState(&x,&y);

   unsigned int xpos,ypos;
   xpos = unsigned(double(x)/core->get_screen_width()*320);
   ypos = unsigned(double(y)/core->get_screen_height()*200);

   // check for whole area
   if ((ypos<81 || ypos > 180) || (xpos<64 || xpos>248))
      selected_area=-1;
   else
   if (ypos<104) selected_area=0; else
   if (ypos<128) selected_area=1; else
   if (ypos<153) selected_area=2; else
      selected_area=3;
}
