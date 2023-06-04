//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2022,2023 Underworld Adventures Team
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
/// \file GameInstance.hpp
/// \brief game instance class
//
#pragma once

#include "IGameInstance.hpp"
#include "base/Settings.hpp"
#include "base/ResourceManager.hpp"
#include "ui/IndexedImage.hpp"
#include "ui/ImageManager.hpp"
#include "script/IScripting.hpp"
#include "underworld/GameLogic.hpp"
#include "underworld/GameStrings.hpp"
#include "physics/PhysicsModel.hpp"
#include "game/GameConfig.hpp"
#include "DebugServer.hpp"

/// game instance class
class GameInstance : public IGameInstance
{
public:
   /// ctor
   GameInstance();
   /// dtor
   virtual ~GameInstance() {}

   /// initializes new game with the currently set game prefix
   void InitNewGame();

   /// initializes a custom game with given game prefix
   void InitCustomGame(const std::string& customGamePrefix);

   /// loads a savegame from filename
   void LoadSavegame(const std::string& savegameFilename);

   /// runs the game instance in a standalone, non-interactive game loop
   void RunStandalone(double tickRate = 20.0);

   /// processes a single tick, using given elapsed time
   void Tick(double elapsed);

   // virtual IGameInstance methods

   /// initializes game instance
   virtual void InitGame() override;

   /// cleans up game instance
   virtual void DoneGame() override;

   /// pauses (or unpauses) game
   virtual bool PauseGame(bool pause) override;

   /// returns game config object
   virtual const GameConfig& GetGameConfig() override { return m_gameConfig; }

   /// returns game settings
   virtual Base::Settings& GetSettings() override
   {
      return m_settings;
   }

   /// returns resource manager
   virtual Base::ResourceManager& GetResourceManager() override
   {
      return *m_resourceManager.get();
   }

   /// returns savegames manager
   virtual Base::SavegamesManager& GetSavegamesManager() override
   {
      return *m_savegamesManager.get();
   }

   /// returns image manager
   virtual ImageManager& GetImageManager() override
   {
      return *m_imageManager.get();
   };

   /// returns scripting
   virtual IScripting& GetScripting() override
   {
      return *m_scripting;
   }

   /// returns debug server
   virtual IDebugServer& GetDebugger() override
   {
      return m_debugServer;
   }

   /// returns game strings
   virtual GameStrings& GetGameStrings() override
   {
      return m_gameLogic->GetGameStrings();
   }

   /// returns the underworld object
   virtual Underworld::Underworld& GetUnderworld() override
   {
      return m_gameLogic->GetUnderworld();
   }

   /// returns the game logic object
   virtual Underworld::GameLogic& GetGameLogic() override
   {
      return *m_gameLogic;
   }

   /// returns the physics model
   virtual Physics::PhysicsModel& GetPhysicsModel() override
   {
      return m_physicsModel;
   }

private:
   /// loads game.cfg for given game prefix
   void LoadGameConfig(const std::string& gamePrefix);

   /// applies loaded game config infos to game instance
   void ApplyGameConfig();

private:
   /// indicates if the game is currently paused
   bool m_isPaused;

   /// game config
   GameConfig m_gameConfig;

   /// game settings
   Base::Settings m_settings;

   /// resource files manager
   std::unique_ptr<Base::ResourceManager> m_resourceManager;

   /// savegames manager
   std::unique_ptr<Base::SavegamesManager> m_savegamesManager;

   /// image manager
   std::unique_ptr<ImageManager> m_imageManager;

   /// game logic object
   std::unique_ptr<Underworld::GameLogic> m_gameLogic;

   /// physics model for the game
   Physics::PhysicsModel m_physicsModel;

   /// scripting implementation
   std::unique_ptr<IScripting> m_scripting;

   /// underworld debugger - server side
   DebugServer m_debugServer;
};
