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
/*! \file ingame_orig.cpp

   a

*/

// needed includes
#include "common.hpp"
#include "ingame_orig.hpp"
#include <cmath>


// ua_ingame_orig_screen methods

void ua_ingame_orig_screen::init()
{
   uworld.init(core->get_settings(),core->get_texmgr());

   setup_opengl();

   // pos values
   xangle = 0.f;
   yangle = 0.f;
   xpos = 0.f;
   ypos = 0.f;

   playerx = 31.5;
   playery = 1.5;
   playerxangle = 0.f;
   playeryangle = 270.f;

   mapmode = true;

   leftbuttondown = rightbuttondown = false;
}

void ua_ingame_orig_screen::done()
{
   uworld.done();
}

void ua_ingame_orig_screen::handle_event(SDL_Event &event)
{
   switch(event.type)
   {
   case SDL_KEYDOWN:
      // handle key presses
      handle_key_down(event.key.keysym);
      break;

   case SDL_MOUSEMOTION:
   case SDL_MOUSEBUTTONDOWN:
   case SDL_MOUSEBUTTONUP:
      handle_mouse_action(event);
      break;
   }
}

void ua_ingame_orig_screen::handle_key_down(SDL_keysym &keysym)
{
   switch(keysym.sym)
   {
   case SDLK_PAGEUP:
//      if (curlevel>0)
//         uworld.move_player(0.f,0.f,--curlevel);
      break;

   case SDLK_PAGEDOWN:
//      if (curlevel<8)
//         uworld.move_player(0.f,0.f,++curlevel);
      break;

   case SDLK_UP:
      if (!mapmode)
      {
         playerx += 0.5*cos((90.0-playerxangle)/180*3.1415);
         playery += 0.5*sin((90.0-playerxangle)/180*3.1415);
      }
      break;
   case SDLK_DOWN:
      if (!mapmode)
      {
         playerx += 0.5*cos((270.0-playerxangle)/180*3.1415);
         playery += 0.5*sin((270.0-playerxangle)/180*3.1415);
      }
      break;
   case SDLK_RIGHT:
      if (!mapmode)
      {
         playerx += 0.5*cos((0.0-playerxangle)/180*3.1415);
         playery += 0.5*sin((0.0-playerxangle)/180*3.1415);
      }
      break;

   case SDLK_LEFT:
      if (!mapmode)
      {
         playerx += 0.5*cos((180.0-playerxangle)/180*3.1415);
         playery += 0.5*sin((180.0-playerxangle)/180*3.1415);
      }
      break;

   case SDLK_m:
      mapmode = !mapmode;
      break;
   }
}

void ua_ingame_orig_screen::handle_mouse_action(SDL_Event &event)
{
   switch(event.type)
   {
   case SDL_MOUSEMOTION:
      // mouse has moved
      if (leftbuttondown)
      {
         int x,y;
         SDL_GetRelativeMouseState(&x,&y);
         if (!mapmode)
         {
            playerxangle -= x*0.2;
            playeryangle += y*0.2;
         }
         else
         {
            xangle -= x*0.5;
            yangle += y*0.5;
         }
      }
      else
      if (rightbuttondown)
      {
         int x,y;
         SDL_GetRelativeMouseState(&x,&y);
         xpos += x*0.1;
         ypos += y*0.1;
      }
      break;

   case SDL_MOUSEBUTTONDOWN:
      {
         Uint8 state = SDL_GetRelativeMouseState(NULL,NULL);
         if (SDL_BUTTON(state)==SDL_BUTTON_LEFT)
         {
            leftbuttondown = true;
            rightbuttondown = false;
         }
         else
         {
            leftbuttondown = false;
            rightbuttondown = true;
         }
      }
      break;

   case SDL_MOUSEBUTTONUP:
      leftbuttondown = false;
      rightbuttondown = false;
      break;
   }
}

void ua_ingame_orig_screen::render()
{
   // clear color and depth buffers
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   glLoadIdentity();

   if (mapmode)
   {
      // position move
      glTranslatef( xpos+0.0, 8.0, -40.0+ypos );

      // rotation
      glRotatef( yangle, 1.0, 0.0, 0.0 );
      glRotatef( -xangle, 0.0, 0.0, 1.0 );

      // move to center of map
      glTranslatef( -32.0, -32.0, 0.0 );
   }
   else
   {
      // rotation
      glRotatef( playeryangle, 1.0, 0.0, 0.0 );
      glRotatef( playerxangle, 0.0, 0.0, 1.0 );

      // move to position on map
      glTranslatef( -(playerx+0.5), -(playery+0.5),
         -(uworld.get_player_height(playerx+0.5,playery+0.5)) );
   }

   // draw ground square
   glBegin(GL_QUADS);
   glColor3ub(32,32,32);
   glVertex3f(0,0,-0.1f);
   glVertex3f(64,0,-0.1f);
   glVertex3f(64,64,-0.1f);
   glVertex3f(0,64,-0.1f);
   glEnd();

   uworld.render(core->get_texmgr());
}

int tickcount=0;

void ua_ingame_orig_screen::tick()
{
/*   if (tickcount++>200)
   {
      core->pop_screen();
   }*/
   xangle+=1.f;
}

void ua_ingame_orig_screen::setup_opengl()
{
   // smooth shading
   glShadeModel(GL_SMOOTH);

   // culling
   glCullFace(GL_BACK);
   glFrontFace(GL_CCW);
   glEnable(GL_CULL_FACE);

   // z-buffer
   glEnable(GL_DEPTH_TEST);

   // enable texturing
   glEnable(GL_TEXTURE_2D);

   // alpha blending
   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

   // clear color
   glClearColor(0, 0, 0, 0);
}
