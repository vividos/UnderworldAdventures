//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
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
/// \file Game.hpp
/// \brief game class definition
//
#pragma once

#include <SDL.h>
#include "base/Settings.hpp"
#include "base/ResourceManager.hpp"
#include "ui/Screen.hpp"
#include "audio/Audio.hpp"
#include "ui/IndexedImage.hpp"
#include "renderer/Renderer.hpp"
#include "script/IScripting.hpp"
#include "underw/GameLogic.hpp"
#include "GameInterface.hpp"
#include "DebugServer.hpp"
#include "GameStrings.hpp"

/// main game class
class Game : public IGame
{
public:
   /// ctor
   Game();
   /// dtor
   virtual ~Game() {}

   /// initializes video, audio, etc.
   void Init();

   /// parses command line arguments
   void ParseArgs(unsigned int argc, const char** argv);

   /// runs the game's main loop
   void Run();

   /// cleans up all stuff
   void Done();

   // IBasicGame methods
   virtual double GetTickRate() override
   {
      return m_tickRate;
   }

   virtual bool PauseGame(bool pause) override;
   virtual Base::Settings& GetSettings() override
   {
      return m_settings;
   }

   virtual Base::ResourceManager& GetResourceManager() override
   {
      return *m_resourceManager.get();
   }

   virtual Base::SavegamesManager& GetSavegamesManager() override
   {
      return *m_savegamesManager.get();
   }

   virtual IScripting& GetScripting() override
   {
      return *m_scripting;
   }

   /// \todo replace with IDebugServer
   virtual DebugServer& GetDebugger() override
   {
      return m_debugServer;
   }

   virtual GameStrings& GetGameStrings() override
   {
      return m_gameLogic->GetGameStrings();
   }

   virtual Underworld::Underworld& GetUnderworld() override
   {
      return m_gameLogic->GetUnderworld();
   }

   virtual Underworld::GameLogic& GetGameLogic() override
   {
      return *m_gameLogic;
   }
   virtual IUserInterface* GetUserInterface() override
   {
      return m_userInterface;
   }

   // IGame methods
   virtual void InitGame() override;
   virtual void DoneGame() override;
   virtual Audio::AudioManager& GetAudioManager() override
   {
      return *m_audioManager.get();
   }

   virtual ImageManager& GetImageManager() override
   {
      return *m_imageManager;
   }

   virtual Renderer& GetRenderer() override
   {
      return m_renderer;
   }

   virtual void ReplaceScreen(Screen* newScreen, bool saveCurrent) override;
   virtual void RemoveScreen() override;
   virtual unsigned int GetScreenXRes() override { return m_width; }
   virtual unsigned int GetScreenYRes() override { return m_height; }

protected:
   /// initializes SDL and creates a window
   void InitSDL();

   /// processes SDL events
   void ProcessEvents();

   /// deletes current screen and pops off last screen from m_screenStack
   void PopScreen();

   /// sets window icon
   void SetWindowIcon() const;

   /// toggles fullscreen and windowed mode
   void ToggleFullscreen();

protected:
   /// game configuration
   Base::Settings m_settings;

   /// resource files manager
   std::unique_ptr<Base::ResourceManager> m_resourceManager;

   SDL_Window* m_window;

   SDL_GLContext m_context;

   /// screen width
   unsigned int m_width;

   /// screen height
   unsigned int m_height;

   /// controls how many ticks per second are done
   const unsigned int m_tickRate;

   /// indicates if game can be exited
   bool m_exitGame;

   /// indicates if the game is currently paused
   bool m_isPaused;

   /// action to carry out after init
   /*! actions: 0=normal game start, 1=load savegame, 2=load custom game */
   unsigned int m_initAction;

   /// savegame to load
   std::string m_savegameName;

   /// current screen
   Screen* m_currentScreen;

   /// stack of screens
   std::vector<Screen*> m_screenStack;

   /// true when tick timer should be resetted for the next cycle
   bool m_resetTickTimer;

   /// audio manager object
   std::unique_ptr<Audio::AudioManager> m_audioManager;

   /// savegames manager
   std::unique_ptr<Base::SavegamesManager> m_savegamesManager;

   /// image manager
   std::unique_ptr<ImageManager> m_imageManager;

   /// renderer class
   Renderer m_renderer;

   /// scripting class
   IScripting* m_scripting;

   /// game logic object
   /// underworld object
   std::unique_ptr<Underworld::GameLogic> m_gameLogic;

   /// user interface; the ingame screen
   IUserInterface* m_userInterface;

   /// screen queued to destroy
   Screen* m_screenToDestroy;

   /// underworld debugger - server side
   DebugServer m_debugServer;
};
