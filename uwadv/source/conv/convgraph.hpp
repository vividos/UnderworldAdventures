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
/*! \file convgraph.hpp

   \brief conversation graph

*/
//! \ingroup conv

//@{

// include guard
#ifndef uwadv_convgraph_hpp_
#define uwadv_convgraph_hpp_

// needed includes
#include <vector>
#include <list>
#include <deque>
#include <set>
#include "codevm.hpp"


// enums

//! conversation graph item type
enum ua_conv_graph_item_type
{
   gt_opcode,        //!< simple assembler opcode
   gt_func_start,    //!< function start
   gt_func_end,      //!< function end
   gt_operator,      //!< operator that combines one or more expressions and may yield an expression
   gt_statement,     //!< complete statement that returns no value
   gt_expression,    //!< expression that yields exactly one value
};


// structs

//! conversation graph item
struct ua_conv_graph_item
{
   //! ctor
   ua_conv_graph_item() :itemtype(gt_opcode), pos(0), xref_count(0),
      is_processed(false){}

   //! type of this item
   ua_conv_graph_item_type itemtype;

   //! position in code segment
   Uint16 pos;

   //! label name, if any
   std::string label_name;

   //! number of times the label is jumped to/branched/called/used
   unsigned int xref_count;

   //! indicates if item is already processed
   bool is_processed;

   //! data for the gt_opcode itemtype
   struct ua_opcode_data
   {
      ua_opcode_data():opcode(0), arg(0){}

      //! opcode values
      Uint16 opcode, arg;

      //! jump target for branches and jumps
      std::string jump_target;

      Uint16 jump_target_pos;

   } opcode_data;

   //! data for the gt_statement itemtype
   struct ua_statement_data
   {
      //! ctor
      ua_statement_data():indent_change(0){}

      //! statement text
      std::string statement;

      //! suggested indent change
      int indent_change;

   } statement_data;

   //! data for the gt_operator itemtype
   struct ua_operator_data
   {
      //! ctor
      ua_operator_data():sto_swap_args(false), returned_type(ua_dt_void),
         op_opcode(0), op_arg(0), prec_level(0xff){}

      //! operator opcode and argument
      Uint16 op_opcode, op_arg;

      //! number of expression this operator consumes
      Uint16 needed_expr;
      //! indicates if operator returns an expression
      bool returns_expr;

      //! return type of expression, if any
      ua_conv_datatype returned_type;

      //! swap expressions on STO opcode?
      bool sto_swap_args;

      //! operator precedence level
      unsigned int prec_level;

   } operator_data;

   //! data for the gt_expression itemtype
   struct ua_expression_data
   {
      //! ctor
      ua_expression_data():is_pushi_imm(false), prec_level(0xff){}

      //! expression text
      std::string expression;

      //! indicates if expression is an address to a memory location or a value
      bool is_address;

      //! indicates if expression comes from an PUSHI opcode
      bool is_pushi_imm;

      //! immediate value for PUSHI opcodes
      Uint16 pushi_imm_value;

      //! expression precedence level
      unsigned int prec_level;

   } expression_data;
};

//! infos about local arrays in functions
struct ua_conv_func_array_info
{
   //! number of local variable with array start
   Uint16 local_start;

   //! size of array
   unsigned int array_size;
};


//! infos about a specific function
struct ua_conv_func_info
{
   //! ctor
   ua_conv_func_info()
      :return_type(ua_dt_void), start(0){}

   //! start in code, when no intrisic
   Uint16 start;

   //! return type
   ua_conv_datatype return_type;

   //! list of parameter types
   std::vector<ua_conv_datatype> param_types;

   //! list of local variable types
   std::vector<ua_conv_datatype> locals_types;

   //! array info
   std::vector<ua_conv_func_array_info> array_info;
};


// classes

//! conversation graph
class ua_conv_graph
{
public:
   //! list of graph items
   typedef std::list<ua_conv_graph_item> graph_list;
   //! graph item iterator
   typedef graph_list::iterator graph_iterator;

   //! const graph item iterator
   typedef graph_list::const_iterator const_graph_iterator;

   //! map with all function infos, key is function name
   typedef std::map<std::string, ua_conv_func_info> function_map;

public:
   //! ctor
   ua_conv_graph(){}

   //! initializes conversation graph
   void init(std::vector<Uint16> code,
      Uint16 codestart, Uint16 codeend,
      std::vector<std::string>& strings,
      std::map<Uint16,ua_conv_imported_item>& imported_funcs,
      std::map<Uint16,ua_conv_imported_item>& imported_vars, Uint16 glob_reserved);

   //! only disassembles conversation code
   void disassemble();

   //! decompiles conversation code
   void decompile();

   //! returns graph
   inline const graph_list& get_graph() const;

   //! formats text for a graph item suitable to output
   void format_graph_item(std::string& item_text,
      const ua_conv_graph_item& item) const;

   //! formats text for an opcode graph item
   void format_opcode(std::string& opcode_text,
      const ua_conv_graph_item& item) const;

protected:
   // worker methods

   //! collect all call, branch and jump references
   void collect_xrefs();

   //! searches function starts and ends
   void search_functions();

   //! fixes call and jump targets
   void fix_call_jump_targets();

   //! analyzes function for expressions and operators
   void analyze_function(ua_conv_func_info& info);

   //! combines operators with needed expressions
   void combine_operators(const graph_iterator& start,
      const graph_iterator& stop);

   //! find out control structs
   void find_control_structs(const graph_iterator& start,
      const graph_iterator& stop);

   //! adds if and if-else statements
   void process_if_else(bool with_else, const graph_iterator& expr_iter,
      graph_iterator else_iter, const graph_iterator& endif_iter);

   //! adds switch-case statements
   void process_switch(graph_iterator expr_iter, graph_iterator jmp_iter);

   //! adds while statement
   void process_while(const graph_iterator& expr_iter,
      const graph_iterator& while_end_iter);

   //! does function post processing
   void post_process_function(ua_conv_func_info& info);


   // helper functions

   //! finds pos in graph of first item with target == item.pos
   graph_iterator find_pos(Uint16 target);

   bool match_opcode_pattern(const graph_iterator iter, Uint16* pattern,
      unsigned int length);

   //! returns true when item at iterator is given (unprocessed) opcode
   bool is_opcode(graph_iterator iter, Uint16 opcode) const;

   //! converts expression with immediate value from PUSHI to global variable
   void pushi_immediate_to_global(graph_iterator iter);

   //! returns memory variable name from memory index
   std::string get_memory_varname(Uint16 mem_idx);

protected:
   //! conversation graph
   graph_list graph;

   //! all functions
   function_map functions;

   //! queue of functions to analyze
   std::deque<ua_conv_func_info> analysis_queue;

   //! all conversation strings
   std::vector<std::string> strings;

   //! map with all intrinsic functions
   std::map<Uint16,ua_conv_imported_item> imported_funcs;

   //! map with all imported variables
   std::map<Uint16,ua_conv_imported_item> imported_vars;

   //! set with processed functions
   std::set<Uint16> processed_funcs;

   //! number of globals on stack before imported vars
   Uint16 nglobals;
};


// inline functions

const ua_conv_graph::graph_list& ua_conv_graph::get_graph() const
{
   return graph;
}

inline const char* ua_type_to_str(ua_conv_datatype type)
{
   switch(type)
   {
   case ua_dt_void: return "void";
   case ua_dt_int: return "int";
   case ua_dt_string: return "string";
   default:
      return "unknown";
   }
}


#endif
//@}
