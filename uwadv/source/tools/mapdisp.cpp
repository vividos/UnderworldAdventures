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
/*! \file mapdisp.cpp

   level map display program

   the program's executable must be placed in the folder where the uw.exe
   lives. uw_demo works, too. the executable needs the SDL.dll, placed into
   the same dir as itself.

   the user can rotate the map by dragging, holding the left mouse button.
   when dragging with the right mouse button, the map is moved on the screen.
   with the cursor up and down keys the distance to the viewpoint can be
   adjusted. finally, page-up and page-down loads another map level.

*/

// needed includes
#include "common.hpp"
#include "underworld.hpp"


// globals

static bool can_exit = false;
static double xpos = 0.0;
static double ypos = 0.0;
static double zpos = 32.0;
static double xangle = 0.0;
static double yangle = 0.0;
static bool leftbuttondown = false;
static bool rightbuttondown = false;

static int curlevel=0;
static std::vector<ua_level> levels;
static ua_texture_manager texmgr;
static ua_settings settings;


// prototypes

// levelmap loader, defined in resource/maploader.cpp
void ua_import_levelmaps(ua_settings &settings, const char *folder,
   std::vector<ua_level> &levels);


// functions

void load_level(int level)
{
   // load new level
   curlevel=level;
   levels[curlevel].prepare_textures(texmgr);
}

void init_mapdisp()
{
   // we don't load settings, just set the needed one
   settings.gtype = ua_game_uw1;
   settings.uw1_path = "./";

   // check if we only have the demo
   FILE *fd = fopen("./data/level13.st","rb");
   if (fd!=NULL)
   {
      settings.gtype = ua_game_uw_demo;
      fclose(fd);
   }

   // init texture manager
   texmgr.init(settings);

   ua_import_levelmaps(settings,"data/",levels);

   // load first level
   load_level(0);
}

void handle_key_down(SDL_keysym* keysym)
{
   switch(keysym->sym)
   {
   case SDLK_ESCAPE:
      can_exit = true;
      break;

   case SDLK_PAGEUP:
      if (curlevel>0)
         load_level(curlevel-1);
      break;

   case SDLK_PAGEDOWN:
      if (curlevel<levels.size()-1)
         load_level(curlevel+1);
      break;

   case SDLK_UP:
      zpos+=2.0;
      break;

   case SDLK_DOWN:
      zpos-=2.0;
      break;

   default:
       break;
   }
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
         handle_key_down( &event.key.keysym );
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

      case SDL_QUIT:
         can_exit = true;
         break;
      }
   }
}

void draw_screen()
{
   // clear color and depth buffers
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glLoadIdentity();

   // position move
   glTranslated( xpos, -ypos, -zpos );

   // rotation
   glRotated( yangle, 1.0, 0.0, 0.0 );
   glRotated( xangle, 0.0, 0.0, 1.0 );

   // move to center of map
   glTranslated( -32.0, -32.0, 0.0 );

   // draw ground square
   glBegin(GL_QUADS);
   glColor3ub(32,32,32);
   glVertex3d(0,0,-0.1);
   glVertex3d(64,0,-0.1);
   glVertex3d(64,64,-0.1);
   glVertex3d(0,64,-0.1);
   glEnd();

   // render map
   levels[curlevel].render(texmgr);

   SDL_GL_SwapBuffers();
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
   glEnable(GL_DEPTH_TEST);

   // enable texturing
   glEnable(GL_TEXTURE_2D);

   // clear color
   glClearColor(0,0,0,0);

   // camera setup

   // set projection matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   gluPerspective(90.0, ratio, 0.25, 256.0);
//   glOrtho(-32.0*ratio,32.0*ratio, -32.0,32.0, 0.25, 256.0);

   // switch back to modelview matrix
   glMatrixMode(GL_MODELVIEW);
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

   // set window caption
   SDL_WM_SetCaption("Underworld Adventures: Map Display",NULL);

   // set OpenGL video attributes
   SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
   SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

   // set video mode
   if(SDL_SetVideoMode(width,height,bpp,SDL_OPENGL)==0)
   {
      fprintf(stderr,"failed to set video mode: %s\n", SDL_GetError());
      return 1;
   }

   // init more stuff
   setup_opengl(width,height);
   init_mapdisp();

   Uint32 now, fcstart = SDL_GetTicks();
   unsigned int renders=0;

   // main loop
   while(!can_exit)
   {
      process_events();
      draw_screen();
      renders++;

      now = SDL_GetTicks();

      if (now-fcstart > 2000)
      {
         // set new caption
         char buffer[256];
         sprintf(buffer,"Underworld Adventures: Map Display; %3.1f frames/s",
            renders*1000.f/(now-fcstart));

         SDL_WM_SetCaption(buffer,NULL);

         // restart counting
         renders = 0;
         fcstart = now;
      }
   }

   // finish off SDL
   SDL_Quit();

   return 0;
}
