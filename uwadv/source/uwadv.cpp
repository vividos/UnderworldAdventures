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
/*! \file uwadv.cpp

   main game loop and OpenGL rendering code

*/

// needed includes
#include "common.hpp"
#include "uwadv.hpp"
#include "screens/ingame_orig.hpp"


// ua_game methods

ua_game::ua_game()
{
   exit_game = false;
   audio = NULL;
   screen = NULL;
}

void ua_game::init()
{
   // First, initialize SDL's video subsystem. */
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

   // enable system event messages
   SDL_EventState(SDL_SYSWMEVENT,SDL_ENABLE);

   // set window caption
   SDL_WM_SetCaption("Underworld Adventures",NULL);

   // set up GL attributes
   SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
   SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

   // setup video mode
   width = 640;
   height = 480;
   int bpp = info->vfmt->BitsPerPixel;
   int flags = SDL_OPENGL/* | SDL_FULLSCREEN*/;

   if(SDL_SetVideoMode(width, height, bpp, flags)==0)
   {
      std::string text("video mode set failed: ");
      text.append(SDL_GetError());
      throw ua_exception(text.c_str());
   }


   // load settings
   settings.load();

   // init textures
   texmgr.init(settings);

   // load game strings
   gstr.load(settings);

   // init audio
   audio = ua_audio_interface::get_audio_interface();
   audio->init(settings);

   // create new user interface
   screen = new ua_ingame_orig_screen;
   screenstack.clear();

   screen->set_core(this);
   screen->init();
}

void ua_game::run()
{
   Uint32 now, then;
   then = SDL_GetTicks();

   // main game loop
   while(!exit_game)
   {
      now = SDL_GetTicks();
      Uint32 elapsed = (now - then);
      then = now;

      if (elapsed>(1.f/30.f))
         game_logic();

      // process incoming events
      process_events();

      // draw the screen
      draw_screen();
   }
}

void ua_game::done()
{
   delete audio;
   SDL_Quit();
}


// private ua_game methods

void ua_game::process_events()
{
   SDL_Event event;

   // get another event
   while(SDL_PollEvent(&event))
   {
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

      case SDL_MOUSEMOTION:
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
         screen->handle_mouse_action(event);
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
   case SDLK_0:
   case SDLK_1:
   case SDLK_2:
   case SDLK_3:
   case SDLK_4:
   case SDLK_5:
   case SDLK_6:
   case SDLK_7:
   case SDLK_8:
   case SDLK_9:
      // play a midi track
      audio->start_music(keysym->sym-SDLK_0);
      break;

   default:
      screen->handle_key_down(*keysym);
      break;
   }
}

void ua_game::draw_screen()
{
   screen->render();

   // finished
   SDL_GL_SwapBuffers();
}

void ua_game::game_logic()
{
   screen->tick();
}

void ua_game::push_screen(ua_ui_screen_base *screen)
{
}

void ua_game::replace_screen(ua_ui_screen_base *screen)
{
}
