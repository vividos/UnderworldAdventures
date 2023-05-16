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
/// \file BasicGame.cpp
/// \brief basic game implementation
//
#include "pch.hpp"
#include "BasicGame.hpp"
#include "TextFile.hpp"
#include "GameConfigLoader.hpp"
#include "import/Import.hpp"
#include "import/GameStringsImporter.hpp"
#include "physics/GeometryProvider.hpp"

BasicGame::BasicGame()
   :m_isPaused(false)
{
   UaTrace("initializing game ...\n\n");

   // init files manager; settings are loaded here, too
   Base::LoadSettings(m_settings);
   m_resourceManager = std::make_unique<Base::ResourceManager>(m_settings);
}

void BasicGame::InitNewGame()
{
   InitGame();

   Import::LoadUnderworld(
      GetSettings(),
      GetResourceManager(),
      GetUnderworld());

   GetScripting().InitNewGame();
}

void BasicGame::InitCustomGame(const std::string& customGamePrefix)
{
   m_settings.SetValue(Base::settingGamePrefix, customGamePrefix);

   UaTrace("starting custom game, with prefix %s\n",
      customGamePrefix.c_str());

   if (customGamePrefix == "uw2")
   {
      m_settings.SetGameType(Base::gameUw2);
      m_settings.SetValue(Base::settingUnderworldPath,
         m_settings.GetString(Base::settingUw2Path));
   }

   InitGame();
}

void BasicGame::LoadSavegame(const std::string& savegameFilename)
{
   UaTrace("loading savegame from file %s\n", savegameFilename.c_str());

   Base::Savegame sg = m_savegamesManager->GetSavegameFromFile(
      savegameFilename.c_str());

   // set game prefix
   std::string gamePrefix = sg.GetSavegameInfo().m_gamePrefix;
   m_settings.SetValue(Base::settingGamePrefix, gamePrefix);

   m_settings.SetValue(Base::settingUnderworldPath,
      m_settings.GetString(gamePrefix == "uw1"
         ? Base::settingUw1Path
         : Base::settingUw2Path));

   InitGame();

   GetUnderworld().Load(sg);
}

void BasicGame::RunStandalone(double tickRate)
{
   Uint32 now, then = SDL_GetTicks();

   bool exitGame = false;

   unsigned int tickCount = 0;

   // main game loop
   while (!exitGame)
   {
      now = SDL_GetTicks();

      while ((now - then) > (1000.0 / tickRate))
      {
         then += Uint32(1000.0 / tickRate);

         // do game logic
         Tick(double(tickCount) / tickRate);
         tickCount++;

         // check if debugger is still running
         if (!m_debugServer.IsDebuggerRunning())
            exitGame = true;
      }

      // do server side debug processing
      m_debugServer.Tick();

      SDL_Delay(10);
   }
}

void BasicGame::Tick(double elapsed)
{
   if (!m_isPaused)
      GetGameLogic().EvaluateUnderworld(elapsed);

   // do server side debug processing
   m_debugServer.Tick();
}

void BasicGame::InitGame()
{
   // rescan, with proper underworld path
   m_resourceManager->Rescan(m_settings);

   std::string gamePrefix{ m_settings.GetString(Base::settingGamePrefix) };

   UaTrace("initializing game; prefix: %s\n", gamePrefix.c_str());

   m_savegamesManager = std::make_unique<Base::SavegamesManager>(m_settings);
   m_savegamesManager->SetNewGamePrefix(gamePrefix);
   m_savegamesManager->Rescan();

   LoadGameConfig(gamePrefix);

   UaTrace("using generic uw-path: %s\n",
      m_settings.GetString(Base::settingUnderworldPath).c_str());

   m_imageManager = std::make_unique<ImageManager>(GetResourceManager());
   m_imageManager->Init();

   m_gameLogic = std::make_unique<Underworld::GameLogic>(m_scripting);

   UaTrace("loading game strings ... ");
   Import::GameStringsImporter importer(GetGameStrings());
   importer.LoadDefaultStringsPakFile(GetResourceManager());
   UaTrace("done\n\n");

   Import::ImportProperties(
      GetResourceManager(),
      GetGameLogic().GetObjectProperties());

   Import::ImportItemCombineEntries(
      GetResourceManager(),
      GetGameLogic().GetUnderworld().GetPlayer().GetInventory());

   m_debugServer.Init();

   // load language specific .pak file
   {
      UaTrace("loading language-specific strings ... ");

      std::string langPakFilename{ gamePrefix };
      langPakFilename.append("/lang.pak");

      Base::SDL_RWopsPtr rwops = m_resourceManager->GetResourceFile(langPakFilename);
      if (rwops != nullptr)
      {
         Import::GameStringsImporter gamestringsImporter{ GetGameStrings() };
         gamestringsImporter.LoadStringsPakFile(rwops);

         UaTrace("language \"%s\"\n",
            GetGameStrings().GetString(0x0a00, 0).c_str());
      }
      else
         UaTrace("not available\n");
   }
}

// tries to load %prefix%/game.cfg
void BasicGame::LoadGameConfig(const std::string& gamePrefix)
{
   std::string gameConfigFilename{ gamePrefix };
   gameConfigFilename.append("/game.cfg");

   GameConfigLoader configLoader{ *this, &m_scripting };

   Base::SDL_RWopsPtr gameConfig =
      m_resourceManager->GetResourceFile(gameConfigFilename.c_str());

   // no game.cfg found? too bad ...
   if (gameConfig == nullptr)
   {
      std::string text = "could not find game.cfg for game prefix ";
      text += gamePrefix;
      throw Base::Exception(text.c_str());
   }

   Base::TextFile textFile{ gameConfig };
   configLoader.Load(textFile);
}

void BasicGame::DoneGame()
{
   m_debugServer.Shutdown();

   if (m_scripting != nullptr)
   {
      m_scripting->Done();
      delete m_scripting;
      m_scripting = nullptr;
   }
}

bool BasicGame::PauseGame(bool pause)
{
   bool oldPaused = m_isPaused;
   m_isPaused = pause;
   return oldPaused;
}
