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
   case 0x0000: // NOP
      break;

   case 0x0001: // OPADD
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg1 + arg2);
      }
      break;

   case 0x0002: // OPMUL
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg1 * arg2);
      }
      break;

   case 0x0003: // OPSUB
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg2 - arg1);
      }
      break;

   case 0x0004: // OPDIV
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         if (arg1==0)
            throw ua_ex_div_by_zero;
         stack.push(arg2 / arg1);
      }
      break;

   case 0x0005: // OPMOD
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         if (arg1==0)
            throw ua_ex_div_by_zero;
         stack.push(arg2 % arg1);
      }
      break;

   case 0x0006: // OPOR
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg2 || arg1);
      }
      break;

   case 0x0007: // OPAND
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg2 && arg1);
      }
      break;

   case 0x0008: // OPNOT
      stack.push(!stack.pop());
      break;

   case 0x0009: // TSTGT
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg2 > arg1);
      }
      break;

   case 0x000A: // TSTGE
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg2 >= arg1);
      }
      break;

   case 0x000B: // TSTLT
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg2 < arg1);
      }
      break;

   case 0x000C: // TSTLE
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg2 <= arg1);
      }
      break;

   case 0x000D: // TSTEQ
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg2 == arg1);
      }
      break;

   case 0x000E: // TSTNE
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg2 != arg1);
      }
      break;

   case 0x000F: // JMP
      instrp = code[instrp+1]-1;
      break;

   case 0x0010: // BEQ
      {
         Uint16 arg1 = stack.pop();
         if (arg1 == 0)
            instrp += code[instrp+1];
         else
            instrp++;
      }
      break;

   case 0x0011: // BNE
      {
         Uint16 arg1 = stack.pop();
         if (arg1 != 0)
            instrp += code[instrp+1];
         else
            instrp++;
      }
      break;

   case 0x0012: // BRA
      instrp += code[instrp+1]+1;
      break;

   case 0x0013: // CALL local
      // stack value points to next instruction after call
      stack.push(instrp+1);
      instrp = code[instrp+1]-1;
      call_level++;
      break;

   case 0x0014: // CALL imported
      {
         Uint16 arg1 = code[++instrp];
         imported_func(arg1);
      }
      break;

   case 0x0015: // RET
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

   case 0x0016: // PUSHI
      stack.push(code[++instrp]);
      break;

   case 0x0017: // PUSHI_EFF
      stack.push(basep + code[++instrp]);
      break;

   case 0x0018: // POP
      stack.pop();
      break;

   case 0x0019: // SWAP
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         stack.push(arg1);
         stack.push(arg2);
      }
      break;

   case 0x001A: // PUSHBP
      stack.push(basep);
      break;

   case 0x001B: // POPBP
      {
         Uint16 arg1 = stack.pop();
         basep = arg1;
      }
      break;

   case 0x001C: // SPTOBP
      basep = stack.get_stackp();
      break;

   case 0x001D: // BPTOSP
      stack.set_stackp(basep);
      break;

   case 0x001E: // ADDSP
      {
         Uint16 arg1 = stack.pop();

         // fill reserved stack space with dummy values
         for(int i=0; i<arg1; i++)
            stack.push(0xdddd);
      }
      break;

   case 0x001F: // FETCHM
      {
         Uint16 arg1 = stack.pop();

         fetchm_priv(arg1);

         stack.push(stack.at(arg1));
      }
      break;

   case 0x0020: // STO
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();

         sto_priv(arg2,arg1);

         stack.set(arg2,arg1);
      }
      break;

   case 0x0021: // OFFSET
      {
         Uint16 arg1 = stack.pop();
         Uint16 arg2 = stack.pop();
         arg1 += arg2 - 1 ;
         stack.push(arg1);
      }
      break;

   case 0x0022: // START
      // do nothing (or?)
      break;

   case 0x0023: // SAVE_REG
      {
         Uint16 arg1 = stack.pop();
         result_register = arg1;
      }
      break;

   case 0x0024: // PUSH_REG
      stack.push(result_register);
      break;

   case 0x0026: // EXIT_OP
      // do nothing (or?)
      break;

   case 0x0027: // SAY_OP
      {
         Uint16 arg1 = stack.pop();
         say_op(arg1);
      }
      break;

   case 0x0028: // RESPOND_OP
      break;

   case 0x0029: // OPNEG
      {
         Uint16 arg1 = stack.pop();
         stack.push(-arg1);
      }
      break;

   default: // unknown opcode
      throw ua_ex_unk_opcode;
      break;
   }

   ++instrp;
}
