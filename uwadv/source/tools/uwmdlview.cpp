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
/*! \file uwmdlview.cpp

   \brief underworld 3d model viewer

*/

// needed includes
#include "common.hpp"
#include "models.hpp"

// globals

std::vector<ua_model3d_ptr> allmodels;
unsigned int curmodel = 1;
bool can_exit = false;
bool light_on = false;
bool force_fps_update = false;

double xpos = 0.0;
double ypos = 0.0;
double zpos = 1.6;
double xangle = 30.0;
double yangle = -60.0;
bool leftbuttondown = false;
bool rightbuttondown = false;


// external functions

extern bool ua_model_decode_builtins(const char* filename,
   std::vector<ua_model3d_ptr>& allmodels);


// functions

void uwmdlview_init()
{
   if (ua_file_exists("./uw.exe"))
      ua_model_decode_builtins("./uw.exe",allmodels);
   else
   if (ua_file_exists("./uwdemo.exe"))
      ua_model_decode_builtins("./uwdemo.exe",allmodels);
   else
   if (ua_file_exists("./uw2.exe"))
      ua_model_decode_builtins("./uw2.exe",allmodels);
}

void setup_opengl(unsigned int width, unsigned int height)
{
   // OpenGL setup

   float ratio = float(width)/height;
   // set up viewport
   glViewport(0,0,width,height);

   // smooth shading
   glShadeModel(GL_SMOOTH);

   // culling
   glCullFace(GL_BACK);
   glFrontFace(GL_CCW);
   glDisable(GL_CULL_FACE);

   // z-buffer
   glEnable(GL_DEPTH_TEST);

   // clear color
   glClearColor(0,0,0,0);

   // camera setup

   // set projection matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   gluPerspective(90.0, ratio, 0.5, 256.0);

   // switch back to modelview matrix
   glMatrixMode(GL_MODELVIEW);

   // set light source
   glEnable(GL_LIGHT0);

   GLfloat pos_light0[4] = { 2.0, 1.0, 5.0, 0.0 };
   glLightfv(0, GL_POSITION, pos_light0);
}

void draw_screen()
{
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   glLoadIdentity();

   // position move
   glTranslated( xpos, -ypos, -zpos );

   // rotation
   glRotated( yangle, 1.0, 0.0, 0.0 );
   glRotated( xangle, 0.0, 0.0, 1.0 );

   // draw ground square
   glBegin(GL_QUADS);
   glColor3ub(32,32,32);

   glVertex3d(-1.0,-1.0,-0.01);
   glVertex3d( 1.0,-1.0,-0.01);
   glVertex3d( 1.0, 1.0,-0.01);
   glVertex3d(-1.0, 1.0,-0.01);
   glEnd();

   if (light_on)
      glEnable(GL_LIGHTING);

   // render model
   ua_vector3d base(0.0, 0.0, 0.0);
   allmodels[curmodel]->render(base);

   if (light_on)
      glDisable(GL_LIGHTING);

   SDL_GL_SwapBuffers();
}

void process_events()
{
   SDL_Event event;

   // check for new event
   while(SDL_PollEvent(&event))
   {
      switch(event.type)
      {
      case SDL_QUIT:
         can_exit = true;
         break;

      case SDL_KEYDOWN:
         switch(event.key.keysym.sym)
         {
         case SDLK_PAGEUP:
            if (curmodel<31)
            {
               curmodel++;
               force_fps_update = true;
            }
            break;

         case SDLK_PAGEDOWN:
            if (curmodel>0)
            {
               curmodel--;
               force_fps_update = true;
            }
            break;

         case SDLK_UP:
            zpos+=0.25;
            break;

         case SDLK_DOWN:
            zpos-=0.25;
            break;

         case SDLK_l:
            light_on = !light_on;
            break;

		 default:
			 break;
         }
         break;

      case SDL_MOUSEBUTTONDOWN:
         {
            // check which mouse button was pressed
            Uint8 state = SDL_GetRelativeMouseState(NULL,NULL);
            if (SDL_BUTTON(state)==SDL_BUTTON_LEFT)
               leftbuttondown = true;
            else
               rightbuttondown = true;
         }
         break;

      case SDL_MOUSEBUTTONUP:
         leftbuttondown = false;
         rightbuttondown = false;
         break;

      case SDL_MOUSEMOTION:
         {
            // adjust values according to mouse movement
            int x,y;
            SDL_GetRelativeMouseState(&x,&y);
            if (leftbuttondown)
            {
               xangle += x*0.2;
               yangle += y*0.2;
            }
            else
            if (rightbuttondown)
            {
               xpos += x*0.2;
               ypos += y*0.2;
            }
         }
         break;
      }
   }
}

int main(int argc, char* argv[])
{
   // init SDL
   if(SDL_Init(SDL_INIT_VIDEO)<0)
   {
      fprintf(stderr,"error initializing video: %s\n", SDL_GetError());
      return 1;
   }

   // get info about video
   const SDL_VideoInfo* info = SDL_GetVideoInfo();
   if(!info)
   {
      fprintf(stderr,"error getting video info: %s\n", SDL_GetError());
      return 1;
   }

   int width = 640;
   int height = 480;
   int bpp = info->vfmt->BitsPerPixel;

   // set OpenGL video attributes
   SDL_GL_SetAttribute(SDL_GL_RED_SIZE,5);
   SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,5);
   SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,5);
   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

   // set video mode
   if(SDL_SetVideoMode(width,height,bpp,SDL_OPENGL)==0)
   {
      fprintf(stderr,"failed to set video mode: %s\n", SDL_GetError());
      return 1;
   }


   uwmdlview_init();

   setup_opengl(width,height);


   // main loop
   Uint32 now, fcstart = SDL_GetTicks();
   unsigned int renders=0;

   // main loop
   while(!can_exit)
   {
      process_events();
      draw_screen();
      renders++;

      now = SDL_GetTicks();

      if (now-fcstart > 2000 || force_fps_update)
      {
         // set new caption
         char buffer[256];
         sprintf(buffer,"underworld model viewer - current model: %u - %3.1f frames/s",
            curmodel,renders*1000.f/(now-fcstart));

         SDL_WM_SetCaption(buffer,NULL);

         // restart counting
         renders = 0;
         fcstart = now;
         force_fps_update = false;
      }
   }

   // finish off SDL
   SDL_Quit();

   return 0;
}


// fake functions to get linking to work

#include "files.hpp"

SDL_RWops* ua_files_manager::get_uadata_file(const char* relpath)
{
   return NULL;
}
