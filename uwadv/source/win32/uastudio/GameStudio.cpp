//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2004,2005,2019,2022,2023 Underworld Adventures Team
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
#include "game/GameInstance.hpp"

/// Underworld Adventures studio
class GameStudio
{
public:
   /// ctor
   GameStudio();

   void Init();
   void Done();

   void Run();

private:
   /// game instance to run the game studio on
   GameInstance m_gameInstance;
};


GameStudio::GameStudio()
{
   UaTrace("Underworld Adventures Studio\n\n");

   DebugServer& debugServer =
      *dynamic_cast<DebugServer*>(&m_gameInstance.GetDebugger());

   debugServer.SetStudioMode();
}

void GameStudio::Init()
{
   // set uw1 as path
   Base::Settings& settings = m_gameInstance.GetSettings();
   settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));

   std::string gamePrefix{ "uw1" };
   settings.SetValue(Base::settingGamePrefix, gamePrefix);

   m_gameInstance.InitNewGame();

   m_gameInstance.PauseGame(true);

   m_gameInstance.GetDebugger().StartDebugger(&m_gameInstance);
}

void GameStudio::Done()
{
   m_gameInstance.GetGameLogic().RegisterUserInterface(nullptr);

   m_gameInstance.DoneGame();

   SDL_Quit();
}

void GameStudio::Run()
{
   UaTrace("\nuastudio main loop started\n");

   m_gameInstance.RunStandalone();

   UaTrace("uastudio main loop ended\n\n");
}


/// uastudio main function
int main(int argc, char* argv[])
{
   UNUSED(argc);
   UNUSED(argv);

   try
   {
      GameStudio studio;

      studio.Init();
      studio.Run();
      studio.Done();
   }
   catch (const std::exception& ex)
   {
      std::string text("Error occured: ");
      text.append(ex.what());

      UaTrace(text.c_str());

      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Underworld Adventures Studio", text.c_str(), nullptr);
   }

   return 0;
}
