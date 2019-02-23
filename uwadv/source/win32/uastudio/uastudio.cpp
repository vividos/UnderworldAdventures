//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2004,2005,2019 Underworld Adventures Team
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
/// \file uastudio.cpp
/// \brief underworld adventures studio main
//
#include "common.hpp"
#include "base/settings.hpp"
#include "base/resourcemanager.hpp"
#include "base/textfile.hpp"
#include "script/scripting.hpp"
#include "underw/underworld.hpp"
#include "import/import.hpp"
#include "import/gamestringsimporter.hpp"
#include "game_interface.hpp"
#include "gamestrings.hpp"
#include "uwadv/debug.hpp"
#include "uwadv/gamecfg.hpp"
#include <ctime>

/// Underworld Adventures studio
class uastudio :
   public IBasicGame,
   public IUserInterface
{
public:
   /// ctor
   uastudio()
   {
   }

   void init();
   void done();

   void run();

   // virtual IBasicGame methods
   virtual double get_tickrate() { return 20.0; }
   virtual bool pause_game(bool pause);
   virtual Base::Settings& get_settings() override { return settings; }
   virtual Base::ResourceManager& GetResourceManager() override { return *m_resourceManager.get(); }
   virtual Base::SavegamesManager& get_savegames_manager() override { return *m_savegamesManager.get(); };
   virtual IScripting& get_scripting() override { return *scripting; }
   virtual ua_debug_server& get_debugger() override { return debug_server; }
   virtual GameStrings& GetGameStrings() override { return gamestrings; };
   virtual Underworld::Underworld& GetUnderworld() override { return m_gameLogic->GetUnderworld(); }
   virtual Underworld::GameLogic& GetGameLogic() override { return *m_gameLogic.get(); }
   virtual IUserInterface* GetUserInterface() override { return this; }

   // virtual IGame methods
   virtual void init_game();
   virtual void done_game();

   // virtual IUserInterface methods
      /// notifies callback class
   virtual void uw_notify(EUserInterfaceNotification notify,
      unsigned int param = 0)
   {
      UaTrace("Notify: %u, param=%u\n", notify, param);
   }

   /// called to print text to textscroll
   virtual void uw_print(const char* text)
   {
      UaTrace("Print: %s\n", text);
   }

   /// starts conversation with object in current level, on list position
   virtual void uw_start_conversation(Uint16 list_pos)
   {
      UaTrace("StartConversation: list_pos=%04x\n", list_pos);
   }

protected:
   /// game settings
   Base::Settings settings;

   /// resource files manager
   std::unique_ptr<Base::ResourceManager> m_resourceManager;

   /// savegames manager
   std::unique_ptr<Base::SavegamesManager> m_savegamesManager;

   /// game logic object
   std::unique_ptr<Underworld::GameLogic> m_gameLogic;

   /// scripting class
   IScripting* scripting;

   /// underworld debugger - server side
   ua_debug_server debug_server;

   /// game strings
   GameStrings gamestrings;

   /// indicates if the game is currently paused
   bool paused;
};



void uastudio::init()
{
   UaTrace("Underworld Adventures Studio\n\n");

   // init random number generator
   srand((unsigned)time(NULL));

   // init files manager; settings are loaded here, too
   Base::LoadSettings(settings);
   m_resourceManager = std::make_unique<Base::ResourceManager>(settings);

   // set uw1 as path
   std::string prefix("uw1");
   settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));
   settings.SetValue(Base::settingGamePrefix, prefix);

   // re-init, with proper underworld path
   m_resourceManager = std::make_unique<Base::ResourceManager>(settings);

   // init savegames manager
   m_savegamesManager = std::make_unique<Base::SavegamesManager>(settings);

   // init game components; uw prefix and path is now known
   init_game();

   // init debug server and start
   debug_server.init();
   debug_server.start_debugger(this);

   paused = true;
}

void uastudio::done()
{
   done_game();

   debug_server.shutdown();

   SDL_Quit();
}

void uastudio::run()
{
   UaTrace("\nuastudio main loop started\n");

   Uint32 now, then = SDL_GetTicks();

   bool exit_game = false;

   paused = true;

   const double tickrate = 20.0;
   unsigned int tickcount = 0;

   // main game loop
   while (!exit_game)
   {
      now = SDL_GetTicks();

      while ((now - then) > (1000.0 / tickrate))
      {
         then += Uint32(1000.0 / tickrate);

         // do game logic
         if (!paused)
         {
            m_gameLogic->EvaluateUnderworld(double(tickcount) / tickrate);

            tickcount++;
         }

         // check if debugger is still running
         if (!debug_server.is_debugger_running())
            exit_game = true;
      }

      // do server side debug processing
      debug_server.tick();

      Sleep(10);
   }

   UaTrace("uastudio main loop ended\n\n");
}

bool uastudio::pause_game(bool pause)
{
   bool old_paused = paused;
   paused = pause;
   return old_paused;
}

void uastudio::init_game()
{
   std::string prefix(settings.GetString(Base::settingGamePrefix));

   UaTrace("initializing game; prefix: %s\n", prefix.c_str());

   // load game config file
   std::string gamecfg_name(prefix);
   gamecfg_name.append("/game.cfg");

   m_savegamesManager->SetNewGamePrefix(prefix.c_str());

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

   m_gameLogic = std::make_unique<Underworld::GameLogic>(this, scripting);

   Import::GameStringsImporter importer(GetGameStrings());
   importer.LoadDefaultStringsPakFile(GetResourceManager());

   UaTrace("using generic uw-path: %s\n",
      settings.GetString(Base::settingUnderworldPath).c_str());

   // TODO: load savegame
   {
      // load initial game
      Import::LoadUnderworld(get_settings(), GetResourceManager(), GetUnderworld());

      // init new game
      get_scripting().init_new_game();
   }
}

void uastudio::done_game()
{
   if (scripting != NULL)
   {
      scripting->done();
      delete scripting;
      scripting = NULL;
   }
}

#undef main

/// uastudio main function
int main()
{
#ifndef HAVE_DEBUG // in debug mode the debugger catches exceptions
   try
   {
#endif
      uastudio studio;

      studio.init();
      studio.run();
      studio.done();
#ifndef HAVE_DEBUG
   }
   catch (const Base::Exception& ex)
   {
      std::string text("caught Base::Exception:\n\r");
      text.append(ex.what());
      UaTrace(text.c_str());

      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Underworld Adventures Studio", text.c_str(), NULL);
   }
   catch (const std::exception& ex)
   {
      ex;
      UaTrace("caught std::exception\n");
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Underworld Adventures Studio", "std::exception", NULL);
   }
#endif

   return 0;
}
