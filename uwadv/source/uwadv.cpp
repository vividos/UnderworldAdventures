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

   \brief main game and loop code

*/

// needed includes
#include "common.hpp"
#include "uwadv.hpp"
#include "screens/uwadv_menu.hpp"
//#include "screens/ingame_orig.hpp" // TODO use screens/ingame.hpp
//#include "screens/start_splash.hpp"
/*
#include "gamecfg.hpp"
#include "screens/uwadv_menu.hpp"

#include <iostream>
*/
#include <ctime>


#ifndef WIN32
// Generic game class. win32 has a specialization in game_win32
ua_uwadv_game_implement(ua_uwadv_game);
#endif


// tables

enum ua_game_arg_type
{
   ua_arg_help,
   ua_arg_game,
};

struct ua_arg_entry
{
   ua_game_arg_type type;  //!< type of argument
   const char* shortopt;   //!< short option chars
   const char* longopt;    //!< long option string
   unsigned int numparams; //!< number of parameters for option
};

static
ua_arg_entry arg_params[] =
{
   { ua_arg_help, "hH?", "help", 0 },
   { ua_arg_game, "g", "game", 1 },
};


// ua_uwadv_game methods

ua_uwadv_game::ua_uwadv_game()
:tickrate(20), exit_game(false), screen_to_destroy(NULL)
{
   printf("Underworld Adventures"
#ifdef HAVE_DEBUG
      " (debug mode)"
#endif
      "\nhttp://uwadv.sourceforge.net/\n\n");

   srand((unsigned)time(NULL));
}

void ua_uwadv_game::init()
{
   ua_trace("initializing game ...\n\n");

   // init files manager; settings are loaded here, too
   files_manager.init(settings);

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

   // init savegames manager
   savegames_manager.init(settings);

   // init SDL window
   init_sdl();

/*
   // clean screen
   glClearColor(0,0,0,0);
   glClear(GL_COLOR_BUFFER_BIT);
   SDL_GL_SwapBuffers();
*/

   // switch off cursor
   SDL_ShowCursor(0);

   // normal game start
   init_action = 0;
   curscreen = NULL;
}

/*! reads in command line arguments
    args can be of the form -<char> (short option) or --<string> (long option)
    the table arg_params specifies all options that can be processed by uwadv
    shortopt contains all chars which can be used for this option
    longopt is the string that is used for the long option
    numparams specifies how many parameters are used for the specific option
*/
void ua_uwadv_game::parse_args(unsigned int argc, const char** argv)
{
   ua_trace("parsing command line arguments\n");

   for(unsigned int i=1; i<argc; i++)
   {
      std::string arg(argv[i]);
      if (arg.size()==0)
         continue;

      bool use_shortopt = true;

      // check if short or long arg
      if (arg.at(0)=='-')
      {
         if (arg.size()>1 && arg.at(1)=='-')
         {
            // long option
            use_shortopt = false;
            arg.erase(0,2);
         }
         else
            arg.erase(0,1);
      }
      else
      {
         // TODO support "" arguments

         // user specified a savegame to load
         init_action = 1;
         savegame_name = arg;
         continue;
      }

      // search for arg in table
      unsigned int entry=0;
      for(;entry<SDL_TABLESIZE(arg_params); entry++)
      {
         std::string::size_type pos = 0;

         // search for parameter
         if (use_shortopt)
            pos = arg.find_first_of(arg_params[entry].shortopt);
         else
            pos = arg.find(arg_params[entry].longopt);

         if (pos != std::string::npos && pos ==0)
            break; // found entry
      }

      if (entry==SDL_TABLESIZE(arg_params))
      {
         ua_trace(" %s option not found in table: %s\n",
            use_shortopt ? "short" : "long",
            arg.c_str());
         continue;
      }

      // check if enough parameters were passed
      if (i+arg_params[entry].numparams>=argc)
      {
         ua_trace(" not enough parameters for option %s (%u required)\n",
            argv[i],arg_params[entry].numparams);
         return;
      }

      // check actual parameter
      switch(arg_params[entry].type)
      {
      case ua_arg_help:
         // print help string
         printf(
            "Underworld Adventures command line options\n"
            " available options:\n"
            "  -h -? -H  --help           shows help\n"
            "  -g <game> --game <game>    starts custom game\n"
            );
         return;
         break;

      case ua_arg_game:
         init_action = 2;

         std::string custom_game_prefix(argv[i+1]);
         settings.set_value(ua_setting_game_prefix, custom_game_prefix);
         break;
      }

      // jump over arguments
      i += arg_params[entry].numparams;
   }
}

#define HAVE_FRAMECOUNT

void ua_uwadv_game::run()
{
   switch(init_action)
   {
   case 0: // normal start
      // start with uwadv menu screen
      replace_screen(new ua_uwadv_menu_screen,false);
      break;

   case 1: // load savegame
      {
         ua_trace("loading savegame from file %s\n",savegame_name.c_str());

         ua_savegame sg = savegames_manager.get_savegame_from_file(
            savegame_name.c_str());

         // set game prefix
         settings.set_value(ua_setting_game_prefix,
            sg.get_savegame_info().game_prefix);

         init_game();

         underworld.load_game(sg);

         // immediately start game
         //replace_screen(new ua_ingame_orig_screen,false);
      }
      break;

   case 2: // start custom game
      {
         // set prefix
         ua_trace("starting custom game\n");
         //settings.set_value(ua_setting_game_prefix,custom_game_prefix);

         init_game();

#ifndef HAVE_DEBUG
         // start splash screen
         //replace_screen(new ua_start_splash_screen,false);
#else
         // for uw2 testing; splash screens don't work yet
         underworld.import_savegame(settings,"data/",true);
         //replace_screen(new ua_ingame_orig_screen,false);
#endif
      }
      break;
   }


   ua_trace("\nmain loop started\n");

   Uint32 now, then;
   Uint32 fcstart;
   fcstart = then = SDL_GetTicks();

   unsigned int ticks=0, renders=0;

   exit_game = false;

   // main game loop
   while(!exit_game)
   {
      now = SDL_GetTicks();

      while ((now - then) > (1000.0/tickrate))
      {
         then += Uint32(1000.0/tickrate);

         ticks++;

         // do game logic
         curscreen->tick();

         // check if there is a screen to destroy
         if (screen_to_destroy != NULL)
         {
            screen_to_destroy->destroy();
            delete screen_to_destroy;
            screen_to_destroy = NULL;

            reset_tick_timer = true;
            break;
         }
/*
         // do texmgr tick
         texmgr.tick(1.0/tickrate);

         //Johnm - need to do logic for critters
         critter_pool.tick(1.0/tickrate);
*/
      }

      // do debug processing (uwadv thread)
//      debug->tick();

      // process incoming events
      process_events();

      if (exit_game) break;

      // reset timer when needed
      if (reset_tick_timer)
      {
         then = now = SDL_GetTicks();
         reset_tick_timer = false;
      }

      // draw the screen
      {
         curscreen->draw();

         // finished
         // TODO move this to somewhere else
         SDL_GL_SwapBuffers();
      }

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

void ua_uwadv_game::done()
{
   // free current screen
   if (curscreen!=NULL)
   {
      curscreen->destroy();
      delete curscreen;
      curscreen = NULL;
   }

   // free all screens on screen stack
   unsigned int max = screenstack.size();
   for(unsigned int i=0; i<max; i++)
   {
      screenstack[i]->destroy();
      delete screenstack[i];
   }
/*
   delete debug;

   underworld.done();
*/

   SDL_Quit();
}

void ua_uwadv_game::error_msg(const char* msg)
{
   ua_trace(msg);
}

void ua_uwadv_game::init_sdl()
{
   // output SDL version number
   {
      const SDL_version* ver = SDL_Linked_Version();
      ua_trace("using SDL %u.%u.%u\n",ver->major, ver->minor, ver->patch);
   }

   // first, initialize SDL's video subsystem
   if(SDL_Init(SDL_INIT_VIDEO) < 0)
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

   // set window caption
   SDL_WM_SetCaption("Underworld Adventures",NULL);

   // set up OpenGL attributes
   SDL_GL_SetAttribute(SDL_GL_RED_SIZE,5);
   SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,5);
   SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,5);
   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

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

   ua_trace("\n");

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

   // setup OpenGL viewport; doesn't change during game
   glViewport(0, 0, width, height);
}

void ua_uwadv_game::process_events()
{
   SDL_Event event;

   // get another event
   while(SDL_PollEvent(&event))
   {
      // let the screen handle the event first
      curscreen->process_event(event);

      switch(event.type)
      {
      case SDL_KEYDOWN:
         // handle key presses
         if (event.key.keysym.sym == SDLK_x &&
            (event.key.keysym.mod & KMOD_ALT) != 0)
            exit_game = true;
         break;

      case SDL_QUIT:
         // handle quit requests
         exit_game=true;
         break;

      case SDL_USEREVENT:
         // handle user events
         switch(event.user.code)
         {
         case ua_event_destroy_screen:
            pop_screen();
            break;

         default:
            break;
         }
         break;

      default:
         break;
      }
   }
}

void ua_uwadv_game::init_game()
{
   std::string prefix(settings.get_string(ua_setting_game_prefix));

   ua_trace("initializing game; prefix: %s\n",prefix.c_str());

   // load game config file
   std::string gamecfg_name(prefix);
   gamecfg_name.append("/game.cfg");
/*
   // init scripting before loading game.cfg (lua_State is needed)
   underworld.get_scripts().init(&underworld);

   // try to load %prefix%/game.cfg
   {
      ua_gamecfg_loader cfgloader;
      cfgloader.init(this);

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
*/
   ua_trace("using generic uw-path: %s\n",
      settings.get_string(ua_setting_uw_path).c_str());

   // init renderer
   renderer.init(settings);

/*
   // init underworld
   underworld.init(settings,filesmgr);

   // after loading all scripts, init the stuff
   underworld.get_scripts().lua_init_script();
*/

   // init audio
   audio_manager.init(settings,files_manager);

/*
   // load critters
   critter_pool.load(settings);
   critter_pool.prepare(texmgr);

   // init model manager
   model_manager.init(this);

   // create debug interface instance
   debug = ua_debug_interface::get_new_debug_interface(this);
*/

   // load language specific .pak file
   {
      ua_trace("loading language-specific strings ... ");

      std::string langpak_name(prefix);
      langpak_name.append("/lang.pak");

      SDL_RWops* rwops = files_manager.get_uadata_file(langpak_name.c_str());
      if (rwops != NULL)
      {
         underworld.get_strings().load(rwops);
         SDL_RWclose(rwops);

         ua_trace("language \"%s\"\n",
            underworld.get_strings().get_string(0x0a00,0).c_str());
      }
      else
         ua_trace("not available\n");
   }

   // reset tick timer
   reset_tick_timer = true;
}

void ua_uwadv_game::pop_screen()
{
   // clear screen; this can take a while
   renderer.clear();

   curscreen->destroy();
   delete curscreen;

   if (screenstack.size()!=0)
   {
      // get last pushed screen
      curscreen = screenstack.back();
      screenstack.pop_back();

      // send resume event
      SDL_Event user_event;
      user_event.type = SDL_USEREVENT;
      user_event.user.code = ua_event_resume_screen;
      user_event.user.data1 = NULL;
      user_event.user.data2 = NULL;
      SDL_PushEvent(&user_event);
   }
   else
   {
      // no more screens available
      curscreen = NULL;
      exit_game = true;
   }
}

void ua_uwadv_game::replace_screen(ua_screen* new_screen, bool save_current)
{
   // clear screen; this can take a while
   renderer.clear();

   if (save_current)
   {
      // save on screenstack
      screenstack.push_back(curscreen);
   }
   else
   {
      // defer screen destruction
      if (curscreen != NULL)
         screen_to_destroy = curscreen;
   }

   // initialize new screen
   curscreen = new_screen;

   curscreen->set_game_interface(this);
   curscreen->init();

   // reset tick timer
   reset_tick_timer = true;
}

void ua_uwadv_game::remove_screen()
{
   // send "destroy screen" event
   SDL_Event event;
   event.type = SDL_USEREVENT;
   event.user.code = ua_event_destroy_screen;
   SDL_PushEvent(&event);
}
