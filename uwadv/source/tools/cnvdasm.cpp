/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Underworld Adventures Team

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

   \brief conversation disassembler and decompiler

*/

// needed includes
#include "common.hpp"
#include "cnvdasm.hpp"
#include "conv/opcodes.hpp"
#include "conv/convgraph.hpp"


// ua_conv_dasm methods

bool ua_conv_dasm::init(ua_gamestrings* mygs, const char *cnvfile,
   unsigned int conv)
{
   gs = mygs;

   // check if string block is available
   {
      std::map<int,std::vector<std::string> >& allstrings = gs->get_allstrings();

      if (allstrings.find(0x0e00+conv) == allstrings.end())
      {
         printf("could not find string block 0x%04x.",0x0e00+conv);
         return false;
      }
   }

   if (!ua_conv_code_vm::load_code(cnvfile,conv))
   {
      printf("could not load code ...\n");
      return false;
   }

   return true;
}

void ua_conv_dasm::disassemble()
{
   // collect cross refs
   collect_xrefs();

   // output all stuff

   printf(";\n; conversation #%u, string block 0x%04x\n"
      "; npc name: %s\n;\n\n",conv_nr,strblock,
      gs->get_string(7,conv_nr+16).c_str());

   unsigned int max = code.size();
   for(unsigned int i=0; i<max; i++)
   {
      // output xrefs
      {
         std::map<Uint16,std::vector<Uint16> >::iterator iter = all_xrefs.find(i);

         if (iter != all_xrefs.end())
         {
            printf("\n ;| xref:");

            const std::vector<Uint16>& xrefs = (*iter).second;

            unsigned int max = xrefs.size();
            for(unsigned int i=0; i<max; i++)
               printf(" %04x",xrefs[i]);

            printf("\n");
         }
      }

      // output opcode/arg
      {
         Uint16 opcode = code[i];
         Uint16 arg = 0;
         Uint16 pos = i;

         if (opcode<=op_last && ua_conv_instructions[opcode].args>0)
            arg = code[++i];

         printf("%04x:   ",pos);

         print_opcode(opcode,arg,pos);

         if (opcode == op_RET) printf("\n");
      }

      printf("\n");
   }

   printf("\n; conversation #%u ended\n\n",conv_nr);
}

void ua_conv_dasm::decompile(bool with_opcodes)
{
   ua_conv_graph cgraph;

   // load code into conversation graph and process
   cgraph.init(code,0,code.size(),gs->get_block(strblock));
   cgraph.process();

   // TODO: output code
}


void ua_conv_dasm::collect_xrefs()
{
   unsigned int max = code.size();
   for(unsigned int i=0; i<max; i++)
   {
      Uint16 opcode = code[i];

      if (opcode == op_JMP || opcode == op_BEQ || opcode == op_BNE ||
          opcode == op_BRA || opcode == op_CALL)
      {
         // calculate jump target
         Uint16 target = code[i+1];

         // adjust for relative jumps
         if (opcode == op_BEQ || opcode == op_BNE || opcode == op_BRA)
            target += i+1;

         // search for target in xrefs
         std::map<Uint16,std::vector<Uint16> >::iterator iter = all_xrefs.find(target);

         if (iter == all_xrefs.end())
         {
            // no item yet; create new xref list
            std::vector<Uint16> new_vect;
            new_vect.push_back(i);

            all_xrefs.insert(
               std::make_pair<Uint16,std::vector<Uint16> >(target,new_vect));
         }
         else
         {
            // add position to existing xrefs
            (*iter).second.push_back(i);
         }
      }

      // advance position when argument was present
      if (opcode<=op_last && ua_conv_instructions[opcode].args>0)
         ++i;
   }
}

void ua_conv_dasm::print_opcode(Uint16 opcode, Uint16 arg, Uint16 pos)
{
   if (opcode>op_last)
   {
      printf("??? (0x%04x)",opcode);
   }
   else
   {
      printf(ua_conv_instructions[opcode].mnemonic);
      printf(ua_conv_instructions[opcode].argcode,arg);
   }

   if (opcode == op_BRA || opcode == op_BEQ || opcode == op_BNE)
      printf(" // target: %04x",(Sint16)arg+pos+1);
}


// main function

// we don't need SDL's main here
#undef main


int main(int argc, char *argv[])
{
   printf("cnvdasm - ultima underworld conversation script disassembler / decompiler\n\n");

   if (argc<4)
   {
      printf("syntax: cnvdasm <command> <cnv-archive> <conv-slot-start> [<conv-slot-end>]\n"
         "   command can either be \"dasm\" (= disassembler), \"dec\" (= decompiler) or\n"
         "   \"decasm\" (= decompiler with disassembled opcodes).\n"
         "   <conv-slot-end> can be omitted.\n\n"
         "   examples: cnvdasm dasm data/cnv.ark 1 320\n"
         "   cnvdasm dec data/cnv.ark 4\n\n");
      return 0;
   }

   unsigned int start = atoi(argv[3]);
   unsigned int end = argc>4 ? atoi(argv[4]) : start;

   // load game strings
   ua_gamestrings gs;
   {
      printf("loading game strings");

      // construct game strings file name
      std::string gstrname(argv[2]);

      std::string::size_type pos = gstrname.find("cnv.ark");
      if (pos!=std::string::npos)
      {
         gstrname.erase(pos);
         gstrname.append("strings.pak");

         // try loading game strings
         try
         {
            gs.load(gstrname.c_str());
         }
         catch(...)
         {
            printf("failed loading game strings from %s\n",gstrname.c_str());
            return 0;
         }
      }

      printf("\n\n");
   }

   // determine what to do
   if (0==strcmp(argv[1],"dasm"))
   {
      // disassemble all slots
      for(unsigned int slot=start; slot<=end; slot++)
      {
         ua_conv_dasm dasm;

         if (dasm.init(&gs,argv[2],slot))
            dasm.disassemble();
      }
   }
   else
   if ( 0==strcmp(argv[1],"dec") || 0==strcmp(argv[1],"decasm") )
   {
      bool with_opcodes = 0==strcmp(argv[1],"decasm");

      // decompile all slots
      for(unsigned int slot=start; slot<=end; slot++)
      {
         ua_conv_dasm dasm;

         if (dasm.init(&gs,argv[2],slot))
            dasm.decompile(with_opcodes);
      }
   }
   else
      printf("unknown command %s.",argv[1]);

   return 0;
}
