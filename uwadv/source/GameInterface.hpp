//
// Underworld Adventures - an Ultima Underworld hacking project
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

namespace Base
{
   class Settings;
   class ResourceManager;
   class SavegamesManager;
}
namespace Audio
{
   class AudioManager;
}
namespace Underworld
{
   class Underworld;
   class GameLogic;
}
class ImageManager;
class Renderer;
class IScripting;
class DebugServer;
class Screen;
class GameStrings;
class IUserInterface;
class IPhysicsModelCallback;
class PhysicsModel;

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


/// basic game interface class, without ui stuff
class IBasicGame
{
public:
   /// returns game tickrate
   virtual double GetTickRate() = 0;

   /// pauses or unpauses game
   virtual bool PauseGame(bool pause) = 0;

   /// returns settings object
   virtual Base::Settings& GetSettings() = 0;

   /// returns settings object
   virtual Base::ResourceManager& GetResourceManager() = 0;

   /// returns savegames manager object
   virtual Base::SavegamesManager& GetSavegamesManager() = 0;

   /// returns scripting object
   virtual IScripting& GetScripting() = 0;

   /// returns debugger object
   virtual DebugServer& GetDebugger() = 0;

   /// returns game strings object
   virtual GameStrings& GetGameStrings() = 0;

   /// returns underworld object
   virtual Underworld::Underworld& GetUnderworld() = 0;

   virtual Underworld::GameLogic& GetGameLogic() = 0;

   /// returns user interface instance; may be null
   virtual IUserInterface* GetUserInterface() = 0;

   /// returns physics model callback
   virtual IPhysicsModelCallback& GetPhysicsModelCallback() = 0;

   /// initializes game; only called after all stuff is initialized and ready
   virtual void InitGame() = 0;

   /// cleans up game
   virtual void DoneGame() = 0;
};


/// game interface class
class IGame : public IBasicGame
{
public:
   /// returns audio manager
   virtual Audio::AudioManager& GetAudioManager() = 0;

   /// returns image manager object
   virtual ImageManager& GetImageManager() = 0;

   /// returns renderer object
   virtual Renderer& GetRenderer() = 0;

   /// returns physics model
   virtual PhysicsModel& GetPhysicsModel() = 0;

   /// replaces current screen with new one; saves current on a screen stack when selected
   virtual void ReplaceScreen(Screen* newScreen, bool saveCurrent) = 0;

   /// removes current screen at next event processing
   virtual void RemoveScreen() = 0;

   /// registers or unregisters user interface
   virtual void RegisterUserInterface(IUserInterface* userInterface) = 0;

   /// returns screen x resolution
   virtual unsigned int GetScreenXRes() = 0;
   /// returns screen y resolution
   virtual unsigned int GetScreenYRes() = 0;

protected:
   /// ctor
   IGame() {}
};
