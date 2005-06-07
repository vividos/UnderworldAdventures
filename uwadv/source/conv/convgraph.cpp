/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file convgraph.cpp

   \brief conversation graph

*/

// needed includes
#include "common.hpp"
#include "convgraph.hpp"
#include "opcodes.hpp"
#include <sstream>
#include <iomanip>


// ua_conv_graph methods

void ua_conv_graph::init(std::vector<Uint16> code,
   Uint16 codestart, Uint16 codeend,
   std::vector<std::string>& mystrings,
   std::map<Uint16,ua_conv_imported_item>& myimported_funcs,
   std::map<Uint16,ua_conv_imported_item>& myimported_vars,
   Uint16 myglob_reserved)
{
   strings.clear();
   strings = mystrings;

   imported_funcs.clear();
   imported_funcs = myimported_funcs;

   imported_vars.clear();
   imported_vars = myimported_vars;

   processed_funcs.clear();

   nglobals = myglob_reserved;

   // convert code segment to graph
   for(Uint16 i=codestart; i<codeend; i++)
   {
      // fill item struct
      ua_conv_graph_item item;
      item.pos = i;
      item.opcode_data.opcode = code[i];

      if (item.opcode_data.opcode <= op_last &&
          ua_conv_instructions[item.opcode_data.opcode].args != 0)
         item.opcode_data.arg = code[++i];
      else
         item.opcode_data.arg = 0;

      // add it to graph
      graph.push_back(item);
   }
}

void ua_conv_graph::disassemble()
{
   collect_xrefs();
}

void ua_conv_graph::decompile()
{
   collect_xrefs();
   search_functions();
   fix_call_jump_targets();

   // initially add start() as function
   {
      ua_conv_func_info start_func;
      start_func.return_type = ua_dt_void;
      start_func.start = 0;

      analysis_queue.push_back(start_func);
   }

   // analyze all functions in the analysis queue
   // the queue may grow while analyzing a function
   while(analysis_queue.size() > 0)
   {
      ua_conv_func_info info = analysis_queue.front();
      analysis_queue.pop_front();

      graph_iterator iter = find_pos(info.start);

      ua_assert(0 != iter->label_name.size())

      // check if function was already analyzed
      if (functions.find(iter->label_name) == functions.end())
      {
//         ua_trace("analyzing function \"%s\" at %04x\n", iter->label_name.c_str(), info.start);

         analyze_function(info);

         post_process_function(info);

         // add to known functions
         functions.insert(std::make_pair<std::string, ua_conv_func_info>(iter->label_name, info));
      }
   }
}

void ua_conv_graph::format_graph_item(std::string& item_text,
   const ua_conv_graph_item& item) const
{
   switch(item.itemtype)
   {
   case gt_func_start:
      item_text = "gt_func_start";
      break;

   case gt_func_end:
      item_text = "gt_func_end";
      break;

   case gt_opcode:
      format_opcode(item_text, item);
      break;

   case gt_statement:
      item_text = item.statement_data.statement;
      break;

   case gt_operator:
      {
         std::ostringstream buffer;
         buffer <<
            (item.is_processed ? "   // " : "") <<
            "operator " <<
            ua_conv_instructions[item.operator_data.op_opcode].mnemonic <<
            ", needs " << item.operator_data.needed_expr << " expressions, yields " <<
            (item.operator_data.returns_expr ? "an" : "no") << " expression; level=" <<
            item.operator_data.prec_level;

         item_text = buffer.str();
      }
      break;

   case gt_expression:
      {
         std::ostringstream buffer;
         buffer <<
            (item.is_processed ? "   // " : "") <<
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

void ua_conv_graph::format_opcode(std::string& opcode_text,
   const ua_conv_graph_item& item) const
{
   ua_assert(item.itemtype == gt_opcode);

   std::ostringstream buffer;
   buffer << "   ";

   if (item.is_processed)
      buffer << "// ";

   // code segment address
   buffer << std::setfill('0') << std::setw(4) <<
      std::setbase(16) << item.pos << " ";

   if (item.opcode_data.opcode>op_last)
   {
      // unknown opcode
      buffer << "??? (0x" << std::setfill('0') <<
         std::setw(4) << std::setbase(16) << item.opcode_data.opcode << ")";
   }
   else
   {
      Uint16 opcode = item.opcode_data.opcode;

      buffer << ua_conv_instructions[opcode].mnemonic;

      if (ua_conv_instructions[item.opcode_data.opcode].args > 0 &&
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
         ua_assert(imported_funcs.find(ifunc) != imported_funcs.end());
         
         buffer << " " << imported_funcs.find(ifunc)->second.name;
      }
      else
      if (ua_conv_instructions[item.opcode_data.opcode].args > 0)
      {
         // unknown, not resolved by collect_xrefs, or PUSH, PUSHI_EFF
         // just format the string
         buffer << " 0x"<< std::setfill('0') <<
            std::setw(4) << std::setbase(16) << item.opcode_data.arg;
      }
   }

   opcode_text = buffer.str();
}

void ua_conv_graph::collect_xrefs()
{
   ua_assert(graph.size()>0);

   // start function
   graph.front().label_name = "start";
   graph.front().xref_count = 1;

   graph_iterator iter,stop;
   iter=graph.begin();
   stop=graph.end();

   for(;iter!=stop; iter++)
   {
      ua_conv_graph_item& item = *iter;
      if (item.itemtype != gt_opcode)
         continue;
      Uint16 opcode = item.opcode_data.opcode;

      if (opcode == op_JMP || opcode == op_BEQ || opcode == op_BNE ||
          opcode == op_BRA || opcode == op_CALL)
      {
         // calculate jump target
         Uint16 target = item.opcode_data.arg;

         // adjust for relative jumps
         if (opcode == op_BEQ || opcode == op_BNE || opcode == op_BRA)
            target = static_cast<Uint16>((static_cast<Uint32>(target) + static_cast<Uint32>(item.pos + 1)) & 0xFFFF);

         // find target position
         graph_iterator target_pos = find_pos(target);
         ua_assert(target_pos != graph.end());

         ua_conv_graph_item& target_item = *target_pos;

         if (target_item.xref_count == 0)
         {
            // not a target yet, generate label name from pos
            std::ostringstream buffer;
            buffer << "label_" << std::setfill('0') <<
               std::setw(4) << std::setbase(16) << target_item.pos;
            target_item.label_name = buffer.str();
         }

         // copy over label name and pos
         item.opcode_data.jump_target = target_item.label_name;
         item.opcode_data.jump_target_pos = target_item.pos;

         // increase xref count
         target_item.xref_count++;
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
void ua_conv_graph::search_functions()
{
   graph_iterator iter,stop;
   iter = graph.begin();
   stop = graph.end();

   // go through code and search for functions
   for(;iter!=stop; iter++)
   {
      // check for function entry code
      Uint16 pattern_func_start[4] =
      { op_PUSHBP, op_SPTOBP, op_PUSHI, op_ADDSP };

      if (match_opcode_pattern(iter, pattern_func_start, 4))
      {
         // found entry code

         // test if function started with START opcode
         bool func_start = false;
         if (iter != graph.begin())
         {
            graph_iterator iter_start = iter;
            iter_start--;

            func_start = iter_start->itemtype == gt_opcode &&
               iter_start->opcode_data.opcode == op_START;

            if (func_start)
               --iter;
         }

         // set up new function start item
         ua_conv_graph_item& item = *iter;

         ua_conv_graph_item func_item;
         func_item.itemtype = gt_func_start;
         func_item.pos = item.pos;
         func_item.xref_count = item.xref_count;

         // generate function name from label, if applicable
         if (item.label_name.size()==0 && item.xref_count==0)
         {
            // unused func
            std::ostringstream buffer;
            buffer << "unused_" << std::setfill('0') <<
               std::setw(4) << std::setbase(16) << item.pos;
            item.label_name = buffer.str();
         }
         else
         if (item.label_name.find("label_") == 0)
         {
            // label to func
            item.label_name.replace(0,6,"func_");
         }

         func_item.label_name = item.label_name;
         item.label_name = "";
         item.xref_count = 0;

         graph.insert(iter,func_item); // insert before iter

         // advance iterator

         // started with START opcode?
         if (func_start)
         {
            iter->is_processed = true; iter++; // jump over START
         }
         iter->is_processed = true; iter++; // PUSHBP
         iter->is_processed = true; iter++; // SPTOBP

         // find out number of local variables
         //func_item.function_data.locals_size = iter->opcode_data.arg;

         iter->is_processed = true; iter++; // PUSHI
         iter->is_processed = true;         // ADDSP
      }

      // check for function exit code
      Uint16 pattern_func_end[3] = { op_BPTOSP, op_POPBP, op_RET };

      if (match_opcode_pattern(iter, pattern_func_end, 3))
      {
         // set up new function end item
         ua_conv_graph_item& item = *iter;

         ua_conv_graph_item func_item;
         func_item.itemtype = gt_func_end;
         func_item.pos = item.pos;

         graph.insert(iter,func_item);

         iter->is_processed = true; iter++; // BPTOSP
         iter->is_processed = true; iter++; // POPBP
         iter->is_processed = true;         // RET
      }
   }
}

void ua_conv_graph::fix_call_jump_targets()
{
   graph_iterator iter,stop;
   iter = graph.begin();
   stop = graph.end();

   // go through code and search for functions
   for(;iter!=stop; iter++)
   {
      // as a "service" we correct jump targets for opcodes here
      if (iter->itemtype == gt_opcode && iter->opcode_data.opcode == op_CALL)
      {
         graph_iterator target_iter = find_pos(iter->opcode_data.arg);
         iter->opcode_data.jump_target = target_iter->label_name;
      }
   }
}

void ua_conv_graph::analyze_function(ua_conv_func_info& info)
{
   // search start and end of function
   graph_iterator iter,start,stop;
   start = stop = find_pos(info.start);

   // search first opcode
   while(start != graph.end() && start->itemtype != gt_opcode)
      start++;

   stop = start;

   // search function end
   while(stop != graph.end() && stop->itemtype != gt_func_end)
      stop++;

   // go through the function and replace with expressions and operations
   // expressions are incomplete values on the stack
   // operations are complete control statements that may consume zero or more
   // expressions
   for(iter=start; iter!=stop; iter++)
   {
      // only examine opcodes that weren't processed yet
      if (iter->itemtype != gt_opcode || iter->is_processed)
         continue;

      Uint16 pattern_local_array[3] = { op_PUSHI, op_PUSHI_EFF, op_OFFSET };
      Uint16 pattern_return_stmt[2] = { op_SAVE_REG, op_POP };

      ua_conv_graph_item& item = *iter;
      Uint16 opcode = item.opcode_data.opcode;

      // check for some simple control structures
      if (opcode == op_SAY_OP)
      {
         // say op
         item.is_processed = true;

         // consumes 1 expression
         ua_conv_graph_item oper_item;
         oper_item.itemtype = gt_operator;
         oper_item.pos = iter->pos;
         oper_item.operator_data.op_opcode = opcode;
         oper_item.operator_data.needed_expr = 1;
         oper_item.operator_data.returns_expr = false;
         oper_item.operator_data.prec_level =
            ua_conv_instructions[opcode].op_prec_level;

         // insert just before this opcode
         graph.insert(iter,oper_item);
      }
      else
      if (opcode == op_EXIT_OP)
      {
         // exit statement
         item.is_processed = true;

         ua_conv_graph_item oper_item;
         oper_item.itemtype = gt_statement;
         oper_item.pos = iter->pos;
         oper_item.statement_data.statement = "exit;";

         graph.insert(iter,oper_item);
      }
      else
      if (opcode == op_CALLI)
      {
         // intrinsic call
         item.is_processed = true;
         Uint16 intrisic_no = item.opcode_data.arg;

         --iter; // previous PUSHI opcode tells number of arguments

         ua_assert(true == is_opcode(iter,op_PUSHI));

         // set PUSHI opcode to processed
         iter->is_processed = true;

         ua_conv_graph_item oper_item;
         oper_item.itemtype = gt_operator;
         oper_item.pos = iter->pos;

         // note: this number may be wrong, e.g. for babl_menu
         //oper_item.control_data.needed_expr = iter->opcode_data.arg;

         // find out number of function args by counting following POP opcodes
         Uint16 arguments = 0;
         graph_iterator pop_iter = iter; pop_iter++; pop_iter++;
         while(pop_iter != stop && is_opcode(pop_iter, op_POP))
         {
            pop_iter->is_processed = true;
            arguments++;
            pop_iter++;
         }

         oper_item.operator_data.op_opcode = opcode;
         oper_item.operator_data.op_arg = intrisic_no;
         // note: CALLI functions have 2 parameter, but the 1st is the number
         // of arguments, so subtract 1 from this number again
         oper_item.operator_data.needed_expr = arguments-1;

         oper_item.operator_data.returns_expr = false;

         // check if a PUSH_REG follows
         if (is_opcode(pop_iter,op_PUSH_REG))
         {
            pop_iter->is_processed = true;
            oper_item.operator_data.returns_expr = true;
         }

         oper_item.operator_data.prec_level =
            ua_conv_instructions[opcode].op_prec_level;

         // insert just before this opcode
         graph.insert(iter,oper_item);

         ++iter;
      }
      else
      if (opcode == op_CALL)
      {
         // local function call
         Uint16 call_target = item.opcode_data.arg;
         item.is_processed = true;

         ua_conv_graph_item oper_item;
         oper_item.itemtype = gt_operator;
         oper_item.pos = iter->pos;

         // find out number of function args by counting following POP opcodes
         Uint16 arguments = 0;
         graph_iterator pop_iter = iter; pop_iter++;
         while(pop_iter != stop && is_opcode(pop_iter, op_POP))
         {
            pop_iter->is_processed = true;
            arguments++;
            pop_iter++;
         }

         oper_item.operator_data.op_opcode = opcode;
         oper_item.operator_data.op_arg = call_target;
         oper_item.operator_data.needed_expr = arguments;

         oper_item.operator_data.returns_expr = false;

         // check if a PUSH_REG follows
         if (is_opcode(pop_iter,op_PUSH_REG))
         {
            pop_iter->is_processed = true;
            oper_item.operator_data.returns_expr = true;
         }

         oper_item.operator_data.prec_level =
            ua_conv_instructions[opcode].op_prec_level;

         // insert just before this opcode
         graph.insert(iter,oper_item);

         // function call
         ua_conv_func_info new_info;
         new_info.start = item.opcode_data.arg;
         new_info.return_type = ua_dt_void; // type is void until known better
         // note: info.param_types is unknown for now

         // is recursive call?
         if (new_info.start == info.start)
            ua_trace("discovered recursive function call at %04x\n", item.pos);
         else
         {
            // put in analysis queue, when not already processed
            if (processed_funcs.find(new_info.start) == processed_funcs.end())
            {
               processed_funcs.insert(new_info.start);
               analysis_queue.push_back(new_info);
            }
         }
      }
      else
      // compare/arithmetic/logical operators
      if (opcode == op_TSTEQ || opcode == op_TSTGT || opcode == op_TSTGE ||
          opcode == op_TSTLT || opcode == op_TSTLE || opcode == op_TSTNE ||
          opcode == op_OPOR  || opcode == op_OPAND || opcode == op_OPADD ||
          opcode == op_OPSUB || opcode == op_OPMUL || opcode == op_OPDIV ||
          opcode == op_OPMOD )
      {
         // binary operator
         item.is_processed = true;

         ua_conv_graph_item oper_item;
         oper_item.itemtype = gt_operator;
         oper_item.pos = iter->pos;
         oper_item.operator_data.op_opcode = opcode;
         oper_item.operator_data.needed_expr = 2;
         oper_item.operator_data.returns_expr = true;
         oper_item.operator_data.returned_type = ua_dt_int;
         oper_item.operator_data.prec_level =
            ua_conv_instructions[opcode].op_prec_level;

         graph.insert(iter,oper_item);
      }
      else
      // unary negate
      if (opcode == op_OPNEG)
      {
         // unary operator
         item.is_processed = true;

         ua_conv_graph_item oper_item;
         oper_item.itemtype = gt_operator;
         oper_item.pos = iter->pos;
         oper_item.operator_data.op_opcode = opcode;
         oper_item.operator_data.needed_expr = 1;
         oper_item.operator_data.returns_expr = true;
         oper_item.operator_data.returned_type = ua_dt_int;
         oper_item.operator_data.prec_level =
            ua_conv_instructions[opcode].op_prec_level;

         graph.insert(iter,oper_item);
      }
      else
      // logical not
      if (opcode == op_OPNOT)
      {
         // unary operator
         item.is_processed = true;

         ua_conv_graph_item oper_item;
         oper_item.itemtype = gt_operator;
         oper_item.pos = iter->pos;
         oper_item.operator_data.op_opcode = opcode;
         oper_item.operator_data.needed_expr = 1;
         oper_item.operator_data.returns_expr = true;
         oper_item.operator_data.returned_type = ua_dt_int;
         oper_item.operator_data.prec_level =
            ua_conv_instructions[opcode].op_prec_level;

         graph.insert(iter,oper_item);
      }
      else 
      // assignment operator
      if (opcode == op_STO)
      {
         item.is_processed = true;

         ua_conv_graph_item oper_item;
         oper_item.itemtype = gt_operator;
         oper_item.pos = iter->pos;
         oper_item.operator_data.op_opcode = opcode;
         oper_item.operator_data.needed_expr = 2;
         oper_item.operator_data.returns_expr = false;
         oper_item.operator_data.returned_type = ua_dt_void;
         oper_item.operator_data.prec_level =
            ua_conv_instructions[opcode].op_prec_level;

         // check if a SWAP precedes the STO opcode
         graph_iterator swap_iter = iter;
         ua_assert(iter != graph.begin());
         swap_iter--; 

         if (is_opcode(swap_iter, op_SWAP))
         {
            oper_item.operator_data.sto_swap_args = true;

            swap_iter->is_processed = true;
         }

         graph.insert(iter,oper_item);
      }
      else
      // local array
      if (match_opcode_pattern(iter, pattern_local_array,
         SDL_TABLESIZE(pattern_local_array)))
      {
         // array offset
         graph_iterator arr_iter = iter;
         Uint16 offset = arr_iter->opcode_data.arg;
         arr_iter->is_processed = true;

         // TODO arrays with offset == 0 really allowed?
         ua_assert(offset <= 0x7fff && offset >= 0);
         arr_iter++;

         // local var index
         Uint16 local_idx = arr_iter->opcode_data.arg;
         arr_iter->is_processed = true;
         arr_iter++;

         // offset
         arr_iter->is_processed = true;

         // set up expression item
         ua_conv_graph_item local_array_item;
         local_array_item.itemtype = gt_expression;
         local_array_item.pos = iter->pos;
         local_array_item.expression_data.is_address = true;

         std::ostringstream buffer;
         buffer << "local_" << local_idx <<
            "[" << offset << "]";

         local_array_item.expression_data.expression = buffer.str();

         // add to info.array_info
         add_array_info(info, local_idx, offset);

         graph.insert(iter,local_array_item);
         iter++; iter++;
      }
      else
      // address-of local var
      if (opcode == op_PUSHI_EFF)
      {
         // local var index
         Uint16 local_idx = iter->opcode_data.arg;
         iter->is_processed = true;

         // set up expression item
         ua_conv_graph_item local_var_item;
         local_var_item.itemtype = gt_expression;
         local_var_item.pos = iter->pos;
         local_var_item.expression_data.is_address = true;

         // check if a FETCHM follows; then it's not the address of the
         // local var but the value
         graph_iterator fetchm_iter = iter; fetchm_iter++;
         ua_assert(fetchm_iter != graph.end());
         if (is_opcode(fetchm_iter, op_FETCHM))
         {
            fetchm_iter->is_processed = true;
            local_var_item.expression_data.is_address = false;
         }

         std::ostringstream buffer;
         if (local_idx > 0x7fff)
         {
            unsigned int param_num = (-(Sint16)local_idx)-1;

            // param value
            buffer << "param" << param_num;

            // this expression is an address, since param values are always
            // passed by reference
            local_var_item.expression_data.is_address = true;

            // set param info
            if (param_num > info.param_types.size())
               info.param_types.resize(param_num, ua_dt_int);
         }
         else
         {
            buffer << "local_" << local_idx;

            // add local variable info
            if (local_idx >= info.locals_types.size())
               info.locals_types.resize(local_idx+1, ua_dt_unknown);

            ua_assert(local_idx < info.locals_types.size());

            info.locals_types[local_idx] = ua_dt_int;
         }

         local_var_item.expression_data.expression = buffer.str();

         graph.insert(iter,local_var_item);
      }
      else
      // immediate value
      if (opcode == op_PUSHI)
      {
         // check if CALLI follows; if yes, don't process this PUSHI
         graph_iterator next_iter = iter; next_iter++;
         if (!is_opcode(next_iter, op_CALLI))
         {
            iter->is_processed = true;

            // set up expression item
            ua_conv_graph_item immediate_item;
            immediate_item.itemtype = gt_expression;
            immediate_item.pos = iter->pos;
            immediate_item.expression_data.is_address = false;

            // check if a FETCHM follows; then it's not an immediate value,
            // but the value of a global var
            graph_iterator fetchm_iter = iter; fetchm_iter++;
            ua_assert(fetchm_iter != graph.end());

            if (is_opcode(fetchm_iter, op_FETCHM))
            {
               fetchm_iter->is_processed = true;

               // global or private variable value
               Uint16 var_idx = iter->opcode_data.arg;

               immediate_item.expression_data.expression = get_memory_varname(var_idx);
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

            graph.insert(iter,immediate_item);
         }
      }
      else
      // return-statement
      if (match_opcode_pattern(iter, pattern_return_stmt,
         SDL_TABLESIZE(pattern_return_stmt)))
      {
         // returns value from function
         iter->is_processed = true;

         graph_iterator next_iter = iter; next_iter++;
         ua_assert(next_iter != graph.begin());
         next_iter->is_processed = true;

         // set up expression item
         ua_conv_graph_item return_item;
         return_item.itemtype = gt_operator;
         return_item.pos = iter->pos;
         return_item.operator_data.op_opcode = opcode;
         return_item.operator_data.needed_expr = 1;
         return_item.operator_data.returns_expr = false;
         return_item.operator_data.returned_type = ua_dt_int;
         return_item.operator_data.prec_level =
            ua_conv_instructions[opcode].op_prec_level;

         graph.insert(iter,return_item);

         // remember return type
         info.return_type = ua_dt_int;
      }
      // dereference-operator
      if (opcode == op_FETCHM)
      {
         // turns an address-of variable to value-of variable
         iter->is_processed = true;

         // set up expression item
         ua_conv_graph_item deref_item;
         deref_item.itemtype = gt_operator;
         deref_item.pos = iter->pos;
         deref_item.operator_data.op_opcode = opcode;
         deref_item.operator_data.needed_expr = 1;
         deref_item.operator_data.returns_expr = true;
         deref_item.operator_data.returned_type = ua_dt_int;
         deref_item.operator_data.prec_level =
            ua_conv_instructions[opcode].op_prec_level;

         graph.insert(iter,deref_item);
      }
      else
      // array-index-operator
      if (opcode == op_OFFSET)
      {
         // indexes an array
         iter->is_processed = true;

         // set up expression item
         ua_conv_graph_item arr_idx_item;
         arr_idx_item.itemtype = gt_operator;
         arr_idx_item.pos = iter->pos;
         arr_idx_item.operator_data.op_opcode = opcode;
         arr_idx_item.operator_data.needed_expr = 2;
         arr_idx_item.operator_data.returns_expr = true;
         arr_idx_item.operator_data.returned_type = ua_dt_int;
         arr_idx_item.operator_data.prec_level =
            ua_conv_instructions[opcode].op_prec_level;

         graph.insert(iter,arr_idx_item);
      }
   }

   combine_operators(start,stop);
   find_control_structs(start,stop);
   add_goto_jumps(start,stop);
}

void ua_conv_graph::add_array_info(ua_conv_func_info& info, Uint16 local_idx, Uint16 offset)
{
   unsigned int max = info.array_info.size();
   for(unsigned int i=0; i<max; i++)
   {
      if (info.array_info[i].local_start == local_idx)
      {
         // found info; update array size
         ua_conv_func_array_info& array_info = info.array_info[i];
         array_info.array_size = ua_max(array_info.array_size,offset);

         return;
      }      
   }

   // new array
   ua_conv_func_array_info array_info;
   array_info.local_start = local_idx;
   array_info.array_size = offset;

   info.array_info.push_back(array_info);
}

/*!
    search operators and try to build new expressions until no operators are
    left
*/
void ua_conv_graph::combine_operators(/*ua_conv_func_info& info*/
   const graph_iterator& start, const graph_iterator& stop)
{
   while(true)
   {
      // search next operator
      graph_iterator iter, last_oper = graph.end();
      for(iter=start; iter!=stop; iter++)
      {
         if (iter->itemtype == gt_operator && !iter->is_processed)
         {
            // found operator
            last_oper = iter;
            break;
         }
      }

      if (last_oper == graph.end())
         break; // no more operators

      // combine found operator with expressions to form new expression
      graph_iterator oper = last_oper;

      bool statement_between = false;

      // search n previous expressions
      std::vector<graph_iterator> expressions;

      Uint16 min_expr_pos = oper->pos;

      iter = oper;
      while(iter != start && expressions.size() < oper->operator_data.needed_expr)
      {
         if (iter->itemtype == gt_expression && !iter->is_processed)
         {
            // found an expression
            expressions.push_back(iter);

            // remember expression with minimum position
            if (iter->pos < min_expr_pos)
               min_expr_pos = iter->pos;
         }
         if (iter->itemtype == gt_statement)
            statement_between = true;
         iter--;
      }

      ua_assert(iter != start);
      ua_assert(expressions.size() == oper->operator_data.needed_expr);

      // set up new item; for now we assume it's another expression
      ua_conv_graph_item new_expression;
      new_expression.pos = min_expr_pos;

      // now that we have n expressions, we can combine it with the operator
      switch(oper->operator_data.op_opcode)
      {
      case op_CALL:
      case op_CALLI:
         {
            std::ostringstream buffer;

            if (oper->operator_data.op_opcode == op_CALLI)
            {
               Uint16 val = oper->operator_data.op_arg;
               ua_assert(imported_funcs.find(val) != imported_funcs.end());

               buffer << imported_funcs.find(val)->second.name;
            }
            else
               buffer << "func_" << std::setfill('0') <<
                  std::setw(4) << std::setbase(16) << oper->operator_data.op_arg;
            buffer << "(";

            // do parameter
            unsigned int max=expressions.size();
            for(unsigned int n=0; n<max; n++)
            {
               ua_conv_graph_item& param = *expressions[n];

               // check if lvalue is from PUSHI opcode
               if (param.expression_data.is_pushi_imm)
                  pushi_immediate_to_global(expressions[n]);

               // param must be address, since all params are passed by reference/pointer
               ua_assert(true == param.expression_data.is_address);

               buffer << param.expression_data.expression;

               if (n < max-1)
                  buffer << ", ";
            }

            // since a statement appeared between this operator and the
            // expressions used, insert the new expression on the place the
            // operator is located; the reason for this is because uw1 conv.
            // code can use temporary local variables that are initialized and
            // used as parameter for function calls
            if (statement_between)
               new_expression.pos = oper->pos;

            // decide if function call returns data
            if (oper->operator_data.returns_expr)
            {
               buffer << ")";

               new_expression.itemtype = gt_expression;
               new_expression.expression_data.expression = buffer.str();
               new_expression.expression_data.is_address = false;
            }
            else
            {
               buffer << ");";

               new_expression.itemtype = gt_statement;
               new_expression.statement_data.statement = buffer.str();
            }
         }
         break;

      case op_SAVE_REG:
         {
            ua_assert(expressions.size() == 1);
            ua_assert(false == expressions[0]->expression_data.is_address)

            new_expression.itemtype = gt_statement;

            std::ostringstream buffer;
            buffer << "return " <<
               expressions[0]->expression_data.expression << ";";

            new_expression.statement_data.statement = buffer.str();
         }
         break;

      case op_FETCHM:
         {
            ua_assert(expressions.size() == 1);
            ua_assert(true == expressions[0]->expression_data.is_address);

            new_expression.itemtype = gt_expression;
            new_expression.expression_data = expressions[0]->expression_data;
            new_expression.expression_data.is_address = false;
         }
         break;

      case op_OFFSET:
         {
            ua_assert(expressions.size() == 2);

            // check if expression 0 is from PUSHI opcode
            if (expressions[0]->expression_data.is_pushi_imm)
               pushi_immediate_to_global(expressions[0]);

            ua_assert(true == expressions[0]->expression_data.is_address);
            ua_assert(false == expressions[1]->expression_data.is_address);

            new_expression.itemtype = gt_expression;
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
            ua_assert(expressions.size() == 2);

            new_expression.itemtype = gt_expression;

            graph_iterator rvalue, lvalue;
            rvalue = expressions[0];
            lvalue = expressions[1];

            unsigned int precedence = oper->operator_data.prec_level;

            ua_conv_graph_item& value = *lvalue;

            // check that both lvalue and rvalue are value-of
            ua_assert(false == lvalue->expression_data.is_address);
            ua_assert(false == rvalue->expression_data.is_address);

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
            buffer << " " << ua_conv_instructions[oper->operator_data.op_opcode].operator_text << " ";
            
            // rvalue
            if (precedence > rvalue->expression_data.prec_level)
               buffer << "(" << rvalue->expression_data.expression << ")";
            else
               buffer << rvalue->expression_data.expression;

            new_expression.expression_data.expression = buffer.str();
            new_expression.expression_data.is_address = false;

            // the new expression has the lowest precedence of all used expressions
            unsigned int new_prec = ua_min(expressions[0]->expression_data.prec_level,
               expressions[1]->expression_data.prec_level);
            new_prec = ua_min(new_prec,precedence);

            new_expression.expression_data.prec_level = new_prec;
         }
         break;

      case op_OPNEG:
         {
            ua_assert(expressions.size() == 1);
            ua_assert(false == expressions[0]->expression_data.is_address);

            new_expression.itemtype = gt_expression;
            new_expression.expression_data = expressions[0]->expression_data;

            std::string newtext("-");
            newtext += new_expression.expression_data.expression;

            new_expression.expression_data.expression = newtext;
         }
         break;

      case op_OPNOT:
         {
            ua_assert(expressions.size() == 1);
            ua_assert(false == expressions[0]->expression_data.is_address);

            new_expression.itemtype = gt_expression;
            new_expression.expression_data = expressions[0]->expression_data;

            std::string newtext("!");
            newtext += new_expression.expression_data.expression;

            new_expression.expression_data.expression = newtext;
         }
         break;

      case op_SAY_OP:
         {
            ua_assert(expressions.size() == 1);

            // no, we have a statement
            new_expression.itemtype = gt_statement;

            // SAY with PUSHI before?
            ua_assert(true == expressions[0]->expression_data.is_pushi_imm);

            Uint16 string_id = expressions[0]->expression_data.pushi_imm_value;
            ua_assert(string_id < strings.size());

            // replace linefeeds in text
            std::string outtext(strings[string_id]);

            std::string::size_type pos = 0;
            while(std::string::npos != (pos = outtext.find('\n')))
               outtext.replace(pos,1,"\\n");

            std::ostringstream buffer;
            buffer << "say(\"" << outtext << "\");";
            new_expression.statement_data.statement = buffer.str();
         }
         break;

      case op_STO:
         {
            ua_assert(expressions.size() == 2);

            new_expression.itemtype = gt_statement;

            graph_iterator rvalue, lvalue;
            rvalue = expressions[0];
            lvalue = expressions[1];

            // swap iterators if SWAP opcode was found
            if (oper->operator_data.sto_swap_args)
               std::swap(rvalue,lvalue);

            // check if lvalue is from PUSHI opcode
            if (lvalue->expression_data.is_pushi_imm)
               pushi_immediate_to_global(lvalue);

            // check if assignment is for call to babl_menu or babl_fmenu
            check_babl_menu(oper, stop, lvalue, rvalue);

            // check that lvalue is address-of and rvalue is value-of
            ua_assert(true == lvalue->expression_data.is_address);
            ua_assert(false == rvalue->expression_data.is_address);

            std::ostringstream buffer;
            buffer << lvalue->expression_data.expression << " = " <<
               rvalue->expression_data.expression << ";";

            new_expression.statement_data.statement = buffer.str();

            // also relocate store expression; see op_CALL/op_CALLI part for
            // further comments
            if (statement_between)
               new_expression.pos = expressions[0]->pos;
         }
         break;

      default:
         // opcode appeared that wasn't processed
         ua_assert(false);
         break;
      }

      // mark operator as "done"
      oper->is_processed = true;

      // mark expressions as "done"
      unsigned int max = expressions.size();
      for(unsigned int i=0; i<max; i++)
         expressions[i]->is_processed = true;

      // insert new expression before first expression used for this new one
      graph_iterator insert_iter = find_pos(new_expression.pos);
      ua_assert(insert_iter != graph.end());

      graph.insert(insert_iter,new_expression);
   }
}

void ua_conv_graph::check_babl_menu(graph_iterator& start,
   const graph_iterator& stop, graph_iterator& lvalue, graph_iterator& rvalue)
{
   ua_assert(rvalue->itemtype == gt_expression);
   ua_assert(lvalue->itemtype == gt_expression);

   if (!rvalue->expression_data.is_pushi_imm ||
       lvalue->expression_data.expression.find("[") == std::string::npos)
      return;

   graph_iterator iter = start;
   while(iter != stop && start != graph.end())
   {
      // search for babl_menu and babl_fmenu intrinsic calls
      if (iter->itemtype == gt_opcode &&
          iter->opcode_data.opcode == op_CALLI)
      {
         std::string intr_name(
            imported_funcs.find(iter->opcode_data.arg)->second.name);

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

         if (str_id >= strings.size())
            break;

         ua_assert(str_id < strings.size());

         if (str_id > 0)
         {
            std::ostringstream buffer;
            buffer << "\"" << strings[str_id].c_str() << "\"";

            rvalue->expression_data.expression = buffer.str();
            rvalue->expression_data.is_pushi_imm = false;
         }
         break;
      }

      iter++;
   }
}

void ua_conv_graph::find_control_structs(const graph_iterator& start,
   const graph_iterator& stop)
{
   graph_iterator iter;

   // search for remaining opcodes
   for(iter=start; iter!=stop; iter++)
   {
      // only examine opcodes that weren't processed yet
      if (iter->itemtype != gt_opcode || iter->is_processed)
         continue;

      Uint16 opcode = iter->opcode_data.opcode;

      // start for if, if-else, while and switch statement
      if (opcode == op_BEQ)
      {
         ua_conv_graph_item item = *iter;

         iter->is_processed = true;

         // search expression before this opcode
         graph_iterator expr_iter = iter;

         while(expr_iter != start && (expr_iter->itemtype != gt_expression || expr_iter->is_processed))
            expr_iter--;

         ua_assert(expr_iter != start);
         ua_assert(expr_iter->itemtype == gt_expression);

         // now examine jump target of BEQ opcode, label1
         graph_iterator label1_iter = find_pos(iter->opcode_data.jump_target_pos);

         graph_iterator op_iter = label1_iter;
         op_iter--;

         // check if an opcode precedes label1
         if (op_iter->itemtype == gt_opcode && !op_iter->is_processed)
         {
            // yes, opcode before label1
            // when we have BRA, it might be "if-else" or "while"
            if (is_opcode(op_iter, op_BRA))
            {
               // it's "while" when the BRA jumps up, else it's "if-else"
               if (op_iter->opcode_data.jump_target_pos < op_iter->pos)
               {
                  // "while" control statement
                  // check that the BRA target points to the expression found
                  //ua_assert(expr_iter->pos == op_iter->opcode_data.jump_target_pos);
                  if (expr_iter->pos != op_iter->opcode_data.jump_target_pos)
                  {
                     // the BRA target doesn't point to the expression used at
                     // the start of the while() loop. that indicates a for()
                     // statement which isn't parsed yet. the BRA opcode
                     // instead jumps to the for loop break condition check
                     // happens in conv 10
                     ua_trace("possible for() statement found on pos %04x\n",
                        expr_iter->pos);
                  }

                  graph_iterator label2_iter = find_pos(op_iter->opcode_data.jump_target_pos);

                  process_while(expr_iter,label1_iter);

                  // mark items as "done"
                  expr_iter->is_processed = true;
               }
               else
               {
                  // "if-else", determine endif position
                  graph_iterator endif_iter =
                     find_pos(op_iter->opcode_data.jump_target_pos);

                  process_if_else(true,expr_iter, label1_iter, endif_iter);
               }

               // mark items as "done"
               op_iter->is_processed = true;
               expr_iter->is_processed = true;
            }
            else
            // when we have JMP, it is "switch"
            if (is_opcode(op_iter, op_JMP))
            {
               // determine if we have a switch statement
               if (is_switch_statement(expr_iter, op_iter))
                  process_switch(expr_iter, op_iter);
               else
                  process_if_else(false, expr_iter, graph.end(), label1_iter);
            }
            else
            // unknown opcode
               ua_assert(false);
         }
         else
         {
            // no opcode, so it must be simple "if"
            process_if_else(false,expr_iter, graph.end(), label1_iter);
         }
      }
   }
}

void ua_conv_graph::process_if_else(bool with_else,
   const graph_iterator& expr_iter, graph_iterator else_iter,
   const graph_iterator& endif_iter)
{
   expr_iter->is_processed = true;

   // if (expr) statement and opening brace
   {
      ua_conv_graph_item if_statement;

      // if statement
      if_statement.itemtype = gt_statement;
      if_statement.pos = expr_iter->pos;

      std::ostringstream buffer;
      buffer << "if (" << expr_iter->expression_data.expression << ")";

      if_statement.statement_data.statement = buffer.str();

      graph.insert(expr_iter, if_statement);

      // opening brace
      if_statement.statement_data.statement = "{";
      if_statement.statement_data.indent_change_after = 1;

      graph.insert(expr_iter, if_statement);
   }

   // else statement
   if (with_else)
   {
      ua_assert(else_iter != graph.end())

      ua_conv_graph_item else_statement;

      else_statement.itemtype = gt_statement;
      else_statement.pos = else_iter->pos;
      else_statement.statement_data.statement = "}";
      else_statement.statement_data.indent_change_before = -1;

      graph.insert(else_iter, else_statement);

      else_statement.statement_data.statement = "else";
      else_statement.statement_data.indent_change_before = 0;

      graph.insert(else_iter, else_statement);

      else_statement.statement_data.statement = "{";
      else_statement.statement_data.indent_change_after = 1;

      graph.insert(else_iter, else_statement);
   }

   // closing brace
   {
      ua_conv_graph_item endif_statement;

      endif_statement.itemtype = gt_statement;
      endif_statement.pos = endif_iter->pos-2;

      std::ostringstream buffer;
#ifdef HAVE_END_STMT_INFO
      buffer << "} // end if (" << expr_iter->expression_data.expression << ")";
#else
      buffer << "} // end if";
#endif

      endif_statement.statement_data.statement = buffer.str();
      endif_statement.statement_data.indent_change_before = -1;

      graph.insert(endif_iter, endif_statement);
   }
}

void ua_conv_graph::process_switch(
   graph_iterator expr_iter, graph_iterator jmp_iter)
{
   // first expression; find out switch variable
   std::string expr_base(expr_iter->expression_data.expression);
   std::string::size_type pos = expr_base.find(" == ");
   ua_assert(pos != std::string::npos);

   // switch statement
   {
      ua_conv_graph_item switch_statement;
      switch_statement.itemtype = gt_statement;
      switch_statement.pos = expr_iter->pos;

      std::ostringstream buffer;
      buffer << "switch (" << expr_base.substr(0,pos) << ")";

      switch_statement.statement_data.statement = buffer.str();

      graph.insert(expr_iter, switch_statement);

      // opening brace
      switch_statement.statement_data.statement = "{";
      switch_statement.statement_data.indent_change_after = 1;

      graph.insert(expr_iter, switch_statement);
   }
   pos += 4;

   graph_iterator switch_end_iter =
      find_pos(jmp_iter->opcode_data.jump_target_pos);

   std::string expr(expr_base);

   // process as many case statements we can find
   do
   {
      // case statement
      {
         ua_conv_graph_item case_statement;
         case_statement.itemtype = gt_statement;
         case_statement.pos = expr_iter->pos;

         std::ostringstream buffer;
         buffer << "case " << expr.substr(pos) << ":";

         case_statement.statement_data.statement = buffer.str();
         case_statement.statement_data.indent_change_before = -1;
         case_statement.statement_data.indent_change_after = 1;

         graph.insert(expr_iter, case_statement);
      }

      // break statement
      {
         ua_conv_graph_item break_statement;
         break_statement.itemtype = gt_statement;
         break_statement.pos = jmp_iter->pos;

         break_statement.statement_data.statement = "break;";

         graph.insert(jmp_iter, break_statement);
      }

      expr_iter->is_processed = true;
      jmp_iter->is_processed = true;

      // determine new expression iterator
      expr_iter = jmp_iter;
      expr_iter++;

      // already at end of switch statement?
      if (expr_iter == switch_end_iter)
         break;

      // TODO debug code
      ua_conv_graph_item& expr_item = *expr_iter;
      // TODO fix this: switch with only one expression, conv 6
      if (expr_item.itemtype != gt_expression)
         break;

      // determine next case value from expression
      ua_assert(expr_iter->itemtype == gt_expression);
      expr = expr_iter->expression_data.expression;

      ua_assert(0 == expr.find(expr_base.substr(0,pos)));

      graph_iterator op_beq_iter = expr_iter;
      op_beq_iter++;

      // search next BEQ opcode
      while((op_beq_iter->itemtype != gt_opcode ||
             op_beq_iter->is_processed) &&
            op_beq_iter != switch_end_iter)
         op_beq_iter++;

      ua_assert(op_beq_iter->itemtype == gt_opcode);
      ua_assert(is_opcode(op_beq_iter, op_BEQ));

      op_beq_iter->is_processed = true;

      Uint16 jmp_item_pos = op_beq_iter->opcode_data.jump_target_pos;
      jmp_iter = find_pos(jmp_item_pos);
      jmp_iter--;

      ua_conv_graph_item& jmp_item = *jmp_iter;

      // search opcode before item
      while(jmp_iter->itemtype != gt_opcode &&
            jmp_iter->pos < jmp_item_pos)
      {
         jmp_iter--;
         ua_conv_graph_item& jmp_item = *jmp_iter;
      }

      ua_assert(jmp_iter->itemtype == gt_opcode);
      ua_assert(is_opcode(jmp_iter,op_JMP));

   } while(expr_iter != switch_end_iter);


   // switch end statement
   {
      ua_conv_graph_item switch_statement;
      switch_statement.itemtype = gt_statement;
      // use end switch iterator pos, only 2 opcodes before
      switch_statement.pos = switch_end_iter->pos-2;

      std::ostringstream buffer;
#ifdef HAVE_END_STMT_INFO
      buffer << "} // end switch (" << expr.substr(0,pos-4) << ")";
#else
      buffer << "} // end switch";
#endif

      switch_statement.statement_data.statement = buffer.str();
      switch_statement.statement_data.indent_change_before = -1;

      graph.insert(switch_end_iter, switch_statement);
   }
}

bool ua_conv_graph::is_switch_statement(graph_iterator expr_iter,
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
      find_pos(jmp_iter->opcode_data.jump_target_pos);

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
      if (expr_iter->itemtype != gt_expression)
         break;

      graph_iterator op_beq_iter = expr_iter;
      op_beq_iter++;

      // search next BEQ opcode
      while((op_beq_iter->itemtype != gt_opcode ||
             op_beq_iter->is_processed) &&
            op_beq_iter != switch_end_iter)
         op_beq_iter++;

      // special case for uw1, conversation 7:
      if (op_beq_iter->itemtype != gt_opcode)
         break;

      ua_assert(op_beq_iter->itemtype == gt_opcode);
      ua_assert(is_opcode(op_beq_iter, op_BEQ));

      jmp_iter = find_pos(op_beq_iter->opcode_data.jump_target_pos);
      jmp_iter--;

      // special case for uw1, conversation 136, 144
      if (jmp_iter->itemtype != gt_opcode || !is_opcode(jmp_iter,op_JMP))
         break;

      ua_assert(jmp_iter->itemtype == gt_opcode);
      ua_assert(is_opcode(jmp_iter,op_JMP));

   } while(expr_iter != switch_end_iter);

   // only one expression?
   unsigned int max = allexpressions.size();
   if (max <= 1)
      return false;

   // compare all expressions for same pattern
   for(unsigned int i=1; i<max; i++)
   {
      if (std::string::npos == allexpressions[i].find(" == "))
         return false;

      if (0 != allexpressions[i].find(expr_base.substr(0,pos)))
         return false;
   }

   return true;
}

void ua_conv_graph::process_while(
   const graph_iterator& expr_iter, const graph_iterator& while_end_iter)
{
   // while-statement
   ua_conv_graph_item while_statement;

   while_statement.itemtype = gt_statement;
   while_statement.pos = expr_iter->pos;

   std::ostringstream buffer;
   buffer << "while (" << expr_iter->expression_data.expression << ")";

   while_statement.statement_data.statement = buffer.str();

   graph.insert(expr_iter, while_statement);

   // opening brace
   while_statement.statement_data.statement = "{";
   while_statement.statement_data.indent_change_after = 1;

   graph.insert(expr_iter, while_statement);

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
   while_statement.pos = while_end_iter->pos-2;

   graph.insert(while_end_iter, while_statement);
}

void ua_conv_graph::post_process_function(ua_conv_func_info& info)
{
   // get function start
   graph_iterator func_start = find_pos(info.start);
   ua_conv_graph_item& item = *func_start;
   ua_assert(item.itemtype == gt_func_start);

   item.is_processed = true;

   ua_conv_graph_item func_item;
   func_item.itemtype = gt_statement;
   func_item.pos = item.pos;

   std::ostringstream buffer;

   // return type
   buffer << ua_type_to_str(info.return_type) << " ";

   // function name
   buffer << item.label_name << "(";

   // parameters
   const std::vector<ua_conv_datatype>& param_types = info.param_types;

   unsigned int max = param_types.size();
   for(unsigned int i=0; i<max; i++)
   {
      buffer << ua_type_to_str(param_types[i]) << " param" << i+1;
      if (i<max-1)
         buffer << ", ";
   }

   buffer << ")";
   buffer << " // referenced " << item.xref_count << " times";

   func_item.statement_data.statement = buffer.str();

   graph.insert(func_start,func_item);

   func_item.label_name = "";

   // opening brace
   func_item.statement_data.statement = "{";
   func_item.statement_data.indent_change_after = 1;

   graph.insert(func_start,func_item);

   func_item.statement_data.indent_change_after = 0;

   // all locals and arrays
   ua_conv_graph_item locals_item;
   locals_item.itemtype = gt_statement;
   locals_item.pos = item.pos;

   max = info.locals_types.size();
   for(unsigned int j=0; j<max; j++)
   {
      ua_conv_datatype type = info.locals_types[j];

      if (type != ua_dt_unknown)
      {
         std::ostringstream buffer;
         buffer << ua_type_to_str(type) << " local_" << j << ";";

         locals_item.statement_data.statement = buffer.str();

         graph.insert(func_start,locals_item);
      }
   }

   // arrays
   max = info.array_info.size();
   for(unsigned int n=0; n<max; n++)
   {
      ua_conv_func_array_info array_info = info.array_info[n];

      std::ostringstream buffer;

      ua_assert(array_info.local_start <= info.locals_types.size());

      buffer << ua_type_to_str(info.locals_types[array_info.local_start]) <<
         " local_" << array_info.local_start <<
         "[" << array_info.array_size << "];";

      locals_item.statement_data.statement = buffer.str();

      graph.insert(func_start,locals_item);
   }

   // add function end
   graph_iterator func_end = func_start;
   while(func_end != graph.end() && func_end->itemtype != gt_func_end)
      func_end++;
   ua_assert(func_end != graph.end() && func_end->itemtype == gt_func_end);

   func_end->is_processed = true;

   func_item.itemtype = gt_statement;
   func_item.pos = func_end->pos;
   func_item.statement_data.statement = "}";
   func_item.statement_data.indent_change_before = -1;

   graph.insert(func_end,func_item);

   // empty line
   func_item.statement_data.statement = "";
   func_item.statement_data.indent_change_before = 0;

   graph.insert(func_end,func_item);
}

void ua_conv_graph::add_goto_jumps(const graph_iterator& start,
   const graph_iterator& stop)
{
   graph_iterator iter;

   // search for remaining opcodes
   for(iter=start; iter!=stop; iter++)
   {
      // only examine opcodes that weren't processed yet
      if (iter->itemtype != gt_opcode || iter->is_processed)
         continue;

      if (iter->opcode_data.opcode == op_JMP)
      {
         iter->is_processed = true;

         // insert goto statement
         ua_conv_graph_item goto_item;
         goto_item.itemtype = gt_statement;
         goto_item.pos = iter->pos;

         std::ostringstream buffer;
         buffer << "goto " << iter->opcode_data.jump_target << ";";
         goto_item.statement_data.statement = buffer.str();

         graph.insert(iter,goto_item);

         graph_iterator target_iter =
            find_pos(iter->opcode_data.jump_target_pos);

         // check if a label was already inserted
         if (target_iter->itemtype != gt_statement ||
            target_iter->statement_data.statement.find("label") != 0)
         {
            // insert goto target label
            ua_conv_graph_item label_item;
            label_item.itemtype = gt_statement;
            label_item.pos = target_iter->pos;

            std::ostringstream buffer2;
            buffer2 << iter->opcode_data.jump_target << ":;";
            label_item.statement_data.statement = buffer2.str();
            label_item.statement_data.indent_change_before = -1;
            label_item.statement_data.indent_change_after = 1;

            graph.insert(target_iter,label_item);
         }
      }
   }
}

ua_conv_graph::graph_iterator ua_conv_graph::find_pos(Uint16 target)
{
   graph_iterator iter,stop;
   iter = graph.begin();
   stop = graph.end();

   for(;iter!=stop;iter++)
      if (iter->pos == target)
         return iter;

   // should not happen
   ua_assert(0);
   return stop;
}

bool ua_conv_graph::match_opcode_pattern(graph_iterator iter, Uint16* pattern,
   unsigned int length)
{
   for(unsigned int i=0; i<length; i++, iter++)
      if (iter == graph.end() || iter->itemtype != gt_opcode ||
          iter->opcode_data.opcode != pattern[i])
         return false;

   // all pattern bytes found, no end reached
   return true;
}

bool ua_conv_graph::is_opcode(graph_iterator iter, Uint16 opcode) const
{
   return (iter->itemtype == gt_opcode &&
      iter->opcode_data.opcode == opcode);
}

void ua_conv_graph::pushi_immediate_to_global(graph_iterator iter)
{
   ua_assert(iter->itemtype == gt_expression);
   ua_assert(true == iter->expression_data.is_pushi_imm);
   ua_assert(false == iter->expression_data.is_address);

   // resolve to proper address
   Uint16 mem_idx = iter->expression_data.pushi_imm_value;

   iter->expression_data.expression = get_memory_varname(mem_idx);

   iter->expression_data.is_pushi_imm = false;
   iter->expression_data.pushi_imm_value = 0;
   iter->expression_data.is_address = true;
}

std::string ua_conv_graph::get_memory_varname(Uint16 mem_idx)
{
   ua_assert(mem_idx < nglobals);

   std::map<Uint16,ua_conv_imported_item>::iterator iter =
      imported_vars.find(mem_idx);

   if (iter != imported_vars.end())
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
