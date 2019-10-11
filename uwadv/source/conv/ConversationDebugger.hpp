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
/// \file ConversationDebugger.hpp
/// \brief conversation debugger
//
#pragma once

#include "Conversation.hpp"
#include "IDebugServer.hpp"

namespace Conv
{
   /// conversation code debugger
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
      virtual DebugServerCodeDebuggerType GetDebuggerType() override;
      virtual void PrepareDebugInfo() override;
      virtual DebugServerCodeDebuggerState GetDebuggerState() const override;
      virtual void SetDebuggerState(DebugServerCodeDebuggerState state) override;
      virtual DebugServerCodeDebuggerCommand GetDebuggerCommand() const override;
      virtual void SetDebuggerCommand(DebugServerCodeDebuggerCommand command) override;
      virtual void GetCurrentPos(size_t& sourcefileIndex, size_t& sourcefileLine,
         size_t& codePosition, bool& isSourcefileValid) override;
      virtual size_t GetNumSourcefiles() const override;
      virtual size_t GetSourcefileName(size_t index, char* buffer, size_t length) override;
      virtual size_t GetNumBreakpoints() const override;
      virtual void GetBreakpointInfo(size_t breakpointIndex,
         size_t& sourcefileIndex, size_t& sourcefileLine,
         size_t& codePosition, bool& visible) const override;

   protected:
      std::string temp_decompile;

      DebugServerCodeDebuggerState state;
      DebugServerCodeDebuggerCommand command;
   };

} // namespace Conv
