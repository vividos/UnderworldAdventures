//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2023 Underworld Adventures Team
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
/// \file GameScreenHost.cpp
/// \brief game screen host
//
#include "pch.hpp"
#include "GameScreenHost.hpp"
#include "Screen.hpp"

void GameScreenHost::ReplaceScreen(Screen* newScreen, bool saveCurrent)
{
   // clear screen; this can take a while
   m_clearScreenFunc();

   if (saveCurrent)
   {
      // save on m_screenStack
      m_screenStack.push_back(m_currentScreen);
   }
   else
   {
      // defer screen destruction
      if (m_currentScreen != nullptr)
         m_screenToDestroy = m_currentScreen;
   }

   // initialize new screen
   m_currentScreen = newScreen;

   m_currentScreen->Init();

   // reset tick timer
   m_resetTickTimer = true;
}

void GameScreenHost::RemoveScreen()
{
   // send "destroy screen" event
   SDL_Event event{};
   event.type = SDL_USEREVENT;
   event.user.code = gameEventDestroyScreen;
   SDL_PushEvent(&event);
}

void GameScreenHost::Tick(bool& resetTickTimer)
{
   // do game logic
   m_currentScreen->Tick();

   // check if there is a screen to destroy
   if (m_screenToDestroy != nullptr)
   {
      m_screenToDestroy->Destroy();
      delete m_screenToDestroy;
      m_screenToDestroy = nullptr;

      resetTickTimer = true;
   }
}

bool GameScreenHost::PopScreen()
{
   // clear screen; this can take a while
   m_clearScreenFunc();

   m_currentScreen->Destroy();
   delete m_currentScreen;

   if (!m_screenStack.empty())
   {
      // get last pushed screen
      m_currentScreen = m_screenStack.back();
      m_screenStack.pop_back();

      // send resume event
      SDL_Event userEvent{};
      userEvent.type = SDL_USEREVENT;
      userEvent.user.code = gameEventResumeScreen;
      userEvent.user.data1 = nullptr;
      userEvent.user.data2 = nullptr;
      SDL_PushEvent(&userEvent);

      return true;
   }
   else
   {
      // no more screens available
      m_currentScreen = nullptr;
      return false;
   }
}

void GameScreenHost::Cleanup()
{
   // free current screen
   if (m_currentScreen != nullptr)
   {
      m_currentScreen->Destroy();
      delete m_currentScreen;
      m_currentScreen = nullptr;
   }

   // free all screens on screen stack
   size_t max = m_screenStack.size();
   for (size_t screenIndex = 0; screenIndex < max; screenIndex++)
   {
      m_screenStack[screenIndex]->Destroy();
      delete m_screenStack[screenIndex];
   }

   m_screenStack.clear();
}
