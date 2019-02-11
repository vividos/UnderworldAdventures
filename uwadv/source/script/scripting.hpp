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
/// \file scripting.hpp
/// \brief scripting support for Underworld Adventures
//
#pragma once

#include "gamelogic.hpp"

class IBasicGame;

/// scripting language
enum EScriptingLanguage
{
   sriptingLanguageLua = 0,
};

/// item combining result status
/// \todo move to game.hpp
enum EItemCombineStatus
{
   ua_item_combine_failed = 0, ///< failed to combine the items
   ua_item_combine_destroy_first, ///< succeeded; destroyed first item
   ua_item_combine_destroy_second, ///< succeeded; destroyed second item
   ua_item_combine_destroy_both, ///< succeeded; destroyed both items
};

/// scripting interface class
class IScripting
{
public:
   /// ctor
   IScripting() {}
   virtual ~IScripting() {}

   /// inits scripting
   virtual void init(IBasicGame* game) = 0;

   /// loads a script into scripting engine
   virtual bool load_script(const char* basename) = 0;

   /// cleans up scripting
   virtual void done() = 0;

   /// called to init new game
   virtual void init_new_game() = 0;

   /// evaluates a critter in master object list
   virtual void eval_critter(Uint16 pos) = 0;

   /// executes a trigger action
   virtual void trigger_set_off(Uint16 pos) = 0;

   /// user performed an action
   virtual void UserAction(EUserInterfaceUserAction action,
      unsigned int param) = 0;

   /// notifies script that the current level has changed
   virtual void on_changing_level() = 0;

   /// \brief Combines two items
   /// Tries to combine two objects; returns resulting object id or
   /// Underworld::c_itemIDNone, when items couldn't be combined.
   ///
   /// \param item_id1 item id of object to combine
   /// \param item_id2 item id of object to combine with
   /// \param result_id item id of result object
   /// \return status of item combining
   ///
   virtual EItemCombineStatus item_combine(Uint16 item_id1, Uint16 item_id2,
      Uint16& result_id) = 0;

   /// creates new scripting object
   static IScripting* create_scripting(EScriptingLanguage lang);
};
