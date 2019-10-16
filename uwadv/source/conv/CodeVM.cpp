//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file CodeVM.cpp
/// \brief conv code execution functions
//
#include "pch.hpp"
#include "CodeVM.hpp"
#include "Opcodes.hpp"
#include <sstream>

using Conv::CodeVM;

CodeVM::CodeVM()
   :m_conversationSlot(0),
   m_stringBlock(0),
   m_instructionPointer(0xffff),
   m_basePointer(0xffff),
   m_reservedGlobals(0),
   m_callLevel(0),
   m_resultRegister(0xffff),
   m_finished(true),
   m_codeCallback(NULL)
{
}

void CodeVM::Init(Conv::ICodeCallback* codeCallback,
   const Underworld::ConvGlobals& globals)
{
   m_codeCallback = codeCallback;

   // reset pointer
   m_instructionPointer = 0;
   m_basePointer = 0xffff;
   m_resultRegister = 0;
   m_finished = false;
   m_callLevel = 1;

   // init stack: 4k should be enough for anybody.
   m_stack.Init(4096);

   // reserve stack for globals/private globals
   m_stack.SetStackPointer(m_reservedGlobals);

   // load private globals onto stack
   {
      const std::vector<Uint16>& slotGlobals = globals.GetSlotGlobals(m_conversationSlot);

      unsigned int max = static_cast<Uint16>(slotGlobals.size());
      for (unsigned int i = 0; i < max; i++)
         m_stack.Set(i, slotGlobals[i]);
   }

   // load imported globals onto stack
   {
      std::map<Uint16, ImportedItem>::iterator iter, stop;
      iter = m_mapImportedGlobals.begin();
      stop = m_mapImportedGlobals.end();

      for (; iter != stop; ++iter)
      {
         Uint16 pos = (*iter).first;
         ImportedItem& iitem = (*iter).second;

         Uint16 val = GetGlobal(iitem.name.c_str());

         m_stack.Set(pos, val);
      }
   }
}

void CodeVM::Done(Underworld::ConvGlobals& globals)
{
   if (m_conversationSlot == 0xffff)
      return;

   // store back globals from stack
   std::vector<Uint16>& slotGlobals = globals.GetSlotGlobals(m_conversationSlot);

   unsigned int max = static_cast<Uint16>(slotGlobals.size());
   for (unsigned int i = 0; i < max; i++)
      slotGlobals[i] = m_stack.At(i);
}

bool CodeVM::Step()
{
   if (m_finished)
      return false;

   UaAssert(m_instructionPointer < m_code.size());

   Uint16 arg1 = 0, arg2 = 0;
   Uint16 opcode = m_code[m_instructionPointer];

   // execute one instruction
   switch (opcode)
   {
   case op_NOP:
      break;

   case op_OPADD:
      arg1 = m_stack.Pop();
      arg2 = m_stack.Pop();
      m_stack.Push(arg1 + arg2);
      break;

   case op_OPMUL:
      arg1 = m_stack.Pop();
      arg2 = m_stack.Pop();
      m_stack.Push(arg1 * arg2);
      break;

   case op_OPSUB:
      arg1 = m_stack.Pop();
      arg2 = m_stack.Pop();
      m_stack.Push(arg2 - arg1);
      break;

   case op_OPDIV:
      arg1 = m_stack.Pop();
      arg2 = m_stack.Pop();
      if (arg1 == 0)
      {
         UaTrace("CodeVM: OPDIV: division by zero\n");
         m_finished = true;
         return false;
      }
      m_stack.Push(arg2 / arg1);
      break;

   case op_OPMOD:
      arg1 = m_stack.Pop();
      arg2 = m_stack.Pop();
      if (arg1 == 0)
      {
         UaTrace("CodeVM: OPMOD: division by zero\n");
         m_finished = true;
         return false;
      }
      m_stack.Push(arg2 % arg1);
      break;

   case op_OPOR:
      arg1 = m_stack.Pop();
      arg2 = m_stack.Pop();
      m_stack.Push(arg2 || arg1);
      break;

   case op_OPAND:
      arg1 = m_stack.Pop();
      arg2 = m_stack.Pop();
      m_stack.Push(arg2 && arg1);
      break;

   case op_OPNOT:
      m_stack.Push(!m_stack.Pop());
      break;

   case op_TSTGT:
      arg1 = m_stack.Pop();
      arg2 = m_stack.Pop();
      m_stack.Push(arg2 > arg1);
      break;

   case op_TSTGE:
      arg1 = m_stack.Pop();
      arg2 = m_stack.Pop();
      m_stack.Push(arg2 >= arg1);
      break;

   case op_TSTLT:
      arg1 = m_stack.Pop();
      arg2 = m_stack.Pop();
      m_stack.Push(arg2 < arg1);
      break;

   case op_TSTLE:
      arg1 = m_stack.Pop();
      arg2 = m_stack.Pop();
      m_stack.Push(arg2 <= arg1);
      break;

   case op_TSTEQ:
      arg1 = m_stack.Pop();
      arg2 = m_stack.Pop();
      m_stack.Push(arg2 == arg1);
      break;

   case op_TSTNE:
      arg1 = m_stack.Pop();
      arg2 = m_stack.Pop();
      m_stack.Push(arg2 != arg1);
      break;

   case op_JMP:
      m_instructionPointer = m_code[m_instructionPointer + 1] - 1;
      break;

   case op_BEQ:
      arg1 = m_stack.Pop();
      if (arg1 == 0)
         m_instructionPointer += m_code[m_instructionPointer + 1];
      else
         m_instructionPointer++;
      break;

   case op_BNE:
      arg1 = m_stack.Pop();
      if (arg1 != 0)
         m_instructionPointer += m_code[m_instructionPointer + 1];
      else
         m_instructionPointer++;
      break;

   case op_BRA:
      m_instructionPointer += m_code[m_instructionPointer + 1];
      break;

   case op_CALL: // local function
      // stack value points to next instruction after call
      m_stack.Push(m_instructionPointer + 1);
      m_instructionPointer = m_code[m_instructionPointer + 1] - 1;
      m_callLevel++;
      break;

   case op_CALLI: // imported function
   {
      arg1 = m_code[++m_instructionPointer];

      if (m_mapImportedFunctions.find(arg1) == m_mapImportedFunctions.end())
      {
         UaTrace("CodeVM: couldn't find imported function 0x%04x\n", arg1);
         m_finished = true;
         return false;
      }

      ImportedFunc(m_mapImportedFunctions[arg1].name.c_str());
   }
   break;

   case op_RET:
      if (--m_callLevel)
      {
         // conversation ended
         m_finished = true;
      }
      else
      {
         arg1 = m_stack.Pop();
         m_instructionPointer = arg1;
      }
      break;

   case op_PUSHI:
      m_stack.Push(m_code[++m_instructionPointer]);
      break;

   case op_PUSHI_EFF:
      m_stack.Push(m_basePointer + (Sint16)m_code[++m_instructionPointer]);
      break;

   case op_POP:
      m_stack.Pop();
      break;

   case op_SWAP:
      arg1 = m_stack.Pop();
      arg2 = m_stack.Pop();
      m_stack.Push(arg1);
      m_stack.Push(arg2);
      break;

   case op_PUSHBP:
      m_stack.Push(m_basePointer);
      break;

   case op_POPBP:
      arg1 = m_stack.Pop();
      m_basePointer = arg1;
      break;

   case op_SPTOBP:
      m_basePointer = m_stack.GetStackPointer();
      break;

   case op_BPTOSP:
      m_stack.SetStackPointer(m_basePointer);
      break;

   case op_ADDSP:
   {
      arg1 = m_stack.Pop();

      // fill reserved stack space with dummy values
      for (int i = 0; i < arg1; i++)
         m_stack.Push(0xdddd);
   }
   break;

   case op_FETCHM:
      arg1 = m_stack.Pop();

      FetchValue(arg1);

      m_stack.Push(m_stack.At(arg1));
      break;

   case op_STO:
      arg1 = m_stack.Pop();
      arg2 = m_stack.Pop();

      StoreValue(arg2, arg1);

      m_stack.Set(arg2, arg1);
      break;

   case op_OFFSET:
      arg1 = m_stack.Pop();
      arg2 = m_stack.Pop();
      arg1 += arg2 - 1;
      m_stack.Push(arg1);
      break;

   case op_START:
      // do nothing
      break;

   case op_SAVE_REG:
      arg1 = m_stack.Pop();
      m_resultRegister = arg1;
      break;

   case op_PUSH_REG:
      m_stack.Push(m_resultRegister);
      break;

   case op_EXIT_OP:
      // finish processing (we still might be in some sub function)
      m_finished = true;
      break;

   case op_SAY_OP:
      arg1 = m_stack.Pop();
      SayOp(arg1);
      break;

   case op_RESPOND_OP:
      // do nothing
      break;

   case op_OPNEG:
      arg1 = m_stack.Pop();
      m_stack.Push(-arg1);
      break;

   default: // unknown opcode
      UaTrace("CodeVM: unknown opcode 0x%04x\n", opcode);
      m_finished = true;
      break;
   }

   // process next instruction
   ++m_instructionPointer;

   return !m_finished;
}

void CodeVM::ReplacePlaceholder(std::string& text)
{
   std::string::size_type pos = 0;
   while ((pos = text.find('@', pos)) != std::string::npos)
   {
      char source = text[pos + 1];
      char vartype = text[pos + 2];

      signed int param = 0;
      unsigned int value = 0;
      unsigned int length = 0;
      {
         const char* startpos = text.c_str() + pos;
         char* endpos = NULL;
         param = (signed int)strtol(startpos + 3, &endpos, 10);
         length = endpos - startpos;
      }

      // get param value
      switch (source)
      {
      case 'G': // conv global, directly from stack
         value = m_stack.At(static_cast<Uint16>(static_cast<unsigned int>(param)));
         break;
      case 'S': // value from parameter passed to function
         value = m_stack.At(static_cast<Uint16>(static_cast<unsigned int>(m_basePointer + param)));
         break;
      case 'P': // pointer value from parameter passed to function
         param = m_stack.At(static_cast<Uint16>(static_cast<unsigned int>(m_basePointer + param)));
         value = m_stack.At(static_cast<Uint16>(static_cast<unsigned int>(param)));
         break;
      }

      std::string variableText;

      switch (vartype)
      {
      case 'S': // string
         variableText = GetLocalString(static_cast<Uint16>(value));
         break;
      case 'I': // integer
      {
         std::ostringstream buffer;
         buffer << value;
         variableText.assign(buffer.str());
      }
      break;
      }

      // insert value string
      text.replace(pos, length, variableText.c_str());
   }
}

void CodeVM::SetResultRegister(Uint16 value)
{
   m_resultRegister = value;
}

// *=implemented, x=assert
//   babl_menu
//   babl_fmenu
//   print
//   babl_ask
// * compare
// * random
// x plural
// * contains
// x append
// x copy
// x find
// * length
// x val
// x say
// x respond
//   get_quest
//   set_quest
//   sex
//   show_inv
//   give_to_npc
//   give_ptr_npc
//   take_from_npc
//   take_id_from_npc
//   identify_inv
//   do_offer
//   do_demand
//   do_inv_create
//   do_inv_delete
//   check_inv_quality
//   set_inv_quality
//   count_inv
//   setup_to_barter
//   end_barter
//   do_judgement
//   do_decline
//   pause
//   set_likes_dislikes
//   gronk_door
//   set_race_attitude
//   place_object
//   take_from_npc_inv
//   add_to_npc_inv
//   remove_talker
//   set_attitude
//   x_skills
//   x_traps
//   x_obj_pos
//   x_obj_stuff
//   find_inv
//   find_barter
//   find_barter_total

void CodeVM::ImportedFunc(const char* functionName)
{
   UaTrace("CodeVM: executing function \"%s\" with %u arguments\n",
      functionName, m_stack.At(m_stack.GetStackPointer()));

   std::string function(functionName);

   Uint16 argpos = m_stack.GetStackPointer();
   Uint16 argcount = m_stack.At(argpos);
   argpos--;

   if (std::string(functionName) == "babl_menu")
   {
      std::vector<Uint16> answerStringIds;

      // arg1 is ignored
      Uint16 arg2 = m_stack.At(argpos - 1);
      while (m_stack.At(arg2) != 0)
      {
         answerStringIds.push_back(m_stack.At(arg2));
         arg2++;
      }

      m_resultRegister = m_codeCallback->BablMenu(answerStringIds);
   }
   else if (function == "compare")
   {
      UaAssert(argcount == 2);

      // get arguments
      Uint16 arg1 = m_stack.At(argpos--);
      arg1 = m_stack.At(arg1);

      Uint16 arg2 = m_stack.At(argpos);
      arg2 = m_stack.At(arg2);

      // get strings
      std::string str1(m_localStrings[arg1]), str2(m_localStrings[arg2]);

      Base::String::Lowercase(str1);
      Base::String::Lowercase(str2);

      // check if first string contains second
      m_resultRegister = str1 == str2;
   }
   else if (function == "random")
   {
      UaAssert(argcount == 1);

      Uint16 arg = m_stack.At(argpos--);
      arg = m_stack.At(arg);

      // this code assumes that rand() can return RAND_MAX

      // rnum is in the range [0..1[
      double rnum = double(rand()) / double(RAND_MAX + 1);
      rnum *= arg; // now in range [0..arg[
      m_resultRegister = Uint16(rnum + 1.0); // now from [1..arg+1[
   }
   else if (function == "plural")
   {
      UaTrace("CodeVM: intrinsic plural() not implemented");
      UaAssert(false);
   }
   else if (function == "contains")
   {
      UaAssert(argcount == 2);

      // get arguments
      Uint16 arg1 = m_stack.At(argpos--);
      arg1 = m_stack.At(arg1);

      Uint16 arg2 = m_stack.At(argpos);
      arg2 = m_stack.At(arg2);

      // get strings
      std::string str1(m_localStrings[arg1]), str2(m_localStrings[arg2]);

      Base::String::Lowercase(str1);
      Base::String::Lowercase(str2);

      // check if first string contains second
      m_resultRegister = str1.find(str2) != std::string::npos;
   }
   else if (function == "append")
   {
      UaTrace("CodeVM: intrinsic append() not implemented");
      UaAssert(false);
   }
   else if (function == "copy")
   {
      UaTrace("CodeVM: intrinsic copy() not implemented");
      UaAssert(false);
   }
   else if (function == "find")
   {
      UaTrace("CodeVM: intrinsic find() not implemented");
      UaAssert(false);
   }
   else if (function == "length")
   {
      UaAssert(argcount == 1);

      // get argument
      Uint16 arg = m_stack.At(argpos--);
      arg = m_stack.At(arg);

      // return string length
      m_resultRegister = static_cast<Uint16>(m_localStrings[arg].size());
   }
   else if (function == "val")
   {
      UaTrace("CodeVM: intrinsic val() not implemented");
      UaAssert(false);
   }
   else if (function == "say")
   {
      UaTrace("CodeVM: intrinsic say() not implemented");
      UaAssert(false);
   }
   else if (function == "respond")
   {
      UaTrace("CodeVM: intrinsic respond() not implemented");
      UaAssert(false);
   }
   else
      m_resultRegister = m_codeCallback->ExternalFunc(functionName, m_stack);
}

Uint16 CodeVM::AllocString(const char* text)
{
   Uint16 pos = static_cast<Uint16>(m_localStrings.size());
   m_localStrings.push_back(std::string(text));
   return pos;
}

std::string CodeVM::GetLocalString(Uint16 stringIndex)
{
   UaAssert(stringIndex < m_localStrings.size());
   return m_localStrings[stringIndex];
}

void CodeVM::SayOp(Uint16 stringNumber)
{
   m_codeCallback->Say(stringNumber);
}

Uint16 CodeVM::GetGlobal(const char* globalsName)
{
   UaTrace("CodeVM: get global: unknown global %s\n", globalsName);
   return 0;
}

void CodeVM::SetGlobal(const char* globname, Uint16 value)
{
   UaTrace("CodeVM: SetGlobal: unknown global %hs = %04x\n", globname, value);
}

void CodeVM::StoreValue(Uint16 at, Uint16 value)
{
   std::map<Uint16, ImportedItem>::iterator iter =
      m_mapImportedGlobals.find(at);

   if (iter != m_mapImportedGlobals.end())
      UaTrace("CodeVM: storing: %hs = %04x\n", iter->second.name.c_str(), value);
}

void CodeVM::FetchValue(Uint16 at)
{
   std::map<Uint16, ImportedItem>::iterator iter =
      m_mapImportedGlobals.find(at);

   if (iter != m_mapImportedGlobals.end())
      UaTrace("CodeVM: fetching %hs returned %04x\n", iter->second.name.c_str(), m_stack.At(at));
}
