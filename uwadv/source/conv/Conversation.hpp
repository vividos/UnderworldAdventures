//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2005,2019 Underworld Adventures Team
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
/// \file Conversation.hpp
/// \brief conversation
//
#pragma once

#include "CodeVM.hpp"
#include "IDebugServer.hpp"

namespace Underworld
{
   class GameLogic;
}

namespace Conv
{
   class ICodeCallback;

   /// \brief conversation
   /// \details implements conversation using the Underworld object
   /// to implement imported functions and globals
   class Conversation : public CodeVM
   {
   public:
      /// ctor
      Conversation(Underworld::GameLogic& gameLogic)
         :m_gameLogic(gameLogic),
         m_conversationLevel(0),
         m_conversationObjectPos(0)
      {
      }

      /// inits basic conversation
      virtual void Init(unsigned int conversationLevel,
         Uint16 conversationObjectPos,
         ICodeCallback* codeCallback,
         std::vector<std::string>& localStrings);

      /// cleans up basic conversation
      virtual void Done();

   protected:
      // virtual methods from CodeVM
      virtual void ImportedFunc(const char* funcname);
      virtual Uint16 GetGlobal(const char* globname);
      virtual void SetGlobal(const char* globname, Uint16 val);

   protected:
      /// game logic object
      Underworld::GameLogic& m_gameLogic;

      /// underworld level of conversation partner
      unsigned int m_conversationLevel;

      /// object position of conversation partner
      Uint16 m_conversationObjectPos;
   };

#if 0
   /// \todo move to own file
   class ConversationDebugger : public Conversation, public ICodeDebugger
   {
   public:
      ConversationDebugger();

      /// inits debuggable conversation
      virtual void Init(unsigned int conv_level, Uint16 conv_objpos,
         IBasicGame& game, ICodeCallback* codeCallback,
         std::vector<std::string>& localStrings);

      /// cleans up debuggable conversation
      virtual void Done(IBasicGame& game);

   protected:
      // virtual methods from ICodeDebugger
      virtual DebugServerCodeDebuggerType GetDebuggerType();
      virtual void PrepareDebugInfo();
      virtual DebugServerCodeDebuggerState GetDebuggerState() const;
      virtual void SetDebuggerState(DebugServerCodeDebuggerState state);
      virtual DebugServerCodeDebuggerCommand GetDebuggerCommand() const;
      virtual void SetDebuggerCommand(DebugServerCodeDebuggerCommand command);
      virtual void GetCurrentPos(unsigned int& sourcefileIndex, unsigned int& sourcefileLine,
         unsigned int& codePosition, bool& isSourcefileValid);
      virtual unsigned int GetNumSourcefiles() const;
      virtual unsigned int GetSourcefileName(unsigned int index, char* buffer, unsigned int len);
      virtual unsigned int GetNumBreakpoints();
      virtual void GetBreakpointInfo(unsigned int breakpointIndex,
         unsigned int& sourcefileIndex, unsigned int& sourcefileLine,
         unsigned int& codePosition, bool& visible);

   protected:
      std::string temp_decompile;

      DebugServerCodeDebuggerState state;
      DebugServerCodeDebuggerCommand command;
   };
#endif

} // namespace Conv
