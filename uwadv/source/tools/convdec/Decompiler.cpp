//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2014,2019,2021 Underworld Adventures Team
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
/// \file Decompiler.cpp
/// \brief Underworld conversation code decompiler
//
#include "Decompiler.hpp"
#include "Conversation.hpp"
#include "CodeGraph.hpp"
#include "ConvLoader.hpp"
#include "GameStrings.hpp"
#include "ResourceManager.hpp"
#include "FileSystem.hpp"

using namespace Conv;

Decompiler::Decompiler(Uint16 conversationNumber, std::string basePath, GameStrings& strings)
   :m_conversationNumber(conversationNumber),
   m_strings(strings)
{
   Conv::CodeVM codeVM;

   Base::Settings settings;
   settings.SetValue(Base::settingUnderworldPath, basePath);
   Base::ResourceManager resourceManager{ settings };

   if (Base::FileSystem::FileExists(basePath + "uw2.exe"))
      settings.SetGameType(Base::gameUw2);

   if (!Import::LoadConvCode(codeVM, settings, resourceManager, "data/cnv.ark", conversationNumber))
      return;

   std::vector<std::string> stringBlock = strings.GetStringBlock(0x0e00 + conversationNumber);

   m_codeGraph.reset(new Conv::CodeGraph(codeVM.GetCodeSegment(),
      0,
      static_cast<Uint16>(codeVM.GetCodeSegment().size()),
      stringBlock,
      codeVM.GetImportedFunctions(),
      codeVM.GetImportedGlobals(),
      codeVM.GetReservedGlobals()));

   m_codeGraph->Decompile();
}

std::string Decompiler::GetName() const
{
   return m_strings.GetString(7, m_conversationNumber + 16);
}

void Decompiler::Write(FILE* fd, bool showDisassembly)
{
   fputs("; conversation for ", fd);
   fputs(m_strings.GetString(7, m_conversationNumber + 16).c_str(), fd);
   fprintf(fd, " (%u)", m_conversationNumber);
   fputs("\n\n", fd);
   fflush(fd);

   if (m_codeGraph == NULL ||
      m_codeGraph->GetGraph().empty())
   {
      fputs("; conversation contained no code\n", fd);
      fflush(fd);
      return;
   }

   int indentLevel = 0;
   const Conv::CodeGraph::GraphList& g = m_codeGraph->GetGraph();
   Conv::CodeGraph::const_graph_iterator iter = g.begin(), stop = g.end();
   for (; iter != stop; ++iter)
   {
      if (showDisassembly)
      {
         // display label names that have xrefs
         if (iter->m_isProcessed && iter->m_labelName.length() != 0 && iter->m_xrefCount > 0)
         {
            fputs(iter->m_labelName.c_str(), fd);
            fputs(":\n", fd);
         }

         if (iter->m_type != Conv::typeOpcode &&
            iter->m_type != Conv::typeStatement)
            continue;
      }
      else
      {
         if (iter->m_isProcessed)
            continue;
      }

      if (iter->m_type == Conv::typeStatement && iter->statement_data.indent_change_before != 0)
         indentLevel += iter->statement_data.indent_change_before;
      UaAssert(indentLevel >= 0);

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

      // show any unprocessed labels
      if (!iter->m_isProcessed &&
         iter->m_labelName.length() != 0)
      {
         fputs(iter->m_labelName.c_str(), fd);
         fputs(":\n", fd);
      }

      for (int i = 0; i < indentLevel; i++)
         fputs("   ", fd);

      std::string text = iter->Format();
      fputs(text.c_str(), fd);

      // show cross-references in disassembly mode
      if (showDisassembly && iter->m_type == Conv::typeFuncStart)
      {
         std::string funcName = iter->m_labelName;

         const Conv::CodeGraph::FunctionMap& functionMap = m_codeGraph->GetFunctionMap();

         UaAssert(functionMap.find(funcName) != functionMap.end());

         const Conv::FuncInfo& funcInfo = functionMap.find(funcName)->second;

         if (!funcInfo.caller.empty())
         {
            fputs(", caller xrefs: ", fd);
            std::for_each(funcInfo.caller.begin(), funcInfo.caller.end(), [fd](const std::string& caller)
            {
               fputs(caller.c_str(), fd);
               fputs(", ", fd);
            });
         }
      }

      fputs("\n", fd);
      fflush(fd);

      if (iter->m_type == Conv::typeStatement && iter->statement_data.indent_change_after != 0)
         indentLevel += iter->statement_data.indent_change_after;
      UaAssert(indentLevel >= 0);
   }
}
