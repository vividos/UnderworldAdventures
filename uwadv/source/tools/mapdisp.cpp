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
#include "level.hpp"
#include "renderer.hpp"
#include <algorithm>
#include "resource/fread_endian.hpp"


// globals

static bool can_exit = false;
static double xpos = 0.0;
static double ypos = 0.0;
static double zpos = 32.0;
static double midz = 0.0;
static double xangle = 0.0;
static double yangle = 0.0;
static bool leftbuttondown = false;
static bool rightbuttondown = false;

static bool all_maps = false;

static int curlevel=0;
static std::vector<ua_level> levels;
static ua_settings settings;
static ua_texture_manager texmgr;


// list with all triangles of current map
std::vector<ua_triangle3d_textured> alltriangles;


// prototypes

// levelmap loader, defined in resource/maploader.cpp
void ua_import_levelmaps(ua_settings &settings, const char *folder,
   std::vector<ua_level> &levels);


// functions

void load_level(int level)
{
   // load new level
   curlevel=level;

   texmgr.reset();

   // prepare all used textures
   const std::vector<Uint16>& used_textures =
      levels[curlevel].get_used_textures();

   unsigned int max = used_textures.size();
   for(unsigned int n=0; n<max; n++)
      texmgr.prepare(used_textures[n]);

   // collect all triangles
   alltriangles.clear();
   alltriangles.reserve(12000);

   for(unsigned int x=0; x<64; x++)
      for(unsigned int y=0; y<64; y++)
         ua_renderer::get_tile_triangles(levels[curlevel],x,y,alltriangles);

   // sort triangles by texnum
   std::sort(alltriangles.begin(), alltriangles.end());
}

void load_all_levels()
{
   texmgr.reset();

   unsigned int maxlevels = levels.size();

   alltriangles.clear();
   alltriangles.reserve(12000*maxlevels);

   for(unsigned int i=0; i<maxlevels; i++)
   {
      {
         // set new caption
         char buffer[256];
         sprintf(buffer,"Underworld Adventures: Map Display; loading level %u ...",i);

         SDL_WM_SetCaption(buffer,NULL);
      }

      // prepare all used textures
      const std::vector<Uint16>& used_textures =
         levels[i].get_used_textures();

      unsigned int max = used_textures.size();
      for(unsigned int n=0; n<max; n++)
         texmgr.prepare(used_textures[n]);

      // collect all triangles
      for(unsigned int x=0; x<64; x++)
         for(unsigned int y=0; y<64; y++)
            ua_renderer::get_tile_triangles(levels[i],x,y,alltriangles);

      // translate all triangles up
      unsigned int maxtri = alltriangles.size();
      for(unsigned int t=0; t<maxtri; t++)
      {
         for(unsigned int k=0; k<3; k++)
            alltriangles[t].points[k].z += 32.0;
      }
   }

   // sort triangles by texnum
   std::sort(alltriangles.begin(), alltriangles.end());
}

void init_mapdisp()
{
   // we don't load settings, just set the needed one
   settings.set_gametype(ua_game_uw1);
   std::string uw_path("./");
   settings.set_value(ua_setting_uw_path,uw_path);

   // check if we only have the demo
   if (ua_file_exists("./data/level13.st"))
      settings.set_gametype(ua_game_uw_demo);

   // check if we have uw2
   if (ua_file_exists("./uw2.exe"))
      settings.set_gametype(ua_game_uw2);

   // init texture manager
   texmgr.init(settings);

   ua_import_levelmaps(settings,"data/",levels);

   if (all_maps)
   {
      // load all level maps
      load_all_levels();
      zpos = 64.0;
      midz = -16.0;
   }
   else
   {
      // load first level
      load_level(0);
   }
}

void screenshot(bool big);

void handle_key_down(SDL_keysym* keysym)
{
   switch(keysym->sym)
   {
   case SDLK_ESCAPE:
      can_exit = true;
      break;

   case SDLK_PAGEUP:
      if (!all_maps && curlevel>0)
         load_level(curlevel-1);
      break;

   case SDLK_PAGEDOWN:
      if (!all_maps && curlevel<levels.size()-1)
         load_level(curlevel+1);
      break;

   case SDLK_UP:
      zpos+=2.0;
      break;

   case SDLK_DOWN:
      zpos-=2.0;
      break;

   case SDLK_s:
      // do large screenshot
      screenshot(true);
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
   glTranslated( -32.0, -32.0, midz );

   // draw ground square
   glBegin(GL_QUADS);
   glColor3ub(32,32,32);
   glVertex3d(0,0,-0.1);
   glVertex3d(64,0,-0.1);
   glVertex3d(64,64,-0.1);
   glVertex3d(0,64,-0.1);
   glEnd();

   // render all triangles
   glColor3ub(192,192,192);
   unsigned int max = alltriangles.size();
   for(unsigned int i=0; i<max; i++)
   {
      ua_triangle3d_textured &tri = alltriangles[i];

      texmgr.use(tri.texnum);

      glBegin(GL_TRIANGLES);
      for(int j=0; j<3; j++)
      {
         glTexCoord2d(tri.tex_u[j],tri.tex_v[j]);
         glVertex3d(tri.points[j].x, tri.points[j].y, tri.points[j].z*0.125);
      }
      glEnd();
   }
}

void screenshot(bool big)
{
   unsigned int xres = 640;
   unsigned int yres = 480;

   if (big)
   {
      // store proj. matrix
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glMatrixMode(GL_MODELVIEW);

      unsigned int tilex = 4;
      unsigned int tiley = 4;

      std::vector<Uint32> image;
      image.reserve(xres*yres*tilex*tiley);

      double left, right, bottom, top;

      for(unsigned int x=0; x<tilex; x++)
      {
         for(int y=tiley-1; y>=0; y--)
         {
            {
               // set new caption
               char buffer[256];
               sprintf(buffer,"Underworld Adventures: Map Display; capturing image %u x %u",
                  x+1,tiley-y);

               SDL_WM_SetCaption(buffer,NULL);
            }

            left = -0.25 + x/double(tilex*2);
            right = -0.25 + (x+1)/double(tilex*2);
            bottom = -0.25 + y/double(tiley*2);
            top = -0.25 + (y+1)/double(tiley*2);

            left *= double(xres)/yres;
            right *= double(xres)/yres;
            left *= double(tilex)/tiley;
            right *= double(tilex)/tiley;

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glFrustum(left, right, bottom, top, 0.25, 256.0);
            glMatrixMode(GL_MODELVIEW);

            draw_screen();

            // store scanlines
            glReadBuffer(GL_BACK);

            // read in all scanlines
            for(int n=yres-1; n>=0; n--)
            {
               unsigned int pos =
                  (x*xres)+
                  (n*xres*tilex) +
                  ((y)*xres*yres*tilex)
                  ;

               glReadPixels(0, n, xres, 1, GL_BGRA_EXT, GL_UNSIGNED_BYTE,
                  &image[pos]);

               //fwrite(&scanline[0],xres,4,fd);
            }
            // show captured image
            SDL_GL_SwapBuffers();
         }
      }

      SDL_WM_SetCaption("Underworld Adventures: Map Display; saving captured image ...",NULL);

      // saving tga file
      {
         // write tga header
         FILE *fd = fopen("mapdisp-shot.tga","wb");
         {
            // write header
            fputc(0,fd); // id length
            fputc(0,fd); // color map flag
            fputc(2,fd); // tga type (2=truecolor)
            fwrite16(fd,0); // color map origin
            fwrite16(fd,0); // color map length
            fputc(0,fd); // color map depth
            fwrite16(fd,0); // x origin
            fwrite16(fd,0); // y origin
            fwrite16(fd,xres*tilex); // width
            fwrite16(fd,yres*tiley); // height
            fputc(32,fd); // bits per pixel
            fputc(0x20,fd); // image descriptor; 0x00 = bottomup
         }

         for(int line=yres*tiley-1; line>=0; line--)
            fwrite(&image[line*xres*tilex],xres*tilex,4,fd);

         fclose(fd);
      }

      // restore projection matrix
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
   }
   else
   {
      // normal capture
   }
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

   // check if we have "allmaps" param
   if (argc>1 && strcmp(argv[1],"allmaps")==0)
      all_maps = true;

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
      SDL_GL_SwapBuffers();

      renders++;

      now = SDL_GetTicks();

      if (now-fcstart > 2000)
      {
         // set new caption
         char buffer[256];
         sprintf(buffer,"Underworld Adventures: Map Display, Level %u; %3.1f frames/s, %u triangles",
            all_maps ? 0 : curlevel+1, renders*1000.f/(now-fcstart), alltriangles.size());

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


// fake functions to prevent linking with all the uwadv stuff

ua_level& ua_underworld::get_current_level()
{
   return levels[curlevel];
}

unsigned int ua_player::get_attr(ua_player_attributes) const
{
   return 0;
}

#include "files.hpp"

SDL_RWops* ua_files_manager::get_uadata_file(const char *filename)
{
   return SDL_RWFromFile(filename,"rb");
}
