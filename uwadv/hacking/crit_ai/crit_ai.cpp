/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2003 Underworld Adventures Team

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
/*! \file crit_ai.cpp

   \brief critter AI workbench

*/

// needed includes
#include "common.hpp"
#include "underworld.hpp"
#include "gamecfg.hpp"


// global variables
bool can_exit = false;
unsigned int width, height;

enum action_mode
{
   mode_none=0,
   mode_pathfind_1st,
   mode_pathfind_2nd,
   mode_pathfind_show
};



// core classes
ua_underworld underworld;
ua_settings settings;
ua_files_manager filesmgr;
ua_texture_manager texmgr;

class dummy_core: public ua_game_core_interface
{
public:
   dummy_core():audio(NULL){}
   virtual void init_game(){}
   virtual unsigned int get_screen_width(){ return width; }
   virtual unsigned int get_screen_height(){ return height; }
   virtual unsigned int get_tickrate(){ return 20.0; }
   virtual ua_audio_interface &get_audio(){ return *audio; }
   virtual ua_gamestrings &get_strings(){ return underworld.get_strings(); }
   virtual ua_settings &get_settings(){ return settings; }
   virtual ua_texture_manager &get_texmgr(){ return texmgr; }
   virtual ua_files_manager &get_filesmgr(){ return filesmgr; }
   virtual ua_critter_pool& get_critter_pool(){ return critter_pool; };
   virtual ua_model3d_manager& get_model_manager(){ return model_manager; };
   virtual ua_savegames_manager& get_savegames_mgr(){ return savegames_mgr; };
   virtual ua_debug_interface* get_debug_interface(){ return NULL; }
   virtual ua_underworld &get_underworld(){ return underworld; }

   virtual void push_screen(ua_ui_screen_base *newscreen){}
   virtual void replace_screen(ua_ui_screen_base *newscreen){}
   virtual void pop_screen(){}

   ua_critter_pool critter_pool;
   ua_model3d_manager model_manager;
   ua_savegames_manager savegames_mgr;
   ua_audio_interface* audio;
};

dummy_core core;


// functions

void crit_ai_init()
{
   filesmgr.init(settings);

   underworld.get_scripts().init(&underworld);

   {
      // using uw1 game
      std::string prefix("uw1");
      settings.set_value(ua_setting_game_prefix,prefix);

      ua_trace("initializing game; prefix: %s\n",prefix.c_str());

      // load game config file
      std::string gamecfg_name(prefix);
      gamecfg_name.append("/game.cfg");

      // try to load %prefix%/game.cfg
      ua_gamecfg_loader cfgloader;
      cfgloader.init(&core);

      SDL_RWops* gamecfg = filesmgr.get_uadata_file(gamecfg_name.c_str());

      // no game.cfg found? too bad ...
      if (gamecfg == NULL)
      {
         std::string text("could not find game.cfg for game prefix ");
         text.append(prefix.c_str());
         throw new ua_exception(text.c_str());
      }

      cfgloader.load(gamecfg);
   }

   underworld.init(settings,filesmgr);
   underworld.get_scripts().lua_init_script();
   texmgr.init(settings);

   // import default game
   underworld.import_savegame(settings,"data/",true);

   // prepare all used wall/ceiling textures
   {
      const std::vector<Uint16>& used_textures =
         underworld.get_current_level().get_used_textures();

      unsigned int max = used_textures.size();
      for(unsigned int n=0; n<max; n++)
         texmgr.prepare(used_textures[n]);
   }

/*
   // prepare all switch, door and tmobj textures
   {
      unsigned int n;
      for(n=0; n<16; n++) texmgr.prepare(ua_tex_stock_switches+n);
      for(n=0; n<13; n++) texmgr.prepare(ua_tex_stock_door+n);
      for(n=0; n<33; n++) texmgr.prepare(ua_tex_stock_tmobj+n);
   }
*/


   // OpenGL setup

   glViewport(0,0,width,height);

   // state flags

   glEnable(GL_TEXTURE_2D);

   // culling
//   glCullFace(GL_BACK);
//   glFrontFace(GL_CCW);
   glDisable(GL_CULL_FACE);

   // z-buffer
   glDisable(GL_DEPTH_TEST);

   // camera setup

   // set projection matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   gluOrtho2D(0.0, 64.0, 0.0, 64.0);

   // switch back to modelview matrix
   glMatrixMode(GL_MODELVIEW);
}

void crit_ai_done()
{
}

void draw_screen()
{
   glClear(GL_COLOR_BUFFER_BIT);

   ua_level& curlevel = underworld.get_current_level();

   for(unsigned int y=0; y<64; y++)
   for(unsigned int x=0; x<64; x++)
   {
      ua_levelmap_tile& tile = curlevel.get_tile(x,y);

      unsigned int omit_vertex = 1 | 2 | 4 | 8;

      glColor3ub(255,255,255);
      texmgr.use(tile.texture_floor);

      switch(tile.type)
      {
      case ua_tile_solid:
         glColor3ub(0,0,0);
         break;

      case ua_tile_diagonal_se: omit_vertex &= 1 | 2 | 4; break;
      case ua_tile_diagonal_sw: omit_vertex &= 1 | 2 | 8; break;
      case ua_tile_diagonal_nw: omit_vertex &= 1 | 4 | 8; break;
      case ua_tile_diagonal_ne: omit_vertex &= 2 | 4 | 8; break;

      case ua_tile_slope_n:
      case ua_tile_slope_e:
      case ua_tile_slope_s:
      case ua_tile_slope_w:
         glColor3ub(192,192,192);
         break;

      default:
         break;
      }

      // render quad
      glBegin(omit_vertex < 15 ? GL_TRIANGLES : GL_QUADS);
      glTexCoord2d(0.0, 0.0); if (omit_vertex&1) glVertex2i(x,  y);
      glTexCoord2d(1.0, 0.0); if (omit_vertex&2) glVertex2i(x+1,y);
      glTexCoord2d(1.0, 1.0); if (omit_vertex&4) glVertex2i(x+1,y+1);
      glTexCoord2d(0.0, 1.0); if (omit_vertex&8) glVertex2i(x,  y+1);
      glEnd();
   }

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
         case SDLK_q:
            can_exit = true;

         default:
            break;
         }
         break;

      case SDL_MOUSEBUTTONDOWN:
         /*{
            // check which mouse button was pressed
            Uint8 state = SDL_GetRelativeMouseState(NULL,NULL);
            if (SDL_BUTTON(state)==SDL_BUTTON_LEFT)
               leftbuttondown = true;
            else
               rightbuttondown = true;
         }*/
         break;

      case SDL_MOUSEBUTTONUP:
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

   width = 640;
   height = 640;
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

   crit_ai_init();


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

      if (now-fcstart > 2000/* || force_fps_update*/)
      {
         // set new caption
         char buffer[256];
         sprintf(buffer,"Underworld Adventures: Critter AI workbench - %3.1f frames/s",
            renders*1000.f/(now-fcstart));

         SDL_WM_SetCaption(buffer,NULL);

         // restart counting
         renders = 0;
         fcstart = now;
         //force_fps_update = false;
      }
   }

   crit_ai_done();

   // finish off SDL
   SDL_Quit();

   return 0;
}
