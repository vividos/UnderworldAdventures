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
/// \file ConversationDebugger.cpp
/// \brief conversation debugger
//
#include "pch.hpp"
#include "ConversationDebugger.hpp"
#include "IDebugServer.hpp"
#include "GameLogic.hpp"
#include "GameStrings.hpp"
#include "CodeGraph.hpp"
#include <filesystem>

using Conv::ConversationDebugger;

ConversationDebugger::ConversationDebugger(Underworld::GameLogic& gameLogic, CodeVM& codeVM)
   :m_gameLogic(gameLogic),
   m_codeVM(codeVM),
   m_state(codeDebuggerStateInactive),
   m_command(codeDebuggerCommandRun),
   m_stepOverFunctionCallDepth(0),
   m_currentPositionSourcefileLine(unsigned(-1))
{
}

void ConversationDebugger::Init(IDebugServer& debugServer)
{
   m_debugServer = std::ref(debugServer);

   m_state = codeDebuggerStateRunning;
   m_command = codeDebuggerCommandRun;

   // notify debugger of start of code debugger
   debugServer.StartCodeDebugger(this);
}

void ConversationDebugger::Done()
{
   m_state = codeDebuggerStateInactive;

   // notify debugger of end of code debugger
   m_debugServer.value().get().EndCodeDebugger(this);
}

void ConversationDebugger::EvaluateDebuggerState()
{
   // find current line, based on instruction pointer
   bool lineHasChanged = false;

   Uint16 codePos = m_codeVM.GetInstructionPointer();

   auto lineIter = m_codeToLineMapping.find(codePos);
   if (lineIter != m_codeToLineMapping.end())
   {
      lineHasChanged = m_currentPositionSourcefileLine != lineIter->second;
      m_currentPositionSourcefileLine = lineIter->second;
   }

   DebugServerCodeDebuggerState oldDebuggerState = GetDebuggerState();

   // check commands
   Uint16 currentOpcode = m_codeVM.GetCodeSegment()[codePos];

   if (GetDebuggerState() == codeDebuggerStateRunning)
   {
      switch (GetDebuggerCommand())
      {
      case codeDebuggerCommandRun:
         SetDebuggerState(codeDebuggerStateRunning);
         break;

      case codeDebuggerCommandStepOver:
         if (lineHasChanged)
         {
            if (m_stepOverFunctionCallDepth == 0)
               SetDebuggerState(codeDebuggerStateBreak);
         }

         if (currentOpcode == op_CALL)
         {
            if (GetDebuggerState() == codeDebuggerStateRunning)
               m_stepOverFunctionCallDepth++; //
            else
               m_stepOverFunctionCallDepth = 0; // start of stepping over a function
         }

         if (currentOpcode == op_RET)
         {
            --m_stepOverFunctionCallDepth; // returning from function
            if (m_stepOverFunctionCallDepth == 0)
            {
               // returned from stepped over function
               SetDebuggerState(codeDebuggerStateRunning);
               SetDebuggerCommand(codeDebuggerCommandStepInto);
            }
         }
         break;

      case codeDebuggerCommandStepInto:
         if (lineHasChanged ||
            currentOpcode == op_CALL)
         {
            SetDebuggerState(codeDebuggerStateBreak);
         }
         break;

      case codeDebuggerCommandStepOut:
         if (currentOpcode == op_RET)
         {
            // when returning set "step over" command to stop at the next line
            SetDebuggerState(codeDebuggerStateRunning);
            SetDebuggerCommand(codeDebuggerCommandStepOver);
         }
         break;
      }
   }

   CheckBreakpoints();

   // notify server when state changed
   if (oldDebuggerState != GetDebuggerState())
   {
      UaAssert(m_debugServer.has_value());
      m_debugServer.value().get().SendCodeDebuggerStatusUpdate(m_debuggerId);
   }
}

void ConversationDebugger::CheckBreakpoints()
{
   // TODO implement
}

bool ConversationDebugger::ContinueSteppingCode() const
{
   return GetDebuggerState() != codeDebuggerStateBreak;
}

DebugServerCodeDebuggerType ConversationDebugger::GetDebuggerType() const
{
   return codeDebuggerTypeUnderworldConversation;
}

void ConversationDebugger::PrepareDebugInfo()
{
   std::vector<std::string> stringBlock =
      m_gameLogic.GetGameStrings().GetStringBlock(
         m_codeVM.GetStringBlock());

   Conv::CodeGraph codeGraph(
      m_codeVM.GetCodeSegment(),
      0,
      static_cast<Uint16>(m_codeVM.GetCodeSegment().size()),
      stringBlock,
      m_codeVM.GetImportedFunctions(),
      m_codeVM.GetImportedGlobals(),
      m_codeVM.GetReservedGlobals());

   codeGraph.Decompile();

   Uint16 conversationNumber = m_codeVM.GetStringBlock() - 0x0e00;

   std::filesystem::path filename = std::filesystem::temp_directory_path();
   filename.append(std::string{ "uw-conv-" } + std::to_string(conversationNumber) + ".uwc");

   m_decompileFilename = filename.string();

   FILE* fd = fopen(m_decompileFilename.c_str(), "wt");

   int lineNumber = 1;

   fputs("; conversation for ", fd);
   fputs(m_gameLogic.GetGameStrings().GetString(7, conversationNumber + 16).c_str(), fd);
   fputs("\n\n", fd);
   fflush(fd);
   lineNumber += 2;

   if (codeGraph.GetGraph().empty())
   {
      fputs("; conversation contained no code\n", fd);
      fclose(fd);
      return;
   }

   bool displayDisassembly = false;

   int indentLevel = 0;
   const Conv::CodeGraph::GraphList& g = codeGraph.GetGraph();
   Conv::CodeGraph::const_graph_iterator iter = g.begin(), stop = g.end();
   for (; iter != stop; ++iter)
   {
      if (!displayDisassembly)
      {
         if (iter->m_type != Conv::typeOpcode &&
            iter->m_type != Conv::typeStatement)
            continue;
      }
      else
      {
         if (iter->m_type != Conv::typeStatement)
            continue;
      }

      m_lineToCodeMapping.insert(std::make_pair(lineNumber, iter->m_pos));

      if (m_codeToLineMapping.find(iter->m_pos) == m_codeToLineMapping.end())
         m_codeToLineMapping.insert(std::make_pair(iter->m_pos, lineNumber));

      if (iter->m_type == Conv::typeStatement && iter->statement_data.indent_change_before != 0)
         indentLevel += iter->statement_data.indent_change_before;
      UaAssert(indentLevel >= 0);

      // skip unused functions
      if (iter->m_type == Conv::typeFuncStart)
      {
         std::string funcName = iter->m_labelName;

         if (funcName.find("unused_") == 0 || iter->m_xrefCount == 0)
         {
            while (iter != stop && iter->m_type != Conv::typeFuncEnd)
               ++iter;

            // jump over opcodes from function end
            std::advance(iter, 3);

            continue;
         }
      }

      for (int i = 0; i < indentLevel; i++)
         fputs("   ", fd);

      std::string text = iter->Format();
      fputs(text.c_str(), fd);
      fputs("\n", fd);
      fflush(fd);
      lineNumber ++;

      if (iter->m_type == Conv::typeStatement && iter->statement_data.indent_change_after != 0)
         indentLevel += iter->statement_data.indent_change_after;

      UaAssert(indentLevel >= 0);
   }

   fclose(fd);
}

DebugServerCodeDebuggerState ConversationDebugger::GetDebuggerState() const
{
   std::unique_lock lock{ m_debuggerMutex };
   return m_state;
}

void ConversationDebugger::SetDebuggerState(DebugServerCodeDebuggerState state)
{
   std::unique_lock lock{ m_debuggerMutex };
   m_state = state;
}

DebugServerCodeDebuggerCommand ConversationDebugger::GetDebuggerCommand() const
{
   std::unique_lock lock{ m_debuggerMutex };
   return m_command;
}

void ConversationDebugger::SetDebuggerCommand(DebugServerCodeDebuggerCommand command)
{
   std::unique_lock lock{ m_debuggerMutex };
   m_command = command;
}

void ConversationDebugger::GetCurrentPos(size_t& sourcefileIndex, size_t& sourcefileLine,
   size_t& codePosition, bool& isSourcefileValid)
{
   std::unique_lock lock{ m_debuggerMutex };

   codePosition = m_codeVM.GetInstructionPointer();
   isSourcefileValid = !m_decompileFilename.empty();
   sourcefileIndex = isSourcefileValid ? 0 : unsigned(-1);
   sourcefileLine = m_currentPositionSourcefileLine;
}

size_t ConversationDebugger::GetNumSourcefiles() const
{
   return m_decompileFilename.empty() ? 0 : 1;
}

size_t ConversationDebugger::GetSourcefileName(size_t index, char* buffer, size_t length)
{
   if (m_decompileFilename.empty())
      return 0;

   size_t size = m_decompileFilename.size();

   if (buffer == nullptr || length == 0 || length < size + 1)
      return size + 1;

   std::copy_n(
      m_decompileFilename.begin(),
      size,
      buffer);

   buffer[size] = 0;

   return size + 1;
}

size_t ConversationDebugger::GetNumBreakpoints() const
{
   std::unique_lock lock{ m_debuggerMutex };
   return m_breakpointsList.size();
}

void ConversationDebugger::GetBreakpointInfo(
   size_t breakpointIndex,
   size_t& sourcefileIndex, size_t& sourcefileLine,
   size_t& codePosition, bool& visible) const
{
   UaAssert(breakpointIndex < GetNumBreakpoints());
   if (breakpointIndex >= GetNumBreakpoints())
      return;

   std::unique_lock lock{ m_debuggerMutex };

   const DebugCodeBreakpointInfo& info = m_breakpointsList[breakpointIndex];

   visible = info.visible;
   sourcefileIndex = info.pos.sourcefileIndex;
   sourcefileLine = info.pos.sourcefileLine;
   codePosition = info.pos.codePosition;
}
