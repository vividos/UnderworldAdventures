//
// Underworld Adventures - an Ultima Underworld hacking project
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

class IBasicGame;

/// scripting language
enum ScriptingLanguage
{
   scriptingLanguageLua = 0,
};

/// item combining result status
/// \todo move to game.hpp
enum ItemCombineStatus
{
   itemCombineFailed = 0, ///< failed to combine the items
   itemCombineDestroyFirst, ///< succeeded; destroyed first item
   itemCombineDestroySecond, ///< succeeded; destroyed second item
   itemCombineDestroyBoth, ///< succeeded; destroyed both items
};

/// scripting interface class
class IScripting
{
public:
   /// ctor
   IScripting() {}
   virtual ~IScripting() {}

   /// inits scripting
   virtual void Init(IBasicGame* game) = 0;

   /// loads a script into scripting engine
   virtual bool LoadScript(const char* basename) = 0;

   /// cleans up scripting
   virtual void Done() = 0;

   /// called to init new game
   virtual void InitNewGame() = 0;

   /// evaluates a critter in master object list
   virtual void EvalCritter(Uint16 pos) = 0;

   /// executes a trigger action
   virtual void TriggerSetOff(Uint16 pos) = 0;

   /// user performed an action
   virtual void UserAction(UserInterfaceUserAction action,
      unsigned int param) = 0;

   /// notifies script that the current level has changed
   virtual void OnChangingLevel() = 0;

   /// \brief Combines two items
   /// Tries to combine two objects; returns resulting object id or
   /// Underworld::c_itemIDNone, when items couldn't be combined.
   ///
   /// \param itemId1 item id of object to combine
   /// \param itemId2 item id of object to combine with
   /// \param resultId item id of result object
   /// \return status of item combining
   ///
   virtual ItemCombineStatus ItemCombine(Uint16 itemId1, Uint16 itemId2,
      Uint16& resultId) = 0;

   /// creates new scripting object
   static IScripting* CreateScripting(ScriptingLanguage lang);
};
