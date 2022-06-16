//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2019 Underworld Adventures Team
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
/// \file GameInterface.hpp
/// \brief game interface class
//
#pragma once

#include "IBasicGame.hpp"

namespace Audio
{
   class AudioManager;
}

class Renderer;
class RenderWindow;
class Viewport;
class Screen;
class IUserInterface;

/// game events that can be sent to the event queue via SDL_PushEvent using SDL_USEREVENT
enum GameEvents
{
   /// sent to destroy the current screen, pop the last one off the stack as new current screen
   gameEventDestroyScreen = 0,

   /// event sent when a screen resumes processing, e.g. when getting popped off the screen stack
   gameEventResumeScreen,

   /// sent to current screen when an text edit window finished editing
   gameEventTexteditFinished,

   /// sent to screen when user aborted text editing
   gameEventTexteditAborted,

   /// last (dummy) event
   gameEventLast
};


/// game interface class
class IGame : public IBasicGame
{
public:
   /// returns game tickrate
   virtual double GetTickRate() const = 0;

   /// returns audio manager
   virtual Audio::AudioManager& GetAudioManager() = 0;

   /// returns renderer object
   virtual Renderer& GetRenderer() = 0;

   /// returns renderer viewport
   virtual RenderWindow& GetRenderWindow() = 0;

   /// returns renderer viewport
   virtual Viewport& GetViewport() = 0;

   /// replaces current screen with new one; saves current on a screen stack when selected
   virtual void ReplaceScreen(Screen* newScreen, bool saveCurrent) = 0;

   /// removes current screen at next event processing
   virtual void RemoveScreen() = 0;

   /// returns screen x resolution
   virtual unsigned int GetScreenXRes() = 0;
   /// returns screen y resolution
   virtual unsigned int GetScreenYRes() = 0;

protected:
   /// ctor
   IGame() {}
};
