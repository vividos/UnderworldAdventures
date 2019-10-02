//
// Underworld Adventures - an Ultima Underworld remake project
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
/// \file GameStudio.cpp
/// \brief underworld adventures studio main
//
#include "common.hpp"
#include "base/Settings.hpp"
#include "base/ResourceManager.hpp"
#include "base/TextFile.hpp"
#include "script/IScripting.hpp"
#include "underw/Underworld.hpp"
#include "import/Import.hpp"
#include "import/GameStringsImporter.hpp"
#include "GameInterface.hpp"
#include "GameStrings.hpp"
#include "uwadv/DebugServer.hpp"
#include "uwadv/GameConfigLoader.hpp"
#include "physics/PhysicsModel.hpp"
#include <ctime>

/// Underworld Adventures studio
class GameStudio :
   public IBasicGame,
   public IUserInterface,
   public IPhysicsModelCallback
{
public:
   /// ctor
   GameStudio()
   {
   }

   void Init();
   void Done();

   void Run();

   // virtual IBasicGame methods
   virtual double GetTickRate() { return 20.0; }
   virtual bool PauseGame(bool pause);
   virtual Base::Settings& GetSettings() override { return m_settings; }
   virtual Base::ResourceManager& GetResourceManager() override { return *m_resourceManager.get(); }
   virtual Base::SavegamesManager& GetSavegamesManager() override { return *m_savegamesManager.get(); };
   virtual IScripting& GetScripting() override { return *m_scripting; }
   virtual DebugServer& GetDebugger() override { return m_debugServer; }
   virtual GameStrings& GetGameStrings() override { return m_gameStrings; };
   virtual Underworld::Underworld& GetUnderworld() override { return m_gameLogic->GetUnderworld(); }
   virtual Underworld::GameLogic& GetGameLogic() override { return *m_gameLogic.get(); }
   virtual IUserInterface* GetUserInterface() override { return this; }
   virtual IPhysicsModelCallback& GetPhysicsModelCallback() override { return *this; }

   // virtual IGame methods
   virtual void InitGame();
   virtual void DoneGame();

   // virtual IUserInterface methods

   /// notifies callback class
   virtual void Notify(UserInterfaceNotification notify,
      unsigned int param = 0) override
   {
      UaTrace("Notify: %u, param=%u\n", notify, param);
   }

   /// called to print text to textscroll
   virtual void PrintScroll(const char* text) override
   {
      UaTrace("Print: %s\n", text);
   }

   /// starts conversation with object in current level, on list position
   virtual void StartConversation(Uint16 listPos) override
   {
      UaTrace("StartConversation: listPos=%04x\n", listPos);
   }

   // virtual IPhysicsModelCallback methods
   virtual void GetSurroundingTriangles(unsigned int xpos,
      unsigned int ypos, std::vector<Triangle3dTextured>& allTriangles) override
   {
      // returns no triangles
   }

protected:
   /// game settings
   Base::Settings m_settings;

   /// resource files manager
   std::unique_ptr<Base::ResourceManager> m_resourceManager;

   /// savegames manager
   std::unique_ptr<Base::SavegamesManager> m_savegamesManager;

   /// game logic object
   std::unique_ptr<Underworld::GameLogic> m_gameLogic;

   /// scripting class
   IScripting* m_scripting;

   /// underworld debugger - server side
   DebugServer m_debugServer;

   /// game strings
   GameStrings m_gameStrings;

   /// indicates if the game is currently paused
   bool m_isPaused;
};



void GameStudio::Init()
{
   UaTrace("Underworld Adventures Studio\n\n");

   // init random number generator
   srand((unsigned)time(NULL));

   // init files manager; settings are loaded here, too
   Base::LoadSettings(m_settings);
   m_resourceManager = std::make_unique<Base::ResourceManager>(m_settings);

   // set uw1 as path
   std::string prefix("uw1");
   m_settings.SetValue(Base::settingUnderworldPath, m_settings.GetString(Base::settingUw1Path));
   m_settings.SetValue(Base::settingGamePrefix, prefix);

   // rescan files, with proper underworld path
   m_resourceManager->Rescan(m_settings);

   // init savegames manager
   m_savegamesManager = std::make_unique<Base::SavegamesManager>(m_settings);

   // init game components; uw prefix and path is now known
   InitGame();

   // init debug server and start
   m_debugServer.Init();
   m_debugServer.StartDebugger(this);

   m_isPaused = true;
}

void GameStudio::Done()
{
   DoneGame();

   m_debugServer.Shutdown();

   SDL_Quit();
}

void GameStudio::Run()
{
   UaTrace("\nuastudio main loop started\n");

   Uint32 now, then = SDL_GetTicks();

   bool exitGame = false;

   m_isPaused = true;

   const double tickrate = 20.0;
   unsigned int tickcount = 0;

   // main game loop
   while (!exitGame)
   {
      now = SDL_GetTicks();

      while ((now - then) > (1000.0 / tickrate))
      {
         then += Uint32(1000.0 / tickrate);

         // do game logic
         if (!m_isPaused)
         {
            m_gameLogic->EvaluateUnderworld(double(tickcount) / tickrate);

            tickcount++;
         }

         // check if debugger is still running
         if (!m_debugServer.IsDebuggerRunning())
            exitGame = true;
      }

      // do server side debug processing
      m_debugServer.Tick();

      Sleep(10);
   }

   UaTrace("uastudio main loop ended\n\n");
}

bool GameStudio::PauseGame(bool pause)
{
   bool isPaused = m_isPaused;
   m_isPaused = pause;
   return isPaused;
}

void GameStudio::InitGame()
{
   std::string prefix(m_settings.GetString(Base::settingGamePrefix));

   UaTrace("initializing game; prefix: %s\n", prefix.c_str());

   // load game config file
   std::string gamecfg_name(prefix);
   gamecfg_name.append("/game.cfg");

   m_savegamesManager->SetNewGamePrefix(prefix.c_str());

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

   m_gameLogic = std::make_unique<Underworld::GameLogic>(m_scripting);
   GetGameLogic().RegisterUserInterface(this);

   Import::GameStringsImporter importer(GetGameStrings());
   importer.LoadDefaultStringsPakFile(GetResourceManager());

   Import::ImportProperties(GetResourceManager(), GetGameLogic().GetObjectProperties());

   UaTrace("using generic uw-path: %s\n",
      m_settings.GetString(Base::settingUnderworldPath).c_str());

   // TODO: load savegame
   {
      // load initial game
      Import::LoadUnderworld(GetSettings(), GetResourceManager(), GetUnderworld());

      // init new game
      GetScripting().InitNewGame();
   }
}

void GameStudio::DoneGame()
{
   if (m_scripting != NULL)
   {
      m_scripting->Done();
      delete m_scripting;
      m_scripting = NULL;
   }
}

/// uastudio main function
int main(int argc, char* argv[])
{
   UNUSED(argc);
   UNUSED(argv);

#ifndef HAVE_DEBUG // in debug mode the debugger catches exceptions
   try
   {
#endif
      GameStudio studio;

      studio.Init();
      studio.Run();
      studio.Done();
#ifndef HAVE_DEBUG
   }
   catch (const Base::Exception& ex)
   {
      std::string text("caught Base::Exception: %s\n\r", ex.what());
      text.append(ex.what());
      UaTrace(text.c_str());

      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Underworld Adventures Studio", text.c_str(), NULL);
   }
   catch (const std::exception& ex)
   {
      UaTrace("caught std::exception: %s\n", ex.what());
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Underworld Adventures Studio", "std::exception", NULL);
   }
#endif

   return 0;
}
