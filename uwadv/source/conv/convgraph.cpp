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


// ua_conv_graph methods

void ua_conv_graph::init(std::vector<Uint16> code,
   Uint16 codestart, Uint16 codeend,
   std::vector<std::string>& mystrings,
   std::vector<std::string>& myimported_funcs,
   std::vector<std::string>& myimported_vars,
   Uint16 mynumglobals)
{
   strings.clear();
   strings = mystrings;

   imported_funcs.clear();
   imported_funcs = myimported_funcs;

   imported_vars.clear();
   imported_vars = myimported_vars;

   nglobals = mynumglobals;

   printf("converting to graph\n");

   // convert code segment to graph
   for(Uint16 i=codestart; i<codeend; i++)
   {
      // fill item struct
      ua_conv_graph_item item;
      item.pos = i;
      item.opcode = code[i];

      if (item.opcode <= op_last && ua_conv_instructions[item.opcode].args != 0)
         item.arg = code[++i];
      else
         item.arg = 0;

      // add it to graph
      graph.push_back(item);
   }

   // add "extern private[]" decl
   {
      ua_conv_graph_item declitem;
      declitem.itemtype = gt_decl;

      char buffer[256];
      sprintf(buffer,"extern int private[%u];\n",imported_vars.size());
      declitem.plaintext.append(buffer);

      graph.insert(graph.begin(),declitem);
   }
}

void ua_conv_graph::process()
{
   printf("searching functions\n");
   search_functions();

   printf("replacing expression opcodes\n");
   replace_expressions();

   printf("folding expressions\n");
   fold_expressions();

   printf("adding control structures\n");
   find_control_structs();

   printf("fixing up functions\n");
   fixup_functions();
}


/* replace function entry code:

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
   // go through code and search for functions
   for(unsigned int i=0; i<graph.size(); i++)
   {
      // check for function entry code
      if (i+3<graph.size() &&
          is_opcode(i,op_PUSHBP) &&
          is_opcode(i+1,op_SPTOBP) &&
          is_opcode(i+2,op_PUSHI) &&
          is_opcode(i+3,op_ADDSP) )
      {
         // found entry code
         graph[i].itemtype = gt_opcode_done;
         graph[i+1].itemtype = gt_opcode_done;
         graph[i+2].itemtype = gt_opcode_done;
         graph[i+3].itemtype = gt_opcode_done;

         // insert function item
         {
            ua_conv_graph_item funcitem;
            funcitem.itemtype = gt_control;
            funcitem.indent_change = 1;
            funcitem.pos = graph[i].pos;

            // generate new function name
            {
               char buffer[256];

               // opcode START precedes function?
               if (i>0 && is_opcode(i-1,op_START))
               {
                  graph[--i].itemtype = gt_opcode_done;
                  strcpy(buffer,"main");
                  funcitem.pos--;
               }
               else
                  sprintf(buffer,"func_%04x",graph[i].pos);

               funcitem.plaintext.assign(buffer);
            }

            // remember local function
            ua_conv_func_info funcinfo;
            funcinfo.start = graph[i].pos;
            functions.push_back(funcinfo);

            graph.insert(graph.begin()+i,funcitem);
            i++;
         }

         // insert "locals"
         if (graph[i+2].arg>0)
         {
            ua_conv_graph_item declitem;
            declitem.itemtype = gt_decl;
            declitem.pos = graph[i].pos;

            char buffer[256];
            sprintf(buffer,"int locals[%u];\n",graph[i+2].arg);
            declitem.plaintext.assign(buffer);

            graph.insert(graph.begin()+i,declitem);
            i++;
         }

         i += 4;
      }

      // check for function exit code
      if (i+2<graph.size() &&
          is_opcode(i,op_BPTOSP) &&
          is_opcode(i+1,op_POPBP) &&
          is_opcode(i+2,op_RET) )
      {
         // found entry code
         graph[i].itemtype = gt_opcode_done;
         graph[i+1].itemtype = gt_opcode_done;
         graph[i+2].itemtype = gt_opcode_done;

         ua_conv_graph_item funcitem;
         funcitem.itemtype = gt_control;
         funcitem.indent_change = -1;
         funcitem.plaintext.assign("} // end func");
         funcitem.pos = graph[i].pos;

         graph.insert(graph.begin()+i,funcitem);

         i += 3;
      }
   }
}

void ua_conv_graph::replace_expressions()
{
   char buffer[256];

   // go through code and replace code with expressions and operators
   for(unsigned int i=0; i<graph.size(); i++)
   {
      unsigned int max = graph.size();

      // PUSHI: global/private address or immediate value (don't know yet)
      if (is_opcode(i,op_PUSHI))
      {
         graph[i].itemtype = gt_opcode_done;

         ua_conv_graph_item expritem;
         expritem.itemtype = gt_expression;
         expritem.expr_finished = false;
         expritem.expr_address = false;
         expritem.arg = graph[i].arg; // save arg, in case we need it
         expritem.pos = graph[i].pos;

         // do expression string
         sprintf(buffer,"%u",graph[i].arg);
         expritem.plaintext.assign(buffer);

         graph.insert(graph.begin()+i,expritem);
         max++;
         i++;
      }

      // PUSHI_EFF: local/param pointer
      if (is_opcode(i,op_PUSHI_EFF))
      {
         graph[i].itemtype = gt_opcode_done;

         ua_conv_graph_item expritem;
         expritem.itemtype = gt_expression;
         expritem.expr_finished = false;
         expritem.expr_address = true;
         expritem.arg = graph[i].arg; // save arg, in case we need it
         expritem.pos = graph[i].pos;

         // do expression string
         if (graph[i].arg < 0x8000)
         {
            sprintf(buffer,"&locals[%u]",graph[i].arg);
         }
         else
         {
            int param_no = -(Sint16)graph[i].arg-1;
            sprintf(buffer,"&param%u",param_no);
            // TODO: store number of used param in funcinfo
         }
         expritem.plaintext.assign(buffer);

         graph.insert(graph.begin()+i,expritem);
         max++;
         i++;
      }

      // basic operators
      if (is_opcode(i,op_OFFSET) ||
          is_opcode(i,op_FETCHM) ||
          is_opcode(i,op_OPNEG) ||
          is_opcode(i,op_OPNOT) )
      {
         graph[i].itemtype = gt_opcode_done;

         ua_conv_graph_item operitem;
         operitem.itemtype = gt_operator;
         operitem.opcode = graph[i].opcode;
         operitem.arg = graph[i].arg;
         operitem.expr_finished = false;
         operitem.expr_address = graph[i].expr_address;

         if (graph[i].opcode == op_OFFSET)
            operitem.oper_needed = 2;
         else
            operitem.oper_needed = 1;

         graph.insert(graph.begin()+i,operitem);
         max++;
         i++;
      }

      // compare/logical operators
      if (is_opcode(i,op_TSTEQ) ||
          is_opcode(i,op_TSTGT) ||
          is_opcode(i,op_TSTGE) ||
          is_opcode(i,op_TSTLT) ||
          is_opcode(i,op_TSTLE) ||
          is_opcode(i,op_TSTNE) ||
          is_opcode(i,op_OPOR)  ||
          is_opcode(i,op_OPAND) ||
          is_opcode(i,op_OPADD) ||
          is_opcode(i,op_OPSUB) ||
          is_opcode(i,op_OPMUL) ||
          is_opcode(i,op_OPDIV) ||
          is_opcode(i,op_OPMOD) )
      {
         graph[i].itemtype = gt_opcode_done;

         ua_conv_graph_item operitem;
         operitem.itemtype = gt_operator;
         operitem.opcode = graph[i].opcode;
         operitem.arg = graph[i].arg;
         operitem.expr_finished = false;
         operitem.expr_address = false;

         switch(graph[i].opcode)
         {
         case op_TSTEQ: operitem.plaintext.assign(" == "); break;
         case op_TSTGT: operitem.plaintext.assign(" > "); break;
         case op_TSTGE: operitem.plaintext.assign(" >= "); break;
         case op_TSTLT: operitem.plaintext.assign(" < "); break;
         case op_TSTLE: operitem.plaintext.assign(" <= "); break;
         case op_TSTNE: operitem.plaintext.assign(" != "); break;
         case op_OPOR:  operitem.plaintext.assign(" || "); break;
         case op_OPAND: operitem.plaintext.assign(" && "); break;
         case op_OPADD: operitem.plaintext.assign(" + "); break;
         case op_OPSUB: operitem.plaintext.assign(" - "); break;
         case op_OPMUL: operitem.plaintext.assign(" * "); break;
         case op_OPDIV: operitem.plaintext.assign(" / "); break;
         case op_OPMOD: operitem.plaintext.assign(" % "); break;
         }

         graph.insert(graph.begin()+i,operitem);
         max++;
         i++;
      }

      // misc. operators
      if (is_opcode(i,op_STO) ||
          is_opcode(i,op_SAY_OP) ||
          is_opcode(i,op_CALLI) ||
          is_opcode(i,op_CALL) ||
          is_opcode(i,op_SAVE_REG) )
      {
         graph[i].itemtype = gt_opcode_done;

         ua_conv_graph_item operitem;
         operitem.itemtype = gt_operator;
         operitem.opcode = graph[i].opcode;
         operitem.arg = graph[i].arg;
         operitem.pos = graph[i].pos;
         operitem.expr_finished = true; // operator finishes whole expression
         operitem.expr_address = false;

         switch(graph[i].opcode)
         {
         case op_STO:
            operitem.oper_needed = 2;
            break;

         case op_SAY_OP:
            operitem.oper_needed = 1;
            break;

         case op_CALL:
         case op_CALLI:
            {
               operitem.oper_needed = 0;

               unsigned int j=i+1;
               while(is_opcode(j,op_POP) && j<max)
               {
                  operitem.oper_needed++;
                  graph[j].itemtype = gt_opcode_done;
                  j++;
               }

               if (is_opcode(j,op_PUSH_REG))
               {
                  // call return value is part of expression
                  operitem.expr_finished = false;
                  graph[j].itemtype = gt_opcode_done;
               }
            }
            break;

         case op_SAVE_REG:
            operitem.oper_needed = 1;
            operitem.expr_finished = true;
            if (i+1<graph.size() && is_opcode(i+1,op_POP))
               graph[i+1].itemtype = gt_opcode_done;
            break;
         }

         graph.insert(graph.begin()+i,operitem);
         max++;
         i++;
      }
   }
}

/*
   general strategy:
   search operators and try to build new expressions until no operators
   are left
*/
void ua_conv_graph::fold_expressions()
{
   // go through code and search for operators
   for(unsigned int i=0; i<graph.size(); i++)
   {
      if (graph[i].itemtype != gt_operator)
         continue;

      // found operator
      ua_conv_graph_item& item = graph[i];

      unsigned int expr_needed = item.oper_needed;
      std::vector<unsigned int> expr_indices;

      // search for needed expressions
      unsigned int j=i;

      if (expr_needed>0)
      {
         for(j=i-1; j>0 && graph[j].itemtype != gt_control; j--)
         {
            if (graph[j].itemtype == gt_expression && !graph[j].expr_finished)
            {
               // found unfinished expression to use
               expr_indices.push_back(j);
               if (--expr_needed==0)
                  break;
            }
         }
      }

      // construct new expression item
      ua_conv_graph_item new_expr;
      new_expr.itemtype = gt_expression;
      new_expr.expr_finished = item.expr_finished;

      if (item.oper_needed>0)
         new_expr.pos = graph[expr_indices[item.oper_needed-1]].pos;
      else
         new_expr.pos = graph[i].pos; // should not happen

      unsigned int new_expr_pos = j;

      // construct new expression string
      std::string new_plaintext;
      switch(item.opcode)
      {
      case op_OFFSET:
         {
            ua_conv_graph_item& opitem0 = graph[expr_indices[0]];

            if (!opitem0.expr_address)
            {
               // value can be pointer or global/private address
               std::string ptrtext;
               if (opitem0.plaintext.find("param") != std::string::npos)
               {
                  // paramX seems to be a pointer, so add array index
                  ptrtext.assign(opitem0.plaintext);
               }
               else
               {
                  // value is global/private
                  format_global(opitem0.arg,ptrtext);
               }

               char buffer[256];
               sprintf(buffer,"&%s[%u]",
                  ptrtext.c_str(),graph[expr_indices[1]].arg);

               new_plaintext.assign(buffer);
            }
            else
            {
               new_expr.arg = opitem0.arg + graph[expr_indices[1]].arg-1;

               char buffer[256];
               sprintf(buffer,"&locals[%u]",new_expr.arg);
               new_plaintext.append(buffer);
            }

            new_expr.expr_address = true;
         }
         break;

      case op_FETCHM:
         {
            ua_conv_graph_item& opitem = graph[expr_indices[0]];
            if (!opitem.expr_address)
            {
               if (opitem.plaintext.find("param") != std::string::npos ||
                   opitem.plaintext.find("local") != std::string::npos )
               {
                  new_plaintext.assign(opitem.plaintext);
                  new_plaintext.append("[0]");
               }
               else
                  // fetch global/private value
                  new_plaintext.erase();
                  format_global(opitem.arg,new_plaintext);
            }
            else
            {
               if (opitem.plaintext[0]=='&')
               {
                  new_plaintext.assign(opitem.plaintext);
                  new_plaintext.erase(0,1);
                  new_expr.expr_address = false;
               }
               else
               {
                  char buffer[256];

                  if (opitem.plaintext.find("param") != std::string::npos)
                  {
                     int param_no = -(Sint16)opitem.arg-1;
                     sprintf(buffer,"param%u",param_no);
                  }
                  else
                  if (opitem.plaintext.find("locals") != std::string::npos)
                  {
                     sprintf(buffer,"locals[%u]",opitem.arg);
                  }
                  else
                     // should not happen: dereferencing global
                     strcpy(buffer,opitem.plaintext.c_str());

                  new_plaintext.assign(buffer);
               }
            }

            new_expr.expr_address = false;
         }
         break;

      case op_OPNEG:
         {
            new_plaintext.assign("-");
            new_plaintext.append(graph[expr_indices[0]].plaintext);
         }
         break;

      case op_OPNOT:
         {
            new_plaintext.assign("!");
            new_plaintext.append(graph[expr_indices[0]].plaintext);
         }
         break;

      case op_SAVE_REG:
         {
            new_plaintext.assign("return ");
            new_plaintext.append(graph[expr_indices[0]].plaintext);
         }
         break;

      case op_STO:
         {
            // swap indices?
            bool swap = i>0 && is_opcode(i-1,op_SWAP);

            if (swap)
               graph[i-1].itemtype = gt_opcode_done;

            ua_conv_graph_item& rvalue = graph[expr_indices[swap ? 1 : 0]];
            ua_conv_graph_item& lvalue = graph[expr_indices[swap ? 0 : 1]];

            std::string rstr(rvalue.plaintext);
            std::string lstr(lvalue.plaintext);

            // check if lvalue is address
            if (!lvalue.expr_address)
            {
               // store to global/private
               lstr.erase();
               format_global(lvalue.arg,lstr);
            }
            else
            {
               if (lstr[0]=='&')
                  lstr.erase(0,1);

               if (lstr.compare("locals")==0)
               {
                  char buffer[256];
                  sprintf(buffer,"[%u]",lvalue.arg);
                  lstr.append(buffer);
               }
            }


            new_plaintext.assign(lstr);
            new_plaintext.append(" = ");
            new_plaintext.append(rstr);
         }
         break;

      case op_TSTEQ:
      case op_TSTGT:
      case op_TSTGE:
      case op_TSTLT:
      case op_TSTLE:
      case op_TSTNE:
      case op_OPAND:
      case op_OPADD:
      case op_OPSUB:
      case op_OPMUL:
      case op_OPDIV:
      case op_OPMOD:
         {
            new_plaintext.assign(graph[expr_indices[1]].plaintext);
            new_plaintext.append(item.plaintext);
            new_plaintext.append(graph[expr_indices[0]].plaintext);
         }
         break;

      case op_OPOR:
         {
            new_plaintext.assign("(");
            new_plaintext.append(graph[expr_indices[1]].plaintext);
            new_plaintext.append(item.plaintext);
            new_plaintext.append(graph[expr_indices[0]].plaintext);
            new_plaintext.append(")");
         }
         break;

      case op_SAY_OP:
         {
            new_plaintext.assign("say( \"");

            ua_conv_graph_item& opitem = graph[expr_indices[0]];
            if (!opitem.expr_address)
            {
               std::string text(strings[opitem.arg]);

               std::string::size_type pos = 0;
               while( (pos = text.find('\n',pos)) != std::string::npos)
               {
                  text.replace(pos,1,"\\n\"\n      \"");
                  pos += 7;
               }

               new_plaintext.append(text);
            }
            else
               new_plaintext.append(opitem.plaintext);

            new_plaintext.append("\" )");
         }
         break;

      case op_CALL:
      case op_CALLI:
         {
            if (item.opcode == op_CALLI)
            {
               // take imported function name
               new_plaintext.assign(imported_funcs[item.arg]);
            }
            else
            {
               // take function name
               unsigned int pos = find_pos(item.arg);
               if (pos==0)
               {
                  char buffer[256];
                  sprintf(buffer,"func_%04x",item.arg);
                  new_plaintext.assign(buffer);
               }
               else
                  new_plaintext.assign(graph[pos].plaintext);
            }

            std::string params;
            unsigned int max = expr_indices.size();
            for(unsigned int n=0; n<max; n++)
            {
               params.append(graph[expr_indices[n]].plaintext);
               if (n<max-1)
                  params.append(", ");
            }

            if (params.size()>0)
            {
               new_plaintext.append("( ");
               new_plaintext.append(params);
               new_plaintext.append(" )");
            }
            else
               new_plaintext.append("()");

            if (item.oper_needed>0)
            {
               // search for new insertion position before last finished expression
               unsigned int n = i-1;

               while(n>j && n>0)
               {
                  if (graph[n].itemtype == gt_expression)
                  {
                     if (graph[n].expr_finished)
                        break; // found a finished expression
                     else
                        new_expr_pos = n; // unfinished expression; new last position
                  }
                  n--;
               }
            }
         }
         break;
      }

      new_expr.plaintext.assign(new_plaintext);

      // delete old operator
      graph.erase(graph.begin()+i);
      if (new_expr_pos>i) new_expr_pos--;
      i--;

      // delete all old expressions
      {
         for(unsigned int n=0; n<expr_indices.size(); n++)
         {
            graph.erase(graph.begin()+expr_indices[n]);
            if (new_expr_pos>expr_indices[n]) new_expr_pos--;
            i--;
         }
      }

      // insert new expression
      graph.insert(graph.begin()+new_expr_pos,new_expr);
      i++;
   }
}

void ua_conv_graph::find_control_structs()
{
   // go through code and search for control structures
   for(unsigned int i=0; i<graph.size(); i++)
   {
      if (is_opcode(i,op_EXIT_OP))
      {
         graph[i].itemtype = gt_opcode_done;
         i++;
      }

      // check for if/switch/while
      if (is_opcode(i,op_BEQ))
      {
         graph[i].itemtype = gt_opcode_done;

         // calculate branch target
         Uint16 target = graph[i].arg + graph[i].pos + 1;
         unsigned int branch_target = find_pos(target);

         // find expression to use
         unsigned int expr_pos = i-1;
         while( expr_pos>0 && graph[expr_pos].itemtype != gt_expression && graph[expr_pos].expr_finished )
            expr_pos--;

         // check if we have a "while" loop
         if (is_opcode(branch_target-1,op_BRA) &&
             graph[branch_target-1].arg>0x7fff)
         {
            graph[branch_target-1].itemtype = gt_opcode_done;

            // while loop end
            insert_control("} // while",branch_target,graph[branch_target-1].pos,-1);

            // add "while" statement
            {
               std::string whiletext("while ( ");
               whiletext.append(graph[expr_pos].plaintext);
               whiletext.append(" ) {");
               insert_control(whiletext.c_str(),expr_pos,graph[expr_pos].pos,1);

               // delete expression
               graph.erase(graph.begin()+expr_pos+1);
            }
            i++;
         }
         else
         // check if we have a "switch" control statement
         if (graph[branch_target+1].opcode == graph[expr_pos+1].opcode &&
             graph[branch_target+1].opcode == op_PUSHI_EFF &&
             graph[branch_target+1].arg == graph[expr_pos+1].arg &&

             graph[branch_target+2].opcode == graph[expr_pos+2].opcode &&
             graph[branch_target+2].opcode == op_FETCHM &&

             graph[branch_target+3].opcode == graph[expr_pos+3].opcode &&
             graph[branch_target+3].opcode == op_PUSHI)
         {
            char buffer[256];

            // "switch" control statement
            {
               sprintf(buffer,"switch ( locals[%u] ) {",graph[expr_pos+1].arg);
               std::string sw_text(buffer);

               // add statement
               insert_control(sw_text.c_str(),expr_pos,graph[expr_pos].pos,0);

               expr_pos++; branch_target++;
            }

            Uint16 endsw_pos = 0;
            do
            {
               // do "case" label
               {
                  sprintf(buffer,"case %u:",graph[expr_pos+3].arg);

                  std::string case_text(buffer);
                  insert_control(case_text.c_str(),expr_pos,graph[expr_pos].pos,1);

                  // delete expression
                  graph.erase(graph.begin()+expr_pos+1);
               }

               // do "break" label
               {
                  std::string break_text("break;");
                  insert_control(break_text.c_str(),branch_target,graph[branch_target-1].pos,-1);

                  if (is_opcode(branch_target-1,op_JMP))
                  {
                     graph[branch_target-1].itemtype = gt_opcode_done;
                     endsw_pos = graph[branch_target-1].arg;
                  }
                  else
                  if (is_opcode(branch_target-1,op_BRA))
                  {
                     graph[branch_target-1].itemtype = gt_opcode_done;
                     endsw_pos = graph[branch_target-1].arg + graph[branch_target-1].pos +1;
                  }

                  branch_target++;
               }

               // find next "branch target"
               expr_pos = branch_target;

               // check if branch target == end switch position
               if (graph[branch_target].pos == endsw_pos)
                  break;

               // JMP right after BEQ?
/*               if (is_opcode(branch_target-1,op_JMP) &&
                   graph[branch_target-2].opcode == op_BEQ)
                   break;*/

               if (graph[branch_target].itemtype == gt_expression)
               {
                  int j = branch_target+1;
                  while(!is_opcode(j,op_BEQ)) j++;
                  branch_target = find_pos(graph[j].arg + graph[j].pos +1);
                  graph[j].itemtype = gt_opcode_done;
               }

               // loop when we have another "case"
            }
            while(graph[expr_pos+1].opcode == op_PUSHI_EFF &&
               graph[expr_pos+2].opcode == op_FETCHM &&
               graph[expr_pos+3].opcode == op_PUSHI);

            if (is_opcode(branch_target-1,op_JMP))
               graph[branch_target-1].itemtype = gt_opcode_done;

            // add "end switch"
            {
               std::string endsw_text("} // end switch");
               //endsw_text.append(graph[i].plaintext);

               insert_control(endsw_text.c_str(),branch_target,graph[branch_target-1].pos,0);

//               if (is_opcode(label_endif-1,op_JMP))
//                  graph[label_endif-1].itemtype = gt_opcode_done;
            }
         }
         else
         {
            // "if" control statement
            {
               std::string iftext("if ( ");
               iftext.append(graph[expr_pos].plaintext);
               iftext.append(" ) {");

               // add "if" statement
               insert_control(iftext.c_str(),expr_pos,graph[expr_pos].pos,1);

               // delete expression
               graph.erase(graph.begin()+expr_pos+1);
            }

            // find out endif target
            Uint16 target_endif = 0;
            {
               int j=branch_target-1;

               // check opcode and get endif target
               if (is_opcode(j,op_JMP))
               {
                  target_endif = graph[j].arg;
                  graph[j].itemtype = gt_opcode_done;
               }
               else
               if (is_opcode(j,op_BRA))
               {
                  target_endif = graph[j].arg + graph[j].pos +1;
                  graph[j].itemtype = gt_opcode_done;
               }
               else
                  target_endif = target; // no else part

               // check for another jump

               // search last opcode
               j--;

               // check again, there might be another jump/branch
               if (is_opcode(j,op_JMP))
               {
                  target_endif = graph[j].arg;
                  graph[j].itemtype = gt_opcode_done;
               }
               else
               if (is_opcode(j,op_BRA))
               {
                  target_endif = graph[j].arg + graph[j].pos +1;
                  graph[j].itemtype = gt_opcode_done;
               }
            }

            unsigned int label_endif = find_pos(target_endif);
            label_endif = find_pos(graph[label_endif-1].pos);

            // search last pos
            while(graph[label_endif+1].pos == graph[label_endif].pos)
               label_endif++;

            label_endif++;

            // add "endif" statement
            {
               std::string endif_text("} // end if");
               //endif_text.append(graph[expr_pos].plaintext);

               insert_control(endif_text.c_str(),label_endif,graph[label_endif-1].pos,-1);

               if (is_opcode(label_endif-1,op_JMP))
                  graph[label_endif-1].itemtype = gt_opcode_done;
            }

            // add "else" statement, when used
            if (label_endif != branch_target)
            {
               std::string endif_text("} else {");
               //endif_text.append(graph[expr_pos].plaintext);

               // we have an else statement
               insert_control(endif_text.c_str(),branch_target,graph[branch_target-1].pos,-1);
            }

         }
      }

      if (is_opcode(i,op_JMP))
      {
         graph[i].itemtype = gt_opcode_done;

         unsigned int pos = find_pos(graph[i].arg);
         if (graph[pos+1].itemtype == gt_opcode_done &&
             graph[pos+1].opcode == op_BPTOSP)
         {
            // jump to end of function == "return;"
            insert_control("return;",i,graph[i].pos,0);
         }
         else
         {
            // ugly "goto" to label

            std::string label_text;
            {
               char buffer[256];
               sprintf(buffer,"label_%04x",graph[i].arg);
               label_text.assign(buffer);
            }
            unsigned label_pos = find_pos(graph[i].arg);

            // insert "goto label"
            {
               std::string goto_text("goto ");
               goto_text.append(label_text);
               goto_text.append(";");

               insert_control(goto_text.c_str(),i,graph[i].pos,0);

               if (label_pos>i) label_pos++;
            }

            // insert label
            {
               label_text.append(":;");

               insert_control(label_text.c_str(),label_pos,graph[label_pos].pos,0);
            }
         }
      }
   }
}

void ua_conv_graph::fixup_functions()
{
   unsigned int max = functions.size();
   for(unsigned int i=0; i<max; i++)
   {
      ua_conv_func_info& funcinfo = functions[i];

      // get function name
      unsigned int pos = find_pos(funcinfo.start);

      if (graph[pos].plaintext.find("int locals[") != std::string::npos)
         pos--;

      std::string funcname(graph[pos].plaintext);

      // create param string
      std::string params;

      for(unsigned int i=0; i<funcinfo.numparam; i++)
      {
         if (i==0) params.append(" ");

         char buffer[20];
         sprintf(buffer,"param%u ",i);
         params.append(buffer);
      }

      // create function decl.
      char buffer[256];
      sprintf(buffer,"%s %s(%s) {",
         funcinfo.return_type == ua_dt_string? "string" :
         funcinfo.return_type == ua_dt_int ? "int" : "void",
         funcname.c_str(), params.c_str());

      graph[pos].plaintext.assign(buffer);
   }
}

unsigned int ua_conv_graph::find_pos(Uint16 target)
{
   unsigned int max = graph.size();
   for(unsigned int i=0; i<max; i++)
      if (graph[i].pos == target)
         return i;
   return 0; // should not happen
}

bool ua_conv_graph::is_opcode(unsigned int pos, Uint16 opcode)
{
   return (graph[pos].itemtype == gt_opcode && graph[pos].opcode == opcode);
}

void ua_conv_graph::format_global(Uint16 offset,std::string& str)
{
   char buffer[256];
   if (offset<nglobals)
      sprintf(buffer,"global[%u]",offset);
   else
   {
      if (offset<nglobals+imported_vars.size())
         strcpy(buffer,imported_vars[offset-nglobals].c_str());
      else
         sprintf(buffer,"private[%u]",offset-nglobals-imported_vars.size());
   }
   str.append(buffer);
}

void ua_conv_graph::insert_control(const char* str, unsigned int graph_pos,
   Uint16 opcode_pos, int indent_change)
{
   ua_conv_graph_item control_item;
   control_item.itemtype = gt_control;
   control_item.indent_change = indent_change;
   control_item.plaintext.assign(str);
   control_item.pos = opcode_pos;

   graph.insert(graph.begin()+graph_pos,control_item);
}
