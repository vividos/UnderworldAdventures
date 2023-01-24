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
#include "game/BasicGame.hpp"

/// Underworld Adventures studio
class GameStudio :
   public IUserInterface
{
public:
   /// ctor
   GameStudio()
   {
      UaTrace("Underworld Adventures Studio\n\n");
   }

   void Init();
   void Done();

   void Run();

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

   /// starts showing cutscene with given number
   virtual void ShowCutscene(unsigned int cutsceneNumber) override
   {
      UaTrace("ShowCutscene: number=%u\n", cutsceneNumber);
   }

   /// shows map
   virtual void ShowMap() override
   {
      UaTrace("ShowMap\n");
   }

private:
   /// game instance to run the game studio on
   BasicGame m_gameInstance;
};


void GameStudio::Init()
{
   // set uw1 as path
   Base::Settings& settings = m_gameInstance.GetSettings();
   settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));

   std::string gamePrefix{ "uw1" };
   settings.SetValue(Base::settingGamePrefix, gamePrefix);

   m_gameInstance.InitNewGame();

   m_gameInstance.GetGameLogic().RegisterUserInterface(this);

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
