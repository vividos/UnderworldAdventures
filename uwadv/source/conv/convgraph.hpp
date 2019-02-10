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
/// \file convgraph.hpp
/// \brief conversation graph
//
#pragma once

#include <vector>
#include <list>
#include <deque>
#include <set>
#include "codevm.hpp"

namespace Conv
{
   /// code graph item type
   enum GraphItemType
   {
      typeOpcode = 0,   ///< assembler opcode
      typeFuncStart,    ///< function start
      typeFuncEnd,      ///< function end
      typeOperator,      ///< operator that combines one or more expressions and may yield an expression
      typeStatement,     ///< complete statement that returns no value
      typeExpression,    ///< expression that yields exactly one value
   };

   /// code graph item
   struct CodeGraphItem
   {
      /// ctor
      CodeGraphItem()
         :m_type(typeOpcode),
         m_pos(0),
         m_xrefCount(0),
         m_isProcessed(false)
      {
      }

      /// type of this item
      GraphItemType m_type;

      /// position in code segment
      Uint16 m_pos;

      /// label name, if any
      std::string m_labelName;

      /// number of times the label is jumped to/branched/called/used
      unsigned int m_xrefCount;

      /// indicates if item is already processed
      bool m_isProcessed;

      /// formats item suitable for display
      std::string Format() const;

      /// formats opcode (item must have type typeOpcode)
      std::string FormatOpcode() const;

      /// data for the typeOpcode itemtype
      struct OpcodeData
      {
         /// ctor
         OpcodeData()
            :opcode(0),
            arg(0),
            jump_target_pos(0)
         {
         }

         /// opcode values
         Uint16 opcode, arg;

         /// jump target for branches, jumps and calls
         std::string jump_target;

         /// absolute position of jump target
         Uint16 jump_target_pos;
      };

      /// data for the typeStatement graph item
      struct StatementData
      {
         /// ctor
         StatementData()
            :indent_change_before(0),
            indent_change_after(0)
         {
         }

         /// statement text
         std::string statement;

         /// suggested indent change before this line
         int indent_change_before;

         /// suggested indent change after this line
         int indent_change_after;
      };

      /// data for the typeOperator itemtype
      struct OperatorData
      {
         /// ctor
         OperatorData()
            :op_opcode(0),
            op_arg(0),
            needed_expr(0),
            returns_expr(false),
            returned_type(dataTypeVoid),
            sto_swap_args(false),
            prec_level(0xff)
         {
         }

         /// operator opcode and argument
         Uint16 op_opcode, op_arg;

         /// number of expression this operator consumes
         Uint16 needed_expr;

         /// indicates if operator returns an expression
         bool returns_expr;

         /// return type of expression, if any
         EDataType returned_type;

         /// swap expressions on STO opcode?
         bool sto_swap_args;

         /// operator precedence level
         unsigned int prec_level;
      };

      /// data for the typeExpression graph item
      struct ExpressionData
      {
         /// ctor
         ExpressionData()
            :is_address(false),
            is_pushi_imm(false),
            pushi_imm_value(0),
            prec_level(0xff)
         {
         }

         /// expression text
         std::string expression;

         /// indicates if expression is an address to a memory location or a value
         bool is_address;

         /// indicates if expression comes from an PUSHI opcode
         bool is_pushi_imm;

         /// immediate value for PUSHI opcodes
         Uint16 pushi_imm_value;

         /// expression precedence level
         unsigned int prec_level;
      };

      OpcodeData opcode_data;
      StatementData statement_data;
      OperatorData operator_data;
      ExpressionData expression_data;
   };

   /// infos about a local array in a function
   struct ArrayInfo
   {
      /// number of local variable with array start
      Uint16 local_start;

      /// size of array
      unsigned int array_size;
   };

   /// infos about a function
   struct FuncInfo
   {
      /// ctor
      FuncInfo()
         :start(0),
         end(0),
         return_type(dataTypeVoid)
      {
      }

      /// function name
      std::string name;

      /// start in code, when no intrisic
      Uint16 start;

      /// start of func end in code
      Uint16 end;

      /// return type
      EDataType return_type;

      /// list of parameter types
      std::vector<EDataType> param_types;

      /// list of local variable types
      std::vector<EDataType> locals_types;

      /// array info
      std::vector<ArrayInfo> array_info;

      /// all caller functions
      std::set<std::string> caller;
   };

   /// conversation graph
   class CodeGraph
   {
   public:
      /// list of graph items
      typedef std::list<CodeGraphItem> GraphList;

      /// graph item iterator
      typedef GraphList::iterator graph_iterator;

      /// const graph item iterator
      typedef GraphList::const_iterator const_graph_iterator;

      /// map with all function infos, key is function name
      typedef std::map<std::string, FuncInfo> FunctionMap;

   public:
      /// ctor
      CodeGraph(
         std::vector<Uint16> code,
         Uint16 codestart,
         Uint16 codeend,
         std::vector<std::string>& strings,
         std::map<Uint16, ImportedItem>& mapImportedFunctions,
         std::map<Uint16, ImportedItem>& mapImportedVariables,
         Uint16 reservedGlobals);

      /// returns graph
      const GraphList& GetGraph() const { return m_graph; }

      /// returns function map
      const FunctionMap& GetFunctionMap() const { return m_functionMap; }

      /// only disassembles conversation code
      void Disassemble();

      /// decompiles conversation code
      void Decompile();

   private:
   /// initializes graph with opcodes
   void InitGraph(const std::vector<Uint16>& code,
      Uint16 codestart, Uint16 codeend);

      /// collect all call, branch and jump references
      void CollectXrefs();

      /// finds function starts and ends
      void FindFunctions();

      /// fixes call and jump targets
      void fix_call_jump_targets();

      /// analyzes function for expressions and operators
      void AnalyzeFunction(FuncInfo& info);

      /// adds array info to function
      void add_array_info(FuncInfo& info, Uint16 local_idx,
         Uint16 offset);

      /// combines operators with needed expressions
      void combine_operators(const graph_iterator& start,
         const graph_iterator& stop);

      /// checks if babl_menu or babl_fmenu follows STO operation
      void CheckBablMenu(graph_iterator& start, const graph_iterator& stop,
         graph_iterator& lvalue, graph_iterator& rvalue);

      /// find out control structs
      void find_control_structs(const graph_iterator& start,
         const graph_iterator& stop);

      /// adds if and if-else statements
      void process_if_else(bool with_else, const graph_iterator& expr_iter,
         graph_iterator else_iter, const graph_iterator& endif_iter);

      /// adds switch-case statements
      void process_switch(graph_iterator expr_iter, graph_iterator jmp_iter);

      /// determines if the expressions matches structure for a switch statement
      bool is_switch_statement(graph_iterator expr_iter, graph_iterator op_iter);

      /// adds while statement
      void process_while(const graph_iterator& expr_iter,
         const graph_iterator& while_end_iter);

      /// does function post processing
      void post_process_function(FuncInfo& info);

      /// adds goto jumps
      void add_goto_jumps(const graph_iterator& start,
         const graph_iterator& stop);


      // helper functions

      void format_graph_item(std::string& item_text,
         const CodeGraphItem& item) const;

      /// formats text for an opcode graph item
      void format_opcode(std::string& opcode_text,
         const CodeGraphItem& item) const;

      /// finds pos in graph of first item with target == item.pos
      graph_iterator FindPos(Uint16 target);

   /// checks if next following opcodes match a given pattern
      bool MatchOpcodePattern(const graph_iterator iter, Uint16* pattern,
         unsigned int length) const;

      /// returns true when item at iterator is given (unprocessed) opcode
      bool is_opcode(graph_iterator iter, Uint16 opcode) const;

      /// converts expression with immediate value from PUSHI to global variable
      void PushiImmediateToGlobal(graph_iterator iter);

      /// returns memory variable name from memory index
      std::string GetMemoryVarName(Uint16 mem_idx);

      /// converts data type to string
      static const char* DataTypeToString(EDataType type);

   private:
      /// conversation graph
      GraphList m_graph;

      /// function map
      FunctionMap m_functionMap;

      /// queue of functions to analyze
      std::deque<FuncInfo> m_analysis_queue;

      /// set with processed functions
      std::set<Uint16> m_processed_funcs;

      /// number of globals on stack before imported vars
      Uint16 nglobals;

      /// all conversation strings
      std::vector<std::string> m_strings;

      /// map with all intrinsic functions
      std::map<Uint16, ImportedItem> m_mapImportedFunctions;

      /// map with all imported variables
      std::map<Uint16, ImportedItem> m_mapImportedVariables;
   };

} // namespace Conv
