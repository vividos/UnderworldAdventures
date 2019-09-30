//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2019 Underworld Adventures Team
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
/// \file MainGameLoop.hpp
/// \brief main game loop
//
#pragma once

#include <string>
#include <atomic>

union SDL_Event;

/// main game loop for SDL2 game
class MainGameLoop
{
public:
   /// creates a new main game loop object
   MainGameLoop(const char* windowTitle, bool updateFrameCount = false);

   /// runs game loop
   void RunGameLoop(unsigned int tickRate);

   /// quits game loop; can be called from other threads
   void QuitLoop();

   // new virtual methods

   /// called to update caption
   virtual void UpdateCaption(const char* windowTitle) { windowTitle; }

   /// processes game logic
   virtual void OnTick(bool& resetTickTimer) { resetTickTimer; }

   /// renders screen
   virtual void OnRender() {}

   /// called when new event was received
   virtual void OnEvent(SDL_Event& event) { event; }

private:
   /// processes new events
   void ProcessEvents();

private:
   /// window title to use for formatting
   std::string m_windowTitle;

   /// indicates if frame count should be updated in the title
   bool m_updateFrameCount;

   /// indicates that the game loop should exit
   std::atomic<bool> m_exitLoop;

   /// indicates if the application is currently active (in foreground)
   bool m_appActive;
};
