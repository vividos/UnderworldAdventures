//
// Underworld Adventures - an Ultima Underworld remake project
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

#include <SDL2/SDL.h>
#include "ui/Screen.hpp"
#include "audio/Audio.hpp"
#include "renderer/RenderWindow.hpp"
#include "renderer/Viewport.hpp"
#include "renderer/Renderer.hpp"
#include "game/GameInstance.hpp"
#include "game/GameScreenHost.hpp"
#include "MainGameLoop.hpp"

/// main game class
class Game :
   public MainGameLoop,
   public IGame
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

   // virtual IGame methods

   virtual IGameInstance& GetGameInstance() override { return m_gameInstance; }
   virtual const IGameInstance& GetConstGameInstance() const override { return m_gameInstance; }

   /// sets up game
   virtual void SetupGame() override;

   virtual double GetTickRate() const override
   {
      return m_tickRate;
   }

   virtual Audio::AudioManager& GetAudioManager() override
   {
      return *m_audioManager.get();
   }

   virtual Renderer& GetRenderer() override
   {
      return m_renderer;
   }

   virtual RenderWindow& GetRenderWindow() override
   {
      UaAssert(m_renderWindow != nullptr);
      return *m_renderWindow.get();
   }

   virtual Viewport& GetViewport() override
   {
      UaAssert(m_viewport != nullptr);
      return *m_viewport.get();
   }

   virtual IScreenHost& GetScreenHost() override
   {
      return m_gameScreenHost;
   }

   // MainGameLoop virtual methods

   /// sets new window title
   virtual void UpdateCaption(const char* windowTitle) override;

   /// does tick processing
   virtual void OnTick(bool& resetTickTimer) override;

   /// renders the screen
   virtual void OnRender() override;

   /// processes SDL events
   virtual void OnEvent(SDL_Event& event) override;

private:
   /// initializes SDL and creates a window
   void InitSDL();

   /// toggles fullscreen and windowed mode
   void ToggleFullscreen();

   /// clears screen with black color
   void ClearScreen();

private:
   /// game instance
   GameInstance m_gameInstance;

   /// render window
   std::unique_ptr<RenderWindow> m_renderWindow;

   /// render viewport
   std::unique_ptr<Viewport> m_viewport;

   /// game screen host
   GameScreenHost m_gameScreenHost;

   /// screen width
   unsigned int m_width;

   /// screen height
   unsigned int m_height;

   /// controls how many ticks per second are done
   const unsigned int m_tickRate;

   /// action to carry out after init
   /*! actions: 0=normal game start, 1=load savegame, 2=load custom game */
   unsigned int m_initAction;

   /// savegame to load
   std::string m_savegameName;

   /// audio manager object
   std::unique_ptr<Audio::AudioManager> m_audioManager;

   /// renderer class
   Renderer m_renderer;
};
