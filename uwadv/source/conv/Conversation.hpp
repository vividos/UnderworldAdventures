//
// Underworld Adventures - an Ultima Underworld remake project
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
      virtual void ImportedFunc(const char* funcname) override;
      virtual Uint16 GetGlobal(const char* globname) override;
      virtual void SetGlobal(const char* globname, Uint16 val) override;

   protected:
      /// game logic object
      Underworld::GameLogic& m_gameLogic;

      /// underworld level of conversation partner
      unsigned int m_conversationLevel;

      /// object position of conversation partner
      Uint16 m_conversationObjectPos;
   };

} // namespace Conv
