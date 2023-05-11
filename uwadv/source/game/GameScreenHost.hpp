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
/// \file GameScreenHost.hpp
/// \brief game screen host
//
#pragma once

#include <functional>
#include "IScreenHost.hpp"

/// <summary>
/// Game screen host
/// </summary>
class GameScreenHost : public IScreenHost
{
public:
   /// ctor
   GameScreenHost(std::function<void()> clearScreenFunc)
      :m_clearScreenFunc(clearScreenFunc)
   {
   }

   /// replaces current screen with a new one
   virtual void ReplaceScreen(Screen* newScreen, bool saveCurrent) override;

   /// removes current screen, going back to the previous screen, if any
   virtual void RemoveScreen() override;

   /// returns currently active game screen, or nullptr when none are left
   Screen* GetCurrentScreen() { return m_currentScreen; }

   /// processes a game tick
   void Tick(bool& resetTickTimer);

   /// pops a screen from stack; returns false when there are no more screens
   bool PopScreen();

   /// cleans up all remaining game screens
   void Cleanup();

private:
   /// function to clear the screen
   std::function<void()> m_clearScreenFunc;

   /// current screen
   Screen* m_currentScreen = nullptr;

   /// stack of screens
   std::vector<Screen*> m_screenStack;

   /// true when tick timer should be resetted for the next cycle
   bool m_resetTickTimer = true;

   /// screen queued to destroy
   Screen* m_screenToDestroy = nullptr;
};
