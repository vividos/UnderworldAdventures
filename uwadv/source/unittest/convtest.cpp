/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2005 Underworld Adventures Team

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
/*! \file convtest.cpp

   \brief conversation regression test

*/

// needed includes
#include "common.hpp"
#include "unittest.hpp"
#include "codevm.hpp"
#include "convgraph.hpp"
#include "gamestrings.hpp"
#include <sstream>
#include <iomanip>


//! conversation regression test
/*! This test allows conversation decompiling regression testing. Before
    changing conv. decompiler, use the create_conv_dumps() test function to
    create conversation files to test against when running
    test_conv_regression(). The conversation files are stored in the folder
    "./convdec", under the name convXXXX.txt, where XXXX is the four-digit
    hexadecimal conversation slot number.
*/
class ua_test_conv_regression: public ua_testcase
{
public:
   // generate test suite
   CPPUNIT_TEST_SUITE(ua_test_conv_regression)
      // uncomment this to create conversation regression base files
      //CPPUNIT_TEST(create_conv_dumps)
      CPPUNIT_TEST(test_conv_regression)
   CPPUNIT_TEST_SUITE_END()

protected:
   //! creates conversation dumps
   void create_conv_dumps();

   //! tests conversation files for regressions
   void test_conv_regression();

   // helper functions

   bool dump_conversation(FILE* out, Uint16 convslot);
   void output_graph(FILE* out, ua_conv_graph& cgraph);
   bool compare_files(const char* filename_ref, const char* filename_comp);
};

// register test suite
CPPUNIT_TEST_SUITE_REGISTRATION(ua_test_conv_regression);


// methods

void ua_test_conv_regression::create_conv_dumps()
{
   Uint16 convslot_start = 1;
   Uint16 convslot_max = 320;

   ua_mkdir("./convdec/", 700);

   for (Uint16 convslot=convslot_start; convslot < convslot_max; convslot++)
   {
      std::ostringstream buffer;
      buffer << "./convdec/conv" <<
         std::hex << std::setfill('0') << std::setw(4) << convslot << ".txt";

      if (ua_file_exists(buffer.str().c_str()))
         continue; // don't rewrite existing files

      ua_trace("writing conversation %04x...\n", convslot);

      FILE* fd = fopen(buffer.str().c_str(), "wt");
      dump_conversation(fd, convslot);
      fclose(fd);
   }
}

void ua_test_conv_regression::test_conv_regression()
{
   std::vector<std::string> filelist;
   ua_find_files("./convdec/conv*.txt", filelist);

   ua_test_tempdir tempdir;

   std::vector<std::string>::size_type max = filelist.size();
   for (std::vector<std::string>::size_type i=0; i<max; i++)
   {
      std::string filename_ref(filelist[i]);

      Uint16 convslot = static_cast<Uint16>(strtoul(filename_ref.substr(10 + 4, 4).c_str(), NULL, 16));

      std::string filename = tempdir.get_pathname();
      filename += "/";
      filename += filename_ref.substr(10);

      ua_trace("loading conversation %04x...\n", convslot);

      FILE* fd = fopen(filename.c_str(), "wt");
      dump_conversation(fd, convslot);
      fclose(fd);

      ua_trace("comparing files...\n");

      CPPUNIT_ASSERT(true == compare_files(filename_ref.c_str(), filename.c_str()));
   }
}

bool ua_test_conv_regression::dump_conversation(FILE* out, Uint16 convslot)
{
   ua_settings& settings = get_settings();

   ua_conv_globals cg;
   ua_conv_code_vm code_vm;
   ua_gamestrings gstr;

   // load stuff
   {
      gstr.init(settings);

      ua_uw_import import;
      import.load_conv_globals(cg, settings, "data/", true);

      cg.get_globals(convslot);

      // load code into vm
      std::string cnv_ark_name = settings.get_string(ua_setting_uw_path);
      cnv_ark_name += "data/cnv.ark";

      if (!import.load_conv_code(code_vm, cnv_ark_name.c_str(), convslot))
      {
         fprintf(out, "// conversation slot #%u (0x%04x) not available.\n\n",
            convslot, convslot);
         return false;
      }
   }

   // get local strings
   std::vector<std::string> localstrings;

   gstr.get_stringblock(code_vm.get_strblock(),localstrings);

   // output header
   fprintf(out, "// conversation slot #%u (0x%04x)\n", convslot, convslot);
   fprintf(out, "// name: %s\n", gstr.get_string(7,convslot+16).c_str());
   fprintf(out, "//\n");

   // put code into conv graph
   ua_conv_graph conv_graph;

   std::vector<Uint16>& code = code_vm.get_code_segment();
   conv_graph.init(code, 0, code.size(), localstrings,
      code_vm.get_imported_funcs(),
      code_vm.get_imported_globals(),
      code_vm.get_globals_reserved());

   // do the work and output

   //conv_graph.disassemble();
   conv_graph.decompile();

   output_graph(out, conv_graph);

   fprintf(out, "\n");
   return true;
}


void ua_test_conv_regression::output_graph(FILE* out, ua_conv_graph& cgraph)
{
   // output code
   int indent_level = 0;
   const ua_conv_graph::graph_list& graph = cgraph.get_graph();

   ua_conv_graph::const_graph_iterator iter,stop;
   iter = graph.begin();
   stop = graph.end();
   for (;iter!=stop;iter++)
   {
      const ua_conv_graph_item& item = *iter;

#ifndef SHOW_PROCESSED
      if (item.is_processed && item.itemtype != gt_opcode)
         continue;

      if (item.itemtype == gt_func_start)
      {
         while(iter->itemtype != gt_func_end && iter != graph.end())
            iter++;
         continue;
      }
#endif

      if (item.itemtype == gt_opcode && item.label_name.size()>0)
         fprintf(out, "%s: // referenced %u times\n", item.label_name.c_str(),
            item.xref_count);

#ifdef SHOW_POS
      fprintf(out, "[%04x]  ", item.pos);
#endif

      if (item.itemtype == gt_statement)
         indent_level += item.statement_data.indent_change_before;

      // indendation
      std::string indent;
      {
         for(int i=0; i<indent_level; i++)
            indent += "   ";
      }
      fprintf(out, indent.c_str());

      std::string item_text;
      cgraph.format_graph_item(item_text,item);

      fprintf(out, "%s", item_text.c_str());

      // print according to item type
      switch(item.itemtype)
      {
      case gt_func_start:
         break;

      case gt_func_end:
         break;

      case gt_opcode:
      case gt_operator:
      case gt_expression:
         break;

      default:
         break;
      }

      if (item.itemtype == gt_statement)
         indent_level += item.statement_data.indent_change_after;

      fprintf(out, "\n");
   }
}

bool ua_test_conv_regression::compare_files(const char* filename_ref, const char* filename_comp)
{
   FILE* ref = fopen(filename_ref, "rt");
   if (ref == NULL)
      return false;

   FILE* comp = fopen(filename_comp, "rt");
   if (ref == NULL)
   {
      fclose(ref);
      return false;
   }

   fseek(ref, 0L, SEEK_END);
   long ref_len = ftell(ref);
   fseek(ref, 0L, SEEK_SET);

   fseek(comp, 0L, SEEK_END);
   long comp_len = ftell(comp);
   fseek(comp, 0L, SEEK_SET);

   std::vector<char> ref_bytes, comp_bytes;
   ref_bytes.resize(ref_len);
   comp_bytes.resize(comp_len);

   fread(&ref_bytes[0], ref_len, 1, ref);
   fread(&comp_bytes[0], comp_len, 1, comp);

   fclose(ref);
   fclose(comp);

   ref_bytes.push_back(0);
   comp_bytes.push_back(0);

   char* ref_str = &ref_bytes[0];
   char* comp_str = &comp_bytes[0];

   unsigned int pos = 0;
   unsigned int last_linefeed = static_cast<unsigned int>(-1);
   unsigned int current_line = 1;

   while (*ref_str != 0 && *comp_str != 0)
   {
      if (*ref_str == *comp_str)
      {
         // same characters
         if (*ref_str == '\n')
         {
            last_linefeed = pos;
            current_line++;
         }
      }
      else
      {
         // different characters

         // go back to last newline
         ref_str -= (pos - last_linefeed)-1;
         comp_str -= (pos - last_linefeed)-1;

         // find out last three lines
         std::string context;
         {
            char* ref_start = &ref_bytes[0];
            char* common = ref_str;
            unsigned int count = 4;
            while (common > ref_start && count > 0)
            {
               if (*common == '\n')
               {
                  if (--count == 0)
                  {
                     context.assign(common+1, ref_str - common-1);
                     break;
                  }
               }
               common--;
            }
            if (common == ref_start)
               context.assign(common, ref_str - common);
         }

         // search for next three lines
         char* ref_pos = strchr(ref_str, '\n');
         if (ref_pos != NULL) ref_pos = strchr(ref_pos + 1, '\n');
         if (ref_pos != NULL) ref_pos = strchr(ref_pos + 1, '\n');

         if (ref_pos != NULL)
            *ref_pos = 0;

         char* comp_pos = strchr(comp_str, '\n');
         if (comp_pos != NULL) comp_pos = strchr(comp_pos + 1, '\n');
         if (comp_pos != NULL) comp_pos = strchr(comp_pos + 1, '\n');

         if (comp_pos != NULL)
            *comp_pos = 0;

         // add + and - chars before new lines
         std::string ref_text(ref_str);
         std::string comp_text(comp_str);

         std::string::size_type pos = 0;
         while ( std::string::npos != (pos = ref_text.find('\n', pos)) )
         {
            ref_text.insert(pos+1, 1, '-');
            pos += 2;
         }
         context += "-";
         context += ref_text;

         pos = 0;
         while ( std::string::npos != (pos = comp_text.find('\n', pos)) )
         {
            comp_text.insert(pos+1, 1, '+');
            pos += 2;
         }
         context += "\n+";
         context += comp_text;

         // output differences
         ua_trace("\nfiles %s and %s are different, starting from line %u on!\n"
            "diff -u:\n%s\n",
            filename_ref, filename_comp, current_line, context.c_str());

         return false;
      }

      ref_str++;
      comp_str++;
      pos++;
   }

   // compare successful when all two strings ended; else one file was larger
   return *ref_str == 0 && *comp_str == 0;
}
