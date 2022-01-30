//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2019 Underworld Adventures Team
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
/// \file convdbg.cpp
/// conversation debugger implementation
//
#include "common.hpp"
#include "convdbg.hpp"
#include "GameStringsImporter.hpp"
#include "ConvLoader.hpp"
#include "Opcodes.hpp"
#include <cctype>

void ConversationDebugger::Init(const char* underworldFolder, const char* conversationGlobalsName)
{
   m_settings.SetValue(Base::settingUnderworldPath, underworldFolder);
   m_resourceManager.DetectGameType(m_settings);
   m_resourceManager.Rescan(m_settings);

   printf("loading game strings ...\n");
   Import::GameStringsImporter gameStringsImporter{ m_gameStrings };
   gameStringsImporter.LoadDefaultStringsPakFile(m_resourceManager);

   printf("loading conv globals ...\n");
   Import::LoadConvGlobals(m_convGlobals, m_resourceManager, conversationGlobalsName);

   printf("done loading.\n");
   m_isLoaded = false; // code not loaded yet
}

void ConversationDebugger::Start()
{
   printf("type \"help\" to see all commands\n");

   m_allBreakpoints.clear();

   Uint16 lastlistptr = 0x0000;
   std::string lastCommand;
   m_verbose = false;

   bool endloop = false;
   do
   {
      int n = 0;
      printf("\n");

      // print status
      if (m_isLoaded)
         ListCode(m_instructionPointer);

      char buffer[256] = {};
      printf(">");

      // start getting commands
      fgets(buffer, sizeof(buffer) - 1, stdin);

      if (strlen(buffer) == 0)
         continue;

      std::string command{ buffer };

      // trim last char (is a '\n')
      Base::String::TrimEnd(command);

      if (command.empty())
         command = lastCommand; // repeat last command
      else
         lastCommand = command;

      Base::String::Lowercase(command);

      // check for every command
      if (command == "help" || command == "h" || command == "?")
      {
         printf(
            "convdbg - help\n\n"
            "help       h shows this help\n"
            "conv n       loads a conversation (conv slot n in decimal, not hex)\n"
            "reg        r shows registers\n"
            "info       i shows info about the virtual machine, including breakpoints\n"
            "list [n]   l lists instructions from code memory 0x<n> on (or from current ip)\n"
            "dump       d dumps complete stack\n"
            "step       s advances an instruction\n"
            "cont       c continues until a breakpoint occurs\n"
            "break n    b toggles a breakpoint at code pos 0x<n>\n"
            "verbose    v toggles verboseness of debugger\n"
            "reset        resets virtual machine (e.g. after an exception)\n"
            "exit/quit  x quits the debugger\n\n"
            "short keys can be used for the commands, e.g. 's' for step\n"
            "some commands are only available, when a conversation is loaded\n"
            "\n");
      }

      if (command.substr(0, 4) == "conv")
      {
         if (m_conversationSlot != 0xffff)
            Conv::CodeVM::Done(m_convGlobals);

         int conv = atoi(command.substr(5).c_str());

         // try to load code
         bool ret = false;
         try
         {
            ret = Import::LoadConvCode(*this, m_settings, m_resourceManager, "data/cnv.ark", conv);
         }
         catch (const std::exception & ex)
         {
            printf("caught exception: %s\n", ex.what());
            ret = false;
         }

         // print out status
         if (ret)
         {
            printf("loaded conversation #%04x.\n", conv);
            Conv::CodeVM::Init(this, m_convGlobals/*, m_gameStrings.GetStringBlock(GetStringBlock())*/);
            printf("conversation partner: \"%s\"\n",
               m_gameStrings.GetString(7, 16 + m_conversationSlot).c_str());
            printf("using strings from string block #%04x\n", GetStringBlock());
            m_isLoaded = true;
         }
         else
            printf("conversation %i not available!\n", conv);

         continue;
      }

      if (command == "reg" ||
         command == "r" && command != "reset")
      {
         printf("registers:\n"
            "instrp: %04x   stackp: %04x   basep: %04x   result_reg: %04x\n",
            m_instructionPointer, m_stack.GetStackPointer(), m_basePointer, m_resultRegister);
      }

      if (command == "info" || command == "i")
      {
         if (!m_isLoaded)
         {
            printf("no conversation loaded yet.\n");
            continue;
         }

         // print some infos
         printf("infos about conversation #%u (string block %04x):\n\n",
            m_conversationSlot, GetStringBlock());

         printf("code size: %04zx, call/ret level: %u, verbose mode: %s\n",
            GetCodeSegment().size(), m_callLevel, m_verbose ? "on" : "off");

         printf("private globals: %04x; imported funcs: %04zx; "
            "imported globals: %04zx\n",
            GetReservedGlobals(), GetImportedFunctions().size(), GetImportedGlobals().size());

         // list breakpoints
         size_t bps = m_allBreakpoints.size();
         if (bps == 0)
            printf("no breakpoints set.\n");
         else
         {
            printf("all breakpoints:\n");
            for (size_t i = 0; i < bps; i++)
               printf("%04x%c", m_allBreakpoints[i], i % 8 == 7 ? '\n' : ' ');
         }

         printf("\n");
      }

      if (command == "verbose" || command == "v")
      {
         m_verbose = !m_verbose;
         printf("verbose: %s\n", m_verbose ? "on" : "off");
         continue;
      }

      if (command == "exit" || command == "x" ||
         command == "quit" || command == "q")
      {
         endloop = true;
         continue;
      }

      // for the next functions, code must be loaded
      if (!m_isLoaded)
      {
         printf("no conversation loaded yet; please use \"conv\" to load one.\n");
         continue;
      }

      if (command == "reset")
      {
         // re-init vm
         printf("re-init'ing virtual machine.\n");
         Conv::CodeVM::Init(this, m_convGlobals/*, m_gameStrings.GetStringBlock(GetStringBlock())*/);
         printf("deleting all breakpoints.\n");
         m_allBreakpoints.clear();
      }

      if (command.substr(0, 4) == "list" || command == "l")
      {
         Uint16 ptr = m_instructionPointer;

         // when repeating the command, start from last list ptr
         if (command.length() == 1 || command == "list")
            ptr = lastlistptr;

         Base::String::Replace(command, "list", "");
         Base::String::Replace(command, "l", "");

         ptr = strtoul(command.c_str(), NULL, 16);

         // list a few lines of position
         ptr += ListCode(ptr);
         ptr += ListCode(ptr);
         ptr += ListCode(ptr);
         ptr += ListCode(ptr);
         lastlistptr = ptr + ListCode(ptr);
         continue;
      }

      // reset last list pointer
      lastlistptr = m_instructionPointer;

      if (command == "dump" || command == "d")
      {
         // dump whole stack
         Uint16 stackp = m_stack.GetStackPointer();
         printf("stackp = %04x\n", stackp);

         if (stackp != 0xffff)
            for (Uint16 i = 0; i <= stackp; i++)
            {
               if (i % 8 == 0) printf("%04x:  ", i);

               printf("%04x ", m_stack.At(i));

               if (i % 8 == 7) printf("\n");
            }
         printf("\n");
      }

      if (command == "step" || command == "s" || command == "n")
      {
         n = 1;
      }

      if (command == "cont" || command == "c")
      {
         n = -1;
      }

      if (command.substr(0, 5) == "break" || command == "b")
      {
         size_t pos = command.find(' ');
         if (pos != std::string::npos)
         {
            Uint32 bpt = strtoul(command.substr(pos).c_str(), NULL, 16);
            if (bpt != 0)
            {
               // look if breakpoint already exists in the list

               size_t i, max = m_allBreakpoints.size();
               for (i = 0; i < max; i++)
                  if (m_allBreakpoints[i] == bpt)
                  {
                     m_allBreakpoints.erase(m_allBreakpoints.begin() + i);
                     printf("deleted breakpoint at %04x\n", bpt);
                     break;
                  }

               if (i >= max)
               {
                  m_allBreakpoints.push_back(bpt);
                  printf("added breakpoint at %04x\n", bpt);
               }
            }
         }
      }

      // when we reach here, we run the code n times; if n == -1, we run
      // until exit, an exception is thrown or a callback function is called

      if (n == 0)
         continue;

      while (n > 0 || n == -1)
      {
         // run a single code
         try
         {
            Conv::CodeVM::Step();
         }
         catch (const std::exception & ex)
         {
            printf("caught exception at ip = %04x: %s\n", m_instructionPointer, ex.what());
            n = 0;
            lastCommand.clear();
         }

         // conversation finished?
         if (m_finished)
         {
            printf("conversation ended (at %04x).\n\n", m_instructionPointer);
            n = 0;

            // reinit code vm
            Conv::CodeVM::Done(m_convGlobals);
            Conv::CodeVM::Init(this, m_convGlobals/*, m_gameStrings.GetStringBlock(GetStringBlock())*/);
         }

         // check if breakpoints are reached
         size_t max = m_allBreakpoints.size();
         for (size_t i = 0; i < max; i++)
         {
            if (m_allBreakpoints[i] == m_instructionPointer)
            {
               printf("breakpoint at %04x reached\n", m_instructionPointer);
               n = 0;
            }
         }

         if (n > 0) n--;
      }

   } while (!endloop);
}

int ConversationDebugger::ListCode(Uint16 at)
{
   const std::vector<Uint16>& code = GetCodeSegment();

   if (at > code.size())
   {
      printf("error listing code at %04x!\n", at);
      return 0;
   }

   if (code[at] >= 0x2a)
   {
      // opcode unknown
      return 0;
   }

   // output memory adress and code word(s)
   printf("%04x %04x ", at, code[at]);

   if (Conv::g_convInstructions[code[at]].args == 1)
      printf("%04x", code[at + 1]);
   else
      printf("    ");

   // output mnemonic and possible operand
   printf("    %s", Conv::g_convInstructions[code[at]].mnemonic);
   printf(Conv::g_convInstructions[code[at]].argcode, code[at + 1]);

   printf("\n");
   return Conv::g_convInstructions[m_code[at]].args + 1;
}

void ConversationDebugger::ImportedFunc(const char* functionName)
{
   if (m_verbose)
      printf("%04x: CALLI %04x: \"%s\"\n", m_instructionPointer - 1, m_code[m_instructionPointer], functionName);

   std::string function{ functionName };

   if (function == "babl_fmenu")
   {
      // arg1 is not used
      Uint16 arg2 = m_stack.At(m_stack.GetStackPointer() - 1);
      Uint16 arg3 = m_stack.At(m_stack.GetStackPointer() - 2);

      if (m_verbose)
         printf("babl_fmenu: s[3]=%04x\n", arg3);

      int nr = 1;
      while (m_stack.At(arg2) != 0)
      {
         printf("%i. %s\n", nr++,
            m_gameStrings.GetString(GetStringBlock(), m_stack.At(arg2)).c_str());
         arg2++;
      }

      do
      {
         printf("what do you respond? ");
         char buffer[256];
         fgets(buffer, 255, stdin);

         m_resultRegister = atoi(buffer);
      } while (m_resultRegister < 1 || m_resultRegister > nr - 1);

      printf("response: %u\n\n", m_resultRegister);
   }
   else if (function == "take_from_npc")
   {
      Uint16 arg1 = m_stack.At(m_stack.GetStackPointer());
      Uint16 arg2 = m_stack.At(m_stack.GetStackPointer() - 1);

      printf("<NPC gives player an item>\n"
         "arg1=%04x, *arg2=%04x\n\n", arg1, m_stack.At(arg2));
   }
   else if (function == "set_quest")
   {
      Uint16 arg1 = m_stack.At(m_stack.GetStackPointer());
      Uint16 arg2 = m_stack.At(m_stack.GetStackPointer() - 1);
      Uint16 arg3 = m_stack.At(m_stack.GetStackPointer() - 2);
      printf("<set_quest: arg1=%04x, *arg2=%04x, *arg3=%04x>\n",
         arg1, m_stack.At(arg2), m_stack.At(arg3));
   }
   else if (function == "get_quest")
   {
      Uint16 arg1 = m_stack.At(m_stack.GetStackPointer());
      Uint16 arg2 = m_stack.At(m_stack.GetStackPointer() - 1);
      printf("<get_quest: arg1=%04x, *arg2=%04x>\n", arg1, m_stack.At(arg2));
   }
   else
      Conv::CodeVM::ImportedFunc(functionName);
}

Uint16 ConversationDebugger::GetGlobal(const char* globalName)
{
   printf("querying global value: %s\n", globalName);
   return 0;
}

void ConversationDebugger::SetGlobal(const char* globalName, Uint16 value)
{
   printf("setting global value: %s to value %i\n", globalName, value);
}

void ConversationDebugger::Say(Uint16 index)
{
   if (m_verbose)
      printf("%04x: SAY (%04x)\n", m_instructionPointer, index);

   printf("%s\n\n", m_gameStrings.GetString(GetStringBlock(), index).c_str());
}

Uint16 ConversationDebugger::BablMenu(const std::vector<Uint16>& answerStringIds)
{
   for (size_t answerIndex = 0; answerIndex < answerStringIds.size(); answerIndex++)
   {
      printf("%zu. %s\n",
         answerIndex + 1,
         m_gameStrings.GetString(GetStringBlock(), answerStringIds[answerIndex]).c_str());
   }

   Uint16 answer = 0;
   do
   {
      printf("what do you respond? ");
      char buffer[256];
      fgets(buffer, 255, stdin);

      answer = atoi(buffer);
   }
   while (answer < 1 || answer > answerStringIds.size());

   printf("response: %u\n\n", answer);
   return answer;
}

Uint16 ConversationDebugger::ExternalFunc(const char* funcname, Conv::ConvStack& stack)
{
   printf("calling external function: %s\n", funcname);
   return 0;
}

#if 0
void ConversationDebugger::sto_priv(Uint16 at, Uint16 val)
{
   if (m_verbose)
   {
      if (at < imported_globals.size())
      {
         const ua_conv_imported_item& iitem = imported_globals[at];
         printf("%04x: STO: \"%s\" (%04x) = %04x\n", instrp, iitem.name.c_str(), at, val);
      }
      else
      {
         if (at < glob_reserved)
            printf("%04x: STO: priv[%04x] = %04x\n", m_instructionPointer, at, val);
         //         else
         //            printf("%04x: STO: stack[%04x] = %04x\n",m_instructionPointer,at,val);
      }
   }
}

void ConversationDebugger::fetchm_priv(Uint16 at)
{
   if (m_verbose)
   {
      if (at < imported_globals.size())
      {
         const ua_conv_imported_item& iitem = imported_globals[at];
         printf("%04x: FETCHM: \"%s\" (%04x)\n", m_instructionPointer, iitem.name.c_str(), at);
      }
      else
      {
         if (at < glob_reserved)
            printf("%04x: FETCHM: priv[%04x]\n", m_instructionPointer, at);
         //         else
         //            printf("%04x: FETCHM: stack[%04x]\n",m_instructionPointer,at);
      }
   }
}
#endif


/// main function
int main(int argc, char* argv[])
{
   printf("convdbg - Ultima Underworld conversation script debugger\n\n");

   if (argc < 3)
   {
      printf("syntax: convdbg <underworld-folder> <bglobals-file>\n");
      printf("example: convdbg \"c:\\uw1\" "
         "\"save1\\bglobals.dat\"\n\n");
      return 0;
   }

   printf("underworld folder: %s\n", argv[1]);
   printf("private globals:   %s\n\n", argv[2]);

   ConversationDebugger dbg;

   try
   {
      dbg.Init(argv[1], argv[2]);

      // start debugging
      dbg.Start();
   }
   catch (const std::exception & ex)
   {
      printf("caught an exception: \"%s\"\n", ex.what());
   }
   catch (...)
   {
      printf("caught an unknown exception, exiting!\n");
   }

   return 0;
}
