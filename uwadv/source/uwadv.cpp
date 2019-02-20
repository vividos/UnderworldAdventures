//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Michael Fink
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file uwadv.cpp
/// \brief main game and loop code
//
#include "base.hpp"
#include "uwadv.hpp"
#include "textfile.hpp"
#include "gamecfg.hpp"
#include "screens/uwadv_menu.hpp"
#include "screens/ingame_orig.hpp"
#include "screens/start_splash.hpp"
#include "import/gamestringsimporter.hpp"
#include <ctime>
#include <SDL.h>
#include <SDL_opengl.h>

#ifdef WIN32
#include <SDL_syswm.h>
#include "win32/resource.h"
#endif

#define HAVE_FRAMECOUNT

/// command line argument type
enum ua_game_arg_type
{
   ua_arg_help, ///< help option
   ua_arg_game, ///< game option, to specify custom games
};

/// command line argument data
struct ua_arg_entry
{
   ua_game_arg_type type;  ///< type of argument
   const char* shortopt;   ///< short option chars
   const char* longopt;    ///< long option string
   unsigned int numparams; ///< number of parameters for option
};

/// all command line arguments that uwadv understands
static
ua_arg_entry arg_params[] =
{
   { ua_arg_help, "hH?", "help", 0 },
   { ua_arg_game, "g", "game", 1 },
};

ua_uwadv_game::ua_uwadv_game()
   :tickrate(20),
   exit_game(false),
   screen_to_destroy(NULL),
   scripting(NULL),
   paused(false)
{
   printf("Underworld Adventures"
#ifdef HAVE_DEBUG
      " (debug mode)"
#endif
      "\nhttps://vividos.github.com/UnderworldAdventures/\n\n");

   srand((unsigned)time(NULL));
}

void ua_uwadv_game::init()
{
   UaTrace("initializing game ...\n\n");

   // init files manager; settings are loaded here, too
   m_resourceManager = std::make_unique<Base::ResourceManager>(settings);

   // find out selected screen resolution
   {
      // predefined is 640x480
      width = 640;
      height = 480;

      std::string screen_res(settings.GetString(Base::settingScreenResolution));

      // parse resolution string, format is <xres> x <yres>
      std::string::size_type pos = screen_res.find('x');
      if (pos != std::string::npos)
      {
         std::string yres_str(screen_res.c_str() + pos + 1);

         width = static_cast<unsigned int>(strtol(screen_res.c_str(), NULL, 10));
         height = static_cast<unsigned int>(strtol(yres_str.c_str(), NULL, 10));
      }
   }

   // init SDL window
   init_sdl();

   // normal game start
   init_action = 0;
   curscreen = NULL;
}

/// \brief Reads in command line arguments
/// Args can be of the form "-<char>" (short option) or "--<string>" (long
/// option). The table arg_params specifies all options that can be processed
/// by uwadv.
/// \li shortopt contains all chars which can be used for this option
/// \li longopt is the string that is used for the long option
/// \li numparams specifies how many parameters are used for the specific option
///
/// \param argc number of arguments, just like in the main() call
/// \param argv argument strings
void ua_uwadv_game::parse_args(unsigned int argc, const char** argv)
{
   UaTrace("parsing command line arguments\n");

   for (unsigned int i = 1; i < argc; i++)
   {
      std::string arg(argv[i]);
      if (arg.size() == 0)
         continue;

      bool use_shortopt = true;

      // check if short or long arg
      if (arg.at(0) == '-')
      {
         if (arg.size() > 1 && arg.at(1) == '-')
         {
            // long option
            use_shortopt = false;
            arg.erase(0, 2);
         }
         else
            arg.erase(0, 1);
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
      unsigned int entry = 0;
      for (; entry < SDL_TABLESIZE(arg_params); entry++)
      {
         std::string::size_type pos = 0;

         // search for parameter
         if (use_shortopt)
            pos = arg.find_first_of(arg_params[entry].shortopt);
         else
            pos = arg.find(arg_params[entry].longopt);

         if (pos != std::string::npos && pos == 0)
            break; // found entry
      }

      if (entry == SDL_TABLESIZE(arg_params))
      {
         UaTrace(" %s option not found in table: %s\n",
            use_shortopt ? "short" : "long",
            arg.c_str());
         continue;
      }

      // check if enough parameters were passed
      if (i + arg_params[entry].numparams >= argc)
      {
         UaTrace(" not enough parameters for option %s (%u required)\n",
            argv[i], arg_params[entry].numparams);
         return;
      }

      // check actual parameter
      switch (arg_params[entry].type)
      {
         // print help string
      case ua_arg_help:
         printf(
            "Underworld Adventures command line options\n"
            " available options:\n"
            "  -h -? -H  --help           shows help\n"
            "  -g <game> --game <game>    starts custom game\n"
         );
         return;
         break;

         // game prefix
      case ua_arg_game:
      {
         init_action = 2;

         std::string custom_game_prefix(argv[i + 1]);
         settings.SetValue(Base::settingGamePrefix, custom_game_prefix);
      }
      break;

      default:
         UaAssert(false); // invalid option
         break;
      }

      // jump over arguments
      i += arg_params[entry].numparams;
   }
}

void ua_uwadv_game::run()
{
   switch (init_action)
   {
   case 0: // normal start
      // start with uwadv menu screen
      replace_screen(new ua_uwadv_menu_screen(*this), false);
      break;

   case 1: // load savegame
   {
      UaTrace("loading savegame from file %s\n", savegame_name.c_str());

      Base::Savegame sg = savegames_manager->GetSavegameFromFile(savegame_name.c_str());

      // set game prefix
      settings.SetValue(Base::settingGamePrefix,
         sg.GetSavegameInfo().gamePrefix);

      init_game();

      GetUnderworld().Load(sg);

      // immediately start game
      replace_screen(new ua_ingame_orig_screen(*this), false);
   }
   break;

   case 2: // start custom game
   {
      // set prefix
      UaTrace("starting custom game\n");
      //settings.SetValue(Base::settingGamePrefix,custom_game_prefix);

      init_game();

#ifndef HAVE_DEBUG
      // start splash screen
      replace_screen(new ua_start_splash_screen(*this), false);
#else
      // for uw2 testing; splash screens don't work yet
      //underworld.import_savegame(settings,"data/",true);
      replace_screen(new ua_ingame_orig_screen(*this), false);
#endif
   }
   break;

   default:
      break;
   }


   UaTrace("\nmain loop started\n");

   Uint32 now, then;
   Uint32 fcstart;
   fcstart = then = SDL_GetTicks();

   unsigned int ticks = 0, renders = 0;

   exit_game = false;

   // main game loop
   while (!exit_game)
   {
      now = SDL_GetTicks();

      while ((now - then) > (1000.0 / tickrate))
      {
         then += Uint32(1000.0 / tickrate);

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
      }

      // do server side debug processing
      debug.tick();

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
      curscreen->draw();
      renderer.swap_buffers();

      renders++;

      if ((now - then) > (1000.0 / tickrate))
         then = now - Uint32(1000.0 / tickrate);

#ifdef HAVE_FRAMECOUNT
      now = SDL_GetTicks();

      if (now - fcstart > 2000)
      {
         // set new caption
         char buffer[256];
         sprintf(buffer, "Underworld Adventures: %3.1f ticks/s, %3.1f frames/s",
            ticks*1000.0 / (now - fcstart), renders*1000.0 / (now - fcstart));

         // TODO SDL_SetWindowTitle(NULL, buffer);

         // restart counting
         ticks = renders = 0;
         fcstart = now;
#ifdef HAVE_DEBUG
         // reset time count when rendering lasted longer than 5 seconds
         // it's likely that we just debugged through some code
         if (now - then > 5000)
            then = now;
#endif
      }
#endif
   }

   UaTrace("main loop ended\n\n");
}

void ua_uwadv_game::done()
{
   // free current screen
   if (curscreen != NULL)
   {
      curscreen->destroy();
      delete curscreen;
      curscreen = NULL;
   }

   // free all screens on screen stack
   unsigned int max = screenstack.size();
   for (unsigned int i = 0; i < max; i++)
   {
      screenstack[i]->destroy();
      delete screenstack[i];
   }

   done_game();

   SDL_Quit();
}

void ua_uwadv_game::error_msg(const char* msg)
{
   SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Underworld Adventures", msg, NULL);
}

bool ua_uwadv_game::pause_game(bool pause)
{
   bool old_paused = paused;
   paused = pause;
   return old_paused;
}

void ua_uwadv_game::init_sdl()
{
   // output SDL version number
   {
      SDL_version version = { 0 };
      SDL_GetVersion(&version);
      UaTrace("using SDL %u.%u.%u\n", version.major, version.minor, version.patch);
   }

   // first, initialize SDL's video subsystem
   if (SDL_Init(SDL_INIT_VIDEO) < 0)
   {
      std::string text("video initialization failed: ");
      text.append(SDL_GetError());
      throw Base::Exception(text.c_str());
   }

   // Information about the current video settings
#if 0
   const SDL_VideoInfo* info = NULL;
   info = SDL_GetVideoInfo();

   if (!info)
   {
      // this should probably never happen
      std::string text("video query failed: ");
      text.append(SDL_GetError());
      throw Base::Exception(text.c_str());
   }

   // print video driver stats
   {
      char buffer[256];
      SDL_VideoDriverName(buffer, 256);
      UaTrace("video driver: %s, ram available: %u k\n",
         buffer, info->video_mem);
   }

   // set window caption
   SDL_WM_SetCaption(, NULL);

   // set up OpenGL attributes
   SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
   SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
   SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

   // setup video mode
   int bpp = info->vfmt->BitsPerPixel;
   int flags = SDL_OPENGL |
      (settings.GetBool(ua_setting_fullscreen) ? SDL_FULLSCREEN : 0);

   UaTrace("setting video mode: %u x %u, %u bits%s\n", width, height, bpp,
      settings.GetBool(Base::settingFullscreen) ? ", fullscreen" : "");
#endif

   int flags = settings.GetBool(Base::settingFullscreen) ? SDL_WINDOW_FULLSCREEN : 0;

   m_window = SDL_CreateWindow(
      "Underworld Adventures",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      width, height,
      flags | SDL_WINDOW_OPENGL);

   if (m_window == NULL)
   {
      std::string text("video mode set failed: ");
      text.append(SDL_GetError());
      throw Base::Exception(text.c_str());
   }

#ifdef WIN32
   HINSTANCE inst = (HINSTANCE)GetModuleHandle(NULL);

   HICON icon = ::LoadIcon(inst, MAKEINTRESOURCE(IDI_ICON));
   HICON icon_small = (HICON)::LoadImage(inst,
      MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

   SDL_SysWMinfo wminfo;
   SDL_VERSION(&wminfo.version);

   if (SDL_GetWindowWMInfo(m_window, &wminfo) == 1)
   {
      HWND hwnd = wminfo.info.win.window;
      ::SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon);
      ::SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon_small);
   }
#endif

   m_context = SDL_GL_CreateContext(m_window);

   UaTrace("\n");

   // output some OpenGL diagnostics
   {
      GLint redbits, greenbits, bluebits, alphabits, depthbits;
      glGetIntegerv(GL_RED_BITS, &redbits);
      glGetIntegerv(GL_GREEN_BITS, &greenbits);
      glGetIntegerv(GL_BLUE_BITS, &bluebits);
      glGetIntegerv(GL_ALPHA_BITS, &alphabits);
      glGetIntegerv(GL_DEPTH_BITS, &depthbits);

      UaTrace("OpenGL stats:\n bit depths: red/green/blue/alpha = %u/%u/%u/%u, depth=%u\n",
         redbits, greenbits, bluebits, alphabits, depthbits);

      GLint maxtexsize, maxlights, maxnamestack, maxmodelstack, maxprojstack;
      glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxtexsize);
      glGetIntegerv(GL_MAX_LIGHTS, &maxlights);
      glGetIntegerv(GL_MAX_NAME_STACK_DEPTH, &maxnamestack);
      glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &maxmodelstack);
      glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &maxprojstack);

      UaTrace(" max. texture size = %u x %u, max. lights = %u\n",
         maxtexsize, maxtexsize, maxlights);

      UaTrace(" stack depths: name stack = %u, modelview stack = %u, proj. stack = %u\n",
         maxnamestack, maxmodelstack, maxprojstack);

      const GLubyte* vendor, *renderer, *version;
      vendor = glGetString(GL_VENDOR);
      renderer = glGetString(GL_RENDERER);
      version = glGetString(GL_VERSION);

      UaTrace(" vendor: %s\n renderer: %s\n version: %s\n",
         vendor, renderer, version);

      GLboolean stereo;
      glGetBooleanv(GL_STEREO, &stereo);

      UaTrace(" supports stereo mode: %s\n\n",
         stereo == GL_TRUE ? "yes" : "no");
   }

   // setup OpenGL viewport; doesn't change during game
   glViewport(0, 0, width, height);
}

void ua_uwadv_game::process_events()
{
   SDL_Event event;

   while (SDL_PollEvent(&event))
   {
      // let the screen handle the event first
      curscreen->process_event(event);

      switch (event.type)
      {
      case SDL_KEYDOWN:
         if (event.key.keysym.sym == SDLK_x &&
            (event.key.keysym.mod & KMOD_ALT) != 0)
            exit_game = true;
         break;

      case SDL_QUIT:
         exit_game = true;
         break;

      case SDL_USEREVENT:
         switch (event.user.code)
         {
         case ua_event_destroy_screen:
            pop_screen();
            if (curscreen == NULL)
               return; // don't process events anymore
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
   std::string prefix(settings.GetString(Base::settingGamePrefix));

   UaTrace("initializing game; prefix: %s\n", prefix.c_str());

   // load game config file
   std::string gamecfg_name(prefix);
   gamecfg_name.append("/game.cfg");

   // set new game prefix
   savegames_manager->SetNewGamePrefix(prefix.c_str());

   // try to load %prefix%/game.cfg
   {
      ua_gamecfg_loader cfgloader(*this, &scripting);

      Base::SDL_RWopsPtr gamecfg = m_resourceManager->GetResourceFile(gamecfg_name.c_str());

      // no game.cfg found? too bad ...
      if (gamecfg == NULL)
      {
         std::string text("could not find game.cfg for game prefix ");
         text.append(prefix.c_str());
         throw Base::Exception(text.c_str());
      }

      Base::TextFile textFile(gamecfg);
      cfgloader.Load(textFile);
   }

   UaTrace("using generic uw-path: %s\n",
      settings.GetString(Base::settingUnderworldPath).c_str());

   savegames_manager = std::make_unique<Base::SavegamesManager>(settings);

   image_manager.init(settings);

   renderer.init(*this, m_window);

   IUserInterface* userInterface;
   m_gameLogic = std::make_unique<Underworld::GameLogic>(userInterface, scripting);

   UaTrace("loading game strings ... ");
   Import::GameStringsImporter importer(GetGameStrings());
   importer.LoadDefaultStringsPakFile(GetResourceManager());
   UaTrace("done\n");

   audio_manager = std::make_unique<Audio::AudioManager>(settings);

   debug.init();

   // load language specific .pak file
   {
      UaTrace("loading language-specific strings ... ");

      std::string langpak_name(prefix);
      langpak_name.append("/lang.pak");

      Base::SDL_RWopsPtr rwops = m_resourceManager->GetResourceFile(langpak_name.c_str());
      if (rwops != NULL)
      {
         Import::GameStringsImporter importer(GetGameStrings());
         importer.LoadStringsPakFile(rwops);

         UaTrace("language \"%s\"\n",
            GetGameStrings().GetString(0x0a00, 0).c_str());
      }
      else
         UaTrace("not available\n");
   }

   reset_tick_timer = true;
}

void ua_uwadv_game::done_game()
{
   debug.shutdown();

   if (scripting != NULL)
   {
      scripting->done();
      delete scripting;
      scripting = NULL;
   }
}

void ua_uwadv_game::pop_screen()
{
   // clear screen; this can take a while
   renderer.clear();

   curscreen->destroy();
   delete curscreen;

   if (screenstack.size() != 0)
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
