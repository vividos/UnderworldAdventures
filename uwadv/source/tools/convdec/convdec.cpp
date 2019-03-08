//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2014,2019 Underworld Adventures Team
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
/// \file convdec.cpp
/// \brief Underworld conversation code decompiler
//
#include "Conversation.hpp"
#include "convdec.hpp"
#include "CodeGraph.hpp"
#include "ConvLoader.hpp"
#include "GameStrings.hpp"
#include "GameStringsImporter.hpp"

using namespace Conv;

Decompiler::Decompiler(Uint16 conversationNumber, std::string basePath, GameStrings& strings)
:m_conversationNumber(conversationNumber),
m_strings(strings)
{
   Conv::CodeVM codeVM;

   Import::LoadConvCode(codeVM, (basePath + "data/cnv.ark").c_str(), conversationNumber);

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

void Decompiler::Write(FILE* fd)
{
   fputs("; conversation for ", fd);
   fputs(m_strings.GetString(7, m_conversationNumber + 16).c_str(), fd);
   fputs("\n\n", fd);
   fflush(fd);

   int indentLevel = 0;
   const Conv::CodeGraph::GraphList& g = m_codeGraph->GetGraph();
   Conv::CodeGraph::const_graph_iterator iter = g.begin(), stop = g.end();
   for (; iter != stop; ++iter)
   {
#if 0
      if (iter->m_isProcessed && iter->m_labelName.length() != 0 && iter->m_xrefCount > 0)
      {
         fputs(iter->m_labelName.c_str(), fd);
         fputs(":\n", fd);
      }
#else
      if (iter->m_isProcessed)
         continue;
#endif

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

      //      if (!iter->m_isProcessed && iter->m_labelName.length() != 0)
      if (iter->m_labelName.length() != 0)
      {
         fputs(iter->m_labelName.c_str(), fd);
         fputs(":\n", fd);
      }

      for (int i = 0; i<indentLevel; i++)
         fputs("   ", fd);

      std::string text = iter->Format();
      fputs(text.c_str(), fd);

#if 0
      if (iter->m_type == Conv::typeFuncStart)
      {
         std::string funcName = iter->m_labelName;

         const Conv::CodeGraph::T_mapFunctions& functionMap = graph.GetFunctionMap();

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
#endif

      fputs("\n", fd);
      fflush(fd);

      if (iter->m_type == Conv::typeStatement && iter->statement_data.indent_change_after != 0)
         indentLevel += iter->statement_data.indent_change_after;
      UaAssert(indentLevel >= 0);
   }
}


int main(int argc, char* argv[])
{
   printf("convdec - Ultima Underworld conversation code decompiler\n"
      "Copyright (c) 2003,2004,2019 Michael Fink\n\n");

   if (argc < 3)
   {
      printf("syntax: convdec <basepath> <slotnumber>\n"
         "   basepath is the path to the uw1 or uw2 folder.\n"
         "   slotnumber is the conversation slot number to decompile, either\n"
         "   as decimal or hex\n");
      printf("example: convdev . 0x0001\n"
         "         convdev \"c:\\uw1\\\" 42\n\n");
      return 1;
   }

   std::string basePath = argv[1];
   basePath += "/";

   std::string slotText = argv[2];

   bool isHex = slotText.find("0x") == 0;
   if (isHex)
      slotText.erase(0, 2);

   Uint16 slotNumber = (Uint16)strtol(slotText.c_str(), NULL, isHex ? 16 : 10);

   GameStrings gameStrings;
   Import::GameStringsImporter importer(gameStrings);
   importer.LoadStringsPakFile((basePath + "data/strings.pak").c_str());

   Conv::Decompiler cnvdec(slotNumber, basePath, gameStrings);
   cnvdec.Write(stdout);

   return 0;
}
