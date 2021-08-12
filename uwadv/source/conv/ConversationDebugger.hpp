//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2005,2019,2021 Underworld Adventures Team
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
#include <optional>
#include <mutex>

namespace Conv
{
   class CodeVM;

   /// conversation code debugger
   class ConversationDebugger : public ICodeDebugger
   {
   public:
      /// ctor
      ConversationDebugger(Underworld::GameLogic& gameLogic, CodeVM& codeVM);

      /// initializes code debugger and notifies debug server
      void Init(IDebugServer& debugServer);

      /// cleans up debuggable conversation
      void Done();

      /// evaluate code VM for changed debugger state
      void EvaluateDebuggerState();

      /// returns if conversation should be continued by continuously calling Step()
      bool ContinueSteppingCode() const;

   private:
      /// checks if a breakpoint was reached
      void CheckBreakpoints();

      // virtual methods from ICodeDebugger
      virtual DebugServerCodeDebuggerType GetDebuggerType() const override;
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

   private:
      /// code VM to debug
      CodeVM& m_codeVM;

      /// access game logic
      Underworld::GameLogic& m_gameLogic;

      /// reference to debug server; only valid when Init() was called
      std::optional<std::reference_wrapper<IDebugServer>> m_debugServer;

      /// mutex to protect access to all following member variables
      mutable std::mutex m_debuggerMutex;

      /// current code debugger state
      DebugServerCodeDebuggerState m_state;

      /// current code debugger command
      DebugServerCodeDebuggerCommand m_command;

      /// current position sourcefile line
      size_t m_currentPositionSourcefileLine;

      /// function call depth when doing "step over"
      size_t m_stepOverFunctionCallDepth;

      /// filename of decompiled conversation; may be empty
      std::string m_decompileFilename;

      /// mapping from 1-based line numbers to code position
      std::map<unsigned int, Uint16> m_lineToCodeMapping;

      /// mapping from code position to 1-based line numbers
      std::map<Uint16, unsigned int> m_codeToLineMapping;

      /// list with all breakpoints
      std::vector<DebugCodeBreakpointInfo> m_breakpointsList;
   };

} // namespace Conv
