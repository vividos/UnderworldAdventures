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
SDL_Surface *screen;
ua_cutscene cuts;
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

// taken from a SDL tutorial
void DrawPixel(SDL_Surface *screen, int x, int y,
   Uint8 R, Uint8 G, Uint8 B)
{
  Uint32 color = SDL_MapRGB(screen->format, R, G, B);
  switch (screen->format->BytesPerPixel)
  {
    case 1: // Assuming 8-bpp
      {
        Uint8 *bufp;
        bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
        *bufp = color;
      }
      break;
    case 2: // Probably 15-bpp or 16-bpp
      {
        Uint16 *bufp;
        bufp = (Uint16 *)screen->pixels + y*screen->pitch/2 + x;
        *bufp = color;
      }
      break;
    case 3: // Slow 24-bpp mode, usually not used
      {
        Uint8 *bufp;
        bufp = (Uint8 *)screen->pixels + y*screen->pitch + x * 3;
        if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
        {
          bufp[0] = color;
          bufp[1] = color >> 8;
          bufp[2] = color >> 16;
        } else {
          bufp[2] = color;
          bufp[1] = color >> 8;
          bufp[0] = color >> 16;
        }
      }
      break;
    case 4: // Probably 32-bpp
      {
        Uint32 *bufp;
        bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
        *bufp = color;
      }
      break;
  }
}

void draw_screen()
{
   // get frame and palette
   const Uint8 *ptr = cuts.get_frame(currentframe);
   const Uint8 *palette = cuts.get_palette();

   if(SDL_MUSTLOCK(screen))
      SDL_LockSurface(screen);

   // draw pixels to screen
   int width=cuts.get_width();
   int size=width*cuts.get_height();
   for(int i=0; i<size; i++)
   {
      int x=i%width;
      int y=i/width;

      Uint8 index = ptr[i];

      DrawPixel(screen,x,y,palette[index*3+0],palette[index*3+1],palette[index*3+2]);
   }

   if(SDL_MUSTLOCK(screen))
      SDL_UnlockSurface(screen);

   SDL_Flip(screen);

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

   int width = cuts.get_width();
   int height = cuts.get_height();
   int bpp = info->vfmt->BitsPerPixel;

   // set window caption
   SDL_WM_SetCaption("Animation Viewer",NULL);

   // set video mode
   if((screen=SDL_SetVideoMode(width,height,bpp,SDL_HWSURFACE|SDL_DOUBLEBUF))==0)
   {
      fprintf(stderr,"failed to set video mode: %s\n", SDL_GetError());
      return 1;
   }

   Uint32 now,then;
   then = SDL_GetTicks();

   currentframe=0;

   // main loop
   while(!can_exit)
   {
      process_events();

      now = SDL_GetTicks();
      while ((now - then) > (1000.f/tickrate))
      {
         then += Uint32(1000.f/tickrate);

         draw_screen();

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
            sprintf(buffer,"Animation Viewer; frame: %u",currentframe);
            SDL_WM_SetCaption(buffer,NULL);
         }

      }
      SDL_Delay(0);
   }

   // finish off SDL
   SDL_Quit();

   return 0;
}
