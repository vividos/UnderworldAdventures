/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2004 Underworld Adventures Team

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
/*! \file cnvdec.cpp

   \brief conversation code decompiling

*/

// needed includes
#include "common.hpp"
#include "cnvdec.hpp"


// ua_conv_code_decompiler methods

void ua_conv_code_decompiler::start(std::string& basepath, Uint16 slot)
{
   ua_conv_globals cg;
   ua_conv_code_vm code_vm;

   ua_settings settings;
   settings.set_value(ua_setting_uw_path, basepath);

   // load game strings
   gstr.init(settings);

   // load conversation
   {
      ua_uw_import import;

      import.load_conv_globals(cg,settings,"data/",true);

      // load code into vm
      std::string cnv_ark_name = settings.get_string(ua_setting_uw_path);
      cnv_ark_name += "data/cnv.ark";
      import.load_conv_code(code_vm, cnv_ark_name.c_str(), slot);
   }

   // get local strings
   std::vector<std::string> localstrings;

   gstr.get_stringblock(code_vm.get_strblock(),localstrings);

   // output header
   printf("// conversation slot #%u (0x%04x)\n", slot, slot);
   printf("// name: %s\n", gstr.get_string(7,slot+16).c_str());
   printf("//\n");

   // put code into conv graph
   std::vector<Uint16>& code = code_vm.get_code_segment();
   conv_graph.init(code, 0, code.size(), localstrings,
      code_vm.get_imported_funcs(),
      code_vm.get_imported_globals(),
      code_vm.get_globals_reserved());

   // do the work and output

   //conv_graph.disassemble();
   conv_graph.decompile();

   output_graph();

   printf("\n");
}

//#define SHOW_PROCESSED 1
//#define SHOW_POS 1

void ua_conv_code_decompiler::output_graph()
{
   // output code
   int indent_level = 0;

   ua_conv_graph::const_graph_iterator iter,stop;
   iter = conv_graph.get_graph().begin();
   stop = conv_graph.get_graph().end();

   for(;iter!=stop;iter++)
   {
      const ua_conv_graph_item& item = *iter;

#ifndef SHOW_PROCESSED
      if (item.is_processed)
         continue;

      // jump over functions not decompiled
      if (item.itemtype == gt_func_start)
      {
         while(iter->itemtype != gt_func_end && iter != stop)
            iter++;
         continue;
      }
#endif

      if (item.itemtype == gt_opcode && item.label_name.size()>0)
         printf("%s: // referenced %u times\n",item.label_name.c_str(),
            item.xref_count);

#ifdef SHOW_POS
      printf("[%04x]  ", item.pos);
#endif

      // indendation change?
      if (item.itemtype == gt_statement)
         indent_level += item.statement_data.indent_change_before;

      // indendation
      std::string indent;
      for(int i=0; i<indent_level; i++)
         indent += "   ";
      printf(indent.c_str());

      // format text and output it
      std::string item_text;
      conv_graph.format_graph_item(item_text,item);

      printf("%s", item_text.c_str());

      // indendation change?
      if (item.itemtype == gt_statement)
            indent_level += item.statement_data.indent_change_after;

      printf("\n");
   }
}
