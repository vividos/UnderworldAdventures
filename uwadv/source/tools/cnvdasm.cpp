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
/*! \file cnvdasm.cpp

   conversation disassembler and decompiler

*/

// needed includes
#include "common.hpp"
#include "cnvdasm.hpp"
#include "resource/fread_endian.hpp"
#include "instr.hpp"


// constants

const int ua_dasm_replace_count = 13;


// ua_conv_dasm methods

bool ua_conv_dasm::init(const char *cnvfile,unsigned int conv)
{
   if (!ua_conv_code_vm::load_code(cnvfile,conv))
   {
      printf("could not load code ...\n");
      return false;
   }

   // global '0' string is empty
   if (imported_globals.size()>0 && imported_globals[0].name.empty())
      imported_globals[0].name.assign("value0");

   // try to load game strings
   std::string gstrname(cnvfile);

   std::string::size_type pos = gstrname.find("cnv.ark");
   if (pos!=std::string::npos)
   {
      gstrname.erase(pos);
      gstrname.append("strings.pak");

      try
      {
         gs.load(gstrname.c_str());
         printf("game strings loaded.\n");
      }
      catch(...)
      {
         printf("failed loading game strings.\n");
      }
   }

   return true;
}


void ua_conv_dasm::disassemble(FILE *out)
{
   if (fd==NULL) return;

   fprintf(out,";\n; conversation #%u (string block 0x%04x)\n"
      "; disassembled by cnvdasm\n;\n",conv_nr,strblock);

   // now list all imported funcs and globals

   fprintf(out,"; imported functions:\n");
   unsigned int i,max=imported_funcs.size();
   for(i=0; i<max; i++)
   {
      const ua_conv_imported_item &func = imported_funcs[i];

      fprintf(out,"; id=%04x: %s %s();\n",
         i,func.ret_type == ua_rt_void ? "void" :
            func.ret_type == ua_rt_int ? "int" : "string",
         func.name.c_str());
   }

   fprintf(out,";\n; imported globals:\n");
   max=imported_globals.size();
   for(i=0; i<max; i++)
   {
      const ua_conv_imported_item &func = imported_globals[i];

      if (func.name.empty()) continue;

      fprintf(out,"; id=%04x: %s %s;\n",
         i,func.ret_type == ua_rt_void ? "void" :
            func.ret_type == ua_rt_int ? "int" : "string",
         func.name.c_str());
   }

   fprintf(out,"\n\n");

   // loads code into decoder structure
   load_dec();
   resolve_labels();

   // print out all dec entries
   i,max = dec_entries.size();
   for(i=0; i<max; i++)
   {
      const ua_conv_dec_entry &entry = dec_entries[i];

      if (!entry.label.empty())
         fprintf(out,"%s:\n",entry.label.c_str());

      fprintf(out,"    %s",entry.command.c_str());

      // check if we could print an info comment
      if (entry.code == op_SAY_OP)
      {
         fprintf(out,"        ; \"%s\"",
            gs.get_string(strblock,
               dec_entries[i-1].arg).c_str());
      }

      if (entry.code == op_CALLI)
      {
         fprintf(out,"        ; \"%s\"",
            imported_funcs[entry.arg].name.c_str());
      }

      fprintf(out,"\n");

      if (entry.code == op_RET)
         fprintf(out,"\n");
   }
}

void ua_conv_dasm::decompile(FILE *out)
{
   if (fd==NULL) return;

   fprintf(out,"/*\n   ultima underworld 1 - conversation #%u\n"
      "   string block: 0x%04x\n"
      "   conversation partner: \"%s\"\n\n"
      "   decompiled by cnvdasm\n*/\n",conv_nr,strblock,
      gs.get_string(7,conv_nr+16).c_str());

   load_dec();
   resolve_labels();

   for(unsigned int type=0; type<ua_dasm_replace_count; type++)
   {
      bool restart = false;
      do
      {
         restart = false;

         // go through all entries
         unsigned int max = dec_entries.size();
         for(unsigned int entry=0; entry<max; entry++)
         {
            if (!dec_entries[entry].translated)
               if (replace_constructs(type,entry,max))
               {
                  restart = true;
                  break;
               }
         }

      } while (restart);
   }

   // print out stuff
   unsigned int i,max;

   fprintf(out,"\n#include <stdio.h>\n");
   fprintf(out,"\n// private and globals\n");

   fprintf(out,"int private[0x%04x];\n\n",glob_reserved+1);

   // print out globals
   max=imported_globals.size();
   for(i=0; i<max; i++)
   {
      const ua_conv_imported_item &func = imported_globals[i];

      if (func.name.empty()) continue;

      fprintf(out,"%s %s;\n",
         func.ret_type == ua_rt_void ? "void" :
            func.ret_type == ua_rt_int ? "int" : "string",
         func.name.c_str());
   }

   fprintf(out,"\n// forward declarations\n");

   // print out forward declarations
   max=forward_decl.size();
   for(i=0; i<max; i++)
      fprintf(out,"void %s();\n",forward_decl[i].c_str());

   fprintf(out,"\n// functions\n");

   // print out all dec entries
   bool last_translated = true;
   max = dec_entries.size();
   for(i=0; i<max; i++)
   {
      const ua_conv_dec_entry &entry = dec_entries[i];

      if (last_translated && !entry.translated)
         fprintf(out,"   /*_asm {\n");

      if (!last_translated && entry.translated)
         fprintf(out,"   };*/\n\n");

      if (!entry.label.empty())
      {
         fprintf(out,"%s:",entry.label.c_str());
         if (entry.translated) fprintf(out,";");
         fprintf(out,"\n");
      }

      if (entry.empty_line)
         fprintf(out,"\n");

      int indent = entry.indent_level;

      if (!entry.translated)
         indent+=2;

      for(int j=0; j<indent; j++) fprintf(out,"   ");

      fprintf(out,"%s\n",entry.command.c_str());

      last_translated = entry.translated;
   }

   if (!last_translated && dec_entries[max-1].translated)
   {
      fprintf(out,"   };\n");
   }
}

void ua_conv_dasm::load_dec()
{
   for(unsigned int instrp=0; instrp<codesize; instrp++)
   {
      ua_conv_dec_entry entry;

      entry.code = code[instrp];
      entry.arg = (ua_conv_instructions[entry.code].args==1) ? code[instrp+1] : 0;
      entry.instrp = instrp;
      entry.translated = false;
      entry.indent_level = 0;
      entry.empty_line = false;

      // generate command string
      char buffer[256],buffer2[256];
      sprintf(buffer,"%-10s%s",ua_conv_instructions[entry.code].mnemonic,
         ua_conv_instructions[entry.code].argcode);
      sprintf(buffer2,buffer,entry.arg);
      entry.command.assign(buffer2);

      if (ua_conv_instructions[entry.code].args==1)
         instrp++;

      dec_entries.push_back(entry);
   }
}

void ua_conv_dasm::resolve_labels()
{
   // first entry is label "main"
   dec_entries[0].label.assign("main");

   unsigned int i,max = dec_entries.size();
   for(i=0; i<max; i++)
   {
      ua_conv_dec_entry &entry = dec_entries[i];

      // only check out jumps
      if (entry.code == op_JMP || entry.code == op_BEQ || entry.code == op_BNE ||
         entry.code == op_BRA || entry.code == op_CALL)
      {
         // search for jump target
         Uint16 target = entry.arg;

         // correct relative targets for branch instructions
         if (entry.code == op_BEQ || entry.code == op_BNE ||
            entry.code == op_BRA)
            target += entry.instrp+1;

         for(unsigned int j=0; j<max; j++)
         {
            if (dec_entries[j].instrp == target)
            {
               // found jump target
               char buffer[256];
               if (entry.code == op_CALL)
                  sprintf(buffer,"func_%04x",target);
               else
                  sprintf(buffer,"label_%04x",target);
               dec_entries[j].label = buffer;

               char buffer2[256];
               sprintf(buffer2,"%-10s %s",
                  ua_conv_instructions[entry.code].mnemonic,buffer);

               entry.command = buffer2;
            }
         }
      }
   }
}

bool ua_conv_dasm::replace_constructs(unsigned int type, unsigned int entry, unsigned int max)
{
   char buffer[256],buffer2[256];

   switch (type)
   {
   case 0: // unused operations
      if (dec_entries[entry].code==op_START)
//          dec_entries[entry].code==op_EXIT_OP)
      {
         // kill opcode
         std::string label = dec_entries[entry].label;
         dec_entries.erase(dec_entries.begin()+entry);
         dec_entries[entry].label.append(label);
         return true;
      }
      break;

   case 1: // exit() function
      if (dec_entries[entry].code==op_EXIT_OP)
      {
         ua_conv_dec_entry &item = dec_entries[entry];
         item.translated=true;
         item.indent_level = 1;
         item.empty_line = true;
         item.command.assign("// exit();\n");
         return true;
      }
      break;

   case 2: // function entry code
      if (max-entry<4) break; // we need at least 4 instructions
      if (dec_entries[entry].code==op_PUSHBP &&
          dec_entries[entry+1].code==op_SPTOBP &&
          dec_entries[entry+2].code==op_PUSHI &&
          dec_entries[entry+3].code==op_ADDSP)
      {
         // found
         ua_conv_dec_entry &item = dec_entries[entry];
         item.translated=true;
         item.indent_level = 0;
         item.empty_line = true;

         if (item.label.empty())
         {
            sprintf(buffer,"not_used%04x",item.instrp);
            item.label.assign(buffer);
         }

         sprintf(buffer,"void %s()\n{\n",item.label.c_str());

         if (dec_entries[entry+2].arg==0)
            strcpy(buffer2,"   // no locals\n   int res;\n");
         else
            sprintf(buffer2,"   int locals[0x%04x];\n   int res;\n",
               dec_entries[entry+2].arg+1);
         item.command.assign(buffer);
         item.command.append(buffer2);

         // delete other entries
         dec_entries.erase(dec_entries.begin()+entry+1,dec_entries.begin()+entry+4 );

         // delete label, we have it in the function name
         item.label.erase();
         return true;
      }
      break;

   case 3: // function exit code
      if (max-entry<3) break;
      if (dec_entries[entry].code==op_BPTOSP &&
          dec_entries[entry+1].code==op_POPBP &&
          dec_entries[entry+2].code==op_RET)
      {
         ua_conv_dec_entry &item = dec_entries[entry];
         item.translated=true;
         item.command.assign("return;\n};\n");
         item.indent_level = 1;
         item.empty_line = true;

         // delete other entries
         dec_entries.erase(dec_entries.begin()+entry+1,dec_entries.begin()+entry+3);

         return true;
      }
      break;

   case 4: // store global/private value
      if (max-entry<3) break;
      if (dec_entries[entry].code==op_PUSHI &&
          dec_entries[entry+1].code==op_PUSHI &&
          dec_entries[entry+2].code==op_STO)
      {
         ua_conv_dec_entry &item = dec_entries[entry];
         item.translated=true;
         item.indent_level = 1;

         Uint16 ptr = dec_entries[entry].arg;
         Uint16 val = dec_entries[entry+1].arg;

         sprintf(buffer, "%s = 0x%04x;",format_global(ptr).c_str(),val);

         item.command.assign(buffer);

         // delete other entries
         dec_entries.erase(dec_entries.begin()+entry+1,dec_entries.begin()+entry+3);

         return true;
      }
      break;

   case 5: // say operation
      if (max-entry<2)
         break;
      if (dec_entries[entry].code==op_PUSHI &&
          dec_entries[entry+1].code==op_SAY_OP)
      {
         ua_conv_dec_entry &item = dec_entries[entry];
         item.translated=true;
         item.indent_level = 1;
         item.empty_line = true;

         std::string cmd("printf(\"");

         std::string say_string(gs.get_string(strblock,item.arg));

         for(unsigned int i=0; i<say_string.size(); i++)
         {
            if (say_string[i]=='\n')
            {
               say_string.erase(i,1);
               say_string.insert(i,"\\n\"\n      \"");
               i+=10;
            }
         }

         cmd.append(say_string);
         cmd.append("\");\n");

         item.command.assign(cmd);

         // delete other entries
         dec_entries.erase(dec_entries.begin()+entry+1);
         return true;
      }
      break;

   case 6: // store local var
      if (max-entry<5) break;
      if (dec_entries[entry].code==op_PUSHI &&
          dec_entries[entry+1].code==op_PUSHI_EFF &&
          dec_entries[entry+2].code==op_OFFSET &&
          dec_entries[entry+3].code==op_PUSHI &&
          dec_entries[entry+4].code==op_STO)
      {
         ua_conv_dec_entry &item = dec_entries[entry];
         item.translated=true;
         item.indent_level = 1;

         sprintf(buffer,"locals[0x%04x+0x%04x] = 0x%04x;",
            dec_entries[entry].arg,dec_entries[entry+1].arg-1,
            dec_entries[entry+3].arg);

         item.command.assign(buffer);
         item.arg = dec_entries[entry+3].arg;

         // delete other entries
         dec_entries.erase(dec_entries.begin()+entry+1,dec_entries.begin()+entry+5);
         return true;
      }
      break;

   case 7: // store local var, part 2
      if (max-entry<4) break;
      if (dec_entries[entry].code==op_PUSHI &&
          dec_entries[entry+1].code==op_PUSHI_EFF &&
          dec_entries[entry+2].code==op_SWAP &&
          dec_entries[entry+3].code==op_STO)
      {
         ua_conv_dec_entry &item = dec_entries[entry];
         item.translated=true;
         item.indent_level = 1;

         sprintf(buffer,"locals[0x%04x] = 0x%04x;",
            dec_entries[entry+1].arg,dec_entries[entry].arg);

         item.command.assign(buffer);

         // delete other entries
         dec_entries.erase(dec_entries.begin()+entry+1,dec_entries.begin()+entry+4);
         return true;
      }
      break;

   case 8: // store local var, part 3
      if (max-entry<3) break;
      if (dec_entries[entry].code==op_PUSHI_EFF &&
          dec_entries[entry+1].code==op_PUSHI &&
          dec_entries[entry+2].code==op_STO)
      {
         ua_conv_dec_entry &item = dec_entries[entry];
         item.translated=true;
         item.indent_level = 1;

         sprintf(buffer,"locals[0x%04x] = 0x%04x;",
            dec_entries[entry].arg,dec_entries[entry+1].arg);

         item.command.assign(buffer);
         item.arg = dec_entries[entry+1].arg;

         // delete other entries
         dec_entries.erase(dec_entries.begin()+entry+1,dec_entries.begin()+entry+3);
         return true;
      }
      break;

   case 9: // function call
      if (dec_entries[entry].code==op_CALLI ||
          dec_entries[entry].code==op_CALL)
      {
         std::string cmd,funcname;

         if (dec_entries[entry].code==op_CALL)
         {
            cmd = dec_entries[entry].command;
            std::string::size_type pos = cmd.find("func_");

            cmd.erase(0,pos);
            if (cmd.empty()) cmd.assign("unknown");

            // remember for forward decls
            forward_decl.push_back(cmd);

         }
         else
         {
            // find out function name
            if (dec_entries[entry].arg>imported_funcs.size())
               funcname.assign("unknown_exported");
            else
               funcname.assign(imported_funcs[dec_entries[entry].arg].name);

            cmd.assign("res = ");
            cmd.append(funcname);
         }

         // check for args
         cmd.append("(");

         // find out how many args by counting the POPs

         unsigned int i=entry+1, argcount = 0;
         while(i<max && dec_entries[i].code == op_POP)
            argcount++,i++;

         // delete the POPs
         dec_entries.erase(dec_entries.begin()+entry+1,dec_entries.begin()+entry+1+argcount);

         unsigned int count=argcount;

         // now try to get the args, hopefully there are only PUSHI and PUSHI_EFF's
         for(i=entry-1;count>0;count--,i--)
         {
            if (dec_entries[i].translated)
            {
               // only do non-translated entries
               count++;
               continue;
            }

            if (i!=entry-1)
               cmd.append(", ");

            if (i<max)
            switch(dec_entries[i].code)
            {
            case op_PUSHI:
               sprintf(buffer,"0x%04x",dec_entries[i].arg);
               break;
            case op_PUSHI_EFF:
               sprintf(buffer,"&locals[0x%04x]",dec_entries[i].arg);
               break;
            default:
               // should not happen :-(
               return false;
               break;
            }
            cmd.append(buffer);

            // delete that PUSHI/PUSHI_EFF entry
            dec_entries.erase(dec_entries.begin()+i);
         }

         cmd.append(");\n");

         ua_conv_dec_entry &item = dec_entries[entry-argcount];

         item.command.assign(cmd);
         item.translated=true;
         item.indent_level = 1;
         item.empty_line = true;

         if (dec_entries[entry-argcount].code==op_CALLI && 
            (funcname == "babl_menu" || funcname == "babl_fmenu" ))
         {
            entry -= argcount; entry--;
            std::string comment;

            while( dec_entries[entry].command.find("locals[") != std::string::npos )
            {
               if (dec_entries[entry].arg != 0)
               {
                  comment.assign(" // ");
                  comment.append(gs.get_string(strblock,dec_entries[entry].arg));

                  // replace newlines
                  for(unsigned int i=0; i<comment.size(); i++)
                  if (comment[i]=='\n')
                  {
                     comment.erase(i,1);
                     comment.insert(i,"\\n");
                  }

                  dec_entries[entry].command.append(comment);
               }
               entry--;
            }
         }

         return true;
      }
      break;

   case 10: // function call result store
      if (max-entry<5) break;
      if (dec_entries[entry].code==op_PUSH_REG &&
          dec_entries[entry+1].code==op_PUSHI_EFF &&
          dec_entries[entry+2].code==op_SWAP &&
          dec_entries[entry+3].code==op_STO)
      {
         ua_conv_dec_entry &item = dec_entries[entry];
         item.translated=true;
         item.indent_level = 1;

         sprintf(buffer,"locals[0x%04x] = res;\n",dec_entries[entry+1].arg);
         item.command.assign(buffer);

         // delete other entries
         dec_entries.erase(dec_entries.begin()+entry+1,dec_entries.begin()+entry+4);
      }
      break;

   case 11: // function call result store, part 2
      if (max-entry<2) break;
      if (dec_entries[entry].code==op_PUSH_REG &&
          dec_entries[entry+1].code==op_STO)
      {
         // search valid PUSHI/PUSHI_EFF opcode
         unsigned int pushi_pos = entry-1;
         while(entry<max && dec_entries[pushi_pos].translated) pushi_pos--;

         // check if we have found a nice one
         if (dec_entries[pushi_pos].code != op_PUSHI &&
             dec_entries[pushi_pos].code != op_PUSHI_EFF)
         {
            // we failed :-/
            return false;
         }

         ua_conv_dec_entry &item = dec_entries[entry];

         Uint16 ptr = dec_entries[pushi_pos].arg;

         if (dec_entries[pushi_pos].code == op_PUSHI)
            sprintf(buffer, "%s = reg;",format_global(ptr).c_str());
         else
            sprintf(buffer,"locals[0x%04x] = res;\n",ptr);

         item.command.assign(buffer);
         item.translated=true;
         item.indent_level = 1;

         // delete not needed entries
         dec_entries.erase(dec_entries.begin()+entry+1);
         dec_entries.erase(dec_entries.begin()+pushi_pos);
      }
      break;

   case 12: // search for "switch" and "if" statements; must always be the last ones
      if (max-entry<5) break;
      if ( (dec_entries[entry].code==op_PUSHI_EFF || dec_entries[entry].code==op_PUSHI ) &&
          dec_entries[entry+1].code==op_FETCHM &&
          dec_entries[entry+2].code==op_PUSHI &&
          dec_entries[entry+3].code==op_TSTEQ &&
          dec_entries[entry+4].code==op_BEQ)
      {
         return if_switch_replace(entry,max);
      }
      break;

/*
   case 11+100: // replace jumps and branches
      if (dec_entries[entry].code==op_JMP || dec_entries[entry].code==op_BRA)
      {
         ua_conv_dec_entry &item = dec_entries[entry];
         item.translated=true;
         item.indent_level = 1;

         std::string cmd(item.command);
         std::string::size_type pos = cmd.find("label_");

         cmd.erase(0,pos);

         if (cmd.empty()) cmd.assign("unknown");
         cmd.insert(0,"goto ");
         cmd.append(";\n");

         item.command.assign(cmd);

         return true;
      }
      break;*/
   }

   return false;
}

std::string ua_conv_dasm::format_global(Uint16 offset)
{
   std::string ret;
   char buffer[256];

   if (offset<imported_globals.size())
   {
      if (imported_globals[offset].name.empty())
      {
         sprintf(buffer,"global[0x%04x]",offset);
         ret = buffer;
      }
      else
         ret = imported_globals[offset].name;
   }
   else
   {
      sprintf(buffer,"private[0x%04x]",offset);
      ret = buffer;
   }

   return ret;
}

unsigned int ua_conv_dasm::search_instrp(unsigned int instrp)
{
   unsigned int max = dec_entries.size();
   for(unsigned int i=0; i<max; i++)
   if (dec_entries[i].instrp == instrp)
      return i;
   return 0;
}

bool ua_conv_dasm::if_switch_replace(unsigned int entry, unsigned int max)
{
   // first, follow the label_else, to look if we have "switch" or "if"
   Uint16 label_else_ip = dec_entries[entry+4].instrp+dec_entries[entry+4].arg+1;
   unsigned int label_else = search_instrp(label_else_ip);

   // check if we have "switch" or "if"
   bool have_switch=false;

   if (label_else!=0)
   {
      if (!dec_entries[label_else].translated &&
          (dec_entries[label_else].code==op_PUSHI_EFF || dec_entries[label_else].code==op_PUSHI ) &&
          dec_entries[label_else+1].code==op_FETCHM &&
          dec_entries[label_else+2].code==op_PUSHI &&
          dec_entries[label_else+3].code==op_TSTEQ &&
          dec_entries[label_else+4].code==op_BEQ)
      {
         // check args, too
         if (dec_entries[entry+1].arg == dec_entries[label_else+1].arg)
            have_switch=true;
      }
   }

   char buffer[256];

   if (!have_switch)
   {
      // the "if" case
      ua_conv_dec_entry &if_item = dec_entries[entry];

      // do the "if" line
      if (if_item.code == op_PUSHI_EFF)
         sprintf(buffer,"if (locals[0x%04x] == 0x%04x) {",
            dec_entries[entry].arg,dec_entries[entry+2].arg);
      else
      {
         sprintf(buffer,"if (%s == 0x%04x) {",
            format_global(dec_entries[entry].arg).c_str(),
            dec_entries[entry+2].arg);
      }

      if_item.command.assign(buffer);
      if_item.translated = true;
      if_item.indent_level++;
      if_item.empty_line = true;

      // go to the "label_else" and get "label_endif"

      Uint16 label_endif_ip = dec_entries[label_else-1].arg;

      // fix when a BRA opcode was used
      if (dec_entries[label_else-1].code == op_BRA)
         label_endif_ip += dec_entries[label_else-1].instrp+1;

      unsigned int label_endif = search_instrp(label_endif_ip);

      if (label_endif != 0)
      {
         if (label_endif == label_else)
         {
            // no "else"-part

            // do "label_endif" entry
            ua_conv_dec_entry &item_endif = dec_entries[label_else-1];
            item_endif.command.assign("}; // endif w/o else");
            item_endif.label.erase();
            item_endif.translated = true;
         }
         else
         {
            // endif label, we don't need it anymore
            dec_entries[label_endif].label.erase();

            // do "label_endif" entry

            ua_conv_dec_entry item_endif = dec_entries[label_endif];
            item_endif.command.assign("}; // endif");
            item_endif.empty_line = false;
            item_endif.translated = true;

            dec_entries.insert(dec_entries.begin()+label_endif,item_endif);

            // convert last jump from "then"-part to "label_else" entry
            ua_conv_dec_entry &item_else = dec_entries[label_else-1];
            item_else.command.assign("} else {");
            item_else.indent_level++;
            item_else.translated = true;
            item_else.label.erase();

            dec_entries[label_else].label.erase();
         }

         // indent "then"-part
         unsigned int start,stop;

         start = entry+5; stop = label_else-1;
         for(;start<stop; start++)
            dec_entries[start].indent_level++;

         // indent "else"-part
         start = label_else; stop = label_endif;
         for(;start<stop; start++)
            dec_entries[start].indent_level++;
      }

      // delete all no more used opcodes
      dec_entries.erase(dec_entries.begin()+entry+1,dec_entries.begin()+entry+5);

      return true;
   }
   else
   {
      // the "switch" case
      ua_conv_dec_entry switch_item = dec_entries[entry];

      // do the "switch" line
      if (switch_item.code == op_PUSHI_EFF)
         sprintf(buffer,"switch (locals[0x%04x]) {",
            dec_entries[entry].arg);
      else
         sprintf(buffer,"switch (%s) {",
            format_global(dec_entries[entry].arg).c_str());

      switch_item.command.assign(buffer);
      switch_item.translated = true;
      switch_item.empty_line = false;
      switch_item.indent_level++;

      unsigned int case_item = entry;
      Uint16 orig_arg = dec_entries[entry].arg;

      // the loop
      do
      {
         // next "case"

         ua_conv_dec_entry &item = dec_entries[case_item];

         // do the "case" line
         sprintf(buffer,"case 0x%04x:",dec_entries[case_item+2].arg);

         item.command.assign(buffer);
         item.translated = true;
         item.indent_level++;
         item.empty_line = false;
         item.label.erase();

         // search next case
         unsigned int case_item_ip = dec_entries[case_item+4].instrp+dec_entries[case_item+4].arg+1;

         // delete rest of opcodes
         dec_entries.erase(dec_entries.begin()+case_item+1,dec_entries.begin()+case_item+5);

         Uint16 case_item_next = search_instrp(case_item_ip);
         if (case_item_next==0)
            break;

         // replace the previous "JMP label_endswitch" with a "break;"
         ua_conv_dec_entry &break_item = dec_entries[case_item_next-1];
         break_item.command.assign("break;\n");
         break_item.translated = true;
         break_item.indent_level++;

         // indent all opcodes up to the next "case"
         unsigned int start,stop;

         start = case_item+1; stop = case_item_next;
         for(;start<stop; start++)
            dec_entries[start].indent_level++;

         // check if next one is a valid case item
         if ((dec_entries[case_item_next+0].code!=op_PUSHI_EFF && dec_entries[case_item_next+0].code!=op_PUSHI_EFF ) ||
             dec_entries[case_item_next+1].code!=op_FETCHM ||
             dec_entries[case_item_next+2].code!=op_PUSHI ||
             dec_entries[case_item_next+3].code!=op_TSTEQ ||
             dec_entries[case_item_next+4].code!=op_BEQ ||
             dec_entries[case_item_next].arg != orig_arg)
         {
            // switch statement end

            // get rid of switch_end label
            dec_entries[case_item_next].label.erase();

            // insert "switch_end" element
            ua_conv_dec_entry switch_end = dec_entries[case_item_next-1];
            switch_end.command.assign("}; // end switch\n");
            switch_end.translated = true;
            switch_end.indent_level--;
            dec_entries.insert(dec_entries.begin()+case_item_next,switch_end);
            break;
         }

         // next item
         case_item = case_item_next;
      }
      while (1);

      // finally, insert "switch" statement
      dec_entries.insert(dec_entries.begin()+entry,switch_item);

      return true;
   }
}


// main function

// we don't need SDL's main here
#undef main


int main(int argc, char *argv[])
{
   printf("cnvdasm - ultima underworld 1 conversation script disassembler\n\n");

   if (argc<4)
   {
      printf("syntax: cnvdasm <command> <cnv-archive> <conv-slot>\n"
         "   command can either be \"dasm\" (=disassembler) or \"dec\" (=decompiler).\n"
         "   examples: cnvdasm dasm data/cnv.ark 1\n"
         "   cnvdasm dec data/cnv.ark 3\n\n");
      return 0;
   }

   ua_conv_dasm dasm;
   if (dasm.init(argv[2],atoi(argv[3])))
   {
      if (0==strcmp(argv[1],"dasm"))
         dasm.disassemble(stdout);
      else
         dasm.decompile(stdout);
   }

   return 0;
}
