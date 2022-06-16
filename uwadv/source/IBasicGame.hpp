//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2019,2022 Underworld Adventures Team
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
/// \file IBasicGame.hpp
/// \brief basic game interface
//
#pragma once

namespace Base
{
   class Settings;
   class ResourceManager;
   class SavegamesManager;
}
namespace Physics
{
   class PhysicsModel;
}
namespace Underworld
{
   class Underworld;
   class GameLogic;
}
class ImageManager;
class IScripting;
class IDebugServer;
class GameStrings;
class IUserInterface;

/// basic game interface class, without user interface stuff
class IBasicGame
{
public:
   /// returns game tickrate
   virtual double GetTickRate() const = 0;

   /// pauses or unpauses game
   virtual bool PauseGame(bool pause) = 0;

   /// returns settings object
   virtual Base::Settings& GetSettings() = 0;

   /// returns settings object
   virtual Base::ResourceManager& GetResourceManager() = 0;

   /// returns savegames manager object
   virtual Base::SavegamesManager& GetSavegamesManager() = 0;

   /// returns image manager object
   virtual ImageManager& GetImageManager() = 0;

   /// returns scripting object
   virtual IScripting& GetScripting() = 0;

   /// returns debugger object
   virtual IDebugServer& GetDebugger() = 0;

   /// returns game strings object
   virtual GameStrings& GetGameStrings() = 0;

   /// returns underworld object
   virtual Underworld::Underworld& GetUnderworld() = 0;

   /// returns game logic object
   virtual Underworld::GameLogic& GetGameLogic() = 0;

   /// returns physics model
   virtual Physics::PhysicsModel& GetPhysicsModel() = 0;

   /// returns user interface instance; may be null
   virtual IUserInterface* GetUserInterface() = 0;

   /// initializes game; only called after all stuff is initialized and ready
   virtual void InitGame() = 0;

   /// cleans up game
   virtual void DoneGame() = 0;
};
