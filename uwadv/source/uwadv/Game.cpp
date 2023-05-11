//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019,2022 Underworld Adventures Team
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
#include "pch.hpp"
#include "Game.hpp"
#include "TextFile.hpp"
#include "game/GameConfigLoader.hpp"
#include "screens/UwadvMenuScreen.hpp"
#include "screens/OriginalIngameScreen.hpp"
#include "screens/StartSplashScreen.hpp"
#include "import/Import.hpp"
#include "import/GameStringsImporter.hpp"
#include <ctime>
#include <SDL2/SDL.h>

#ifdef WIN32
#include "win32/resource.h"
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
#ifdef HAVE_DEBUG
   :MainGameLoop("Underworld Adventures", true),
#else
   :MainGameLoop("Underworld Adventures", false),
#endif
   m_tickRate(20),
   m_gameScreenHost([this]() { ClearScreen(); })
{
   printf("Underworld Adventures"
#ifdef HAVE_DEBUG
      " (debug mode)"
#endif
      "\nhttps://vividos.github.com/UnderworldAdventures/\n\n");
}

void Game::Init()
{
   // find out selected screen resolution
   {
      // predefined is 640x480
      m_width = 640;
      m_height = 480;

      std::string screen_res{ GetSettings().GetString(Base::settingScreenResolution) };

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
      if (arg.empty())
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
            " Syntax:\n"
            "   uwadv {options} [savegame-filename] {options}\n"
            " Available options:\n"
            "  -h -? -H  --help           Shows help\n"
            "  -g <game> --game <game>    Starts custom game\n"
         );
         return;

         // game prefix
      case argumentGame:
      {
         m_initAction = 2;

         std::string customGamePrefix{ argv[i + 1] };
         GetSettings().SetValue(Base::settingGamePrefix, customGamePrefix);
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
      m_gameScreenHost.ReplaceScreen(new UwadvMenuScreen(*this), false);
      break;

   case 1: // load savegame
   {
      BasicGame::LoadSavegame(m_savegameName);

      // immediately start game
      m_gameScreenHost.ReplaceScreen(new OriginalIngameScreen(*this), false);
   }
   break;

   case 2: // start custom game
   {
      std::string customGamePrefix =
         GetSettings().GetString(Base::settingGamePrefix);

      BasicGame::InitCustomGame(customGamePrefix);

      if (customGamePrefix == "uw2")
      {
         // start splash screen
         m_gameScreenHost.ReplaceScreen(new StartSplashScreen(*this), false);
      }
      else
      {
         // other custom games currently don't have a splash screen
         m_gameScreenHost.ReplaceScreen(new OriginalIngameScreen(*this), false);
      }
   }
   break;

   default:
      break;
   }


   UaTrace("\n");

   MainGameLoop::RunGameLoop(m_tickRate);
}

void Game::UpdateCaption(const char* windowTitle)
{
   m_renderWindow->SetWindowTitle(windowTitle);
}

void Game::OnTick(bool& resetTickTimer)
{
   m_gameScreenHost.Tick(resetTickTimer);
}

void Game::OnRender()
{
   // draw the screen
   m_gameScreenHost.GetCurrentScreen()->Draw();
   m_renderWindow->SwapBuffers();
}

void Game::Done()
{
   m_gameScreenHost.Cleanup();

   DoneGame();

   SDL_Quit();
}

void Game::InitSDL()
{
   // output SDL version number
   {
      SDL_version version = { 0 };
      SDL_GetVersion(&version);
      UaTrace("using SDL %u.%u.%u\n", version.major, version.minor, version.patch);
   }

   m_renderWindow = std::make_unique<RenderWindow>(
      m_width, m_height,
      "Underworld Adventures",
      GetSettings().GetBool(Base::settingFullscreen));

#ifdef WIN32
   m_renderWindow->SetWindowIcon(IDI_ICON);
#endif

   UaTrace("\n");

   m_viewport = std::make_unique<Viewport>(*m_renderWindow.get());

   m_renderer.SetViewport(m_viewport.get());

   Renderer::PrintOpenGLDiagnostics();
}

void Game::OnEvent(SDL_Event& event)
{
   // let the screen handle the event first
   m_gameScreenHost.GetCurrentScreen()->ProcessEvent(event);

   switch (event.type)
   {
   case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_x &&
         (event.key.keysym.mod & KMOD_ALT) != 0)
         QuitLoop();

      if (event.key.keysym.sym == SDLK_RETURN &&
         (event.key.keysym.mod & KMOD_ALT) != 0)
         ToggleFullscreen();
      break;

   case SDL_USEREVENT:
      switch (event.user.code)
      {
      case gameEventDestroyScreen:
         if (!m_gameScreenHost.PopScreen())
         {
            QuitLoop();
            return; // don't process events anymore
         }
         break;

      default:
         break;
      }
      break;
   }
}

void Game::InitGame()
{
   BasicGame::InitGame();

   m_renderer.InitGame(GetGameInstance());

   m_audioManager = std::make_unique<Audio::AudioManager>(GetSettings(), GetResourceManager());
}

void Game::ToggleFullscreen()
{
   bool isFullscreen = !GetSettings().GetBool(Base::settingFullscreen);
   GetSettings().SetValue(Base::settingFullscreen, isFullscreen);

   m_renderWindow->SetFullscreen(isFullscreen);
}

void Game::ClearScreen()
{
   m_renderWindow->Clear();
   m_renderWindow->SwapBuffers();
}
