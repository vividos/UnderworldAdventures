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
/*! \file codevm.cpp

   \brief conv code execution functions

*/

// needed includes
#include "common.hpp"
#include "codevm.hpp"
#include "opcodes.hpp"
#include <sstream>


// ua_conv_code_vm methods

ua_conv_code_vm::ua_conv_code_vm()
:code_callback(NULL)
{
   instrp = basep = result_register = 0xffff;
   call_level = glob_reserved = 0;
   finished = true;
}

ua_conv_code_vm::~ua_conv_code_vm()
{
}

void ua_conv_code_vm::init(ua_conv_code_callback* the_code_callback,
   ua_conv_globals &cg)
{
   code_callback = the_code_callback;

   // reset pointer
   instrp = 0;
   basep = 0xffff;
   result_register = 0;
   finished = false;
   call_level=1;

   // init stack: 4k should be enough for anybody.
   stack.init(4096);

   // reserve stack for globals/private globals
   stack.set_stackp(glob_reserved);

   // load private globals onto stack
   {
      const std::vector<Uint16>& glob = cg.get_globals(conv_slot);

      unsigned int max=glob.size();
      for(unsigned int i=0; i<max; i++)
         stack.set(i,glob[i]);
   }

   // load imported globals onto stack
   {
      std::map<Uint16,ua_conv_imported_item>::iterator iter,stop;
      iter = imported_globals.begin();
      stop = imported_globals.end();

      for(;iter!=stop; iter++)
      {
         Uint16 pos = (*iter).first;
         ua_conv_imported_item& iitem = (*iter).second;

         Uint16 val = get_global(iitem.name.c_str());

         stack.set(pos,val);
      }
   }
}

void ua_conv_code_vm::done(ua_conv_globals &cg)
{
   if (conv_slot==0xffff)
      return;

   // store back globals from stack
   std::vector<Uint16>& glob = cg.get_globals(conv_slot);

   unsigned int max=glob.size();
   for(unsigned int i=0; i<max; i++)
      glob[i] = stack.at(i);
}

bool ua_conv_code_vm::step()
{
   if (finished)
      return false;

   ua_assert(instrp<code.size());

   Uint16 arg1=0,arg2=0;
   Uint16 opcode = code[instrp];

   // execute one instruction
   switch(opcode)
   {
   case op_NOP:
      break;

   case op_OPADD:
      arg1 = stack.pop();
      arg2 = stack.pop();
      stack.push(arg1 + arg2);
      break;

   case op_OPMUL:
      arg1 = stack.pop();
      arg2 = stack.pop();
      stack.push(arg1 * arg2);
      break;

   case op_OPSUB:
      arg1 = stack.pop();
      arg2 = stack.pop();
      stack.push(arg2 - arg1);
      break;

   case op_OPDIV:
      arg1 = stack.pop();
      arg2 = stack.pop();
      if (arg1==0)
      {
         ua_trace("code_vm: OPDIV: division by zero\n");
         finished = true;
         return false;
      }
      stack.push(arg2 / arg1);
      break;

   case op_OPMOD:
      arg1 = stack.pop();
      arg2 = stack.pop();
      if (arg1==0)
      {
         ua_trace("code_vm: OPMOD: division by zero\n");
         finished = true;
         return false;
      }
      stack.push(arg2 % arg1);
      break;

   case op_OPOR:
      arg1 = stack.pop();
      arg2 = stack.pop();
      stack.push(arg2 || arg1);
      break;

   case op_OPAND:
      arg1 = stack.pop();
      arg2 = stack.pop();
      stack.push(arg2 && arg1);
      break;

   case op_OPNOT:
      stack.push(!stack.pop());
      break;

   case op_TSTGT:
      arg1 = stack.pop();
      arg2 = stack.pop();
      stack.push(arg2 > arg1);
      break;

   case op_TSTGE:
      arg1 = stack.pop();
      arg2 = stack.pop();
      stack.push(arg2 >= arg1);
      break;

   case op_TSTLT:
      arg1 = stack.pop();
      arg2 = stack.pop();
      stack.push(arg2 < arg1);
      break;

   case op_TSTLE:
      arg1 = stack.pop();
      arg2 = stack.pop();
      stack.push(arg2 <= arg1);
      break;

   case op_TSTEQ:
      arg1 = stack.pop();
      arg2 = stack.pop();
      stack.push(arg2 == arg1);
      break;

   case op_TSTNE:
      arg1 = stack.pop();
      arg2 = stack.pop();
      stack.push(arg2 != arg1);
      break;

   case op_JMP:
      instrp = code[instrp+1]-1;
      break;

   case op_BEQ:
      arg1 = stack.pop();
      if (arg1 == 0)
         instrp += code[instrp+1];
      else
         instrp++;
      break;

   case op_BNE:
      arg1 = stack.pop();
      if (arg1 != 0)
         instrp += code[instrp+1];
      else
         instrp++;
      break;

   case op_BRA:
      instrp += code[instrp+1];
      break;

   case op_CALL: // local function
      // stack value points to next instruction after call
      stack.push(instrp+1);
      instrp = code[instrp+1]-1;
      call_level++;
      break;

   case op_CALLI: // imported function
      {
         arg1 = code[++instrp];

         std::string funcname;
         if (imported_funcs.find(arg1) == imported_funcs.end())
         {
            ua_trace("code_vm: couldn't find imported function 0x%04x\n",arg1);
            finished = true;
            return false;
         }

         imported_func(imported_funcs[arg1].name.c_str());
      }
      break;

   case op_RET:
      if (--call_level)
      {
         // conversation ended
         finished = true;
      }
      else
      {
         arg1 = stack.pop();
         instrp = arg1;
      }
      break;

   case op_PUSHI:
      stack.push(code[++instrp]);
         break;

   case op_PUSHI_EFF:
      stack.push(basep + (Sint16)code[++instrp]);
      break;

   case op_POP:
      stack.pop();
      break;

   case op_SWAP:
      arg1 = stack.pop();
      arg2 = stack.pop();
      stack.push(arg1);
      stack.push(arg2);
      break;

   case op_PUSHBP:
      stack.push(basep);
      break;

   case op_POPBP:
      arg1 = stack.pop();
      basep = arg1;
      break;

   case op_SPTOBP:
      basep = stack.get_stackp();
      break;

   case op_BPTOSP:
      stack.set_stackp(basep);
      break;

   case op_ADDSP:
      {
         arg1 = stack.pop();

         // fill reserved stack space with dummy values
         for(int i=0; i<arg1; i++)
            stack.push(0xdddd);
      }
      break;

   case op_FETCHM:
      arg1 = stack.pop();

      fetch_value(arg1);

      stack.push(stack.at(arg1));
      break;

   case op_STO:
      arg1 = stack.pop();
      arg2 = stack.pop();

      store_value(arg2,arg1);

      stack.set(arg2,arg1);
      break;

   case op_OFFSET:
      arg1 = stack.pop();
      arg2 = stack.pop();
      arg1 += arg2 - 1 ;
      stack.push(arg1);
      break;

   case op_START:
      // do nothing
      break;

   case op_SAVE_REG:
      arg1 = stack.pop();
      result_register = arg1;
      break;

   case op_PUSH_REG:
      stack.push(result_register);
      break;

   case op_EXIT_OP:
      // finish processing (we still might be in some sub function)
      finished = true;
      break;

   case op_SAY_OP:
      arg1 = stack.pop();
      say_op(arg1);
      break;

   case op_RESPOND_OP:
      // do nothing
      break;

   case op_OPNEG:
      arg1 = stack.pop();
      stack.push(-arg1);
      break;

   default: // unknown opcode
      ua_trace("code_vm: unknown opcode 0x%04x\n",opcode);
      finished=true;
      break;
   }

   // process next instruction
   ++instrp;

   return !finished;
}

void ua_conv_code_vm::replace_placeholder(std::string& str)
{
   std::string::size_type pos = 0;
   while( (pos = str.find('@',pos)) != std::string::npos )
   {
      char source = str[pos+1];
      char vartype = str[pos+2];

      signed int param = 0;
      unsigned int value = 0;
      unsigned int num_len = 0;
      {
         const char* startpos = str.c_str()+pos;
         char* endpos = NULL;
         param = (signed int)strtol(startpos+3,&endpos,10);
         num_len = endpos-startpos;
      }

      // get param value
      switch(source)
      {
      case 'G':
         value = stack.at(static_cast<unsigned int>(param));
         break;
      case 'S':
         value = stack.at(static_cast<unsigned int>(basep+param));
         break;
      case 'P':
         param = stack.at(static_cast<unsigned int>(basep+param));
         value = stack.at(static_cast<unsigned int>(param));
         break;
      }

      std::string varstr;

      switch(vartype)
      {
      case 'S':
         varstr = code_callback->get_local_string(value);
         break;
      case 'I':
         {
            std::ostringstream buffer;
            buffer << value << std::ends;
            varstr.assign(buffer.str());
         }
         break;
      }

      // insert value string
      str.replace(pos,num_len,varstr.c_str());
   }
}

void ua_conv_code_vm::set_result_register(Uint16 val)
{
   result_register = val;
}

void ua_conv_code_vm::imported_func(const char* funcname)
{
   ua_trace("code_vm: executing function \"%s\" with %u arguments\n",
      funcname, stack.at(stack.get_stackp()));

   result_register = code_callback->external_func(funcname, stack);
}

void ua_conv_code_vm::say_op(Uint16 index)
{
   code_callback->say(index);
}

Uint16 ua_conv_code_vm::get_global(const char* globname)
{
   Uint16 val = code_callback->get_global(globname);
   ua_trace("code_vm: get_global %s returned %04x\n",globname, val);
   return val;
}

void ua_conv_code_vm::set_global(const char* globname, Uint16 val)
{
   ua_trace("code_vm: set_global %s = %04x\n", globname, val);
   code_callback->set_global(globname, val);
}

void ua_conv_code_vm::store_value(Uint16 at, Uint16 val)
{
   std::map<Uint16,ua_conv_imported_item>::iterator iter =
      imported_globals.find(at);

   if (iter!=imported_globals.end())
      ua_trace("code_vm: storing: %s = %04x\n",iter->second.name.c_str(),val);
}

void ua_conv_code_vm::fetch_value(Uint16 at)
{
   std::map<Uint16,ua_conv_imported_item>::iterator iter =
      imported_globals.find(at);

   if (iter!=imported_globals.end())
      ua_trace("code_vm: fetching %s returned %04x\n",iter->second.name.c_str(),stack.at(at));
}
