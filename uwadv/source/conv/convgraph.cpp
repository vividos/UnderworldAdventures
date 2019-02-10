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
/// \file convgraph.cpp
/// \brief conversation graph
//
#include "common.hpp"
#include "convgraph.hpp"
#include "opcodes.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

using Conv::CodeGraph;
using Conv::CodeGraphItem;

CodeGraph::CodeGraph(std::vector<Uint16> code,
   Uint16 codestart, Uint16 codeend,
   std::vector<std::string>& strings,
   std::map<Uint16, Conv::ImportedItem>& myimported_funcs,
   std::map<Uint16, Conv::ImportedItem>& myimported_vars,
   Uint16 myglob_reserved)
{
   m_strings.clear();
   m_strings = strings;

   m_mapImportedFunctions.clear();
   m_mapImportedFunctions = myimported_funcs;

   m_mapImportedVariables.clear();
   m_mapImportedVariables = myimported_vars;

   m_processed_funcs.clear();

   nglobals = myglob_reserved;

   // convert code segment to graph
   for (Uint16 i = codestart; i < codeend; i++)
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

void CodeGraph::Disassemble()
{
   CollectXrefs();
}

void CodeGraph::Decompile()
{
   CollectXrefs();
   FindFunctions();
   fix_call_jump_targets();

   // initially add start() as function
   {
      FuncInfo start_func;
      start_func.return_type = dataTypeVoid;
      start_func.start = 0;

      m_analysis_queue.push_back(start_func);
   }

   // analyze all functions in the analysis queue
   // the queue may grow while analyzing a function
   while (m_analysis_queue.size() > 0)
   {
      FuncInfo info = m_analysis_queue.front();
      m_analysis_queue.pop_front();

      graph_iterator iter = FindPos(info.start);

      UaAssert(0 != iter->m_labelName.size());

      // check if function was already analyzed
      if (m_functionMap.find(iter->m_labelName) == m_functionMap.end())
      {
         //UaTrace("analyzing function \"%s\" at %04x\n", iter->m_labelName.c_str(), info.start);

         AnalyzeFunction(info);

         post_process_function(info);

         // add to known functions
         //m_functionMap.insert(std::make_pair<std::string, Conv::FuncInfo>(iter->m_labelName, info));
         m_functionMap[iter->m_labelName] = info;
      }
   }
}

void CodeGraph::format_graph_item(std::string& item_text,
   const CodeGraphItem& item) const
{
   switch (item.m_type)
   {
   case Conv::typeFuncStart:
      item_text = "typeFuncStart";
      break;

   case Conv::typeFuncEnd:
      item_text = "typeFuncEnd";
      break;

   case Conv::typeOpcode:
      format_opcode(item_text, item);
      break;

   case Conv::typeStatement:
      item_text = item.statement_data.statement;
      break;

   case Conv::typeOperator:
   {
      std::ostringstream buffer;
      buffer <<
         (item.m_isProcessed ? "   // " : "") <<
         "operator " <<
         g_convInstructions[item.operator_data.op_opcode].mnemonic <<
         ", needs " << item.operator_data.needed_expr << " expressions, yields " <<
         (item.operator_data.returns_expr ? "an" : "no") << " expression; level=" <<
         item.operator_data.prec_level;

      item_text = buffer.str();
   }
   break;

   case Conv::typeExpression:
   {
      std::ostringstream buffer;
      buffer <<
         (item.m_isProcessed ? "   // " : "") <<
         "expression: " <<
         item.expression_data.expression <<
         (item.expression_data.is_address ? " (address-of)" : " (value-of)") <<
         "; level=" << item.expression_data.prec_level;
      item_text = buffer.str();
   }
   break;

   default:
      item_text = "// unknown graph item type!";
      break;
   }
}

void CodeGraph::format_opcode(std::string& opcode_text,
   const CodeGraphItem& item) const
{
   UaAssert(item.m_type == typeOpcode);

   std::ostringstream buffer;
   buffer << "   ";

   if (item.m_isProcessed)
      buffer << "// ";

   // code segment address
   buffer << std::setfill('0') << std::setw(4) <<
      std::setbase(16) << item.m_pos << " ";

   if (item.opcode_data.opcode > op_last)
   {
      // unknown opcode
      buffer << "??? (0x" << std::setfill('0') <<
         std::setw(4) << std::setbase(16) << item.opcode_data.opcode << ")";
   }
   else
   {
      Uint16 opcode = item.opcode_data.opcode;

      buffer << g_convInstructions[opcode].mnemonic;

      if (g_convInstructions[item.opcode_data.opcode].args > 0 &&
         item.opcode_data.jump_target.size() > 0 &&
         (opcode == op_JMP || opcode == op_BEQ || opcode == op_BNE ||
            opcode == op_BRA || opcode == op_CALL))
      {
         // label available
         buffer << " " << item.opcode_data.jump_target;
      }
      else
         if (opcode == op_CALLI)
         {
            // intrinsic function name
            Uint16 ifunc = item.opcode_data.arg;
            UaAssert(m_mapImportedFunctions.find(ifunc) != m_mapImportedFunctions.end());

            buffer << " " << m_mapImportedFunctions.find(ifunc)->second.name;
         }
         else
            if (g_convInstructions[item.opcode_data.opcode].args > 0)
            {
               // unknown, not resolved by CollectXrefs, or PUSH, PUSHI_EFF
               // just format the string
               buffer << " 0x" << std::setfill('0') <<
                  std::setw(4) << std::setbase(16) << item.opcode_data.arg;
            }
   }

   opcode_text = buffer.str();
}

void CodeGraph::CollectXrefs()
{
   UaAssert(m_graph.size() > 0);

   // start function
   m_graph.front().m_labelName = "start";
   m_graph.front().m_xrefCount = 1;

   graph_iterator iter, stop;
   iter = m_graph.begin();
   stop = m_graph.end();

   for (; iter != stop; iter++)
   {
      CodeGraphItem& item = *iter;
      if (item.m_type != typeOpcode)
         continue;
      Uint16 opcode = item.opcode_data.opcode;

      if (opcode == op_JMP || opcode == op_BEQ || opcode == op_BNE ||
         opcode == op_BRA || opcode == op_CALL)
      {
         // calculate jump target
         Uint16 target = item.opcode_data.arg;

         // adjust for relative jumps
         if (opcode == op_BEQ || opcode == op_BNE || opcode == op_BRA)
            target = static_cast<Uint16>((static_cast<Uint32>(target) + static_cast<Uint32>(item.m_pos + 1)) & 0xFFFF);

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
   }
}

/*! Searches for function entry and exit code and inserts additional
    highlevel icodes.

   function entry code:
   PUSHBP
   SPTOBP
   PUSHI #n   // n: locals size
   ADDSP

   and function exit code:
   BPTOSP
   POPBP
   RET
*/
void CodeGraph::FindFunctions()
{
   graph_iterator iter, stop;
   iter = m_graph.begin();
   stop = m_graph.end();

   // go through code and search for functions
   for (; iter != stop; iter++)
   {
      // check for function entry code
      Uint16 pattern_func_start[4] =
      { op_PUSHBP, op_SPTOBP, op_PUSHI, op_ADDSP };

      if (MatchOpcodePattern(iter, pattern_func_start, 4))
      {
         // found entry code

         // test if function started with START opcode
         bool func_start = false;
         if (iter != m_graph.begin())
         {
            graph_iterator iter_start = iter;
            iter_start--;

            func_start = iter_start->m_type == typeOpcode &&
               iter_start->opcode_data.opcode == op_START;

            if (func_start)
               --iter;
         }

         // set up new function start item
         CodeGraphItem& item = *iter;

         CodeGraphItem func_item;
         func_item.m_type = typeFuncStart;
         func_item.m_pos = item.m_pos;
         func_item.m_xrefCount = item.m_xrefCount;

         // generate function name from label, if applicable
         if (item.m_labelName.size() == 0 && item.m_xrefCount == 0)
         {
            // unused func
            std::ostringstream buffer;
            buffer << "unused_" << std::setfill('0') <<
               std::setw(4) << std::setbase(16) << item.m_pos;
            item.m_labelName = buffer.str();
         }
         else
            if (item.m_labelName.find("label_") == 0)
            {
               // label to func
               item.m_labelName.replace(0, 6, "func_");
            }

         func_item.m_labelName = item.m_labelName;
         item.m_labelName = "";
         item.m_xrefCount = 0;

         m_graph.insert(iter, func_item); // insert before iter

         // advance iterator

         // started with START opcode?
         if (func_start)
         {
            iter->m_isProcessed = true; iter++; // jump over START
         }
         iter->m_isProcessed = true; iter++; // PUSHBP
         iter->m_isProcessed = true; iter++; // SPTOBP

         // find out number of local variables
         //func_item.function_data.locals_size = iter->opcode_data.arg;

         iter->m_isProcessed = true; iter++; // PUSHI
         iter->m_isProcessed = true;         // ADDSP
      }

      // check for function exit code
      Uint16 pattern_func_end[3] = { op_BPTOSP, op_POPBP, op_RET };

      if (MatchOpcodePattern(iter, pattern_func_end, 3))
      {
         // set up new function end item
         CodeGraphItem& item = *iter;

         CodeGraphItem func_item;
         func_item.m_type = typeFuncEnd;
         func_item.m_pos = item.m_pos;

         m_graph.insert(iter, func_item);

         iter->m_isProcessed = true; iter++; // BPTOSP
         iter->m_isProcessed = true; iter++; // POPBP
         iter->m_isProcessed = true;         // RET
      }
   }
}

void CodeGraph::fix_call_jump_targets()
{
   graph_iterator iter, stop;
   iter = m_graph.begin();
   stop = m_graph.end();

   // go through code and search for functions
   for (; iter != stop; iter++)
   {
      // as a "service" we correct jump targets for opcodes here
      if (iter->m_type == typeOpcode && iter->opcode_data.opcode == op_CALL)
      {
         graph_iterator target_iter = FindPos(iter->opcode_data.arg);
         iter->opcode_data.jump_target = target_iter->m_labelName;
      }
   }
}

void CodeGraph::AnalyzeFunction(Conv::FuncInfo& info)
{
   // search start and end of function
   graph_iterator iter, start, stop;
   start = stop = FindPos(info.start);

   // search first opcode
   while (start != m_graph.end() && start->m_type != typeOpcode)
      start++;

   stop = start;

   // search function end
   while (stop != m_graph.end() && stop->m_type != typeFuncEnd)
      stop++;

   // go through the function and replace with expressions and operations
   // expressions are incomplete values on the stack
   // operations are complete control statements that may consume zero or more
   // expressions
   for (iter = start; iter != stop; iter++)
   {
      // only examine opcodes that weren't processed yet
      if (iter->m_type != typeOpcode || iter->m_isProcessed)
         continue;

      Uint16 pattern_local_array[3] = { op_PUSHI, op_PUSHI_EFF, op_OFFSET };
      Uint16 pattern_return_stmt[2] = { op_SAVE_REG, op_POP };

      Conv::CodeGraphItem& item = *iter;
      Uint16 opcode = item.opcode_data.opcode;

      // check for some simple control structures
      if (opcode == op_SAY_OP)
      {
         // say op
         item.m_isProcessed = true;

         // consumes 1 expression
         CodeGraphItem oper_item;
         oper_item.m_type = typeOperator;
         oper_item.m_pos = iter->m_pos;
         oper_item.operator_data.op_opcode = opcode;
         oper_item.operator_data.needed_expr = 1;
         oper_item.operator_data.returns_expr = false;
         oper_item.operator_data.prec_level =
            g_convInstructions[opcode].op_prec_level;

         // insert just before this opcode
         m_graph.insert(iter, oper_item);
      }
      else if (opcode == op_EXIT_OP)
      {
         // exit statement
         item.m_isProcessed = true;

         CodeGraphItem oper_item;
         oper_item.m_type = typeStatement;
         oper_item.m_pos = iter->m_pos;
         oper_item.statement_data.statement = "exit;";

         m_graph.insert(iter, oper_item);
      }
      else if (opcode == op_CALLI)
      {
         // intrinsic call
         item.m_isProcessed = true;
         Uint16 intrisic_no = item.opcode_data.arg;

         --iter; // previous PUSHI opcode tells number of arguments

         UaAssert(true == is_opcode(iter, op_PUSHI));

         // set PUSHI opcode to processed
         iter->m_isProcessed = true;

         CodeGraphItem oper_item;
         oper_item.m_type = typeOperator;
         oper_item.m_pos = iter->m_pos;

         // note: this number may be wrong, e.g. for babl_menu
         //oper_item.control_data.needed_expr = iter->opcode_data.arg;

         // find out number of function args by counting following POP opcodes
         Uint16 arguments = 0;
         graph_iterator pop_iter = iter; pop_iter++; pop_iter++;
         while (pop_iter != stop && is_opcode(pop_iter, op_POP))
         {
            pop_iter->m_isProcessed = true;
            arguments++;
            pop_iter++;
         }

         oper_item.operator_data.op_opcode = opcode;
         oper_item.operator_data.op_arg = intrisic_no;
         // note: CALLI functions have 2 parameter, but the 1st is the number
         // of arguments, so subtract 1 from this number again
         oper_item.operator_data.needed_expr = arguments - 1;

         oper_item.operator_data.returns_expr = false;

         // check if a PUSH_REG follows
         if (is_opcode(pop_iter, op_PUSH_REG))
         {
            pop_iter->m_isProcessed = true;
            oper_item.operator_data.returns_expr = true;
         }

         oper_item.operator_data.prec_level =
            g_convInstructions[opcode].op_prec_level;

         // insert just before this opcode
         m_graph.insert(iter, oper_item);

         ++iter;
      }
      else if (opcode == op_CALL)
      {
         // local function call
         Uint16 call_target = item.opcode_data.arg;
         item.m_isProcessed = true;

         CodeGraphItem oper_item;
         oper_item.m_type = typeOperator;
         oper_item.m_pos = iter->m_pos;

         // find out number of function args by counting following POP opcodes
         Uint16 arguments = 0;
         graph_iterator pop_iter = iter; pop_iter++;
         while (pop_iter != stop && is_opcode(pop_iter, op_POP))
         {
            pop_iter->m_isProcessed = true;
            arguments++;
            pop_iter++;
         }

         oper_item.operator_data.op_opcode = opcode;
         oper_item.operator_data.op_arg = call_target;
         oper_item.operator_data.needed_expr = arguments;

         oper_item.operator_data.returns_expr = false;

         // check if a PUSH_REG follows
         if (is_opcode(pop_iter, op_PUSH_REG))
         {
            pop_iter->m_isProcessed = true;
            oper_item.operator_data.returns_expr = true;
         }

         oper_item.operator_data.prec_level =
            g_convInstructions[opcode].op_prec_level;

         // insert just before this opcode
         m_graph.insert(iter, oper_item);

         // function call
         FuncInfo new_info;
         new_info.start = item.opcode_data.arg;
         new_info.return_type = dataTypeVoid; // type is void until known better
         // note: info.param_types is unknown for now

         // is recursive call?
         if (new_info.start == info.start)
            UaTrace("discovered recursive function call at %04x\n", item.m_pos);
         else
         {
            // put in analysis queue, when not already processed
            if (m_processed_funcs.find(new_info.start) == m_processed_funcs.end())
            {
               m_processed_funcs.insert(new_info.start);
               m_analysis_queue.push_back(new_info);
            }
         }
      }
      // compare/arithmetic/logical operators
      else if (opcode == op_TSTEQ || opcode == op_TSTGT || opcode == op_TSTGE ||
         opcode == op_TSTLT || opcode == op_TSTLE || opcode == op_TSTNE ||
         opcode == op_OPOR || opcode == op_OPAND || opcode == op_OPADD ||
         opcode == op_OPSUB || opcode == op_OPMUL || opcode == op_OPDIV ||
         opcode == op_OPMOD)
      {
         // binary operator
         item.m_isProcessed = true;

         CodeGraphItem oper_item;
         oper_item.m_type = typeOperator;
         oper_item.m_pos = iter->m_pos;
         oper_item.operator_data.op_opcode = opcode;
         oper_item.operator_data.needed_expr = 2;
         oper_item.operator_data.returns_expr = true;
         oper_item.operator_data.returned_type = dataTypeInt;
         oper_item.operator_data.prec_level =
            g_convInstructions[opcode].op_prec_level;

         m_graph.insert(iter, oper_item);
      }
      // unary negate
      else if (opcode == op_OPNEG)
      {
         // unary operator
         item.m_isProcessed = true;

         CodeGraphItem oper_item;
         oper_item.m_type = typeOperator;
         oper_item.m_pos = iter->m_pos;
         oper_item.operator_data.op_opcode = opcode;
         oper_item.operator_data.needed_expr = 1;
         oper_item.operator_data.returns_expr = true;
         oper_item.operator_data.returned_type = dataTypeInt;
         oper_item.operator_data.prec_level =
            g_convInstructions[opcode].op_prec_level;

         m_graph.insert(iter, oper_item);
      }
      // logical not
      else if (opcode == op_OPNOT)
      {
         // unary operator
         item.m_isProcessed = true;

         CodeGraphItem oper_item;
         oper_item.m_type = typeOperator;
         oper_item.m_pos = iter->m_pos;
         oper_item.operator_data.op_opcode = opcode;
         oper_item.operator_data.needed_expr = 1;
         oper_item.operator_data.returns_expr = true;
         oper_item.operator_data.returned_type = dataTypeInt;
         oper_item.operator_data.prec_level =
            g_convInstructions[opcode].op_prec_level;

         m_graph.insert(iter, oper_item);
      }
      else
         // assignment operator
         if (opcode == op_STO)
         {
            item.m_isProcessed = true;

            CodeGraphItem oper_item;
            oper_item.m_type = typeOperator;
            oper_item.m_pos = iter->m_pos;
            oper_item.operator_data.op_opcode = opcode;
            oper_item.operator_data.needed_expr = 2;
            oper_item.operator_data.returns_expr = false;
            oper_item.operator_data.returned_type = dataTypeVoid;
            oper_item.operator_data.prec_level =
               g_convInstructions[opcode].op_prec_level;

            // check if a SWAP precedes the STO opcode
            graph_iterator swap_iter = iter;
            UaAssert(iter != m_graph.begin());
            swap_iter--;

            if (is_opcode(swap_iter, op_SWAP))
            {
               oper_item.operator_data.sto_swap_args = true;

               swap_iter->m_isProcessed = true;
            }

            m_graph.insert(iter, oper_item);
         }
      // local array
         else if (MatchOpcodePattern(iter, pattern_local_array,
            SDL_TABLESIZE(pattern_local_array)))
         {
            // array offset
            graph_iterator arr_iter = iter;
            Uint16 offset = arr_iter->opcode_data.arg;
            arr_iter->m_isProcessed = true;

            // TODO arrays with offset == 0 really allowed?
            UaAssert(offset <= 0x7fff && offset >= 0);
            arr_iter++;

            // local var index
            Uint16 local_idx = arr_iter->opcode_data.arg;
            arr_iter->m_isProcessed = true;
            arr_iter++;

            // offset
            arr_iter->m_isProcessed = true;

            // set up expression item
            CodeGraphItem local_array_item;
            local_array_item.m_type = typeExpression;
            local_array_item.m_pos = iter->m_pos;
            local_array_item.expression_data.is_address = true;

            std::ostringstream buffer;
            buffer << "local_" << local_idx <<
               "[" << offset << "]";

            local_array_item.expression_data.expression = buffer.str();

            // add to info.array_info
            add_array_info(info, local_idx, offset);

            m_graph.insert(iter, local_array_item);
            iter++; iter++;
         }
      // address-of local var
         else if (opcode == op_PUSHI_EFF)
         {
            // local var index
            Uint16 local_idx = iter->opcode_data.arg;
            iter->m_isProcessed = true;

            // set up expression item
            CodeGraphItem local_var_item;
            local_var_item.m_type = typeExpression;
            local_var_item.m_pos = iter->m_pos;
            local_var_item.expression_data.is_address = true;

            // check if a FETCHM follows; then it's not the address of the
            // local var but the value
            graph_iterator fetchm_iter = iter; fetchm_iter++;
            UaAssert(fetchm_iter != m_graph.end());
            if (is_opcode(fetchm_iter, op_FETCHM))
            {
               fetchm_iter->m_isProcessed = true;
               local_var_item.expression_data.is_address = false;
            }

            std::ostringstream buffer;
            if (local_idx > 0x7fff)
            {
               unsigned int param_num = (-(Sint16)local_idx) - 1;

               // param value
               buffer << "param" << param_num;

               // this expression is an address, since param values are always
               // passed by reference
               local_var_item.expression_data.is_address = true;

               // set param info
               if (param_num > info.param_types.size())
                  info.param_types.resize(param_num, dataTypeInt);
            }
            else
            {
               buffer << "local_" << local_idx;

               // add local variable info
               if (local_idx >= info.locals_types.size())
                  info.locals_types.resize(local_idx + 1, dataTypeUnknown);

               UaAssert(local_idx < info.locals_types.size());

               info.locals_types[local_idx] = dataTypeInt;
            }

            local_var_item.expression_data.expression = buffer.str();

            m_graph.insert(iter, local_var_item);
         }
      // immediate value
         else if (opcode == op_PUSHI)
         {
            // check if CALLI follows; if yes, don't process this PUSHI
            graph_iterator next_iter = iter; next_iter++;
            if (!is_opcode(next_iter, op_CALLI))
            {
               iter->m_isProcessed = true;

               // set up expression item
               CodeGraphItem immediate_item;
               immediate_item.m_type = typeExpression;
               immediate_item.m_pos = iter->m_pos;
               immediate_item.expression_data.is_address = false;

               // check if a FETCHM follows; then it's not an immediate value,
               // but the value of a global var
               graph_iterator fetchm_iter = iter; fetchm_iter++;
               UaAssert(fetchm_iter != m_graph.end());

               if (is_opcode(fetchm_iter, op_FETCHM))
               {
                  fetchm_iter->m_isProcessed = true;

                  // global or private variable value
                  Uint16 var_idx = iter->opcode_data.arg;

                  immediate_item.expression_data.expression = GetMemoryVarName(var_idx);
               }
               else
               {
                  // might be an immediate or a global address
                  std::ostringstream buffer;
                  buffer << iter->opcode_data.arg;

                  immediate_item.expression_data.expression = buffer.str();
                  immediate_item.expression_data.pushi_imm_value = iter->opcode_data.arg;
                  immediate_item.expression_data.is_pushi_imm = true;
               }

               m_graph.insert(iter, immediate_item);
            }
         }
      // return-statement
         else if (MatchOpcodePattern(iter, pattern_return_stmt,
            SDL_TABLESIZE(pattern_return_stmt)))
         {
            // returns value from function
            iter->m_isProcessed = true;

            graph_iterator next_iter = iter; next_iter++;
            UaAssert(next_iter != m_graph.begin());
            next_iter->m_isProcessed = true;

            // set up expression item
            CodeGraphItem return_item;
            return_item.m_type = typeOperator;
            return_item.m_pos = iter->m_pos;
            return_item.operator_data.op_opcode = opcode;
            return_item.operator_data.needed_expr = 1;
            return_item.operator_data.returns_expr = false;
            return_item.operator_data.returned_type = dataTypeInt;
            return_item.operator_data.prec_level =
               g_convInstructions[opcode].op_prec_level;

            m_graph.insert(iter, return_item);

            // remember return type
            info.return_type = dataTypeInt;
         }
      // dereference-operator
      if (opcode == op_FETCHM)
      {
         // turns an address-of variable to value-of variable
         iter->m_isProcessed = true;

         // set up expression item
         CodeGraphItem deref_item;
         deref_item.m_type = typeOperator;
         deref_item.m_pos = iter->m_pos;
         deref_item.operator_data.op_opcode = opcode;
         deref_item.operator_data.needed_expr = 1;
         deref_item.operator_data.returns_expr = true;
         deref_item.operator_data.returned_type = dataTypeInt;
         deref_item.operator_data.prec_level =
            g_convInstructions[opcode].op_prec_level;

         m_graph.insert(iter, deref_item);
      }
      else
         // array-index-operator
         if (opcode == op_OFFSET)
         {
            // indexes an array
            iter->m_isProcessed = true;

            // set up expression item
            CodeGraphItem arr_idx_item;
            arr_idx_item.m_type = typeOperator;
            arr_idx_item.m_pos = iter->m_pos;
            arr_idx_item.operator_data.op_opcode = opcode;
            arr_idx_item.operator_data.needed_expr = 2;
            arr_idx_item.operator_data.returns_expr = true;
            arr_idx_item.operator_data.returned_type = dataTypeInt;
            arr_idx_item.operator_data.prec_level =
               g_convInstructions[opcode].op_prec_level;

            m_graph.insert(iter, arr_idx_item);
         }
   }

   combine_operators(start, stop);
   find_control_structs(start, stop);
   add_goto_jumps(start, stop);
}

void CodeGraph::add_array_info(Conv::FuncInfo& info, Uint16 local_idx, Uint16 offset)
{
   unsigned int max = info.array_info.size();
   for (unsigned int i = 0; i < max; i++)
   {
      if (info.array_info[i].local_start == local_idx)
      {
         // found info; update array size
         ArrayInfo& array_info = info.array_info[i];
         array_info.array_size = std::max<unsigned int>(array_info.array_size, offset);

         return;
      }
   }

   // new array
   ArrayInfo array_info;
   array_info.local_start = local_idx;
   array_info.array_size = offset;

   info.array_info.push_back(array_info);
}

/*!
    search operators and try to build new expressions until no operators are
    left
*/
void CodeGraph::combine_operators(/*FuncInfo& info*/
   const graph_iterator& start, const graph_iterator& stop)
{
   while (true)
   {
      // search next operator
      graph_iterator iter, last_oper = m_graph.end();
      for (iter = start; iter != stop; iter++)
      {
         if (iter->m_type == typeOperator && !iter->m_isProcessed)
         {
            // found operator
            last_oper = iter;
            break;
         }
      }

      if (last_oper == m_graph.end())
         break; // no more operators

      // combine found operator with expressions to form new expression
      graph_iterator oper = last_oper;

      bool statement_between = false;

      // search n previous expressions
      std::vector<graph_iterator> expressions;

      Uint16 min_expr_pos = oper->m_pos;

      iter = oper;
      while (iter != start && expressions.size() < oper->operator_data.needed_expr)
      {
         if (iter->m_type == typeExpression && !iter->m_isProcessed)
         {
            // found an expression
            expressions.push_back(iter);

            // remember expression with minimum position
            if (iter->m_pos < min_expr_pos)
               min_expr_pos = iter->m_pos;
         }
         if (iter->m_type == typeStatement)
            statement_between = true;
         iter--;
      }

      UaAssert(iter != start);
      UaAssert(expressions.size() == oper->operator_data.needed_expr);

      // set up new item; for now we assume it's another expression
      CodeGraphItem new_expression;
      new_expression.m_pos = min_expr_pos;

      // now that we have n expressions, we can combine it with the operator
      switch (oper->operator_data.op_opcode)
      {
      case op_CALL:
      case op_CALLI:
      {
         std::ostringstream buffer;

         if (oper->operator_data.op_opcode == op_CALLI)
         {
            Uint16 val = oper->operator_data.op_arg;
            UaAssert(m_mapImportedFunctions.find(val) != m_mapImportedFunctions.end());

            buffer << m_mapImportedFunctions.find(val)->second.name;
         }
         else
            buffer << "func_" << std::setfill('0') <<
            std::setw(4) << std::setbase(16) << oper->operator_data.op_arg;
         buffer << "(";

         // do parameter
         unsigned int max = expressions.size();
         for (unsigned int n = 0; n < max; n++)
         {
            CodeGraphItem& param = *expressions[n];

            // check if lvalue is from PUSHI opcode
            if (param.expression_data.is_pushi_imm)
               PushiImmediateToGlobal(expressions[n]);

            // param must be address, since all params are passed by reference/pointer
            UaAssert(true == param.expression_data.is_address);

            buffer << param.expression_data.expression;

            if (n < max - 1)
               buffer << ", ";
         }

         // since a statement appeared between this operator and the
         // expressions used, insert the new expression on the place the
         // operator is located; the reason for this is because uw1 conv.
         // code can use temporary local variables that are initialized and
         // used as parameter for function calls
         if (statement_between)
            new_expression.m_pos = oper->m_pos;

         // decide if function call returns data
         if (oper->operator_data.returns_expr)
         {
            buffer << ")";

            new_expression.m_type = typeExpression;
            new_expression.expression_data.expression = buffer.str();
            new_expression.expression_data.is_address = false;
         }
         else
         {
            buffer << ");";

            new_expression.m_type = typeStatement;
            new_expression.statement_data.statement = buffer.str();
         }
      }
      break;

      case op_SAVE_REG:
      {
         UaAssert(expressions.size() == 1);
         UaAssert(false == expressions[0]->expression_data.is_address)

            new_expression.m_type = typeStatement;

         std::ostringstream buffer;
         buffer << "return " <<
            expressions[0]->expression_data.expression << ";";

         new_expression.statement_data.statement = buffer.str();
      }
      break;

      case op_FETCHM:
      {
         UaAssert(expressions.size() == 1);
         UaAssert(true == expressions[0]->expression_data.is_address);

         new_expression.m_type = typeExpression;
         new_expression.expression_data = expressions[0]->expression_data;
         new_expression.expression_data.is_address = false;
      }
      break;

      case op_OFFSET:
      {
         UaAssert(expressions.size() == 2);

         // check if expression 0 is from PUSHI opcode
         if (expressions[0]->expression_data.is_pushi_imm)
            PushiImmediateToGlobal(expressions[0]);

         UaAssert(true == expressions[0]->expression_data.is_address);
         UaAssert(false == expressions[1]->expression_data.is_address);

         new_expression.m_type = typeExpression;
         new_expression.expression_data.is_address = true;

         std::ostringstream buffer;

         buffer << expressions[0]->expression_data.expression <<
            "[" << expressions[1]->expression_data.expression << "]";

         new_expression.expression_data.expression = buffer.str();
      }
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
      {
         UaAssert(expressions.size() == 2);

         new_expression.m_type = typeExpression;

         graph_iterator rvalue, lvalue;
         rvalue = expressions[0];
         lvalue = expressions[1];

         unsigned int precedence = oper->operator_data.prec_level;

         // check that both lvalue and rvalue are value-of
         UaAssert(false == lvalue->expression_data.is_address);
         UaAssert(false == rvalue->expression_data.is_address);

         // do new expression
         // when an expression used has a higher proecedence, put
         // parenthesis around that expression
         std::ostringstream buffer;

         // lvalue
         if (precedence > lvalue->expression_data.prec_level)
            buffer << "(" << lvalue->expression_data.expression << ")";
         else
            buffer << lvalue->expression_data.expression;

         // operator char
         buffer << " " << g_convInstructions[oper->operator_data.op_opcode].operator_text << " ";

         // rvalue
         if (precedence > rvalue->expression_data.prec_level)
            buffer << "(" << rvalue->expression_data.expression << ")";
         else
            buffer << rvalue->expression_data.expression;

         new_expression.expression_data.expression = buffer.str();
         new_expression.expression_data.is_address = false;

         // the new expression has the lowest precedence of all used expressions
         unsigned int new_prec = std::min(expressions[0]->expression_data.prec_level,
            expressions[1]->expression_data.prec_level);
         new_prec = std::min(new_prec, precedence);

         new_expression.expression_data.prec_level = new_prec;
      }
      break;

      case op_OPNEG:
      {
         UaAssert(expressions.size() == 1);
         UaAssert(false == expressions[0]->expression_data.is_address);

         new_expression.m_type = typeExpression;
         new_expression.expression_data = expressions[0]->expression_data;

         std::string newtext("-");
         newtext += new_expression.expression_data.expression;

         new_expression.expression_data.expression = newtext;
      }
      break;

      case op_OPNOT:
      {
         UaAssert(expressions.size() == 1);
         UaAssert(false == expressions[0]->expression_data.is_address);

         new_expression.m_type = typeExpression;
         new_expression.expression_data = expressions[0]->expression_data;

         std::string newtext("!");
         newtext += new_expression.expression_data.expression;

         new_expression.expression_data.expression = newtext;
      }
      break;

      case op_SAY_OP:
      {
         UaAssert(expressions.size() == 1);

         // no, we have a statement
         new_expression.m_type = typeStatement;

         // SAY with PUSHI before?
         UaAssert(true == expressions[0]->expression_data.is_pushi_imm);

         Uint16 string_id = expressions[0]->expression_data.pushi_imm_value;
         UaAssert(string_id < m_strings.size());

         // replace linefeeds in text
         std::string outtext(m_strings[string_id]);

         std::string::size_type pos = 0;
         while (std::string::npos != (pos = outtext.find('\n')))
            outtext.replace(pos, 1, "\\n");

         std::ostringstream buffer;
         buffer << "say(\"" << outtext << "\");";
         new_expression.statement_data.statement = buffer.str();
      }
      break;

      case op_STO:
      {
         UaAssert(expressions.size() == 2);

         new_expression.m_type = typeStatement;

         graph_iterator rvalue, lvalue;
         rvalue = expressions[0];
         lvalue = expressions[1];

         // swap iterators if SWAP opcode was found
         if (oper->operator_data.sto_swap_args)
            std::swap(rvalue, lvalue);

         // check if lvalue is from PUSHI opcode
         if (lvalue->expression_data.is_pushi_imm)
            PushiImmediateToGlobal(lvalue);

         // check if assignment is for call to babl_menu or babl_fmenu
         CheckBablMenu(oper, stop, lvalue, rvalue);

         // check that lvalue is address-of and rvalue is value-of
         UaAssert(true == lvalue->expression_data.is_address);
         UaAssert(false == rvalue->expression_data.is_address);

         std::ostringstream buffer;
         buffer << lvalue->expression_data.expression << " = " <<
            rvalue->expression_data.expression << ";";

         new_expression.statement_data.statement = buffer.str();

         // also relocate store expression; see op_CALL/op_CALLI part for
         // further comments
         if (statement_between)
            new_expression.m_pos = expressions[0]->m_pos;
      }
      break;

      default:
         // opcode appeared that wasn't processed
         UaAssert(false);
         break;
      }

      // mark operator as "done"
      oper->m_isProcessed = true;

      // mark expressions as "done"
      unsigned int max = expressions.size();
      for (unsigned int i = 0; i < max; i++)
         expressions[i]->m_isProcessed = true;

      // insert new expression before first expression used for this new one
      graph_iterator insert_iter = FindPos(new_expression.m_pos);
      UaAssert(insert_iter != m_graph.end());

      m_graph.insert(insert_iter, new_expression);
   }
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
   while (iter != stop && start != m_graph.end())
   {
      // search for babl_menu and babl_fmenu intrinsic calls
      if (iter->m_type == typeOpcode &&
         iter->opcode_data.opcode == op_CALLI)
      {
         std::string intr_name(
            m_mapImportedFunctions.find(iter->opcode_data.arg)->second.name);

         if (intr_name != "babl_menu"/* && intr_name != "babl_fmenu"*/)
            break;

         if (intr_name == "babl_menu")
         {
            // TODO check that lvalue array value matches arg0 of babl_menu
         }
         /*
                  if (intr_name == "babl_fmenu")
                  {
                     // TODO check that lvalue array value matches arg0 of babl_fmenu
                  }
         */
         // determine string id and format string
         Uint16 str_id = rvalue->expression_data.pushi_imm_value;

         if (str_id >= m_strings.size())
            break;

         UaAssert(str_id < m_strings.size());

         if (str_id > 0)
         {
            std::ostringstream buffer;
            buffer << "\"" << m_strings[str_id].c_str() << "\"";

            rvalue->expression_data.expression = buffer.str();
            rvalue->expression_data.is_pushi_imm = false;
         }
         break;
      }

      iter++;
   }
}

void CodeGraph::find_control_structs(const graph_iterator& start,
   const graph_iterator& stop)
{
   graph_iterator iter;

   // search for remaining opcodes
   for (iter = start; iter != stop; iter++)
   {
      // only examine opcodes that weren't processed yet
      if (iter->m_type != typeOpcode || iter->m_isProcessed)
         continue;

      Uint16 opcode = iter->opcode_data.opcode;

      // start for if, if-else, while and switch statement
      if (opcode == op_BEQ)
      {
         CodeGraphItem item = *iter;

         iter->m_isProcessed = true;

         // search expression before this opcode
         graph_iterator expr_iter = iter;

         while (expr_iter != start && (expr_iter->m_type != typeExpression || expr_iter->m_isProcessed))
            expr_iter--;

         UaAssert(expr_iter != start);
         UaAssert(expr_iter->m_type == typeExpression);

         // now examine jump target of BEQ opcode, label1
         graph_iterator label1_iter = FindPos(iter->opcode_data.jump_target_pos);

         graph_iterator op_iter = label1_iter;
         op_iter--;

         // check if an opcode precedes label1
         if (op_iter->m_type == typeOpcode && !op_iter->m_isProcessed)
         {
            // yes, opcode before label1
            // when we have BRA, it might be "if-else" or "while"
            if (is_opcode(op_iter, op_BRA))
            {
               // it's "while" when the BRA jumps up, else it's "if-else"
               if (op_iter->opcode_data.jump_target_pos < op_iter->m_pos)
               {
                  // "while" control statement
                  // check that the BRA target points to the expression found
                  //UaAssert(expr_iter->m_pos == op_iter->opcode_data.jump_target_pos);
                  if (expr_iter->m_pos != op_iter->opcode_data.jump_target_pos)
                  {
                     // the BRA target doesn't point to the expression used at
                     // the start of the while() loop. that indicates a for()
                     // statement which isn't parsed yet. the BRA opcode
                     // instead jumps to the for loop break condition check
                     // happens in conv 10
                     UaTrace("possible for() statement found on pos %04x\n",
                        expr_iter->m_pos);
                  }

                  graph_iterator label2_iter = FindPos(op_iter->opcode_data.jump_target_pos);

                  process_while(expr_iter, label1_iter);

                  // mark items as "done"
                  expr_iter->m_isProcessed = true;
               }
               else
               {
                  // "if-else", determine endif position
                  graph_iterator endif_iter =
                     FindPos(op_iter->opcode_data.jump_target_pos);

                  process_if_else(true, expr_iter, label1_iter, endif_iter);
               }

               // mark items as "done"
               op_iter->m_isProcessed = true;
               expr_iter->m_isProcessed = true;
            }
            else
               // when we have JMP, it is "switch"
               if (is_opcode(op_iter, op_JMP))
               {
                  // determine if we have a switch statement
                  if (is_switch_statement(expr_iter, op_iter))
                     process_switch(expr_iter, op_iter);
                  else
                     process_if_else(false, expr_iter, m_graph.end(), label1_iter);
               }
               else
                  // unknown opcode
                  UaAssert(false);
         }
         else
         {
            // no opcode, so it must be simple "if"
            process_if_else(false, expr_iter, m_graph.end(), label1_iter);
         }
      }
   }
}

void CodeGraph::process_if_else(bool with_else,
   const graph_iterator& expr_iter, graph_iterator else_iter,
   const graph_iterator& endif_iter)
{
   expr_iter->m_isProcessed = true;

   // if (expr) statement and opening brace
   {
      CodeGraphItem if_statement;

      // if statement
      if_statement.m_type = typeStatement;
      if_statement.m_pos = expr_iter->m_pos;

      std::ostringstream buffer;
      buffer << "if (" << expr_iter->expression_data.expression << ")";

      if_statement.statement_data.statement = buffer.str();

      m_graph.insert(expr_iter, if_statement);

      // opening brace
      if_statement.statement_data.statement = "{";
      if_statement.statement_data.indent_change_after = 1;

      m_graph.insert(expr_iter, if_statement);
   }

   // else statement
   if (with_else)
   {
      UaAssert(else_iter != m_graph.end())

         CodeGraphItem else_statement;

      else_statement.m_type = typeStatement;
      else_statement.m_pos = else_iter->m_pos;
      else_statement.statement_data.statement = "}";
      else_statement.statement_data.indent_change_before = -1;

      m_graph.insert(else_iter, else_statement);

      else_statement.statement_data.statement = "else";
      else_statement.statement_data.indent_change_before = 0;

      m_graph.insert(else_iter, else_statement);

      else_statement.statement_data.statement = "{";
      else_statement.statement_data.indent_change_after = 1;

      m_graph.insert(else_iter, else_statement);
   }

   // closing brace
   {
      CodeGraphItem endif_statement;

      endif_statement.m_type = typeStatement;
      endif_statement.m_pos = endif_iter->m_pos - 2;

      std::ostringstream buffer;
#ifdef HAVE_END_STMT_INFO
      buffer << "} // end if (" << expr_iter->expression_data.expression << ")";
#else
      buffer << "} // end if";
#endif

      endif_statement.statement_data.statement = buffer.str();
      endif_statement.statement_data.indent_change_before = -1;

      m_graph.insert(endif_iter, endif_statement);
   }
}

void CodeGraph::process_switch(
   graph_iterator expr_iter, graph_iterator jmp_iter)
{
   // first expression; find out switch variable
   std::string expr_base(expr_iter->expression_data.expression);
   std::string::size_type pos = expr_base.find(" == ");
   UaAssert(pos != std::string::npos);

   // switch statement
   {
      CodeGraphItem switch_statement;
      switch_statement.m_type = typeStatement;
      switch_statement.m_pos = expr_iter->m_pos;

      std::ostringstream buffer;
      buffer << "switch (" << expr_base.substr(0, pos) << ")";

      switch_statement.statement_data.statement = buffer.str();

      m_graph.insert(expr_iter, switch_statement);

      // opening brace
      switch_statement.statement_data.statement = "{";
      switch_statement.statement_data.indent_change_after = 1;

      m_graph.insert(expr_iter, switch_statement);
   }
   pos += 4;

   graph_iterator switch_end_iter =
      FindPos(jmp_iter->opcode_data.jump_target_pos);

   std::string expr(expr_base);

   // process as many case statements we can find
   do
   {
      // case statement
      {
         CodeGraphItem case_statement;
         case_statement.m_type = typeStatement;
         case_statement.m_pos = expr_iter->m_pos;

         std::ostringstream buffer;
         buffer << "case " << expr.substr(pos) << ":";

         case_statement.statement_data.statement = buffer.str();
         case_statement.statement_data.indent_change_before = -1;
         case_statement.statement_data.indent_change_after = 1;

         m_graph.insert(expr_iter, case_statement);
      }

      // break statement
      {
         CodeGraphItem break_statement;
         break_statement.m_type = typeStatement;
         break_statement.m_pos = jmp_iter->m_pos;

         break_statement.statement_data.statement = "break;";

         m_graph.insert(jmp_iter, break_statement);
      }

      expr_iter->m_isProcessed = true;
      jmp_iter->m_isProcessed = true;

      // determine new expression iterator
      expr_iter = jmp_iter;
      expr_iter++;

      // already at end of switch statement?
      if (expr_iter == switch_end_iter)
         break;

      // TODO debug code
      CodeGraphItem& expr_item = *expr_iter;
      // TODO fix this: switch with only one expression, conv 6
      if (expr_item.m_type != typeExpression)
         break;

      // determine next case value from expression
      UaAssert(expr_iter->m_type == typeExpression);
      expr = expr_iter->expression_data.expression;

      UaAssert(0 == expr.find(expr_base.substr(0, pos)));

      graph_iterator op_beq_iter = expr_iter;
      op_beq_iter++;

      // search next BEQ opcode
      while ((op_beq_iter->m_type != typeOpcode ||
         op_beq_iter->m_isProcessed) &&
         op_beq_iter != switch_end_iter)
         op_beq_iter++;

      UaAssert(op_beq_iter->m_type == typeOpcode);
      UaAssert(is_opcode(op_beq_iter, op_BEQ));

      op_beq_iter->m_isProcessed = true;

      Uint16 jmp_item_pos = op_beq_iter->opcode_data.jump_target_pos;
      jmp_iter = FindPos(jmp_item_pos);
      jmp_iter--;

      CodeGraphItem& jmp_item = *jmp_iter;

      // search opcode before item
      while (jmp_iter->m_type != typeOpcode &&
         jmp_iter->m_pos < jmp_item_pos)
      {
         jmp_iter--;
         CodeGraphItem& jmp_item = *jmp_iter;
      }

      UaAssert(jmp_iter->m_type == typeOpcode);
      UaAssert(is_opcode(jmp_iter, op_JMP));

   } while (expr_iter != switch_end_iter);


   // switch end statement
   {
      CodeGraphItem switch_statement;
      switch_statement.m_type = typeStatement;
      // use end switch iterator pos, only 2 opcodes before
      switch_statement.m_pos = switch_end_iter->m_pos - 2;

      std::ostringstream buffer;
#ifdef HAVE_END_STMT_INFO
      buffer << "} // end switch (" << expr.substr(0, pos - 4) << ")";
#else
      buffer << "} // end switch";
#endif

      switch_statement.statement_data.statement = buffer.str();
      switch_statement.statement_data.indent_change_before = -1;

      m_graph.insert(switch_end_iter, switch_statement);
   }
}

bool CodeGraph::is_switch_statement(graph_iterator expr_iter,
   graph_iterator jmp_iter)
{
   // check if expression contains "==" operator
   std::string expr_base(expr_iter->expression_data.expression);
   std::string::size_type pos = expr_base.find(" == ");

   // definitely no switch statement
   if (std::string::npos == pos)
      return false;

   // collect all expressions
   std::vector<std::string> allexpressions;

   graph_iterator switch_end_iter =
      FindPos(jmp_iter->opcode_data.jump_target_pos);

   do
   {
      // remember expression
      allexpressions.push_back(expr_iter->expression_data.expression);

      // determine new expression iterator
      expr_iter = jmp_iter;
      expr_iter++;

      // already at end of switch statement?
      if (expr_iter == switch_end_iter)
         break;

      // switch with only one expression?
      if (expr_iter->m_type != typeExpression)
         break;

      graph_iterator op_beq_iter = expr_iter;
      op_beq_iter++;

      // search next BEQ opcode
      while ((op_beq_iter->m_type != typeOpcode ||
         op_beq_iter->m_isProcessed) &&
         op_beq_iter != switch_end_iter)
         op_beq_iter++;

      // special case for uw1, conversation 7:
      if (op_beq_iter->m_type != typeOpcode)
         break;

      UaAssert(op_beq_iter->m_type == typeOpcode);
      UaAssert(is_opcode(op_beq_iter, op_BEQ));

      jmp_iter = FindPos(op_beq_iter->opcode_data.jump_target_pos);
      jmp_iter--;

      // special case for uw1, conversation 136, 144
      if (jmp_iter->m_type != typeOpcode || !is_opcode(jmp_iter, op_JMP))
         break;

      UaAssert(jmp_iter->m_type == typeOpcode);
      UaAssert(is_opcode(jmp_iter, op_JMP));

   } while (expr_iter != switch_end_iter);

   // only one expression?
   unsigned int max = allexpressions.size();
   if (max <= 1)
      return false;

   // compare all expressions for same pattern
   for (unsigned int i = 1; i < max; i++)
   {
      if (std::string::npos == allexpressions[i].find(" == "))
         return false;

      if (0 != allexpressions[i].find(expr_base.substr(0, pos)))
         return false;
   }

   return true;
}

void CodeGraph::process_while(
   const graph_iterator& expr_iter, const graph_iterator& while_end_iter)
{
   // while-statement
   CodeGraphItem while_statement;

   while_statement.m_type = typeStatement;
   while_statement.m_pos = expr_iter->m_pos;

   std::ostringstream buffer;
   buffer << "while (" << expr_iter->expression_data.expression << ")";

   while_statement.statement_data.statement = buffer.str();

   m_graph.insert(expr_iter, while_statement);

   // opening brace
   while_statement.statement_data.statement = "{";
   while_statement.statement_data.indent_change_after = 1;

   m_graph.insert(expr_iter, while_statement);

   // closing brace
   std::ostringstream buffer2;
#ifdef HAVE_END_STMT_INFO
   buffer2 << "} // end while (" << expr_iter->expression_data.expression << ")";
#else
   buffer2 << "} // end while";
#endif

   while_statement.statement_data.statement = buffer2.str();
   while_statement.statement_data.indent_change_before = -1;
   while_statement.statement_data.indent_change_after = 0;
   while_statement.m_pos = while_end_iter->m_pos - 2;

   m_graph.insert(while_end_iter, while_statement);
}

void CodeGraph::post_process_function(Conv::FuncInfo& info)
{
   // get function start
   graph_iterator func_start = FindPos(info.start);
   CodeGraphItem& item = *func_start;
   UaAssert(item.m_type == typeFuncStart);

   item.m_isProcessed = true;

   CodeGraphItem func_item;
   func_item.m_type = typeStatement;
   func_item.m_pos = item.m_pos;

   std::ostringstream buffer;

   // return type
   buffer << DataTypeToString(info.return_type) << " ";

   // function name
   buffer << item.m_labelName << "(";

   // parameters
   const std::vector<EDataType>& param_types = info.param_types;

   unsigned int max = param_types.size();
   for (unsigned int i = 0; i < max; i++)
   {
      buffer << DataTypeToString(param_types[i]) << " param" << i + 1;
      if (i < max - 1)
         buffer << ", ";
   }

   buffer << ")";
   buffer << " // referenced " << item.m_xrefCount << " times";

   func_item.statement_data.statement = buffer.str();

   m_graph.insert(func_start, func_item);

   func_item.m_labelName = "";

   // opening brace
   func_item.statement_data.statement = "{";
   func_item.statement_data.indent_change_after = 1;

   m_graph.insert(func_start, func_item);

   func_item.statement_data.indent_change_after = 0;

   // all locals and arrays
   CodeGraphItem locals_item;
   locals_item.m_type = typeStatement;
   locals_item.m_pos = item.m_pos;

   max = info.locals_types.size();
   for (unsigned int j = 0; j < max; j++)
   {
      EDataType type = info.locals_types[j];

      if (type != dataTypeUnknown)
      {
         std::ostringstream buffer;
         buffer << DataTypeToString(type) << " local_" << j << ";";

         locals_item.statement_data.statement = buffer.str();

         m_graph.insert(func_start, locals_item);
      }
   }

   // arrays
   max = info.array_info.size();
   for (unsigned int n = 0; n < max; n++)
   {
      ArrayInfo array_info = info.array_info[n];

      std::ostringstream buffer;

      UaAssert(array_info.local_start <= info.locals_types.size());

      buffer << DataTypeToString(info.locals_types[array_info.local_start]) <<
         " local_" << array_info.local_start <<
         "[" << array_info.array_size << "];";

      locals_item.statement_data.statement = buffer.str();

      m_graph.insert(func_start, locals_item);
   }

   // add function end
   graph_iterator func_end = func_start;
   while (func_end != m_graph.end() && func_end->m_type != typeFuncEnd)
      func_end++;
   UaAssert(func_end != m_graph.end() && func_end->m_type == typeFuncEnd);

   func_end->m_isProcessed = true;

   func_item.m_type = typeStatement;
   func_item.m_pos = func_end->m_pos;
   func_item.statement_data.statement = "}";
   func_item.statement_data.indent_change_before = -1;

   m_graph.insert(func_end, func_item);

   // empty line
   func_item.statement_data.statement = "";
   func_item.statement_data.indent_change_before = 0;

   m_graph.insert(func_end, func_item);
}

void CodeGraph::add_goto_jumps(const graph_iterator& start,
   const graph_iterator& stop)
{
   graph_iterator iter;

   // search for remaining opcodes
   for (iter = start; iter != stop; iter++)
   {
      // only examine opcodes that weren't processed yet
      if (iter->m_type != typeOpcode || iter->m_isProcessed)
         continue;

      if (iter->opcode_data.opcode == op_JMP)
      {
         iter->m_isProcessed = true;

         // insert goto statement
         CodeGraphItem goto_item;
         goto_item.m_type = typeStatement;
         goto_item.m_pos = iter->m_pos;

         std::ostringstream buffer;
         buffer << "goto " << iter->opcode_data.jump_target << ";";
         goto_item.statement_data.statement = buffer.str();

         m_graph.insert(iter, goto_item);

         graph_iterator target_iter =
            FindPos(iter->opcode_data.jump_target_pos);

         // check if a label was already inserted
         if (target_iter->m_type != typeStatement ||
            target_iter->statement_data.statement.find("label") != 0)
         {
            // insert goto target label
            CodeGraphItem label_item;
            label_item.m_type = typeStatement;
            label_item.m_pos = target_iter->m_pos;

            std::ostringstream buffer2;
            buffer2 << iter->opcode_data.jump_target << ":;";
            label_item.statement_data.statement = buffer2.str();
            label_item.statement_data.indent_change_before = -1;
            label_item.statement_data.indent_change_after = 1;

            m_graph.insert(target_iter, label_item);
         }
      }
   }
}

CodeGraph::graph_iterator CodeGraph::FindPos(Uint16 target)
{
   graph_iterator iter, stop;
   iter = m_graph.begin();
   stop = m_graph.end();

   for (; iter != stop; iter++)
      if (iter->m_pos == target)
         return iter;

   // should not happen
   UaAssert(0);
   return stop;
}

bool CodeGraph::MatchOpcodePattern(graph_iterator iter, Uint16* pattern,
   unsigned int length) const
{
   for (unsigned int i = 0; i < length; i++, iter++)
      if (iter == m_graph.end() || iter->m_type != typeOpcode ||
         iter->opcode_data.opcode != pattern[i])
         return false;

   // all pattern bytes found, no end reached
   return true;
}

bool CodeGraph::is_opcode(graph_iterator iter, Uint16 opcode) const
{
   return (iter->m_type == typeOpcode &&
      iter->opcode_data.opcode == opcode);
}

void CodeGraph::PushiImmediateToGlobal(graph_iterator iter)
{
   UaAssert(iter->m_type == typeExpression);
   UaAssert(true == iter->expression_data.is_pushi_imm);
   UaAssert(false == iter->expression_data.is_address);

   // resolve to proper address
   Uint16 mem_idx = iter->expression_data.pushi_imm_value;

   iter->expression_data.expression = GetMemoryVarName(mem_idx);

   iter->expression_data.is_pushi_imm = false;
   iter->expression_data.pushi_imm_value = 0;
   iter->expression_data.is_address = true;
}

std::string CodeGraph::GetMemoryVarName(Uint16 mem_idx)
{
   UaAssert(mem_idx < nglobals);

   std::map<Uint16, Conv::ImportedItem>::iterator iter =
      m_mapImportedVariables.find(mem_idx);

   if (iter != m_mapImportedVariables.end())
   {
      return iter->second.name;
   }
   else
   {
      std::ostringstream buffer;
      buffer << "global_" << mem_idx;

      return buffer.str();
   }
}
