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
/*! \file codevm.cpp

   \brief conv code execution functions

   ua_conv_code_vm::step() executes one opcode per call, hopefully mimic'ing
   the original virtual machine as best as possible

*/

// needed includes
#include "common.hpp"
#include "codevm.hpp"
#include "opcodes.hpp"


// ua_conv_globals methods

void ua_conv_globals::load_game(ua_savegame &sg)
{
   sg.begin_section("conv.globals");

   allglobals.clear();

   // get number of slots
   Uint16 max1 = sg.read16();

   for(Uint16 i=0; i<max1; i++)
   {
      // get number of slot globals
      Uint16 max2 = sg.read16();

      std::vector<Uint16> slotglobals;

      // get all slot globals
      for(Uint16 j=0; j<max2; j++)
         slotglobals.push_back(sg.read16());

      allglobals.push_back(slotglobals);
   }

   sg.end_section();
}

void ua_conv_globals::save_game(ua_savegame &sg)
{
   sg.begin_section("conv.globals");

   // write number of conv slots
   unsigned int max1 = allglobals.size();
   sg.write16(max1);

   // for each slot ...
   for(unsigned int i=0; i<max1; i++)
   {
      // write number of slot globals
      unsigned int max2 = allglobals[i].size();
      sg.write16(max2);

      // write all globals
      for(unsigned int j=0; j<max2; j++)
         sg.write16(allglobals[i][j]);
   }

   sg.end_section();
}


// ua_conv_code_vm methods

ua_conv_code_vm::ua_conv_code_vm()
{
   conv_nr = codesize = 0xffff;
   instrp = basep = result_register = 0xffff;
   call_level = glob_reserved = 0;
   finished = true;
}

ua_conv_code_vm::~ua_conv_code_vm()
{
}

void ua_conv_code_vm::init(ua_conv_globals &cg)
{
   // reset pointer
   instrp = 0;
   basep = 0xffff;
   result_register = 0;
   finished = false;

   call_level=1;

   // init stack: 4k should be enough for anybody.
   stack.init(4096);

   // load globals into stack
   const std::vector<Uint16> &glob = cg.get_globals(conv_nr);

   unsigned int max=glob.size();
   for(unsigned int i=0; i<max; i++)
      stack.push(glob[i]);
}

void ua_conv_code_vm::done(ua_conv_globals &cg)
{
   if (conv_nr==0xffff)
      return;

   // store back globals from stack
   std::vector<Uint16> &glob = cg.get_globals(conv_nr);

   unsigned int max=glob.size();
   for(unsigned int i=0; i<max; i++)
      glob[i] = stack.at(i);
}

void ua_conv_code_vm::step() throw(ua_conv_vm_exception)
{
   if (instrp>code.size())
      throw ua_ex_code_access;

   // execute one instruction
   switch(code[instrp])
   {
   case op_NOP:
      break;

   case op_OPADD:
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg1 + arg2);
      }
      break;

   case op_OPMUL:
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg1 * arg2);
      }
      break;

   case op_OPSUB:
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg2 - arg1);
      }
      break;

   case op_OPDIV:
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         if (arg1==0)
            throw ua_ex_div_by_zero;
         stack.push(arg2 / arg1);
      }
      break;

   case op_OPMOD:
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         if (arg1==0)
            throw ua_ex_div_by_zero;
         stack.push(arg2 % arg1);
      }
      break;

   case op_OPOR:
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg2 || arg1);
      }
      break;

   case op_OPAND:
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg2 && arg1);
      }
      break;

   case op_OPNOT:
      stack.push(!stack.pop());
      break;

   case op_TSTGT:
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg2 > arg1);
      }
      break;

   case op_TSTGE:
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg2 >= arg1);
      }
      break;

   case op_TSTLT:
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg2 < arg1);
      }
      break;

   case op_TSTLE:
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg2 <= arg1);
      }
      break;

   case op_TSTEQ:
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg2 == arg1);
      }
      break;

   case op_TSTNE:
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg2 != arg1);
      }
      break;

   case op_JMP:
      instrp = code[instrp+1]-1;
      break;

   case op_BEQ:
      {
         Uint16 arg1 = stack.pop();
         if (arg1 == 0)
            instrp += code[instrp+1];
         else
            instrp++;
      }
      break;

   case op_BNE:
      {
         Uint16 arg1 = stack.pop();
         if (arg1 != 0)
            instrp += code[instrp+1];
         else
            instrp++;
      }
      break;

   case op_BRA:
      instrp += code[instrp+1]+1;
      break;

   case op_CALL: // local function
      // stack value points to next instruction after call
      stack.push(instrp+1);
      instrp = code[instrp+1]-1;
      call_level++;
      break;

   case op_CALLI: // imported function
      {
         Uint16 arg1 = code[++instrp];
         imported_func(arg1);
      }
      break;

   case op_RET:
      {
         if (--call_level)
         {
            // conversation ended
            finished = true;
         }
         else
         {
            Uint16 arg1 = stack.pop();
            instrp = arg1;
         }
      }
      break;

   case op_PUSHI:
      stack.push(code[++instrp]);
      break;

   case op_PUSHI_EFF:
      stack.push(basep + code[++instrp]);
      break;

   case op_POP:
      stack.pop();
      break;

   case op_SWAP:
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg1);
         stack.push(arg2);
      }
      break;

   case op_PUSHBP:
      stack.push(basep);
      break;

   case op_POPBP:
      {
         Uint16 arg1 = stack.pop();
         basep = arg1;
      }
      break;

   case op_SPTOBP:
      basep = stack.get_stackp();
      break;

   case op_BPTOSP:
      stack.set_stackp(basep);
      break;

   case op_ADDSP:
      {
         Uint16 arg1 = stack.pop();

         // fill reserved stack space with dummy values
         for(int i=0; i<arg1; i++)
            stack.push(0xdddd);
      }
      break;

   case op_FETCHM:
      {
         Uint16 arg1 = stack.pop();

         fetchm_priv(arg1);

         stack.push(stack.at(arg1));
      }
      break;

   case op_STO:
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();

         sto_priv(arg2,arg1);

         stack.set(arg2,arg1);
      }
      break;

   case op_OFFSET:
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         arg1 += arg2 - 1 ;
         stack.push(arg1);
      }
      break;

   case op_START:
      // do nothing
      break;

   case op_SAVE_REG:
      {
         Uint16 arg1 = stack.pop();
         result_register = arg1;
      }
      break;

   case op_PUSH_REG:
      stack.push(result_register);
      break;

   case op_EXIT_OP:
      // finish processing (we still might be in some sub function)
      finished = true;
      break;

   case op_SAY_OP:
      {
         Uint16 arg1 = stack.pop();
         say_op(arg1);
      }
      break;

   case op_RESPOND_OP:
      // do nothing
      break;

   case op_OPNEG:
      {
         Uint16 arg1 = stack.pop();
         stack.push(-arg1);
      }
      break;

   default: // unknown opcode
      throw ua_ex_unk_opcode;
      break;
   }

   // process next instruction
   ++instrp;
}
