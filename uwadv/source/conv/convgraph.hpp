/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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

// include guard
#ifndef __uwadv_convgraph_hpp_
#define __uwadv_convgraph_hpp_

// needed includes
#include <vector>
#include "codevm.hpp"


// enums

//! conversation graph item type
enum ua_conv_graph_item_type
{
   gt_decl,          // declaration, like "extern int funcname();"
   gt_control,       // control structures like function start, end, if, switch, endif
   gt_opcode,        // valid, not processed opcode
   gt_opcode_done,   // processed opcode
   gt_expression,    // expression
   gt_operator,      // expression operator
   gt_label,         // label
};


// structs

//! conversation graph item
struct ua_conv_graph_item
{
   //! ctor
   ua_conv_graph_item()
      :itemtype(gt_opcode), opcode(0), arg(0), pos(0xffff),
       expr_finished(true), expr_address(false), oper_needed(2), indent_change(0) {}

   //! type of this item
   ua_conv_graph_item_type itemtype;

   //! opcode values
   Uint16 opcode, arg;

   //! opcode position in code segment
   Uint16 pos;

   //! indicates if expression is finished or has a result value on stack
   bool expr_finished;

   //! indicates if expression is an address or a value
   bool expr_address;

   //! operator: number of operands needed
   unsigned int oper_needed;

   //! number of indendation steps to change
   int indent_change;

   //! plain text of "transformed" item
   std::string plaintext;
};

//! infos about a specific function
struct ua_conv_func_info
{
   ua_conv_func_info()
      :rettype(ua_rt_void), numparam(0), start(0){}

   //! return type
   ua_conv_datatype rettype;

   //! number of parameter
   unsigned int numparam;

   //! start in code
   Uint16 start;
};


// classes

//! conversation graph
class ua_conv_graph
{
public:
   //! ctor
   ua_conv_graph(){}

   //! initializes conversation graph
   void init(std::vector<Uint16> code,
      Uint16 codestart, Uint16 codeend,
      std::vector<std::string>& strings,
      std::vector<std::string>& imported_funcs,
      std::vector<std::string>& imported_vars, Uint16 numglobals);

   //! processes graph
   void process();

   //! returns graph object
   inline const std::vector<ua_conv_graph_item>& get_graph();

protected:
   // worker functions

   //! searches function starts and ends
   void search_functions();

   //! replaces opcodes with expressions
   void replace_expressions();

   //! remove operators by "folding" expressions
   void fold_expressions();

   //! find out control structs
   void find_control_structs();

   //! fixes up function starts
   void fixup_functions();


   // helper functions

   //! finds pos in graph of first item with target == item.pos
   unsigned int find_pos(Uint16 target);

   //! returns true when item at position "pos" is given (unprocessed) opcode
   bool is_opcode(unsigned int pos, Uint16 opcode);

   //! formats a global string from an immediate value
   void format_global(Uint16 offset,std::string& str);

   //! inserts a control item
   void insert_control(const char* str, unsigned int graph_pos,
      Uint16 opcode_pos, int indent_change);

protected:
   //! conversation graph
   std::vector<ua_conv_graph_item> graph;

   //! all local functions
   std::vector<ua_conv_func_info> functions;

   //! all conversation strings
   std::vector<std::string> strings;

   //! name of all imported functions
   std::vector<std::string> imported_funcs;

   //! name of all imported variables
   std::vector<std::string> imported_vars;

   //! number of globals before imported vars
   Uint16 nglobals;
};


// inline functions

const std::vector<ua_conv_graph_item>& ua_conv_graph::get_graph()
{
   return graph;
}


#endif
