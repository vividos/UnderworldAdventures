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
/// \file Game.cpp
/// \brief main game and loop code
//
#include "Base.hpp"
#include "Game.hpp"
#include "TextFile.hpp"
#include "GameConfigLoader.hpp"
#include "screens/UwadvMenuScreen.hpp"
#include "screens/OriginalIngameScreen.hpp"
#include "screens/StartSplashScreen.hpp"
#include "import/GameStringsImporter.hpp"
#include <ctime>
#include <SDL.h>
#include <SDL_opengl.h>

#ifdef WIN32
#include <SDL_syswm.h>
#include "win32/resource.h"
#endif

#ifdef HAVE_DEBUG
#define HAVE_FRAMECOUNT
#endif

/// command line argument type
enum ArgumentType
{
   argumentHelp, ///< help option
   argumentGame, ///< game option, to specify custom games
};

/// command line argument data
struct ArgumentEntry
{
   ArgumentType type;  ///< type of argument
   const char* shortOption;   ///< short option chars
   const char* longOption;    ///< long option string
   unsigned int numParams; ///< number of parameters for option
};

/// all command line arguments that uwadv understands
static
ArgumentEntry s_ArgumentParams[] =
{
   { argumentHelp, "hH?", "help", 0 },
   { argumentGame, "g", "game", 1 },
};

Game::Game()
   :m_tickRate(20),
   m_exitGame(false),
   m_screenToDestroy(NULL),
   m_scripting(NULL),
   m_isPaused(false)
{
   printf("Underworld Adventures"
#ifdef HAVE_DEBUG
      " (debug mode)"
#endif
      "\nhttps://vividos.github.com/UnderworldAdventures/\n\n");

   srand((unsigned)time(NULL));
}

void Game::Init()
{
   UaTrace("initializing game ...\n\n");

   // init files manager; settings are loaded here, too
   Base::LoadSettings(m_settings);
   m_resourceManager = std::make_unique<Base::ResourceManager>(m_settings);

   // find out selected screen resolution
   {
      // predefined is 640x480
      m_width = 640;
      m_height = 480;

      std::string screen_res(m_settings.GetString(Base::settingScreenResolution));

      // parse resolution string, format is <xres> x <yres>
      std::string::size_type pos = screen_res.find('x');
      if (pos != std::string::npos)
      {
         std::string yres_str(screen_res.c_str() + pos + 1);

         m_width = static_cast<unsigned int>(strtol(screen_res.c_str(), NULL, 10));
         m_height = static_cast<unsigned int>(strtol(yres_str.c_str(), NULL, 10));
      }
   }

   // init SDL window
   InitSDL();

   // normal game start
   m_initAction = 0;
   m_currentScreen = NULL;
}

/// \brief Reads in command line arguments
/// Args can be of the form "-<char>" (short option) or "--<string>" (long
/// option). The table s_ArgumentParams specifies all options that can be processed
/// by uwadv.
/// \li shortOption contains all chars which can be used for this option
/// \li longOption is the string that is used for the long option
/// \li numParams specifies how many parameters are used for the specific option
///
/// \param argc number of arguments, just like in the main() call
/// \param argv argument strings
void Game::ParseArgs(unsigned int argc, const char** argv)
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
         m_initAction = 1;
         m_savegameName = arg;
         continue;
      }

      // search for arg in table
      unsigned int entry = 0;
      for (; entry < SDL_TABLESIZE(s_ArgumentParams); entry++)
      {
         std::string::size_type pos = 0;

         // search for parameter
         if (use_shortopt)
            pos = arg.find_first_of(s_ArgumentParams[entry].shortOption);
         else
            pos = arg.find(s_ArgumentParams[entry].longOption);

         if (pos != std::string::npos && pos == 0)
            break; // found entry
      }

      if (entry == SDL_TABLESIZE(s_ArgumentParams))
      {
         UaTrace(" %s option not found in table: %s\n",
            use_shortopt ? "short" : "long",
            arg.c_str());
         continue;
      }

      // check if enough parameters were passed
      if (i + s_ArgumentParams[entry].numParams >= argc)
      {
         UaTrace(" not enough parameters for option %s (%u required)\n",
            argv[i], s_ArgumentParams[entry].numParams);
         return;
      }

      // check actual parameter
      switch (s_ArgumentParams[entry].type)
      {
         // print help string
      case argumentHelp:
         printf(
            "Underworld Adventures command line options\n"
            " available options:\n"
            "  -h -? -H  --help           shows help\n"
            "  -g <game> --game <game>    starts custom game\n"
         );
         return;
         break;

         // game prefix
      case argumentGame:
      {
         m_initAction = 2;

         std::string custom_game_prefix(argv[i + 1]);
         m_settings.SetValue(Base::settingGamePrefix, custom_game_prefix);
      }
      break;

      default:
         UaAssert(false); // invalid option
         break;
      }

      // jump over arguments
      i += s_ArgumentParams[entry].numParams;
   }
}

void Game::Run()
{
   switch (m_initAction)
   {
   case 0: // normal start
      // start with uwadv menu screen
      //ReplaceScreen(new UwadvMenuScreen(*this), false);
      ReplaceScreen(new UwadvMenuScreen(*this), false);
      break;

   case 1: // load savegame
   {
      UaTrace("loading savegame from file %s\n", m_savegameName.c_str());

      Base::Savegame sg = m_savegamesManager->GetSavegameFromFile(m_savegameName.c_str());

      // set game prefix
      m_settings.SetValue(Base::settingGamePrefix,
         sg.GetSavegameInfo().m_gamePrefix);

      InitGame();

      GetUnderworld().Load(sg);

      // immediately start game
      ReplaceScreen(new OriginalIngameScreen(*this), false);
   }
   break;

   case 2: // start custom game
   {
      // set prefix
      UaTrace("starting custom game\n");
      //m_settings.SetValue(Base::settingGamePrefix,custom_game_prefix);

      InitGame();

#ifndef HAVE_DEBUG
      // start splash screen
      ReplaceScreen(new StartSplashScreen(*this), false);
#else
      // for uw2 testing; splash screens don't work yet
      //underworld.import_savegame(settings,"data/",true);
      ReplaceScreen(new OriginalIngameScreen(*this), false);
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

   m_exitGame = false;

   // main game loop
   while (!m_exitGame)
   {
      now = SDL_GetTicks();

      while ((now - then) > (1000.0 / m_tickRate))
      {
         then += Uint32(1000.0 / m_tickRate);

         ticks++;

         // do game logic
         m_currentScreen->Tick();

         // check if there is a screen to destroy
         if (m_screenToDestroy != NULL)
         {
            m_screenToDestroy->Destroy();
            delete m_screenToDestroy;
            m_screenToDestroy = NULL;

            m_resetTickTimer = true;
            break;
         }
      }

      // do server side debug processing
      m_debugServer.Tick();

      // process incoming events
      ProcessEvents();

      if (m_exitGame) break;

      // reset timer when needed
      if (m_resetTickTimer)
      {
         then = now = SDL_GetTicks();
         m_resetTickTimer = false;
      }

      // draw the screen
      m_currentScreen->Draw();
      m_renderer.SwapBuffers();

      renders++;

      if ((now - then) > (1000.0 / m_tickRate))
         then = now - Uint32(1000.0 / m_tickRate);

#ifdef HAVE_FRAMECOUNT
      now = SDL_GetTicks();

      if (now - fcstart > 2000)
      {
         // set new caption
         char buffer[256];
         snprintf(buffer, sizeof(buffer), "Underworld Adventures: %3.1f ticks/s, %3.1f frames/s",
            ticks*1000.0 / (now - fcstart), renders*1000.0 / (now - fcstart));

         SDL_SetWindowTitle(m_window, buffer);

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

void Game::Done()
{
   // free current screen
   if (m_currentScreen != NULL)
   {
      m_currentScreen->Destroy();
      delete m_currentScreen;
      m_currentScreen = NULL;
   }

   // free all screens on screen stack
   unsigned int max = m_screenStack.size();
   for (unsigned int i = 0; i < max; i++)
   {
      m_screenStack[i]->Destroy();
      delete m_screenStack[i];
   }

   DoneGame();

   SDL_Quit();
}

bool Game::PauseGame(bool pause)
{
   bool oldPaused = m_isPaused;
   m_isPaused = pause;
   return oldPaused;
}

void Game::InitSDL()
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
      (m_settings.GetBool(ua_setting_fullscreen) ? SDL_FULLSCREEN : 0);

   UaTrace("setting video mode: %u x %u, %u bits%s\n", m_width, m_height, bpp,
      m_settings.GetBool(Base::settingFullscreen) ? ", fullscreen" : "");
#endif

   int flags = m_settings.GetBool(Base::settingFullscreen) ? SDL_WINDOW_FULLSCREEN : 0;

   m_window = SDL_CreateWindow(
      "Underworld Adventures",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      m_width, m_height,
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

      const GLubyte* vendor, *rendererName, *version;
      vendor = glGetString(GL_VENDOR);
      rendererName = glGetString(GL_RENDERER);
      version = glGetString(GL_VERSION);

      UaTrace(" vendor: %s\n renderer: %s\n version: %s\n",
         vendor, rendererName, version);

      GLboolean stereo;
      glGetBooleanv(GL_STEREO, &stereo);

      UaTrace(" supports stereo mode: %s\n\n",
         stereo == GL_TRUE ? "yes" : "no");
   }

   // setup OpenGL viewport; doesn't change during game
   glViewport(0, 0, m_width, m_height);
}

void Game::ProcessEvents()
{
   SDL_Event event;

   while (SDL_PollEvent(&event))
   {
      // let the screen handle the event first
      m_currentScreen->ProcessEvent(event);

      switch (event.type)
      {
      case SDL_KEYDOWN:
         if (event.key.keysym.sym == SDLK_x &&
            (event.key.keysym.mod & KMOD_ALT) != 0)
            m_exitGame = true;
         break;

      case SDL_QUIT:
         m_exitGame = true;
         break;

      case SDL_USEREVENT:
         switch (event.user.code)
         {
         case gameEventDestroyScreen:
            PopScreen();
            if (m_currentScreen == NULL)
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

void Game::InitGame()
{
   // rescan, with proper underworld path
   m_resourceManager->Rescan(m_settings);

   std::string prefix(m_settings.GetString(Base::settingGamePrefix));

   m_savegamesManager = std::make_unique<Base::SavegamesManager>(m_settings);

   UaTrace("initializing game; prefix: %s\n", prefix.c_str());

   // load game config file
   std::string gamecfg_name(prefix);
   gamecfg_name.append("/game.cfg");

   m_savegamesManager->SetNewGamePrefix(prefix.c_str());
   m_savegamesManager->Rescan();

   // try to load %prefix%/game.cfg
   {
      GameConfigLoader cfgloader(*this, &m_scripting);

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
      m_settings.GetString(Base::settingUnderworldPath).c_str());

   m_imageManager = std::make_unique<ImageManager>(GetResourceManager());
   m_imageManager->Init();

   m_renderer.Init(*this, m_window);

   IUserInterface* userInterface;
   m_gameLogic = std::make_unique<Underworld::GameLogic>(userInterface, m_scripting);

   UaTrace("loading game strings ... ");
   Import::GameStringsImporter importer(GetGameStrings());
   importer.LoadDefaultStringsPakFile(GetResourceManager());
   UaTrace("done\n");

   m_audioManager = std::make_unique<Audio::AudioManager>(GetSettings(), GetResourceManager());

   m_debugServer.Init();

   // load language specific .pak file
   {
      UaTrace("loading language-specific strings ... ");

      std::string langpak_name(prefix);
      langpak_name.append("/lang.pak");

      Base::SDL_RWopsPtr rwops = m_resourceManager->GetResourceFile(langpak_name.c_str());
      if (rwops != NULL)
      {
         Import::GameStringsImporter gamestringsImporter(GetGameStrings());
         gamestringsImporter.LoadStringsPakFile(rwops);

         UaTrace("language \"%s\"\n",
            GetGameStrings().GetString(0x0a00, 0).c_str());
      }
      else
         UaTrace("not available\n");
   }

   m_resetTickTimer = true;
}

void Game::DoneGame()
{
   m_debugServer.Shutdown();

   if (m_scripting != NULL)
   {
      m_scripting->Done();
      delete m_scripting;
      m_scripting = NULL;
   }
}

void Game::PopScreen()
{
   // clear screen; this can take a while
   m_renderer.Clear();

   m_currentScreen->Destroy();
   delete m_currentScreen;

   if (m_screenStack.size() != 0)
   {
      // get last pushed screen
      m_currentScreen = m_screenStack.back();
      m_screenStack.pop_back();

      // send resume event
      SDL_Event user_event;
      user_event.type = SDL_USEREVENT;
      user_event.user.code = gameEventResumeScreen;
      user_event.user.data1 = NULL;
      user_event.user.data2 = NULL;
      SDL_PushEvent(&user_event);
   }
   else
   {
      // no more screens available
      m_currentScreen = NULL;
      m_exitGame = true;
   }
}

void Game::ReplaceScreen(Screen* newScreen, bool saveCurrent)
{
   // clear screen; this can take a while
   m_renderer.Clear();

   if (saveCurrent)
   {
      // save on m_screenStack
      m_screenStack.push_back(m_currentScreen);
   }
   else
   {
      // defer screen destruction
      if (m_currentScreen != NULL)
         m_screenToDestroy = m_currentScreen;
   }

   // initialize new screen
   m_currentScreen = newScreen;

   m_currentScreen->Init();

   // reset tick timer
   m_resetTickTimer = true;
}

void Game::RemoveScreen()
{
   // send "destroy screen" event
   SDL_Event event;
   event.type = SDL_USEREVENT;
   event.user.code = gameEventDestroyScreen;
   SDL_PushEvent(&event);
}
