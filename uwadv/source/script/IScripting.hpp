//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
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
/// \file IScripting.hpp
/// \brief scripting support for Underworld Adventures
//
#pragma once

#include "GameLogic.hpp"

class IGameInstance;

/// scripting language
enum ScriptingLanguage
{
   scriptingLanguageLua = 0,
};

/// scripting interface class
class IScripting
{
public:
   /// ctor
   IScripting() {}
   virtual ~IScripting() {}

   /// inits scripting
   virtual void Init(IGameInstance* game) = 0;

   /// loads a script into scripting engine
   virtual bool LoadScript(const char* basename) = 0;

   /// cleans up scripting
   virtual void Done() = 0;

   /// called to init new game
   virtual void InitNewGame() = 0;

   /// evaluates a critter in object list
   virtual void EvalCritter(Uint16 pos) = 0;

   /// executes a trigger action
   virtual void TriggerSetOff(Uint16 pos) = 0;

   /// user performed an action
   virtual void UserAction(UserInterfaceUserAction action,
      unsigned int param) = 0;

   /// notifies script that the current level has changed
   virtual void OnChangingLevel() = 0;

   /// creates new scripting object
   static IScripting* CreateScripting(ScriptingLanguage lang);
};
