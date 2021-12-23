//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2019 Underworld Adventures Team
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
/// \file cnvdbg.hpp
/// gdb-like conversation debugger
//
#pragma once

#include <vector>
#include "CodeVM.hpp"
#include "Settings.hpp"
#include "ResourceManager.hpp"
#include "GameStrings.hpp"

/// conversation debugger class
class ConversationDebugger :
   public Conv::CodeVM,
   public Conv::ICodeCallback
{
public:
   /// ctor
   ConversationDebugger()
   :m_resourceManager(m_settings)
   {
   }

   /// initializes debugger
   void Init(const char* underworldFolder, const char* conversationGlobalsName);

   /// starts the debugger
   void Start();

private:
   /// lists code at position; returns number of bytes opcode occupies
   int ListCode(Uint16 at);

   // virtual functions from Conv::CodeVM

   virtual void ImportedFunc(const char* functionName) override;

   virtual Uint16 GetGlobal(const char* globalName) override;

   virtual void SetGlobal(const char* globalName, Uint16 value) override;

   // virtual functions from Conv::ICodeCallback

   virtual void Say(Uint16 index) override;

   virtual Uint16 BablMenu(const std::vector<Uint16>& answerStringIds) override;

   virtual Uint16 ExternalFunc(const char* funcname, Conv::ConvStack& stack) override;

   //void sto_priv(Uint16 at, Uint16 val);
   //void fetchm_priv(Uint16 at);

private:
   /// settings
   Base::Settings m_settings;

   /// resource manager
   Base::ResourceManager m_resourceManager;

   /// conversations file name
   const char* m_conversationName;

   /// all game strings
   GameStrings m_gameStrings;

   /// we have the globals 'on board'
   Underworld::ConvGlobals m_convGlobals;

   /// list of all breakpoints
   std::vector<Uint16> m_allBreakpoints;

   /// indicates if conversation is loaded
   bool m_isLoaded;

   /// indicates if verbose mode is on
   bool m_verbose;
};
