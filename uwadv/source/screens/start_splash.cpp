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

   start splash screen

*/

// needed includes
#include "common.hpp"
#include "start_splash.hpp"
#include "ingame_orig.hpp"


// constants

const float ua_splash_blend_time = 0.8f;


// ua_start_splash_screen methods

void ua_start_splash_screen::init()
{
   // start midi music
   core->get_audio().start_music(0,true);

   // setup orthogonal projection
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0,320,0,200);
   glMatrixMode(GL_MODELVIEW);

   glEnable(GL_TEXTURE_2D);
   glDisable(GL_DEPTH_TEST);

   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

   // load/create stuff

   font.init(core->get_settings(),ua_font_big);

   if (core->get_settings().gtype == ua_game_uw_demo)
      img.load_raw(core->get_settings(),"data/presd.byt",5);
   else
      img.load_raw(core->get_settings(),"data/opscr.byt",2);

   tex.convert(core->get_texmgr(),img);
   tex.prepare(false,GL_NEAREST,GL_NEAREST);

   font.create_string(img2,"Underworld Adventures",21);
   tex2.convert(core->get_texmgr(),img2);
   tex2.prepare();

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
      // ends start screen
      if (stage==1)
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

   // calculate light'ness of texture quad
   Uint8 light = 255;

   switch(stage)
   {
   case 0:
      light = Uint8(255*(float(tickcount) / (core->get_tickrate()*ua_splash_blend_time)));
      break;

   case 1:
      light=255;
      break;

   case 2:
      light = Uint8(255-255*(float(tickcount) / (core->get_tickrate()*ua_splash_blend_time)));
      break;

   case 3:
      light=0;
   }

   glColor3ub(light,light,light);

   // draw first quad with screen background
   ua_texture_manager &texmgr = core->get_texmgr();
   tex.use(texmgr);

   glBegin(GL_QUADS);
   glTexCoord2d(0.0,tex.get_tex_v());
   glVertex3i(0,0,0);

   glTexCoord2d(tex.get_tex_u(),tex.get_tex_v());
   glVertex3i(320,0,0);

   glTexCoord2d(tex.get_tex_u(),0.0);
   glVertex3i(320,200,0);

   glTexCoord2d(0.0,0.0);
   glVertex3i(0,200,0);

   glEnd();


   if (core->get_settings().gtype == ua_game_uw_demo)
   {
      // above background, draw font text
      glEnable(GL_BLEND);
      tex2.use(texmgr);

      float scale = 0.9f;
      int xpos=int((320-img2.get_xres()*scale)/2), ypos=3;

      glBegin(GL_QUADS);
      glTexCoord2d(0.0,tex2.get_tex_v());
      glVertex3i(xpos,ypos,0);

      glTexCoord2d(tex2.get_tex_u(),tex2.get_tex_v());
      glVertex3i(int(xpos+img2.get_xres()*scale),ypos,0);

      glTexCoord2d(tex2.get_tex_u(),0.0);
      glVertex3i(int(xpos+img2.get_xres()*scale),int(ypos+img2.get_yres()*scale),0);

      glTexCoord2d(0.0,0.0);
      glVertex3i(xpos,int(ypos+img2.get_yres()*scale),0);

      glEnd();

      glDisable(GL_BLEND);
   }
}

void ua_start_splash_screen::tick()
{
   switch(stage)
   {
   case 0:
   case 2:
      if (++tickcount >= (core->get_tickrate()*ua_splash_blend_time))
         stage++;
      break;

   case 3:
      // start next screen
      core->replace_screen(new ua_ingame_orig_screen);
      break;
   }
}
