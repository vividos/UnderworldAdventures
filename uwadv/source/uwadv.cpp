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
/*! \file uwadv.cpp

   \brief main game loop code

*/

// needed includes
#include "common.hpp"
#include "uwadv.hpp"
#include "screens/uwadv_menu.hpp"
#include <iostream>


// ua_game methods

ua_game::ua_game():tickrate(20),exit_game(false),
   reset_tick_timer(false),audio(NULL),screen(NULL),screen_to_destroy(NULL)
{
   printf("Underworld Adventures\n");
   printf("http://uwadv.sourceforge.net/\n\n");
}

void ua_game::init()
{
   ua_trace("initializing game\n");

   // output SDL version number
   {
      const SDL_version* ver = SDL_Linked_Version();
      ua_trace("using SDL %u.%u.%u\n",ver->major, ver->minor, ver->patch);
   }

   // init files mgr; loads all config files and inits settings, too
   filesmgr.init(settings);

   // init savegames manager, too
   savegames_mgr.init(settings);

   // first, initialize SDL's video subsystem
   if( SDL_Init(SDL_INIT_VIDEO) < 0 )
   {
      std::string text("video initialization failed: ");
      text.append(SDL_GetError());
      throw ua_exception(text.c_str());
   }

   // Information about the current video settings
   const SDL_VideoInfo* info = NULL;
   info = SDL_GetVideoInfo();

   if(!info)
   {
      // this should probably never happen
      std::string text("video query failed: ");
      text.append(SDL_GetError());
      throw ua_exception(text.c_str());
   }

   // print video driver stats
   {
      char buffer[256];
      SDL_VideoDriverName(buffer,256);
      ua_trace("video driver: %s, ram available: %u k\n",
         buffer,info->video_mem);
   }

   // enable system event messages
   SDL_EventState(SDL_SYSWMEVENT,SDL_ENABLE);

   // set window caption
   SDL_WM_SetCaption("Underworld Adventures",NULL);

   // set up GL attributes
   SDL_GL_SetAttribute(SDL_GL_RED_SIZE,5);
   SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,5);
   SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,5);
   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

   // find out selected screen resolution
   {
      // predefined is 640x480
      width = 640;
      height = 480;

      std::string screen_res(settings.get_string(ua_setting_screen_resolution));

      // parse resolution string, format is <xres> x <yres>
      std::string::size_type pos = screen_res.find('x');
      if (pos != std::string::npos)
      {
         std::string yres_str(screen_res.c_str()+pos+1);

         width = static_cast<unsigned int>( strtol(screen_res.c_str(),NULL,10) );
         height = static_cast<unsigned int>( strtol(yres_str.c_str(),NULL,10) );
      }
   }

   // setup video mode
   int bpp = info->vfmt->BitsPerPixel;
   int flags = SDL_OPENGL |
      (settings.get_bool(ua_setting_fullscreen) ? SDL_FULLSCREEN : 0);

   ua_trace("setting video mode: %u x %u, %u bits%s\n", width, height, bpp,
      settings.get_bool(ua_setting_fullscreen) ? ", fullscreen" : "");

   if(SDL_SetVideoMode(width, height, bpp, flags)==0)
   {
      std::string text("video mode set failed: ");
      text.append(SDL_GetError());
      throw ua_exception(text.c_str());
   }

   // output some OpenGL diagnostics
   {
      GLint redbits, greenbits, bluebits, alphabits, depthbits;
      glGetIntegerv(GL_RED_BITS,&redbits);
      glGetIntegerv(GL_GREEN_BITS,&greenbits);
      glGetIntegerv(GL_BLUE_BITS,&bluebits);
      glGetIntegerv(GL_ALPHA_BITS,&alphabits);
      glGetIntegerv(GL_DEPTH_BITS,&depthbits);

      ua_trace("OpenGL stats:\n bit depths: red/green/blue/alpha = %u/%u/%u/%u, depth=%u\n",
         redbits, greenbits, bluebits, alphabits, depthbits);

      GLint maxtexsize, maxlights, maxnamestack, maxmodelstack, maxprojstack;
      glGetIntegerv(GL_MAX_TEXTURE_SIZE,&maxtexsize);
      glGetIntegerv(GL_MAX_LIGHTS,&maxlights);
      glGetIntegerv(GL_MAX_NAME_STACK_DEPTH,&maxnamestack);
      glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH,&maxmodelstack);
      glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH,&maxprojstack);

      ua_trace(" max. texture size = %u x %u, max. lights = %u\n",
         maxtexsize, maxtexsize, maxlights);

      ua_trace(" stack depths: name stack = %u, modelview stack = %u, proj. stack = %u\n",
         maxnamestack, maxmodelstack, maxprojstack);

      const GLubyte* vendor,* renderer,* version;
      vendor = glGetString(GL_VENDOR);
      renderer = glGetString(GL_RENDERER);
      version = glGetString(GL_VERSION);

      ua_trace(" vendor: %s\n renderer: %s\n version: %s\n\n",
         vendor,renderer,version);
   }

   // setup OpenGL viewport
   glViewport(0, 0, width, height);

   // clean screen
   glClearColor(0,0,0,0);
   glClear(GL_COLOR_BUFFER_BIT);
   SDL_GL_SwapBuffers();

   // switch off cursor
   SDL_ShowCursor(0);

   screenstack.clear();

   // start with uwadv menu
   push_screen(new ua_uwadv_menu_screen);
}

#define HAVE_FRAMECOUNT

void ua_game::run()
{
   ua_trace("\nmain loop started\n");

   Uint32 now, then;
   Uint32 fcstart;
   fcstart = then = SDL_GetTicks();

   unsigned int ticks=0,renders=0;

   // main game loop
   while(!exit_game)
   {
      now = SDL_GetTicks();

      while ((now - then) > (1000.0/tickrate))
      {
         then += Uint32(1000.0/tickrate);

         // do game logic
         screen->tick();
         ticks++;

         // check if there is a screen to destroy
         if (screen_to_destroy!=NULL)
         {
            screen_to_destroy->done();
            delete screen_to_destroy;
            screen_to_destroy = NULL;
            break;
         }
      }

      if (exit_game) break;

      // process incoming events
      process_events();

      // reset timer when needed
      if (reset_tick_timer)
      {
         then = now = SDL_GetTicks();
         reset_tick_timer = false;
      }

      // draw the screen
      draw_screen();
      renders++;

      if ((now - then) > (1000.0/tickrate))
         then = now - Uint32(1000.0/tickrate);

#ifdef HAVE_FRAMECOUNT
      now = SDL_GetTicks();

      if (now-fcstart > 2000)
      {
         // set new caption
         char buffer[256];
         sprintf(buffer,"Underworld Adventures: %3.1f ticks/s, %3.1f frames/s",
            ticks*1000.0/(now-fcstart),renders*1000.0/(now-fcstart));

         SDL_WM_SetCaption(buffer,NULL);

         // restart counting
         ticks = renders = 0;
         fcstart = now;
      }
#endif
   }

   ua_trace("main loop ended\n\n");
}

void ua_game::done()
{
   // free current screen
   if (screen!=NULL)
   {
      screen->done();
      delete screen;
   }

   // free all screens on screen stack
   int max = screenstack.size();
   for(int i=0; i<max; i++)
   {
      screenstack[i]->done();
      delete screenstack[i];
   }

   delete debug;

   underworld.done();

   delete audio;
   SDL_Quit();
}

void ua_game::error_msg(const char *msg)
{
   std::cerr << msg << std::endl;
}


// private ua_game methods

void ua_game::process_events()
{
   SDL_Event event;

   // get another event
   while(SDL_PollEvent(&event))
   {
      screen->handle_event(event);

      switch(event.type)
      {
      case SDL_KEYDOWN:
         // handle key presses
         handle_key_down(&event.key.keysym);
         break;

      case SDL_SYSWMEVENT:
         // system message
         system_message(event.syswm);
         break;

      case SDL_QUIT:
         // handle quit requests
         exit_game=true;
         break;
      }
   }
}

void ua_game::handle_key_down(SDL_keysym *keysym)
{
   switch(keysym->sym)
   {
   case SDLK_x:
      // exit when pressing Alt + x
      if ((keysym->mod & KMOD_ALT)!=0)
         exit_game = true;
      break;

   default:
      break;
   }
}

void ua_game::draw_screen()
{
   screen->render();

   // finished
   SDL_GL_SwapBuffers();
}

void ua_game::init_game()
{
   ua_trace("initializing game\n prefix: %s\n uw-path: %s\n",
      settings.get_string(ua_setting_game_prefix).c_str(),
      settings.get_string(ua_setting_uw_path).c_str());

   // init textures
   texmgr.init(settings);

   // init audio
   audio = ua_audio_interface::get_audio_interface();
   audio->init(settings,filesmgr);

   // load critters
   critter_pool.load(settings);
   critter_pool.prepare(texmgr);

   // init model manager
   model_manager.init(this);

   // initializes underworld
   underworld.init(settings,filesmgr);

   // create debug interface instance
   debug = ua_debug_interface::get_new_debug_interface(this);
}

void ua_game::push_screen(ua_ui_screen_base *newscreen)
{
   // save old screen on stack
   if (screen!=NULL)
   {
      screen->suspend();
      screenstack.push_back(screen);
   }

   // we have a new screen
   screen = newscreen;

   screen->set_core(this);
   screen->init();

   // reset tick timer
   reset_tick_timer = true;
}

void ua_game::replace_screen(ua_ui_screen_base *newscreen)
{
   pop_screen();
   push_screen(newscreen);
   exit_game = false;
}

void ua_game::pop_screen()
{
   screen_to_destroy = screen;

   // use 
   if (screenstack.size()==0)
   {
      // no more screens available
      screen=NULL;
      exit_game = true;
   }
   else
   {
      // get last pushed screen
      screen = screenstack.back();
      screenstack.pop_back();
      screen->resume();
   }

   // reset tick timer
   reset_tick_timer = true;
}
