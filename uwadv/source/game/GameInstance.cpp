//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019,2022,2023 Underworld Adventures Team
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
/// \file GameInstance.cpp
/// \brief game instance implementation
//
#include "pch.hpp"
#include "GameInstance.hpp"
#include "TextFile.hpp"
#include "GameConfigLoader.hpp"
#include "import/Import.hpp"
#include "import/GameStringsImporter.hpp"
#include "physics/GeometryProvider.hpp"

GameInstance::GameInstance()
   :m_isPaused(false),
   m_gameConfig(BaseGameNone)
{
   UaTrace("initializing game ...\n\n");

   // init files manager; settings are loaded here, too
   Base::LoadSettings(m_settings);
   m_resourceManager = std::make_unique<Base::ResourceManager>(m_settings);
}

void GameInstance::InitNewGame()
{
   InitGame();

   Import::LoadUnderworld(
      GetSettings(),
      GetResourceManager(),
      GetUnderworld());

   GetScripting().InitNewGame();
}

void GameInstance::InitCustomGame(const std::string& customGamePrefix)
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

void GameInstance::LoadSavegame(const std::string& savegameFilename)
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

void GameInstance::RunStandalone(double tickRate)
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

void GameInstance::Tick(double elapsed)
{
   if (!m_isPaused)
      GetGameLogic().EvaluateUnderworld(elapsed);

   // do server side debug processing
   m_debugServer.Tick();
}

void GameInstance::InitGame()
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

/// tries to load %prefix%/game.cfg
void GameInstance::LoadGameConfig(const std::string& gamePrefix)
{
   std::string gameConfigFilename{ gamePrefix };
   gameConfigFilename.append("/game.cfg");

   Base::SDL_RWopsPtr gameConfig =
      m_resourceManager->GetResourceFile(gameConfigFilename.c_str());

   // no game.cfg found? too bad ...
   if (gameConfig == nullptr)
   {
      std::string text = "could not find game.cfg for game prefix ";
      text += gamePrefix;
      throw Base::Exception(text.c_str());
   }

   m_gameConfig.Load(gameConfig, gameConfigFilename);
   ApplyGameConfig();
}

void GameInstance::ApplyGameConfig()
{
   Base::Settings& settings = GetSettings();
   Base::ResourceManager& resourceManager = GetResourceManager();

   switch (m_gameConfig.GetBaseGame())
   {
   case BaseGameUwDemo:
   case BaseGameUw1:
   {
      // check base path
      std::string base = settings.GetString(Base::settingUw1Path);
      if (base.empty())
         throw Base::Exception("path to uw1 was not specified in config file");

      // check game files
      bool uw1IsDemo = false;
      if (resourceManager.CheckUw1GameFilesAvailable(uw1IsDemo))
      {
         settings.SetGameType(Base::gameUw1);
         settings.SetValue(Base::settingUw1IsUwdemo, uw1IsDemo);
      }
      else
         throw Base::Exception("could not find relevant uw1 game files");

      // set generic uw path to uw1 path
      settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));
   }
   break;

   case BaseGameUw2:
   {
      // check base path
      std::string base = settings.GetString(Base::settingUw2Path);
      if (base.empty())
         throw Base::Exception("path to uw2 was not specified in config file");

      // check for uw2 game files
      if (resourceManager.CheckUw2GameFilesAvailable())
         settings.SetGameType(Base::gameUw2);
      else
         throw Base::Exception("could not find relevant uw2 game files");

      // set generic uw path to uw2 path
      settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw2Path));
      settings.SetGameType(Base::gameUw2);
   }
   break;

   case BaseGameNone:
   default:
      // nothing to check
      break;
   }

   if (m_gameConfig.GetScriptingLanguage() == scriptingLanguageLua)
   {
      // init Lua scripting
      m_scripting = IScripting::CreateScripting(scriptingLanguageLua);

      if (m_scripting == nullptr)
         throw Base::Exception("could not create scripting object");

      m_scripting->Init(this);

      for (const auto& scriptingFilename : m_gameConfig.GetScriptingFilesList())
         GetScripting().LoadScript(scriptingFilename.c_str());
   }

   for (const auto& gameStringsFilename : m_gameConfig.GetGameStringsFilesList())
   {
      Base::SDL_RWopsPtr rwops = GetResourceManager().GetResourceFile(
         gameStringsFilename);

      if (rwops != nullptr)
      {
         // add strings.pak-like file
         Import::GameStringsImporter importer{ GetGameStrings() };
         importer.LoadStringsPakFile(rwops);
      }
      else
         UaTrace("could not load strings file %s\n", gameStringsFilename.c_str());
   }
}

void GameInstance::DoneGame()
{
   m_debugServer.Shutdown();

   if (m_scripting != nullptr)
   {
      m_scripting->Done();
      delete m_scripting;
      m_scripting = nullptr;
   }
}

bool GameInstance::PauseGame(bool pause)
{
   bool oldPaused = m_isPaused;
   m_isPaused = pause;
   return oldPaused;
}
