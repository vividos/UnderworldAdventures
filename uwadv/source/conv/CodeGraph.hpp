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
/// \file CodeGraph.hpp
/// \brief conversation graph
//
#pragma once

#include <vector>
#include <list>
#include <deque>
#include <set>
#include "CodeVM.hpp"

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

      /// data for the typeOpcode graph item
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

      /// data for the typeOperator graph item
      struct OperatorData
      {
         /// ctor
         OperatorData()
            :op_opcode(0),
            op_arg(0),
            numNeededExpressions(0),
            returns_expr(false),
            returned_type(dataTypeVoid),
            sto_swap_args(false),
            prec_level(0xff)
         {
         }

         /// operator opcode and argument
         Uint16 op_opcode, op_arg;

         /// number of expression this operator consumes
         Uint16 numNeededExpressions;

         /// indicates if operator returns an expression
         bool returns_expr;

         /// return type of expression, if any
         DataType returned_type;

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
            :isAddress(false),
            is_pushi_imm(false),
            pushi_imm_value(0),
            prec_level(0xff)
         {
         }

         /// expression text
         std::string expression;

         /// indicates if expression is an address to a memory location or a value
         bool isAddress;

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
      DataType return_type;

      /// list of parameter types
      std::vector<DataType> param_types;

      /// list of local variable types
      std::vector<DataType> locals_types;

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
         Uint16 codeStart, Uint16 codeEnd);

      /// collect all call, branch and jump references
      void CollectXrefs();

      /// finds function starts and ends
      void FindFunctions();

      /// finds function entry point code
      bool FindFunctionEntryPoint(graph_iterator& iter, graph_iterator stop, FuncInfo& funcInfo);

      /// finds function exit code
      bool FindFunctionExitPoint(graph_iterator& iter, graph_iterator stop, FuncInfo& funcInfo);

      /// updates CALL opcode jump targets
      void UpdateCallJumpTargets();

      /// analyze all functions in the analysis queue
      void ProcessFunctionQueue();

      /// analyzes function
      void AnalyzeFunction(FuncInfo& funcInfo);

      // operator / expression analysis functions

      /// adds operator for given opcode item
      CodeGraphItem& AddOperator(graph_iterator& iter,
         Uint16 numNeededExpressions, bool returnsExpression, DataType returnedType);

      /// adds statement for given opcode item
      CodeGraphItem& AddStatement(graph_iterator iter, std::string text, bool setProcessed = true);

      /// adds a generic expression
      CodeGraphItem& AddExpression(graph_iterator& iter, std::string text, bool isAddress);

      /// adds expression for local array access
      void AddLocalArrayExpression(graph_iterator iter, FuncInfo& funcInfo);

      /// adds array access info for given function
      void AddArrayInfo(FuncInfo& info, Uint16 localIndex, Uint16 offset);

      /// adds call operator
      void AddCallOperator(graph_iterator& iter, graph_iterator stop,
         const FuncInfo& funcInfo, bool isIntrinsic);

      /// adds assignment operator
      void AddAssignmentOperator(graph_iterator iter);

      /// adds expression for address-of local var
      void AddAddressOfLocalVarExpression(graph_iterator& iter, graph_iterator stop, FuncInfo& funcInfo);

      /// adds immediate expression (rvalue)
      void AddImmediateExpression(graph_iterator& iter, graph_iterator stop);

      /// combines operators with expressions
      void CombineOperators(FuncInfo& funcInfo);

      /// collects all expressions for an operator
      void CollectExpressions(graph_iterator start, graph_iterator end,
         unsigned int numNeededExpressions, std::vector<graph_iterator>& expressions, bool& isStatementBetween);

      /// combines operator with found expressions
      void CombineOperatorAndExpressions(FuncInfo& funcInfo,
         graph_iterator operatorIter, graph_iterator stop,
         graph_iterator insertIter, std::vector<graph_iterator>& expressions);

      /// combines call operator with its parameters
      void CombineCallOperator(FuncInfo& funcInfo, graph_iterator operatorIter,
         graph_iterator insertIter, std::vector<graph_iterator>& expressions);

      /// replaces a local variable expression with the actual value, to siplify function calls
      void ReplaceLocalExpressionWithValue(FuncInfo& funcInfo, graph_iterator& expressionIter);

      /// combines return operator with expression
      void CombineReturnExpression(graph_iterator insertIter, std::vector<graph_iterator>& expressions);

      /// combines an lvalue expression and dereferences it, creating a new expression
      void CombineDereferenceExpression(graph_iterator insertIter, std::vector<graph_iterator>& expressions);

      /// combines array and index expressions
      void CombineArrayExpression(graph_iterator insertIter, std::vector<graph_iterator>& expressions);

      /// combines arithmetical or logical operator expressions
      void AddBinaryExpression(graph_iterator operatorIter,
         graph_iterator insertIter, std::vector<graph_iterator>& expressions);

      /// combines unary arithmetical or logical operator with expression
      void AddUnaryExpression(graph_iterator operatorIter,
         graph_iterator insertIter, std::vector<graph_iterator>& expressions, std::string prefix);

      /// combines say operator with expression
      void CombineSayOp(graph_iterator insertIter, std::vector<graph_iterator>& expressions);

      /// combines assignment expression (lvalue and rvalue)
      void CombineAssignmentExpressions(graph_iterator operatorIter, graph_iterator stop,
         graph_iterator insertIter, std::vector<graph_iterator>& expressions);

      /// checks if an assignment expression may contain string assignment for babl_menu and babl_fmenu
      void CheckBablMenu(graph_iterator& start,
         const graph_iterator& stop, graph_iterator& lvalue, graph_iterator& rvalue);

      /// replaces an integer expression with the corresponding string from the string block
      void ReplaceIntExpressionWithString(graph_iterator& intExpression);

      // statement analysis functions

      /// finds switch-case structures
      void FindSwitchCase(FuncInfo& funcInfo);

      /// adds next switch case statements
      bool FindAndAddNextSwitchCase(graph_iterator& expressionIter, graph_iterator stop, Uint16& switchEnd, bool& switchAdded);

      /// finds while statements
      void FindWhile(FuncInfo& funcInfo);

      /// finds do-while statements
      void FindDoWhile(FuncInfo& funcInfo);

      /// finds if and if-else statements
      void FindIfElse(FuncInfo& funcInfo);

      /// adds goto and labels for remaining jumps
      void AddGotoJumps(FuncInfo& funcInfo);

      // post processing functions

      /// post processes function
      void PostProcessFunction(FuncInfo& funcInfo);

      /// marks functions unused, with only callers that are unused themselves
      void MarkFunctionsUnused();

      // helper functions

      /// finds first iterator for code position, or end() iterator
      graph_iterator FindPos(Uint16 target);

      /// checks if a given iter points to an opcode item and if it contains given opcode
      bool IsOpcode(const_graph_iterator iter, Uint16 opcode) const;

      /// finds opcode pattern in given range
      graph_iterator FindOpcodePattern(graph_iterator iter, graph_iterator stop,
         const Uint16* pattern, unsigned int length);

      /// checks if next following opcodes match a given pattern
      bool MatchOpcodePattern(const_graph_iterator iter, const_graph_iterator stop,
         const Uint16* pattern, unsigned int length) const;

      /// sets following N opcodes to "processed"
      void SetOpcodesProcessed(graph_iterator iter, graph_iterator stop, unsigned int numOpcodes);

      /// returns variable name of a global or imported variable
      std::string GetMemoryVarName(Uint16 memoryIndex) const;

      /// conver immediate value to global
      void PushiImmediateToGlobal(graph_iterator iter);

      /// converts data type to string
      static const char* DataTypeToString(DataType type);

      /// gets function name for function at given pos
      static std::string GetFunctionName(Uint16 pos);

   private:
      /// conversation graph
      GraphList m_graph;

      /// function map
      FunctionMap m_functionMap;

      /// queue of functions to analyze
      std::deque<std::string> m_analysisQueue;

      /// set with processed functions
      std::set<Uint16> m_processedFunctions;

      // set with all code position ranges where a potential "continue"
      // statement be located in
      std::set<std::tuple<Uint16, Uint16>> m_continuePositions;

      /// number of globals on stack before imported vars
      Uint16 m_numGlobals;

      /// all conversation strings
      std::vector<std::string> m_strings;

      /// map with all intrinsic functions
      std::map<Uint16, ImportedItem> m_mapImportedFunctions;

      /// map with all imported variables
      std::map<Uint16, ImportedItem> m_mapImportedVariables;
   };

} // namespace Conv
