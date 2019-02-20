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
/// \file uwadv.hpp
/// \brief game class definition
//
#pragma once

#include "base/settings.hpp"
#include "base/resourcemanager.hpp"
#include "ui/screen.hpp"
#include "audio/audio.hpp"
#include "ui/image.hpp"
#include "renderer/renderer.hpp"
#include "script/scripting.hpp"
#include "underw/gamelogic.hpp"
#include "game_interface.hpp"
#include "debug.hpp"
#include "gamestrings.hpp"

/// main game class
class ua_uwadv_game : public IGame
{
public:
   /// ctor
   ua_uwadv_game();
   /// dtor
   virtual ~ua_uwadv_game() {}

   /// initializes video, audio, etc.
   void init();

   /// parses command line arguments
   void parse_args(unsigned int argc, const char** argv);

   /// runs the game's main loop
   void run();

   /// cleans up all stuff
   void done();

   // IBasicGame methods
   virtual double get_tickrate() override
   {
      return tickrate;
   }

   virtual bool pause_game(bool pause) override;
   virtual Base::Settings& get_settings() override
   {
      return settings;
   }

   virtual Base::ResourceManager& GetResourceManager() override
   {
      return *m_resourceManager.get();
   }

   virtual Base::SavegamesManager& get_savegames_manager() override
   {
      return *savegames_manager.get();
   }

   virtual IScripting& get_scripting() override
   {
      return *scripting;
   }

   virtual ua_debug_server& get_debugger() override
   {
      return debug;
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
   virtual void init_game() override;
   virtual void done_game() override;
   virtual Audio::AudioManager& get_audio_manager() override
   {
      return *audio_manager.get();
   }

   virtual ua_image_manager& get_image_manager() override
   {
      return image_manager;
   }

   virtual ua_renderer& get_renderer() override
   {
      return renderer;
   }

   virtual void replace_screen(ua_screen* new_screen, bool save_current) override;
   virtual void remove_screen() override;
   virtual unsigned int get_screen_xres() override { return width; }
   virtual unsigned int get_screen_yres() override { return height; }

protected:
   /// initializes SDL and creates a window
   void init_sdl();

   /// processes SDL events
   void process_events();

   /// deletes current screen and pops off last screen from screenstack
   void pop_screen();

protected:
   /// game configuration
   Base::Settings settings;

   /// resource files manager
   std::unique_ptr<Base::ResourceManager> m_resourceManager;

   SDL_Window* m_window;

   SDL_GLContext m_context;

   /// screen width
   unsigned int width;

   /// screen height
   unsigned int height;

   /// controls how many ticks per second are done
   const unsigned int tickrate;

   /// indicates if game can be exited
   bool exit_game;

   /// indicates if the game is currently paused
   bool paused;

   /// action to carry out after init
   /*! actions: 0=normal game start, 1=load savegame, 2=load custom game */
   unsigned int init_action;

   /// savegame to load
   std::string savegame_name;

   /// current screen
   ua_screen* curscreen;

   /// stack of screens
   std::vector<ua_screen*> screenstack;

   /// true when tick timer should be resetted for the next cycle
   bool reset_tick_timer;

   /// audio manager object
   std::unique_ptr<Audio::AudioManager> audio_manager;

   /// savegames manager
   std::unique_ptr<Base::SavegamesManager> savegames_manager;

   /// image manager
   ua_image_manager image_manager;

   /// renderer class
   ua_renderer renderer;

   /// scripting class
   IScripting* scripting;

   /// game logic object
   /// underworld object
   std::unique_ptr<Underworld::GameLogic> m_gameLogic;

   /// user interface; the ingame screen
   IUserInterface* m_userInterface;

   /// screen queued to destroy
   ua_screen* screen_to_destroy;

   /// underworld debugger - server side
   ua_debug_server debug;
};
