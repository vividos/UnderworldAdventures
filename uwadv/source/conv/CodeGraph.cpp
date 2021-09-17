//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2021 Underworld Adventures Team
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
/// \file CodeGraph.cpp
/// \brief conversation graph
//
#include "pch.hpp"
#include "CodeGraph.hpp"
#include "Opcodes.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace Conv;

/// mapping with all external functions and their parameter infos
std::map<std::string, ParameterInfo> g_externalFuncParameterInfos =
{
   { "babl_menu", ParameterInfo({ dataTypeString }, { true }) },
   { "babl_fmenu", ParameterInfo({ dataTypeString, dataTypeInt }, { true, true }) },
   { "length", ParameterInfo({ dataTypeString }) },
   { "compare", ParameterInfo({ dataTypeString, dataTypeString }) },
   { "contains", ParameterInfo({ dataTypeString, dataTypeString }) },
   { "print", ParameterInfo({ dataTypeString }) },
   { "sex", ParameterInfo({ dataTypeString, dataTypeString }) },
};

std::string CodeGraphItem::Format() const
{
   std::string item_text;

   switch (m_type)
   {
   case typeFuncStart:
      item_text = "function start";
      break;

   case typeFuncEnd:
      item_text = "function end";
      break;

   case typeOpcode:
      item_text = FormatOpcode();
      break;

   case typeStatement:
      item_text = statement_data.statement;
      break;

   case typeOperator:
   {
      std::ostringstream buffer;
      buffer <<
         (m_isProcessed ? "   // " : "") <<
         "operator " <<
         g_convInstructions[operator_data.op_opcode].mnemonic <<
         ", needs " << operator_data.numNeededExpressions << " expressions, yields " <<
         (operator_data.returns_expr ? "an" : "no") << " expression;" <<
         (operator_data.sto_swap_args ? " swaps args;" : "") <<
         " level=" << operator_data.prec_level;

      item_text = buffer.str();
   }
   break;

   case typeExpression:
   {
      std::ostringstream buffer;
      buffer <<
         (m_isProcessed ? "   // " : "") <<
         "expression: " <<
         expression_data.expression <<
         (expression_data.isAddress ? " (address-of)" : " (value-of)") <<
         "; level=" << expression_data.prec_level;
      item_text = buffer.str();
   }
   break;

   default:
      UaAssert(false);
      item_text = "// unknown graph item type!";
      break;
   }

   return item_text;
}

std::string CodeGraphItem::FormatOpcode() const
{
   UaAssert(m_type == typeOpcode);

   std::ostringstream buffer;
   buffer << "   ";

   if (m_isProcessed)
      buffer << "// ";

   // code segment address
   buffer << std::setfill('0') << std::setw(4) <<
      std::setbase(16) << m_pos << " ";

   if (opcode_data.opcode > op_last)
   {
      // unknown opcode
      buffer << "??? (0x" << std::setfill('0') <<
         std::setw(4) << std::setbase(16) << opcode_data.opcode << ")";
   }
   else
   {
      Uint16 opcode = opcode_data.opcode;

      buffer << g_convInstructions[opcode].mnemonic;

      if (g_convInstructions[opcode_data.opcode].args > 0 &&
         !opcode_data.jump_target.empty() &&
         (opcode == op_JMP || opcode == op_BEQ || opcode == op_BNE ||
            opcode == op_BRA || opcode == op_CALL))
      {
         // label available
         buffer << " " << opcode_data.jump_target;
      }
      else if (opcode == op_CALLI)
      {
         // intrinsic function name
         buffer << " " << opcode_data.jump_target;
      }
      else if (g_convInstructions[opcode_data.opcode].args > 0)
      {
         // unknown, not resolved by collect_xrefs, or PUSH, PUSHI_EFF
         // just format the string
         buffer << " 0x" << std::setfill('0') <<
            std::setw(4) << std::setbase(16) << opcode_data.arg;
      }
   }

   return buffer.str();
}

CodeGraph::CodeGraph(std::vector<Uint16> code,
   Uint16 codeStart, Uint16 codeEnd,
   std::vector<std::string>& strings,
   std::map<Uint16, Conv::ImportedItem>& mapImportedFunctions,
   std::map<Uint16, Conv::ImportedItem>& mapImportedVariables,
   Uint16 reservedGlobals)
   :m_strings(strings),
   m_mapImportedFunctions(mapImportedFunctions),
   m_mapImportedVariables(mapImportedVariables),
   m_numGlobals(reservedGlobals)
{
   InitGraph(code, codeStart, codeEnd);
}

void CodeGraph::Disassemble()
{
   CollectXrefs();
}

void CodeGraph::Decompile()
{
   CollectXrefs();
   FindFunctions();
   UpdateCallJumpTargets();

   // initially add main() to analysis queue
   m_analysisQueue.push_back("main");

   ProcessFunctionQueue();

   MarkFunctionsUnused();
}

void CodeGraph::InitGraph(const std::vector<Uint16>& code,
   Uint16 codeStart, Uint16 codeEnd)
{
   // convert code segment to graph
   for (Uint16 i = codeStart; i < codeEnd; i++)
   {
      // fill item struct
      CodeGraphItem item;
      item.m_pos = i;
      item.opcode_data.opcode = code[i];

      if (item.opcode_data.opcode <= op_last &&
         g_convInstructions[item.opcode_data.opcode].args != 0)
         item.opcode_data.arg = code[++i];
      else
         item.opcode_data.arg = 0;

      // add it to graph
      m_graph.push_back(item);
   }
}

void CodeGraph::CollectXrefs()
{
   UaAssert(!m_graph.empty());

   // start function
   CodeGraphItem& startItem = m_graph.front();
   startItem.m_labelName = "main";
   startItem.m_xrefCount = 1;

   graph_iterator iter = m_graph.begin(), stop = m_graph.end();

   for (; iter != stop; ++iter)
   {
      CodeGraphItem& item = *iter;
      if (item.m_type != typeOpcode)
         continue;

      Uint16 opcode = item.opcode_data.opcode;

      if (opcode == op_JMP ||
         opcode == op_BEQ ||
         opcode == op_BNE ||
         opcode == op_BRA ||
         opcode == op_CALL)
      {
         // calculate jump target
         Uint16 target = item.opcode_data.arg;

         // adjust for relative jumps
         if (opcode == op_BEQ ||
            opcode == op_BNE ||
            opcode == op_BRA)
            target = static_cast<Uint16>((static_cast<Uint32>(target) + static_cast<Uint32>(item.m_pos + 1)) & 0xFFFF);

         if (opcode == op_CALL &&
            target == 0xFFFF)
            continue; // happens in uw2 code

         // find target position
         graph_iterator target_pos = FindPos(target);
         UaAssert(target_pos != m_graph.end());

         CodeGraphItem& target_item = *target_pos;

         if (target_item.m_xrefCount == 0)
         {
            // not a target yet, generate label name from pos
            std::ostringstream buffer;
            buffer << "label_" << std::setfill('0') <<
               std::setw(4) << std::setbase(16) << target_item.m_pos;
            target_item.m_labelName = buffer.str();
         }

         // copy over label name and pos
         item.opcode_data.jump_target = target_item.m_labelName;
         item.opcode_data.jump_target_pos = target_item.m_pos;

         // increase xref count
         target_item.m_xrefCount++;
      }

      if (opcode == op_CALLI)
      {
         // for intrinsic calls, set target name
         Uint16 ifunc = item.opcode_data.arg;
         UaAssert(m_mapImportedFunctions.find(ifunc) != m_mapImportedFunctions.end());

         item.opcode_data.jump_target = m_mapImportedFunctions.find(ifunc)->second.name;
      }
   }
}

/// Searches for function entry and exit code and inserts additional graph items.
///
/// function entry code:
/// PUSHBP
/// SPTOBP
/// PUSHI #n   // n: locals size
/// ADDSP
///
/// and function exit code:
/// BPTOSP
/// POPBP
/// RET
void CodeGraph::FindFunctions()
{
   graph_iterator iter = m_graph.begin(), stop = m_graph.end();

   while (iter != stop)
   {
      FuncInfo funcInfo;

      if (!FindFunctionEntryPoint(iter, stop, funcInfo))
         break;

      if (!FindFunctionExitPoint(iter, stop, funcInfo))
         break;

      m_functionMap.insert(std::make_pair(funcInfo.name, funcInfo));
   }
}

bool CodeGraph::FindFunctionEntryPoint(graph_iterator& iter, graph_iterator stop, FuncInfo& funcInfo)
{
   // find next function entry code
   const Uint16 pattern_func_start[4] =
   { op_PUSHBP, op_SPTOBP, op_PUSHI, op_ADDSP };

   graph_iterator found = FindOpcodePattern(iter, stop, pattern_func_start, SDL_arraysize(pattern_func_start));

   if (found == stop)
      return false; // no more functions

   // found entry code
   iter = found;

   // set up new function start item
   CodeGraphItem& opcode_item = *iter;

   CodeGraphItem func_item;
   func_item.m_type = typeFuncStart;
   func_item.m_pos = opcode_item.m_pos;

   // find out number of local variables
   graph_iterator iter_pushi = iter;
   std::advance(iter_pushi, 2);
   Uint16 numLocals = iter_pushi->opcode_data.arg;

   funcInfo.locals_types.resize(numLocals);

   // generate function name from label, if applicable
   if (opcode_item.m_pos == 0 || opcode_item.m_pos == 1)
   {
      opcode_item.m_labelName = "main";
      opcode_item.m_xrefCount = 1;
   }
   else if (opcode_item.m_labelName.empty() && opcode_item.m_xrefCount == 0)
   {
      // unused func
      std::ostringstream buffer;
      buffer << "unused_" << std::setfill('0') <<
         std::setw(4) << std::setbase(16) << opcode_item.m_pos;
      opcode_item.m_labelName = buffer.str();
   }
   else if (opcode_item.m_labelName.find("label_") == 0)
   {
      opcode_item.m_labelName = GetFunctionName(opcode_item.m_pos);
   }

   func_item.m_labelName = opcode_item.m_labelName;
   opcode_item.m_labelName = "";

   func_item.m_xrefCount = opcode_item.m_xrefCount;
   opcode_item.m_xrefCount = 0;

   // test if function has START opcode before this one
   bool func_start = false;
   if (iter != m_graph.begin())
   {
      graph_iterator iter_start = iter;
      --iter_start;

      func_start = iter_start->m_type == typeOpcode &&
         iter_start->opcode_data.opcode == op_START;

      if (func_start)
      {
         --iter;
         --func_item.m_pos;
      }
   }

   // insert new func item
   m_graph.insert(iter, func_item); // insert before iter

   // advance iterator
   SetOpcodesProcessed(iter, stop, func_start ? 5 : 4);

   funcInfo.start = func_item.m_pos;
   funcInfo.name = func_item.m_labelName;

   return true;
}

bool CodeGraph::FindFunctionExitPoint(graph_iterator& iter, graph_iterator stop, FuncInfo& funcInfo)
{
   const Uint16 pattern_func_end[3] = { op_BPTOSP, op_POPBP, op_RET };
   graph_iterator found = FindOpcodePattern(iter, stop, pattern_func_end, SDL_arraysize(pattern_func_end));

   if (found == stop)
      return false;

   iter = found;

   // set up new function end item
   CodeGraphItem& end_opcode_item = *iter;

   CodeGraphItem end_item;
   end_item.m_type = typeFuncEnd;
   end_item.m_pos = end_opcode_item.m_pos;

   // transer xref count and label
   end_item.m_xrefCount = end_opcode_item.m_xrefCount;
   end_opcode_item.m_xrefCount = 0;

   end_item.m_labelName = end_opcode_item.m_labelName;

   m_graph.insert(iter, end_item);

   SetOpcodesProcessed(iter, stop, 3);

   funcInfo.end = end_item.m_pos;

   return true;
}

void CodeGraph::UpdateCallJumpTargets()
{
   graph_iterator iter = m_graph.begin(), stop = m_graph.end();

   // go through code and search for functions
   for (; iter != stop; ++iter)
   {
      // correct jump targets for opcodes here
      if (iter->m_type == typeOpcode && iter->opcode_data.opcode == op_CALL)
      {
         if (iter->opcode_data.arg == 0xFFFF)
         {
            iter->opcode_data.jump_target = "invalid_FFFF";
            continue; // happens in uw2 code
         }

         graph_iterator target_iter = FindPos(iter->opcode_data.arg);
         iter->opcode_data.jump_target = target_iter->m_labelName;
      }
   }
}

void CodeGraph::ProcessFunctionQueue()
{
   // the queue may grow while analyzing a function
   while (!m_analysisQueue.empty())
   {
      std::string funcName = m_analysisQueue.front();
      m_analysisQueue.pop_front();

      if (m_functionMap.find(funcName) == m_functionMap.end())
      {
         UaAssert(false);
         continue; // function not found!
      }

      FuncInfo& funcInfo = m_functionMap[funcName];

      if (m_processedFunctions.find(funcInfo.start) != m_processedFunctions.end())
         continue; // already analyzed

      m_processedFunctions.insert(funcInfo.start);

      UaTrace("analyzing function \"%s\" at %04x\n",
         funcName.c_str(), funcInfo.start);

      AnalyzeFunction(funcInfo);

      CombineOperators(funcInfo);

      FindSwitchCase(funcInfo);

      FindWhile(funcInfo);

      FindDoWhile(funcInfo);

      FindIfElse(funcInfo);

      AddGotoJumps(funcInfo);

      PostProcessFunction(funcInfo);
   }
}

void CodeGraph::AnalyzeFunction(FuncInfo& funcInfo)
{
   graph_iterator iter = FindPos(funcInfo.start), stop = FindPos(funcInfo.end);

   if (iter == m_graph.end())
   {
      UaAssert(false);
      // opcode for function vanished?
      return;
   }

   // Go through the function and replace with expressions and operations;
   // Expressions are incomplete values on the stack.
   // Operations are complete control statements that may consume zero or more
   // expressions.
   for (; iter != stop; ++iter)
   {
      // only examine opcodes that weren't processed yet
      if (iter->m_type != typeOpcode || iter->m_isProcessed)
         continue;

      // first, try some pattern we need to process before looking at single opcodes
      const Uint16 pattern_local_array[3] = { op_PUSHI, op_PUSHI_EFF, op_OFFSET };

      if (MatchOpcodePattern(iter, stop, pattern_local_array, 3))
      {
         AddLocalArrayExpression(iter, funcInfo);

         SetOpcodesProcessed(iter, stop, 3);

         std::advance(iter, 3 - 1);
         continue;
      }

      const Uint16 pattern_return_stmt[2] = { op_SAVE_REG, op_POP };

      if (MatchOpcodePattern(iter, stop, pattern_return_stmt, 2))
      {
         AddOperator(iter, 1, false, dataTypeVoid);

         SetOpcodesProcessed(iter, stop, 2);

         // remember return type
         funcInfo.return_type = dataTypeInt;

         std::advance(iter, 2 - 1);
         continue;
      }

      CodeGraphItem& item = *iter;
      Uint16 opcode = item.opcode_data.opcode;

      switch (opcode)
      {
      case op_SAY_OP:
         // consumes 1 expression, returns none
         AddOperator(iter, 1, false, dataTypeVoid);
         break;

      case op_EXIT_OP:
         AddStatement(iter, "exit;");
         break;

         // intrinsic function call
      case op_CALLI:
         AddCallOperator(iter, stop, funcInfo, true);
         break;

         // local function call
      case op_CALL:
         AddCallOperator(iter, stop, funcInfo, false);
         break;

         // compare/arithmetic/logical operators
      case op_TSTEQ:
      case op_TSTGT:
      case op_TSTGE:
      case op_TSTLT:
      case op_TSTLE:
      case op_TSTNE:
      case op_OPOR:
      case op_OPAND:
      case op_OPADD:
      case op_OPSUB:
      case op_OPMUL:
      case op_OPDIV:
      case op_OPMOD:
         // binary operator, returns one int expression
         AddOperator(iter, 2, true, dataTypeInt);
         break;

      case op_OPNEG: // unary negate
      case op_OPNOT: // logical not
         AddOperator(iter, 1, true, dataTypeInt);
         break;

         // assignment operator
      case op_STO:
         AddAssignmentOperator(iter);
         break;

         // address-of local var
      case op_PUSHI_EFF: // note: must be checked after pattern_local_array
         AddAddressOfLocalVarExpression(iter, stop, funcInfo);
         break;

         // immediate value
      case op_PUSHI:
         // check if CALLI follows; if yes, don't process this PUSHI
      {
         graph_iterator next_iter = iter;
         ++next_iter;
         if (IsOpcode(next_iter, op_CALLI))
            continue;
      }

      AddImmediateExpression(iter, stop);
      break;

      // dereference-operator
      case op_FETCHM:
         // turns an address-of variable to value-of variable
         AddOperator(iter, 1, true, dataTypeInt);
         break;

         // array-index-operator
      case op_OFFSET:
         AddOperator(iter, 2, true, dataTypeInt);
         break;
      }
   }
}

CodeGraphItem& CodeGraph::AddOperator(graph_iterator& iter,
   Uint16 numNeededExpressions, bool returnsExpression, DataType returnedType)
{
   CodeGraphItem& item = *iter;
   item.m_isProcessed = true;

   Uint16 opcode = item.opcode_data.opcode;

   CodeGraphItem operatorItem;
   operatorItem.m_type = typeOperator;
   operatorItem.m_pos = iter->m_pos;
   operatorItem.operator_data.op_opcode = opcode;
   operatorItem.operator_data.numNeededExpressions = numNeededExpressions;
   operatorItem.operator_data.returns_expr = returnsExpression;
   operatorItem.operator_data.returned_type = returnedType;
   operatorItem.operator_data.prec_level =
      g_convInstructions[opcode].op_prec_level;

   return *m_graph.insert(iter, operatorItem);
}

CodeGraphItem& CodeGraph::AddStatement(graph_iterator iter, std::string text, bool setProcessed)
{
   CodeGraphItem& item = *iter;

   if (setProcessed)
      item.m_isProcessed = true;

   CodeGraphItem statementItem;
   statementItem.m_type = typeStatement;
   statementItem.m_pos = iter->m_pos;
   statementItem.statement_data.statement = text;

   return *m_graph.insert(iter, statementItem);
}

CodeGraphItem& CodeGraph::AddExpression(graph_iterator& iter, std::string text, bool isAddress)
{
   CodeGraphItem& item = *iter;
   item.m_isProcessed = true;

   CodeGraphItem expressionItem;
   expressionItem.m_type = typeExpression;
   expressionItem.m_pos = iter->m_pos;
   expressionItem.expression_data.expression = text;
   expressionItem.expression_data.isAddress = isAddress;

   if (!iter->m_labelName.empty())
   {
      expressionItem.m_labelName = iter->m_labelName;
      iter->m_labelName.clear();
   }

   return *m_graph.insert(iter, expressionItem);
}

void CodeGraph::AddLocalArrayExpression(graph_iterator iter, FuncInfo& funcInfo)
{
   // get array offset, from the op_PUSHI
   graph_iterator arr_iter = iter;
   UaAssert(arr_iter->m_type == typeOpcode && arr_iter->opcode_data.opcode == op_PUSHI);
   Uint16 offset = arr_iter->opcode_data.arg;

   // TODO arrays with offset == 0 really allowed?
   UaAssert(offset <= 0x7fff && offset >= 0);
   ++arr_iter;

   // get local var index, from op_PUSHI_EFF
   UaAssert(arr_iter->m_type == typeOpcode && arr_iter->opcode_data.opcode == op_PUSHI_EFF);

   Uint16 localIndex = arr_iter->opcode_data.arg;
   ++arr_iter;

   // add expression
   std::ostringstream buffer;
   buffer << "local_" << localIndex << "[" << offset << "]";

   AddExpression(iter, buffer.str(), true);

   AddArrayInfo(funcInfo, localIndex, offset);
}

void CodeGraph::AddArrayInfo(FuncInfo& funcInfo, Uint16 localIndex, Uint16 offset)
{
   size_t max = funcInfo.array_info.size();
   for (size_t i = 0; i < max; i++)
   {
      if (funcInfo.array_info[i].local_start == localIndex)
      {
         // found info; update array size
         ArrayInfo& array_info = funcInfo.array_info[i];
         array_info.array_size = std::max<unsigned int>(array_info.array_size, offset);

         return;
      }
   }

   // new array
   ArrayInfo array_info;
   array_info.local_start = localIndex;
   array_info.array_size = offset;

   funcInfo.array_info.push_back(array_info);
}

void CodeGraph::AddCallOperator(graph_iterator& iter, graph_iterator stop, const FuncInfo& funcInfo, bool isIntrinsic)
{
   iter->m_isProcessed = true;

   // note: when isIntrinsic == true, this is the intrinsic number
   //       when isIntrinsic == false, it is the local function pos
   Uint16 call_target = iter->opcode_data.arg;
   std::string target_name = iter->opcode_data.jump_target;

   if (isIntrinsic)
   {
      --iter; // previous PUSHI opcode tells number of arguments
      UaAssert(true == IsOpcode(iter, op_PUSHI));

      // set PUSHI opcode to "processed"
      iter->m_isProcessed = true;

      // Note: This number may be wrong, e.g. for babl_menu
      // therefore we do not use this, but do the POP counting below.
      //oper_item.control_data.numNeededExpressions = iter->opcode_data.arg;
   }

   // find out number of function args by counting following POP opcodes
   Uint16 arguments = 0;

   graph_iterator pop_iter = iter;
   std::advance(pop_iter, isIntrinsic ? 2 : 1);

   while (pop_iter != stop && IsOpcode(pop_iter, op_POP))
   {
      pop_iter->m_isProcessed = true;
      ++arguments;
      ++pop_iter;
   }

   // check if a PUSH_REG follows
   bool isReturnValue = false;
   if (IsOpcode(pop_iter, op_PUSH_REG))
   {
      pop_iter->m_isProcessed = true;
      isReturnValue = true;
   }

   // note: CALLI functions have 2 parameter, but the 1st is the number
   // of arguments, so subtract 1 from this number again
   if (isIntrinsic)
      --arguments;

   // operator with "arguments" needed expressions
   CodeGraphItem& operatorItem = AddOperator(iter, arguments, isReturnValue,
      isReturnValue ? dataTypeInt : dataTypeVoid);

   if (isIntrinsic)
   {
      if (g_externalFuncParameterInfos.find(target_name) != g_externalFuncParameterInfos.end())
      {
         const ParameterInfo& paramInfo = g_externalFuncParameterInfos[target_name];
         if (operatorItem.operator_data.numNeededExpressions ==
            paramInfo.paramIsArray.size())
         {
            operatorItem.operator_data.expressionTypes = paramInfo.paramTypes;
         }
         else
         {
            UaTrace("warning: number of call expressions doesn't match external function parameter count\n");
         }
      }
   }

   // correct opcode
   if (isIntrinsic)
      operatorItem.operator_data.op_opcode = op_CALLI;

   operatorItem.operator_data.prec_level = g_convInstructions[op_CALL].op_prec_level;

   operatorItem.operator_data.op_arg = call_target;

   std::advance(iter, arguments + (isReturnValue ? 1 : 0) - 1);

   if (!isIntrinsic)
   {
      // is recursive call?
      if (call_target == funcInfo.start)
      {
         UaTrace("discovered recursive function call to %hs() at %04x\n",
            target_name.c_str(), iter->m_pos);
         return;
      }

      if (target_name.find("invalid_") == 0)
         return; // happens in uw2 code

      // add to list of caller
      UaAssert(m_functionMap.find(target_name) != m_functionMap.end());

      FuncInfo& calledFuncInfo = m_functionMap[target_name];
      calledFuncInfo.caller.insert(funcInfo.name);

      // put in analysis queue, when not already processed
      if (m_processedFunctions.find(call_target) == m_processedFunctions.end())
         m_analysisQueue.push_back(target_name);
   }
}

void CodeGraph::AddAssignmentOperator(graph_iterator iter)
{
   iter->m_isProcessed = true;

   // check if a SWAP precedes the STO opcode
   graph_iterator swap_iter = iter;
   UaAssert(iter != m_graph.begin());
   --swap_iter;

   bool stoSwapArgs = false;
   if (IsOpcode(swap_iter, op_SWAP))
   {
      swap_iter->m_isProcessed = true;

      stoSwapArgs = true;
   }

   CodeGraphItem& operatorItem =
      AddOperator(iter, 2, false, dataTypeVoid);

   operatorItem.operator_data.sto_swap_args = stoSwapArgs;
}

void CodeGraph::AddAddressOfLocalVarExpression(graph_iterator& iter, graph_iterator stop, FuncInfo& funcInfo)
{
   // local var index
   Uint16 localIndex = iter->opcode_data.arg;
   iter->m_isProcessed = true;

   // set up expression item
   bool isAddress = true;

   // check if a FETCHM follows; then it's not the address of the
   // local var but the value
   graph_iterator fetchm_iter = iter;
   ++fetchm_iter;
   UaAssert(fetchm_iter != m_graph.end());

   bool isFetchm = IsOpcode(fetchm_iter, op_FETCHM);
   if (isFetchm)
   {
      fetchm_iter->m_isProcessed = true;
      isAddress = false;
   }

   std::ostringstream buffer;
   if (localIndex > 0x7fff)
   {
      unsigned int paramNum = (-(Sint16)localIndex) - 1;

      // param value
      buffer << "param" << paramNum;

      // this expression is an address, since param values are always
      // passed by reference
      isAddress = true;

      // set param info
      if (paramNum > funcInfo.param_types.size())
         funcInfo.param_types.resize(paramNum, dataTypeInt);
   }
   else
   {
      buffer << "local_" << localIndex;

      // add local variable info
      if (localIndex >= funcInfo.locals_types.size())
         funcInfo.locals_types.resize(localIndex + 1, dataTypeUnknown);

      UaAssert(localIndex < funcInfo.locals_types.size());

      funcInfo.locals_types[localIndex] = dataTypeInt;
   }

   AddExpression(iter, buffer.str(), isAddress);

   if (isFetchm)
      ++iter;
}

void CodeGraph::AddImmediateExpression(graph_iterator& iter, graph_iterator stop)
{
   iter->m_isProcessed = true;

   // check if a FETCHM follows; then it's not an immediate value,
   // but the value of a global var
   graph_iterator fetchm_iter = iter;
   ++fetchm_iter;
   UaAssert(fetchm_iter != m_graph.end());

   if (IsOpcode(fetchm_iter, op_FETCHM))
   {
      fetchm_iter->m_isProcessed = true;

      // global or private variable value
      Uint16 varIndex = iter->opcode_data.arg;

      std::string text = GetMemoryVarName(varIndex);

      AddExpression(iter, text, false);

      ++iter;
   }
   else
   {
      // might be an immediate (or a global address, which is resolved later)
      std::ostringstream buffer;
      buffer << iter->opcode_data.arg;

      CodeGraphItem& pushImmItem = AddExpression(iter, buffer.str(), false);

      pushImmItem.expression_data.pushi_imm_value = iter->opcode_data.arg;
      pushImmItem.expression_data.is_pushi_imm = true;
   }
}

void CodeGraph::CombineOperators(FuncInfo& funcInfo)
{
   graph_iterator start = FindPos(funcInfo.start),
      stop = FindPos(funcInfo.end);

   while (true)
   {
      // search next operator
      bool found = false;
      graph_iterator iter;
      for (iter = start; iter != stop; ++iter)
      {
         if (iter->m_type == typeOperator && !iter->m_isProcessed)
         {
            // found operator
            found = true;
            break;
         }
      }

      if (!found)
         break; // no more operators

      // combine found operator with expressions to form new expression
      graph_iterator operatorIter = iter;

      // check if a statement appears between this operator and the
      // expressions used, insert the new expression on the place the
      // operator is located; the reason for this is because uw1 conv.
      // code can use temporary local variables that are initialized and
      // used as parameter for function calls
      bool isStatementBetween = false;

      // search N previous expressions
      std::vector<graph_iterator> expressions;

      if (operatorIter->operator_data.numNeededExpressions > 0)
      {
         CollectExpressions(start, operatorIter,
            operatorIter->operator_data.numNeededExpressions,
            operatorIter->operator_data.expressionTypes,
            expressions, isStatementBetween);
      }

      // when statement was in between, insert at operator place
      // when not: are there expressions? when not, insert at operator place
      // else insert at first expression place
      graph_iterator insertIter = operatorIter;
      if (!isStatementBetween)
      {
         // since expressions are sorted later-to-earlier, use last iter as insert point
         if (!expressions.empty())
            insertIter = expressions.back();
      }

      CombineOperatorAndExpressions(funcInfo, operatorIter, stop, insertIter, expressions);

      // transfer label from first expression
      if (!expressions.empty())
      {
         graph_iterator last_expr_iter = expressions.back();
         if (!last_expr_iter->m_labelName.empty())
         {
            insertIter->m_labelName = last_expr_iter->m_labelName;
            last_expr_iter->m_labelName.clear();
         }
      }

      // mark operator as "done"
      operatorIter->m_isProcessed = true;

      // mark expressions as "done"
      size_t max = expressions.size();
      for (size_t i = 0; i < max; i++)
         expressions[i]->m_isProcessed = true;
   }
}

void CodeGraph::CollectExpressions(graph_iterator start, graph_iterator end,
   unsigned int numNeededExpressions,
   const std::vector<DataType>& expressionTypes,
   std::vector<graph_iterator>& expressions, bool& isStatementBetween)
{
   graph_iterator iter = end;
   --iter;
   while (iter != start && expressions.size() < numNeededExpressions)
   {
      if (iter->m_type == typeExpression && !iter->m_isProcessed)
      {
         // found an expression
         expressions.push_back(iter);
      }

      // remember if there was a statement
      // uw conv code supports something like "in-parameter value setting" like this:
      // get_quest(local_123 = 4);
      // when this is the case, remember that there were statements between
      if (iter->m_type == typeStatement)
         isStatementBetween = true;

      --iter;
   }

   // not enough expressions found between operator and start of function?
   UaAssert(iter != start);
   UaAssert(expressions.size() == end->operator_data.numNeededExpressions);
}

void CodeGraph::CombineOperatorAndExpressions(FuncInfo& funcInfo, graph_iterator operatorIter, graph_iterator stop,
   graph_iterator insertIter, std::vector<graph_iterator>& expressions)
{
   switch (operatorIter->operator_data.op_opcode)
   {
   case op_CALL:
   case op_CALLI:
      CombineCallOperator(funcInfo, operatorIter, insertIter, expressions);
      break;

      // create a "return" statement
   case op_SAVE_REG:
      CombineReturnExpression(insertIter, expressions);
      break;

   case op_FETCHM:
      CombineDereferenceExpression(insertIter, expressions);
      break;

   case op_OFFSET:
      CombineArrayExpression(insertIter, expressions);
      break;

      // comparison operators
   case op_TSTEQ:
   case op_TSTNE:
   case op_TSTGT:
   case op_TSTGE:
   case op_TSTLT:
   case op_TSTLE:
      // arithmetic and logical operators
   case op_OPADD:
   case op_OPMUL:
   case op_OPSUB:
   case op_OPDIV:
   case op_OPMOD:
   case op_OPOR:
   case op_OPAND:
      AddBinaryExpression(operatorIter, insertIter, expressions);
      break;

   case op_OPNEG:
      AddUnaryExpression(operatorIter, insertIter, expressions, "-");
      break;

   case op_OPNOT:
      AddUnaryExpression(operatorIter, insertIter, expressions, "!");
      break;

   case op_SAY_OP:
      CombineSayOp(insertIter, expressions);
      break;

   case op_STO:
      CombineAssignmentExpressions(operatorIter, stop, insertIter, expressions);
      break;

   default:
      // opcode appeared that wasn't processed
      UaAssert(false);
      break;
   }
}

void CodeGraph::CombineCallOperator(FuncInfo& funcInfo, graph_iterator operatorIter,
   graph_iterator insertIter, std::vector<graph_iterator>& expressions)
{
   std::ostringstream buffer;

   bool isIntrinsic = operatorIter->operator_data.op_opcode == op_CALLI;
   if (isIntrinsic)
   {
      // intrinsic
      Uint16 val = operatorIter->operator_data.op_arg;
      UaAssert(m_mapImportedFunctions.find(val) != m_mapImportedFunctions.end());

      buffer << m_mapImportedFunctions.find(val)->second.name;
   }
   else
   {
      buffer << GetFunctionName(operatorIter->operator_data.op_arg);
   }

   buffer << "(";

   // do parameter
   size_t max = expressions.size();
   for (size_t n = 0; n < max; n++)
   {
      CodeGraphItem& param = *expressions[n];

      DataType expressionType = dataTypeInt;
      if (!operatorIter->operator_data.expressionTypes.empty())
      {
         UaAssertMsg(
            n < operatorIter->operator_data.expressionTypes.size(),
            "number of expression types must match number of expressions");

         expressionType = operatorIter->operator_data.expressionTypes[n];
      }

      if (param.expression_data.isAddress &&
         param.expression_data.expression.find("local_") == 0)
         ReplaceLocalExpressionWithValue(funcInfo, expressions[n], expressionType);

      // check if lvalue is from PUSHI opcode
      if (param.expression_data.is_pushi_imm)
         PushiImmediateToGlobal(expressions[n]);

      buffer << param.expression_data.expression;

      if (n < max - 1)
         buffer << ", ";
   }

   // decide if function call returns data
   if (operatorIter->operator_data.returns_expr)
   {
      buffer << ")";

      AddExpression(insertIter, buffer.str(), false);
   }
   else
   {
      buffer << ");";

      AddStatement(insertIter, buffer.str());
   }
}

void CodeGraph::ReplaceLocalExpressionWithValue(FuncInfo& funcInfo, graph_iterator& expressionIter,
   DataType expressionType)
{
   // find statement with assignment to a local variable
   std::string assignment = expressionIter->expression_data.expression + " = ";

   graph_iterator start = FindPos(funcInfo.start);
   graph_iterator iter = expressionIter;
   --iter;
   while (iter != start)
   {
      if (iter->m_type == typeStatement &&
         iter->statement_data.statement.find(assignment) == 0)
         break;
      iter--;
   }

   if (iter == start)
      return; // no assignment found

   std::string value = iter->statement_data.statement.substr(assignment.size());
   size_t endPos = value.find_first_of(';');
   if (endPos != std::string::npos)
      value = value.substr(0, endPos);

   // check that value only consists of digits
   for (char ch : value)
      if (isdigit(ch) == 0 && ch != '-')
         return;

   // mark local variable as unknown, to prevent listing it
   std::string localVarIndex = expressionIter->expression_data.expression.substr(6);
   size_t localIndex = (size_t)strtol(localVarIndex.c_str(), nullptr, 10);
   funcInfo.locals_types[localIndex] = dataTypeUnknown;

   expressionIter->expression_data.isAddress = false;
   expressionIter->expression_data.expression = value;
   expressionIter->expression_data.is_pushi_imm = false;

   if (expressionType == dataTypeString)
   {
      expressionIter->expression_data.pushi_imm_value =
         (Uint16)strtol(value.c_str(), nullptr, 10);
      ReplaceIntExpressionWithString(expressionIter, true);
   }

   iter->m_isProcessed = true;
   iter->statement_data.statement = "// " + iter->statement_data.statement;
}

void CodeGraph::CombineReturnExpression(graph_iterator insertIter, std::vector<graph_iterator>& expressions)
{
   UaAssert(expressions.size() == 1);
   UaAssert(false == expressions[0]->expression_data.isAddress);

   std::ostringstream buffer;
   buffer << "return " <<
      expressions[0]->expression_data.expression << ";";

   AddStatement(insertIter, buffer.str());
}

void CodeGraph::CombineDereferenceExpression(graph_iterator insertIter, std::vector<graph_iterator>& expressions)
{
   UaAssert(expressions.size() == 1);
   UaAssert(true == expressions[0]->expression_data.isAddress);

   CodeGraphItem& newExpression = AddExpression(insertIter, "", false);
   newExpression.expression_data = expressions[0]->expression_data;
   newExpression.expression_data.isAddress = false;
}

void CodeGraph::CombineArrayExpression(graph_iterator insertIter, std::vector<graph_iterator>& expressions)
{
   UaAssert(expressions.size() == 2);

   // check if expression 0 is from PUSHI opcode
   if (expressions[0]->expression_data.is_pushi_imm)
      PushiImmediateToGlobal(expressions[0]);

   UaAssert(true == expressions[0]->expression_data.isAddress);
   UaAssert(false == expressions[1]->expression_data.isAddress);

   std::ostringstream buffer;

   buffer << expressions[0]->expression_data.expression <<
      "[" << expressions[1]->expression_data.expression << "]";

   AddExpression(insertIter, buffer.str(), true);
}

void CodeGraph::AddBinaryExpression(graph_iterator operatorIter,
   graph_iterator insertIter, std::vector<graph_iterator>& expressions)
{
   UaAssert(expressions.size() == 2);

   graph_iterator rvalue, lvalue;
   rvalue = expressions[0];
   lvalue = expressions[1];

   // check special case: left side of equality op is immediate value
   if (operatorIter->operator_data.op_opcode == op_TSTEQ)
   {
      if (lvalue->expression_data.is_pushi_imm)
         std::swap(rvalue, lvalue);
   }

   unsigned int precedence = operatorIter->operator_data.prec_level;

   // check that both lvalue and rvalue are value-of
   UaAssert(false == lvalue->expression_data.isAddress);
   UaAssert(false == rvalue->expression_data.isAddress);

   // when an expression used has a higher precedence, put
   // parenthesis around that expression
   std::ostringstream buffer;

   // lvalue
   if (precedence > lvalue->expression_data.prec_level)
      buffer << "(" << lvalue->expression_data.expression << ")";
   else
      buffer << lvalue->expression_data.expression;

   // operator character
   buffer << " " << g_convInstructions[operatorIter->operator_data.op_opcode].operator_text << " ";

   // rvalue
   if (precedence > rvalue->expression_data.prec_level)
      buffer << "(" << rvalue->expression_data.expression << ")";
   else
      buffer << rvalue->expression_data.expression;

   CodeGraphItem& newExpression = AddExpression(insertIter, buffer.str(), false);

   // the new expression has the lowest precedence of all used expressions
   unsigned int new_precedence = std::min(expressions[0]->expression_data.prec_level,
      expressions[1]->expression_data.prec_level);
   new_precedence = std::min(new_precedence, precedence);

   newExpression.expression_data.prec_level = new_precedence;
}

void CodeGraph::AddUnaryExpression(graph_iterator operatorIter,
   graph_iterator insertIter, std::vector<graph_iterator>& expressions, std::string prefix)
{
   UaAssert(expressions.size() == 1);
   UaAssert(false == expressions[0]->expression_data.isAddress);

   CodeGraphItem& newExpression = AddExpression(insertIter, "", false);

   newExpression.expression_data = expressions[0]->expression_data;

   std::string newtext = prefix + newExpression.expression_data.expression;

   newExpression.expression_data.expression = newtext;
}

void CodeGraph::CombineSayOp(graph_iterator insertIter, std::vector<graph_iterator>& expressions)
{
   UaAssert(expressions.size() == 1);

   // add statement
   // SAY with PUSHI before?
   UaAssert(true == expressions[0]->expression_data.is_pushi_imm);

   Uint16 string_id = expressions[0]->expression_data.pushi_imm_value;
   UaAssert(string_id < m_strings.size());

   // replace linefeeds in text
   std::string text{ m_strings[string_id] };

   Base::String::Replace(text, "\n", "\\n");

   std::ostringstream buffer;
   buffer << "say(\"" << text << "\");";

   AddStatement(insertIter, buffer.str());
}

void CodeGraph::CombineAssignmentExpressions(graph_iterator operatorIter, graph_iterator stop,
   graph_iterator insertIter, std::vector<graph_iterator>& expressions)
{
   UaAssert(expressions.size() == 2);

   graph_iterator rvalue, lvalue;
   rvalue = expressions[0];
   lvalue = expressions[1];

   // swap iterators if SWAP opcode was found
   if (operatorIter->operator_data.sto_swap_args)
      std::swap(rvalue, lvalue);

   // check if lvalue is from PUSHI opcode
   if (lvalue->expression_data.is_pushi_imm)
      PushiImmediateToGlobal(lvalue);

   // check if assignment is for call to babl_menu or babl_fmenu
   CheckBablMenu(operatorIter, stop, lvalue, rvalue);

   // check that lvalue is address-of and rvalue is value-of
   UaAssert(true == lvalue->expression_data.isAddress);
   UaAssert(false == rvalue->expression_data.isAddress);

   std::ostringstream buffer;
   buffer << lvalue->expression_data.expression << " = " <<
      rvalue->expression_data.expression << ";";

   AddStatement(insertIter, buffer.str());
}

void CodeGraph::CheckBablMenu(graph_iterator& start,
   const graph_iterator& stop, graph_iterator& lvalue, graph_iterator& rvalue)
{
   UaAssert(rvalue->m_type == typeExpression);
   UaAssert(lvalue->m_type == typeExpression);

   if (!rvalue->expression_data.is_pushi_imm ||
      lvalue->expression_data.expression.find("[") == std::string::npos)
      return;

   graph_iterator iter = start;
   while (iter != stop)
   {
      // search for intrinsic calls
      if (iter->m_type == typeOperator &&
         iter->operator_data.op_opcode == op_CALLI)
         break; // found

      ++iter;
   }

   if (iter == stop)
      return; // no operator with intrinsic found before end

   // check for babl_menu and babl_fmenu calls
   std::string functionName =
      m_mapImportedFunctions.find(iter->operator_data.op_arg)->second.name;

   if (functionName != "babl_menu" && functionName != "babl_fmenu")
      return;

   if (functionName == "babl_fmenu")
   {
      // check that lvalue array value matches arg0 of babl_fmenu
      graph_iterator prev_iter = iter;
      --prev_iter;
      if (prev_iter->m_type == typeOpcode &&
         prev_iter->opcode_data.opcode == op_PUSHI_EFF)
      {
         std::ostringstream buffer;
         buffer << "local_" << prev_iter->opcode_data.arg << "[";

         if (lvalue->expression_data.expression.find(buffer.str()) == std::string::npos)
            return; // not an expression for the "strings array" argument of babl_fmenu
      }
   }

   ReplaceIntExpressionWithString(rvalue, false);
}

void CodeGraph::ReplaceIntExpressionWithString(graph_iterator& intExpression, bool useBlockComment)
{
   // determine string id and format string
   Uint16 string_id = intExpression->expression_data.pushi_imm_value;

   if (string_id >= m_strings.size())
      return; // invalid string id

   UaAssert(string_id < m_strings.size());

   if (string_id > 0)
   {
      // replace linefeeds in text
      std::string text{ m_strings[string_id] };

      Base::String::Replace(text, "\n", "\\n");

      std::ostringstream buffer;
      buffer << "\"" << text << "\"";

      if (useBlockComment)
         buffer << " /* " << string_id << " */";
      else
         buffer << "; // " << string_id;

      intExpression->expression_data.expression = buffer.str();
      intExpression->expression_data.is_pushi_imm = false;
   }
}

/// For switch-case statement(s), code looks like this:
///
///   {expression1}             switch (variable-part)
///   BEQ label1                case (value-part1)
///   {code}                       {code}
///   JMP label_end ------+        break;
/// label1:               |
///   {expression2}       |     case (value-part2)
///   BEQ label2 ----+    |
///   {code}         |    |        {code}
///   JMP label_end -|----+        break;
/// label2:  <-------+    |
///   {expression}        |     case (value-part3)
///   BEQ label3 ----+    |
///   {code}         |    |        {code}
///   JMP label_end -|----+        break;
///                  V    V
///    ...               ...
/// labelN:               |     case (value-partN)
///   {expression}        |
///   BEQ label_end ------+
///   {code}              |        {code}
///   JMP label_end ------+        break;
/// label_end:   <--------+     switch-end
///   ...
///
/// Expression is split into the "variable-part" of an == operator and the "value-part" that
/// is used in every case expression.
void CodeGraph::FindSwitchCase(FuncInfo& funcInfo)
{
   graph_iterator iter = FindPos(funcInfo.start),
      stop = FindPos(funcInfo.end);

   // find open expression
   for (; iter != stop; ++iter)
   {
      bool isExpression = iter->m_type == typeExpression && !iter->m_isProcessed;
      if (!isExpression)
         continue;

      bool switchAdded = false;

      Uint16 switchEnd = 0;

      graph_iterator expressionIter = iter;

      do
      {
         bool ret = FindAndAddNextSwitchCase(expressionIter, stop, switchEnd, switchAdded);
         if (!ret)
            break;

         // generate end-switch when no expression follows
         if (expressionIter->m_type != typeExpression ||
            expressionIter->m_isProcessed == true ||
            expressionIter->m_pos >= switchEnd)
         {
            if (switchAdded)
            {
               graph_iterator switch_end_iter = expressionIter;
               --switch_end_iter;
               AddStatement(switch_end_iter, "} // end-switch", false);

               break;
            }
         }

      } while (switchEnd != 0 && expressionIter->m_pos < switchEnd);
   }
}

bool CodeGraph::FindAndAddNextSwitchCase(graph_iterator& expressionIter, graph_iterator stop, Uint16& switchEnd, bool& switchAdded)
{
   // a BEQ opcode must follow
   graph_iterator beq_iter = expressionIter;
   for (; beq_iter != stop; ++beq_iter)
   {
      if (!beq_iter->m_isProcessed && beq_iter->m_type == typeOpcode)
      {
         if (beq_iter->opcode_data.opcode == op_BEQ)
            break; // found
         else
         {
            beq_iter = stop;
            break;
         }
      }
   }

   if (beq_iter == stop)
      return false; // no BEQ found, try next expression

   // get JMP opcode, which must precede the BEQ jump target
   graph_iterator break_iter = FindPos(beq_iter->opcode_data.jump_target_pos);
   if (break_iter == m_graph.end())
      return false; // pos not found; give up

   --break_iter;
   if (break_iter->m_isProcessed ||
      break_iter->m_type != typeOpcode ||
      break_iter->opcode_data.opcode != op_JMP)
      return false; // no JMP found, try next expression

   // check if there is a second JMP immediately before this
   graph_iterator second_jmp_iter = break_iter;
   --second_jmp_iter;
   if (second_jmp_iter != stop &&
      !second_jmp_iter->m_isProcessed &&
      second_jmp_iter->m_type == typeOpcode &&
      second_jmp_iter->opcode_data.opcode == op_JMP)
   {
      // is this really a "goto" jump? or fall-through?

      // check if both jump targets are equal
      // when yes, mark both processed
      // when no, it's some sort of jump outside; let AddGotoJumps() handle it
      if (break_iter->opcode_data.jump_target_pos == second_jmp_iter->opcode_data.jump_target_pos)
      {
         break_iter->m_isProcessed = true;
         second_jmp_iter->m_isProcessed = true;
         break_iter = second_jmp_iter;
      }
   }

   // when first call, check if target of BEQ is an expression
   if (!switchAdded)
   {
      graph_iterator next_expr_iter = FindPos(beq_iter->opcode_data.jump_target_pos);

      if (next_expr_iter->m_type != typeExpression || next_expr_iter->m_isProcessed)
         return false; // no expression; maybe we found an if without else statement

      if (next_expr_iter->expression_data.expression.find(" == ") == std::string::npos)
         return false; // no expression that contains a comparison
   }

   if (switchEnd == 0)
      switchEnd = break_iter->opcode_data.jump_target_pos;

   graph_iterator case_iter = expressionIter;

   // insert switch statement
   std::string expression = expressionIter->expression_data.expression;
   if (!switchAdded)
   {
      std::string::size_type pos = expression.find(" == ");
      if (pos == std::string::npos)
      {
         // no expression "variable == value" found; give up
         return false;
      }

      {
         std::ostringstream buffer;
         buffer << "switch (" << expression.substr(0, pos) << ") {";

         AddStatement(expressionIter, buffer.str());
      }

      switchAdded = true;

      ++case_iter;
   }

   // add case statement
   {
      std::ostringstream buffer;

      std::string::size_type pos = expression.find(" == ");
      if (pos != std::string::npos)
      {
         std::string case_value = expression.substr(pos + 4);

         buffer << "case " << case_value << ":";
      }
      else
      {
         // no expression "variable == value" found; give up
         UaAssert(false);
         return false;
      }

      CodeGraphItem& caseStatement = AddStatement(case_iter, buffer.str());
      caseStatement.statement_data.indent_change_after = 1;
   }

   case_iter->m_isProcessed = true;
   beq_iter->m_isProcessed = true;

   // add break statement
   CodeGraphItem& breakStatement = AddStatement(break_iter, "break;");
   breakStatement.statement_data.indent_change_after = -1;

   // jump to next block via beq_iter
   expressionIter = FindPos(beq_iter->opcode_data.jump_target_pos);

   return true;
}

/// For while statement, code looks like this:
///
/// label1:   <----+
///   {expression} |
///   BEQ label2 --|--+
///   {code}       |  |
///   BRA label1 --+  |
/// label2:   <-------+
///   ...
///
void CodeGraph::FindWhile(FuncInfo& funcInfo)
{
   graph_iterator iter = FindPos(funcInfo.start),
      stop = FindPos(funcInfo.end);

   // find open expression
   for (; iter != stop; ++iter)
   {
      bool isExpression = iter->m_type == typeExpression && !iter->m_isProcessed;
      if (!isExpression)
         continue;

      graph_iterator expressionIter = iter;

      // a BEQ opcode must follow
      graph_iterator beq_iter = expressionIter;
      for (; beq_iter != stop; ++beq_iter)
      {
         if (!beq_iter->m_isProcessed && beq_iter->m_type == typeOpcode)
         {
            if (beq_iter->opcode_data.opcode == op_BEQ)
               break; // found
            else
            {
               beq_iter = stop;
               break;
            }
         }
      }

      if (beq_iter == stop)
         continue; // no BEQ found, try next expression

      // BEQ must jump down (when not, there is another control structure)
      UaAssert(beq_iter->m_pos < beq_iter->opcode_data.jump_target_pos);

      // check jump target

      // check if both jump targets are equal
      Uint16 target_pos = beq_iter->opcode_data.jump_target_pos;
      graph_iterator target_iter = FindPos(target_pos);

      // check if there is a BRA opcode before this one
      graph_iterator bra_iter = target_iter;
      --bra_iter;

      bool isBranch = !bra_iter->m_isProcessed && beq_iter->m_type == typeOpcode && bra_iter->opcode_data.opcode == op_BRA;
      if (!isBranch)
         continue;

      Uint16 bra_target_pos = bra_iter->opcode_data.jump_target_pos;

      if (bra_target_pos > target_pos)
         continue; // a BRA opcode, but jumps down; might be an if statement

      // check that BRA target points to our expression
      if (expressionIter->m_pos != bra_target_pos)
      {
         // The BRA target doesn't point to the expression used at
         // the start of the while() loop. That indicates a for()
         // statement which isn't parsed yet. The BRA opcode
         // instead jumps to the for loop break condition check.
         // Happens in conv 10.
         UaTrace("possible for() statement found on pos %04x\n",
            bra_target_pos);

         continue;
      }

      m_continuePositions.insert(std::make_tuple(expressionIter->m_pos, bra_iter->m_pos));

      // add statements
      std::ostringstream buffer;
      buffer << "while (" << expressionIter->expression_data.expression << ") {";

      CodeGraphItem& whileStatement = AddStatement(expressionIter, buffer.str());
      whileStatement.statement_data.indent_change_after = 1;

      beq_iter->m_isProcessed = true;

      CodeGraphItem& whileEndStatement = AddStatement(bra_iter, "} // end-while");
      whileEndStatement.statement_data.indent_change_before = -1;
   }
}

/// For do...while statement, code looks like this:
///
/// label1:   <----+
///   {code}       |
///   {expression} |
///   BEQ label2 --|--+
///   JMP label1 --+  |
/// label2:   <-------+
///   ...
///
void CodeGraph::FindDoWhile(FuncInfo& funcInfo)
{
   graph_iterator iter = FindPos(funcInfo.start),
      stop = FindPos(funcInfo.end);

   // find open expression
   for (; iter != stop; ++iter)
   {
      bool isExpression = iter->m_type == typeExpression && !iter->m_isProcessed;
      if (!isExpression)
         continue;

      graph_iterator expressionIter = iter;

      // a BEQ opcode must follow
      graph_iterator beq_iter = expressionIter;
      for (; beq_iter != stop; ++beq_iter)
      {
         if (!beq_iter->m_isProcessed && beq_iter->m_type == typeOpcode)
         {
            if (beq_iter->opcode_data.opcode == op_BEQ)
               break; // found
            else
            {
               beq_iter = stop;
               break;
            }
         }
      }

      if (beq_iter == stop)
         continue; // no BEQ found, try next expression

      // BEQ must jump down (when not, there is another control structure)
      UaAssert(beq_iter->m_pos < beq_iter->opcode_data.jump_target_pos);

      // check if there is a JMP opcode after the BEQ
      graph_iterator jmp_iter = beq_iter;
      ++jmp_iter;

      bool isJump = !jmp_iter->m_isProcessed &&
         jmp_iter->m_type == typeOpcode &&
         jmp_iter->opcode_data.opcode == op_JMP;

      if (!isJump)
         continue;

      Uint16 jmp_target_pos = jmp_iter->opcode_data.jump_target_pos;
      if (jmp_target_pos > beq_iter->m_pos)
         continue; // a JMP opcode, but jumps down; might be an if statement

      m_continuePositions.insert(std::make_tuple(jmp_target_pos, jmp_iter->m_pos));

      graph_iterator jmp_target_iter = FindPos(jmp_target_pos);

      // add statements
      CodeGraphItem& doStatement = AddStatement(jmp_target_iter, "do {", false);
      doStatement.statement_data.indent_change_after = 1;

      std::ostringstream buffer;
      buffer << "} while (" << expressionIter->expression_data.expression << ");";

      CodeGraphItem& whileStatement = AddStatement(expressionIter, buffer.str());
      whileStatement.statement_data.indent_change_before = -1;

      beq_iter->m_isProcessed = true;
      jmp_iter->m_isProcessed = true;

      // if there's a second JMP following the JMP, mark that processed as well
      graph_iterator second_jmp_iter = jmp_iter;
      ++second_jmp_iter;

      bool isSecondJump = !second_jmp_iter->m_isProcessed &&
         second_jmp_iter->m_type == typeOpcode &&
         second_jmp_iter->opcode_data.opcode == op_JMP;

      if (isSecondJump)
      {
         second_jmp_iter->m_isProcessed = true;
      }
   }
}

/// An if without else branch, code looks like this:
///
///   {expression}
///   BEQ label1 -----+
///   {code}          |
/// label1:   <-------+
///   ...
///
/// Another if (or if-not) variant, code looks like this:
///
///   {expression}
///   BEQ label1 -----+
///   JMP label2 ---+ |
/// label1:   <-----|-+
///   {code}        |
/// label2:   <-----+
///   ...
///
/// Another if-not variant, possibly a bug in the original compiler:
///
///   {expression}
///   BEQ label1 -----+
///   JMP label2 ---+ |
///   BRA label3 -+ | |
/// label1:   <---|-|-+
///   JMP label3 -+ |
/// label3:      -+ |
///   {code}        |
/// label2:   <-----+
///   ...
///
/// For if-else, code looks like this:
///
///   {expression}
///   BEQ label1 -----+
///   {code}          |
///   BRA label2 --+  |
/// label1:   <----|--+
///   {code}       |
/// label2:   <----+
///   ...
///
void CodeGraph::FindIfElse(FuncInfo& funcInfo)
{
   graph_iterator iter = FindPos(funcInfo.start),
      stop = FindPos(funcInfo.end);

   // find open expression
   for (; iter != stop; ++iter)
   {
      bool isExpression = iter->m_type == typeExpression && !iter->m_isProcessed;
      if (!isExpression)
         continue;

      graph_iterator expressionIter = iter;

      // a BEQ opcode must follow
      graph_iterator beq_iter = FindNextGraphItem(expressionIter, stop, typeOpcode);
      if (beq_iter == stop ||
         beq_iter->opcode_data.opcode != op_BEQ)
         continue; // no BEQ found, try next expression

      // BEQ must jump down (when not, there is another control structure)
      UaAssert(beq_iter->m_pos < beq_iter->opcode_data.jump_target_pos);

      // check jump target

      // check if both jump targets are equal
      Uint16 beq_target_pos = beq_iter->opcode_data.jump_target_pos;
      graph_iterator target_iter = FindPos(beq_target_pos);

      // check if there is a BRA opcode before the BEQ target
      graph_iterator opcode_before_target_iter = target_iter;
      --opcode_before_target_iter;

      graph_iterator else_iter = stop;
      graph_iterator endif_iter = stop;
      bool negateExpression = false;

      bool haveElse = !opcode_before_target_iter->m_isProcessed &&
         opcode_before_target_iter->m_type == typeOpcode &&
         opcode_before_target_iter->opcode_data.opcode == op_BRA;

      // for the second if-variant, the BEQ is followed by a JMP, or a JMP and
      // a BRA combination
      graph_iterator opcode_after_beq_iter = beq_iter;
      opcode_after_beq_iter++;

      bool haveIfNotVariant =
         (beq_target_pos == beq_iter->m_pos + 4 || beq_target_pos == beq_iter->m_pos + 6) &&
         !opcode_after_beq_iter->m_isProcessed &&
         opcode_after_beq_iter->m_type == typeOpcode &&
         opcode_after_beq_iter->opcode_data.opcode == op_JMP;

      if (haveElse && !haveIfNotVariant)
      {
         Uint16 bra_target_pos = opcode_before_target_iter->opcode_data.jump_target_pos;

         if (bra_target_pos < beq_target_pos)
         {
            // a BRA opcode, but jumps up; should be discovered by FindWhile() already!
            UaAssert(false);
            continue;
         }

         opcode_before_target_iter->m_isProcessed = true;

         else_iter = opcode_before_target_iter;
         ++else_iter;

         endif_iter = FindPos(bra_target_pos);

         // there might be a JMP opcode before the BRA; set this also to processed
         graph_iterator opcode_before_bra_iter = opcode_before_target_iter;
         opcode_before_bra_iter--;

         if (!opcode_before_bra_iter->m_isProcessed &&
            opcode_before_bra_iter->m_type == typeOpcode &&
            opcode_before_bra_iter->opcode_data.opcode == op_JMP &&
            opcode_before_bra_iter->opcode_data.jump_target_pos == bra_target_pos)
         {
            opcode_before_bra_iter->m_isProcessed = true;
         }

         // there also might be a JMP opcode before the endif
         graph_iterator opcode_before_endif_iter = endif_iter;
         while (opcode_before_endif_iter->m_type != typeOpcode)
            opcode_before_endif_iter--;

         if (!opcode_before_endif_iter->m_isProcessed &&
            opcode_before_endif_iter->m_type == typeOpcode &&
            opcode_before_endif_iter->opcode_data.opcode == op_JMP &&
            opcode_before_endif_iter->opcode_data.jump_target_pos == bra_target_pos)
         {
            opcode_before_endif_iter->m_isProcessed = true;
         }
      }
      else if (haveIfNotVariant)
      {
         opcode_after_beq_iter->m_isProcessed = true; // JMP was processed

         Uint16 jmp_target_pos = opcode_after_beq_iter->opcode_data.jump_target_pos;

         endif_iter = FindPos(jmp_target_pos);

         negateExpression = true;

         // there might be an error in the original compiler that sometimes issues a JMP and a BRA
         // opcode combination; set the opcodes as processed
         graph_iterator opcode_after_jmp_iter = opcode_after_beq_iter;
         opcode_after_jmp_iter++;

         if (!opcode_after_jmp_iter->m_isProcessed &&
            opcode_after_jmp_iter->m_type == typeOpcode &&
            opcode_after_jmp_iter->opcode_data.opcode == op_BRA)
         {
            opcode_after_jmp_iter->m_isProcessed = true;
         }

         graph_iterator beq_target_iter = FindPos(beq_target_pos);
         if (!beq_target_iter->m_isProcessed &&
            beq_target_iter->m_type == typeOpcode &&
            beq_target_iter->opcode_data.opcode == op_JMP &&
            beq_target_iter->opcode_data.jump_target_pos == beq_target_iter->m_pos + 2)
         {
            beq_target_iter->m_isProcessed = true;
         }
      }
      else
      {
         endif_iter = target_iter;
      }

      beq_iter->m_isProcessed = true;

      // insert statements
      std::string expressionText = expressionIter->expression_data.expression;
      if (negateExpression)
      {
         if (expressionText.substr(0, 1) == "!")
            expressionText = expressionText.substr(1);
         else
            expressionText = "!(" + expressionText + ")";
      }

      std::ostringstream buffer;
      buffer << "if (" << expressionText << ") {";

      CodeGraphItem& ifStatement = AddStatement(expressionIter, buffer.str());
      ifStatement.statement_data.indent_change_after = 1;

      if (else_iter != stop &&
         else_iter != endif_iter)
      {
         CodeGraphItem& elseStatement = AddStatement(else_iter, "} else {", false);
         elseStatement.statement_data.indent_change_before = -1;
         elseStatement.statement_data.indent_change_after = 1;
      }

      CodeGraphItem& endifStatement = AddStatement(endif_iter, "} // end-if", false);
      endifStatement.statement_data.indent_change_before = -1;
   }
}

void CodeGraph::AddGotoJumps(FuncInfo& funcInfo)
{
   graph_iterator iter = FindPos(funcInfo.start),
      stop = FindPos(funcInfo.end);

   // find remaining opcodes
   for (; iter != stop; ++iter)
   {
      // only examine opcodes that weren't processed yet
      if (iter->m_type != typeOpcode || iter->m_isProcessed)
         continue;

      if (iter->opcode_data.opcode == op_JMP ||
         iter->opcode_data.opcode == op_BRA)
      {
         graph_iterator goto_target = FindPos(iter->opcode_data.jump_target_pos);
         if (goto_target->m_type == typeFuncEnd)
         {
            AddStatement(iter, "return;");
            continue;
         }

         bool foundContinue = false;
         for (auto positionRange : m_continuePositions)
         {
            if (goto_target->m_pos == std::get<0>(positionRange) &&
               iter->m_pos < std::get<1>(positionRange))
            {
               AddStatement(iter, "continue;");
               foundContinue = true;
            }
         }

         if (foundContinue)
            continue;

         std::ostringstream buffer;
         buffer << "goto " << iter->opcode_data.jump_target << ";";

         AddStatement(iter, buffer.str());

         graph_iterator target_iter = FindPos(iter->opcode_data.jump_target_pos);

         // check if a label was already inserted
         if (target_iter->m_type != typeStatement ||
            target_iter->statement_data.statement.find("label") != 0)
         {
            // insert goto target label
            std::ostringstream buffer2;
            buffer2 << iter->opcode_data.jump_target << ":;";

            CodeGraphItem& labelStatement = AddStatement(target_iter, buffer2.str(), false);
            labelStatement.statement_data.indent_change_before = -1;
            labelStatement.statement_data.indent_change_after = 1;
         }
      }
      else
         UaAssert(false); // unhandled open opcode
   }
}

void CodeGraph::PostProcessFunction(FuncInfo& funcInfo)
{
   // get function start
   graph_iterator iter = FindPos(funcInfo.start);
   CodeGraphItem& startItem = *iter;
   UaAssert(startItem.m_type == typeFuncStart);

   std::ostringstream buffer;

   // return type
   buffer << DataTypeToString(funcInfo.return_type) << " ";

   // function name
   buffer << startItem.m_labelName << "(";

   // parameters
   const std::vector<DataType>& param_types = funcInfo.param_types;

   size_t max = param_types.size();
   for (size_t i = 0; i < max; i++)
   {
      buffer << DataTypeToString(param_types[i]) << " param" << i + 1;
      if (i < max - 1)
         buffer << ", ";
   }

   buffer << ")";
   buffer << " // referenced " << startItem.m_xrefCount << " times";

   CodeGraphItem& startStatement = AddStatement(iter, buffer.str());
   startStatement.statement_data.indent_change_after = 0;

   // opening brace
   CodeGraphItem& funcBrace = AddStatement(iter, "{");
   funcBrace.statement_data.indent_change_after = 1;

   // add all locals and arrays
   max = funcInfo.locals_types.size();
   for (size_t j = 0; j < max; j++)
   {
      DataType type = funcInfo.locals_types[j];

      if (type != dataTypeUnknown)
      {
         std::ostringstream buffer2;
         buffer2 << DataTypeToString(type) << " local_" << j << ";";

         AddStatement(iter, buffer2.str());
      }
   }

   // arrays
   max = funcInfo.array_info.size();
   for (size_t n = 0; n < max; n++)
   {
      ArrayInfo array_info = funcInfo.array_info[n];

      std::ostringstream buffer2;

      UaAssert(array_info.local_start <= funcInfo.locals_types.size());

      DataType type = funcInfo.locals_types[array_info.local_start];
      if (type != dataTypeUnknown)
      {
         buffer2 << DataTypeToString(type) <<
            " local_" << array_info.local_start <<
            "[" << array_info.array_size << "];";

         AddStatement(iter, buffer2.str());
      }
   }

   // add function end
   iter = FindPos(funcInfo.end);
   while (iter != m_graph.end() && iter->m_type != typeFuncEnd)
      ++iter;

   CodeGraphItem& endStatement = AddStatement(iter++, "} // end-function");
   endStatement.statement_data.indent_change_before = -1;

   // empty line
   AddStatement(iter, "");
}

void CodeGraph::MarkFunctionsUnused()
{
   std::for_each(m_functionMap.begin(), m_functionMap.end(), [&](const FunctionMap::value_type& val)
   {
      const FuncInfo& funcInfo = val.second;

      Uint16 startPos = funcInfo.start;
      if (m_processedFunctions.find(startPos) == m_processedFunctions.end())
      {
         if (funcInfo.name.find("unused_") == 0)
            return;

         // unused; mark with 0 xrefs
         graph_iterator iter = FindPos(funcInfo.start);
         UaAssert(iter != m_graph.end());

         UaTrace("removing function %s, it is never called\n", funcInfo.name.c_str());
         iter->m_xrefCount = 0;
      }
   });
}

// helper functions

CodeGraph::graph_iterator CodeGraph::FindPos(Uint16 target)
{
   graph_iterator iter = m_graph.begin(), stop = m_graph.end();

   for (; iter != stop; ++iter)
      if (iter->m_pos == target)
         return iter;

   // should not happen
   UaAssert(false);
   return stop;
}

CodeGraph::graph_iterator CodeGraph::FindNextGraphItem(
   CodeGraph::graph_iterator iter, CodeGraph::graph_iterator stop, GraphItemType itemType)
{
   for (; iter != stop; ++iter)
   {
      if (!iter->m_isProcessed &&
         iter->m_type == itemType)
         break; // found
   }

   return iter;
}

bool CodeGraph::IsOpcode(const_graph_iterator iter, Uint16 opcode) const
{
   return (iter->m_type == typeOpcode &&
      iter->opcode_data.opcode == opcode);
}

CodeGraph::graph_iterator CodeGraph::FindOpcodePattern(graph_iterator iter, graph_iterator stop,
   const Uint16* pattern, unsigned int length)
{
   unsigned int matchCount = 0;

   graph_iterator found = stop;

   for (; iter != stop; ++iter)
   {
      if (iter->m_type != typeOpcode)
         continue;

      if (iter->opcode_data.opcode == pattern[matchCount])
      {
         if (matchCount == 0)
            found = iter;

         ++matchCount;

         if (matchCount == length)
            return found; // found! return start
      }
      else
      {
         matchCount = 0; // reset; not all pattern opcodes matched
         found = stop;
      }
   }

   return stop;
}

bool CodeGraph::MatchOpcodePattern(const_graph_iterator iter, const_graph_iterator stop,
   const Uint16* pattern, unsigned int length) const
{
   for (unsigned int i = 0; i < length; i++, iter++)
      if (iter == stop || iter->m_type != typeOpcode ||
         iter->opcode_data.opcode != pattern[i])
         return false;

   // all pattern bytes found, no end reached
   return true;
}

void CodeGraph::SetOpcodesProcessed(graph_iterator iter, graph_iterator stop,
   unsigned int numOpcodes)
{
   for (unsigned int i = 0; i < numOpcodes && iter != stop; ++iter)
   {
      if (iter->m_type == typeOpcode)
      {
         iter->m_isProcessed = true;
         i++;
      }
   }
}

std::string CodeGraph::GetMemoryVarName(Uint16 memoryIndex) const
{
   UaAssert(memoryIndex < m_numGlobals);

   std::map<Uint16, ImportedItem>::const_iterator iter =
      m_mapImportedVariables.find(memoryIndex);

   if (iter != m_mapImportedVariables.end())
   {
      return iter->second.name;
   }
   else
   {
      std::ostringstream buffer;
      buffer << "global_" << memoryIndex;

      return buffer.str();
   }
}

void CodeGraph::PushiImmediateToGlobal(graph_iterator iter)
{
   UaAssert(iter->m_type == typeExpression);
   UaAssert(true == iter->expression_data.is_pushi_imm);
   UaAssert(false == iter->expression_data.isAddress);

   // resolve to proper address
   Uint16 memoryIndex = iter->expression_data.pushi_imm_value;

   iter->expression_data.expression = GetMemoryVarName(memoryIndex);

   iter->expression_data.is_pushi_imm = false;
   iter->expression_data.pushi_imm_value = 0;
   iter->expression_data.isAddress = true;
}

const char* CodeGraph::DataTypeToString(Conv::DataType type)
{
   switch (type)
   {
   case dataTypeVoid: return "void";
   case dataTypeInt: return "int";
   case dataTypeString: return "string";
   default:
      UaAssert(false);
      return "unknown";
   }
}

std::string CodeGraph::GetFunctionName(Uint16 pos)
{
   std::ostringstream buffer;

   switch (pos)
   {
   case 0x0012:
      buffer << "set_global_and_exit";
      break;

   case 0x0020:
      buffer << "is_npc_not_attacking";
      break;

   case 0x0063:
      buffer << "set_npc_attitude_upset_flee_player_and_exit";
      break;

   case 0x0081:
   case 0x008b:
      buffer << "target_player_and_exit";
      break;

   case 0x00b1:
      buffer << "set_npc_attitude_and_exit";
      break;

   case 0x00c2:
      buffer << "set_npc_attitude_mellow_and_exit";
      break;

   case 0x00d1:
      buffer << "set_npc_attitude_upset_and_exit";
      break;

   case 0x00e0:
      buffer << "set_global_and_exit2";
      break;

   case 0x00ea:
      buffer << "get_game_time";
      break;

   case 0x0106:
      buffer << "subtract_and_check_game_time";
      break;

   case 0x018f:
      buffer << "subtract_and_compare_game_times";
      break;

   case 0x029d:
      buffer << "start";
      break;

   default:
      buffer << "func_" << std::setfill('0') <<
         std::setw(4) << std::setbase(16) << pos;
      break;
   }

   return buffer.str();
}
