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
/*! \file animview.cpp

   deluxe paint animation files viewer

*/

// needed includes
#include "common.hpp"
#include "cutscene.hpp"


// globals
bool can_exit = false;
const int tickrate = 8;
ua_cutscene cuts;
ua_texture tex;
unsigned int currentframe;
unsigned int mode=0;


// functions

bool init_anim(const char *filename)
{
   // load cutscene
   try
   {
      cuts.load(filename);
   }
   catch(...)
   {
      return false;
   }

   return true;
}

void done_anim()
{
   tex.done();
}

void setup_opengl(int width,int height)
{
   float ratio = float(width)/height;

   // set up viewport
   glViewport(0,0,width,height);

   // smooth shading
   glShadeModel(GL_SMOOTH);

   // culling
   glCullFace(GL_BACK);
   glFrontFace(GL_CCW);
   glEnable(GL_CULL_FACE);

   // z-buffer
   glDisable(GL_DEPTH_TEST);

   // enable texturing
   glEnable(GL_TEXTURE_2D);

   // clear color
   glClearColor(0,0,0,0);

   // camera setup

   // set projection matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   gluOrtho2D(0.0, 1.0, 0.0, 1.0);

   // switch back to modelview matrix
   glMatrixMode(GL_MODELVIEW);

   // init texture
   tex.init(NULL,1); // no texmgr available
}

void draw_screen()
{
   // clear color and depth buffers
   glClear(GL_COLOR_BUFFER_BIT);

   glLoadIdentity();

   // load texture with current animation frame
   cuts.get_frame(tex,currentframe);

   // upload texture to graphics card
   tex.use();
   tex.upload(false);

   // draw quad
   glBegin(GL_QUADS);
   glColor3ub(255,255,255);

   double u = tex.get_tex_u(), v = tex.get_tex_v();

   glTexCoord2d(0.0, v  ); glVertex3d(0.0, 0.0, 0.0);
   glTexCoord2d(u  , v  ); glVertex3d(1.0, 0.0, 0.0);
   glTexCoord2d(u  , 0.0); glVertex3d(1.0, 1.0, 0.0);
   glTexCoord2d(0.0, 0.0); glVertex3d(0.0, 1.0, 0.0);

   glEnd();

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
      case SDL_KEYDOWN:
         switch(event.key.keysym.sym)
         {
         case SDLK_ESCAPE:
            can_exit=true;
            break;

         case SDLK_SPACE:
            mode=1-mode;
            break;

         case SDLK_RETURN:
            mode=2;
            break;
         }
         break;

      case SDL_QUIT:
         can_exit = true;
         break;
      }
   }
}

int main(int argc, char* argv[])
{
   // check for proper number of arguments
   if (argc!=2)
   {
      printf("Underworld Adventures: Animation Viewer\n");
      printf(" syntax: cutsdec <cutscene-file>\n");
      printf(" example: cutsdec cs011.n01\n");
      return 1;
   }

   // try to load animation
   if (!init_anim(argv[1]))
   {
      printf("Underworld Adventures: Animation Viewer\n");
      printf("could not open/process file %s\n",argv[1]);
      return 1;
   }

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

   // set window caption
   SDL_WM_SetCaption("Underworld Adventures: Animation Viewer",NULL);

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

   setup_opengl(width,height);

   Uint32 now,then;
   then = SDL_GetTicks();

   currentframe=0;

   // main loop
   while(!can_exit)
   {
      process_events();

      draw_screen();

      now = SDL_GetTicks();
      while ((now - then) > (1000.f/tickrate))
      {
         then += Uint32(1000.f/tickrate);

         // next tick
         if (mode==1 || mode==2)
         {
            currentframe++;
            if (currentframe>=cuts.get_maxframes())
               currentframe=0;

            if (mode==2) mode=0;
         }

         // set window caption
         {
            char buffer[256];
            sprintf(buffer,"Underworld Adventures: Animation Viewer; frame: %u",currentframe);
            SDL_WM_SetCaption(buffer,NULL);
         }

      }
      SDL_Delay(0);
   }

   done_anim();

   // finish off SDL
   SDL_Quit();

   return 0;
}
