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
/*! \file cnvdbg.cpp

   conversation debugger implementation

*/

// needed includes
#include "common.hpp"
#include "cnvdbg.hpp"
#include "resource/fread_endian.hpp"
#include "instr.hpp"
#include <cctype>


int my_stricmp(const char *a, const char *b){ return strcmp(a,b); }


// ua_conv_debugger methods

void ua_conv_debugger::init(const char *thecnvname, const char *strname, const char *bgname)
{
   cnvname = thecnvname;

   printf("loading game strings ...\n");
   gs.load(strname);

   printf("loading conv globals ...\n");
   cg.load(bgname,false);

   printf("done loading.\n");
   loaded=false; // code not loaded yet
}

void ua_conv_debugger::start()
{
   allbreakpoints.clear();

   Uint16 lastlistptr = 0x0000;
   char lastbuffer[256];
   verbose = false;

   bool endloop = false;
   do
   {
      int n=0;
      printf("\n");

      // print status
      if (loaded)
         list_code(instrp);

      char buffer[256];
      printf(">");

      // start getting commands
      fgets(buffer,255,stdin);

      if (strlen(buffer)==0)
         continue;

      // trim last char (is a '\n')
      buffer[strlen(buffer)-1]=0;

      if (buffer[0]==0)
         strcpy(buffer,lastbuffer);
      else
         strcpy(lastbuffer,buffer);

      // check for every command
      if (my_stricmp("help",buffer)==0 || tolower(buffer[0])=='h' || buffer[0]=='?')
      {
         printf(
            "cnvdbg - help\n\n"
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

      if (strncmp("conv",buffer,4)==0)
      {
         if (conv_nr!=0xffff)
            ua_conv_code_vm::done(cg);

         int conv = atoi(buffer+4);

         // try to load code
         bool ret=false;
         try
         {
            ret=load_code(cnvname,conv);
         }
         catch (ua_exception e)
         {
            printf("caught ua_exception: %s\n",e.what());
            ret=false;
         }

         // print out status
         if (ret)
         {
            printf("loaded conversation #%u.\n",conv);
            ua_conv_code_vm::init(cg);
            printf("conversation partner: \"%s\"\n",
               gs.get_string(6,16+conv_nr).c_str());
            printf("using strings from string block #%u\n",strblock);
            loaded=true;
         }
         else
            printf("conversation %u not available!\n",conv);

         continue;
      }

      if (my_stricmp("reg",buffer)==0 ||
          (tolower(buffer[0])=='r' && strncmp("reset",buffer,4)!=0))
      {
         printf("registers:\n"
            "instrp: %04x   stackp: %04x   basep: %04x   result_reg: %04x\n",
               instrp,stack.get_stackp(),basep,result_register);
      }

      if (my_stricmp("info",buffer)==0 || tolower(buffer[0])=='i')
      {
         if (!loaded)
         {
            printf("no conversation loaded yet.\n");
            continue;
         }

         // print some infos
         printf("infos about conversation #%u (string block %u):\n\n",
            conv_nr,strblock);

         printf("code size: %04x, call/ret level: %u, verbose mode: %s\n",
            codesize,call_level,verbose ? "on" : "off");

         printf("private globals: %04x; imported funcs: %04x; "
            "imported globals: %04x\n",
            glob_reserved,imported_funcs.size(),imported_globals.size());

         // list breakpoints
         unsigned int bps = allbreakpoints.size();
         if (bps==0)
            printf("no breakpoints set.\n");
         else
         {
            printf("all breakpoints:\n");
            for(unsigned int i=0; i<bps; i++)
               printf("%04x%c",allbreakpoints[i],i%8==7 ? '\n' : ' ');
         }

         printf("\n");
      }

      if (my_stricmp("verbose",buffer)==0 || tolower(buffer[0])=='v')
      {
         verbose = !verbose;
         printf("verbose: %s\n",verbose ? "on" : "off");
         continue;
      }

      if (my_stricmp("exit",buffer)==0 || tolower(buffer[0])=='x' ||
          my_stricmp("quit",buffer)==0 || tolower(buffer[0])=='q')
      {
         endloop=true;
         continue;
      }

      // for the next functions, code must be loaded
      if (!loaded)
      {
         printf("no conversation loaded yet; please use \"conv\" to load one.\n");
         continue;
      }

      if (strncmp("reset",buffer,4)==0)
      {
         // re-init vm
         printf("re-init'ing virtual machine.\n");
         ua_conv_code_vm::init(cg);
         printf("deleting all breakpoints.\n");
         allbreakpoints.clear();
      }

      if (strncmp("list",buffer,4)==0 || tolower(buffer[0])=='l')
      {
         Uint16 ptr = instrp;

         // when repeating the command, start from last list ptr
         if (buffer[1]==0 || buffer[4]==0)
            ptr = lastlistptr;

         char *pos = strchr(buffer,' ');
         if (pos!=NULL)
            ptr = strtoul(pos,NULL,16);

         // list a few lines of position
         ptr += list_code(ptr);
         ptr += list_code(ptr);
         ptr += list_code(ptr);
         ptr += list_code(ptr);
         lastlistptr = ptr + list_code(ptr);
         continue;
      }

      // reset last list pointer
      lastlistptr = instrp;

      if (my_stricmp("dump",buffer)==0 || tolower(buffer[0])=='d')
      {
         // dump whole stack
         Uint16 stackp = stack.get_stackp();
         printf("stackp = %04x\n",stackp);

         if (stackp!=0xffff)
         for(Uint16 i=0; i<=stackp; i++)
         {
            if ( i%8 == 0) printf("%04x:  ",i);

            printf("%04x ",stack.at(i));

            if ( i%8 == 7) printf("\n");
         }
         printf("\n");
      }

      if (my_stricmp("step",buffer)==0 || tolower(buffer[0])=='s' || tolower(buffer[0])=='n')
      {
         n=1;
      }

      if (my_stricmp("cont",buffer)==0 || tolower(buffer[0])=='c')
      {
         n=-1;
      }

      if (strncmp("break",buffer,5)==0 || tolower(buffer[0])=='b')
      {
         char *pos = strchr(buffer,' ');
         if (pos!=NULL)
         {
            Uint32 bpt = strtoul(pos,NULL,16);
            if (bpt!=0)
            {
               // look if breakpoint already exists in the list

               int i,max = allbreakpoints.size();
               for(i=0; i<max; i++)
               if (allbreakpoints[i]==bpt)
               {
                  allbreakpoints.erase(allbreakpoints.begin()+i);
                  printf("deleted breakpoint at %04x\n",bpt);
                  break;
               }

               if (i>=max)
               {
                  allbreakpoints.push_back(bpt);
                  printf("added breakpoint at %04x\n",bpt);
               }
            }
         }
      }

      // when we reach here, we run the code n times; if n == -1, we run
      // until exit, an exception is thrown or a callback function is called

      if (n==0)
         continue;

      while(n>0 || n==-1)
      {
         // run a single code
         try
         {
            step();
         }
         catch (ua_conv_vm_exception e)
         {
            printf("caught exception at ip = %04x: ",instrp);

            switch(e)
            {
            case ua_ex_div_by_zero:
               printf("division by zero");
               break;

            case ua_ex_stack_access:
               printf("invalid stack access");
               break;

            case ua_ex_unk_opcode:
               printf("unknown opcode");
               break;

            default:
               printf("unknown exception! (%u)",e);
               break;
            }

            printf("\n");
            n=0;
            lastbuffer[0]=0;
         }

         // conversation finished?
         if (finished)
         {
            printf("conversation ended (at %04x).\n\n",instrp);
            n=0;

            // reinit code vm
            ua_conv_code_vm::done(cg);
            ua_conv_code_vm::init(cg);
         }

         // check if breakpoints are reached
         int max = allbreakpoints.size();
         for(int i=0; i<max; i++)
         {
            if (allbreakpoints[i]==instrp)
            {
               printf("breakpoint at %04x reached\n",instrp);
               n=0;
            }
         }

         if (n>0) n--;
      }

   } while (!endloop);
}

int ua_conv_debugger::list_code(Uint16 at)
{
   if (at>codesize)
   {
      printf("error listing code at %04x!\n",at);
      return 0;
   }

   if (code[at]>=0x2a)
   {
      // opcode unknown
      return 0;
   }

   // output memory adress and code word(s)
   printf("%04x %04x ",at,code[at]);

   if (ua_conv_instructions[code[at]].args==1)
      printf("%04x",code[at+1]);
   else
      printf("    ");

   // output mnemonic and possible operand
   printf("    %s",ua_conv_instructions[code[at]].mnemonic);
   printf(ua_conv_instructions[code[at]].argcode,code[at+1]);

   printf("\n");
   return ua_conv_instructions[code[at]].args+1;
}

void ua_conv_debugger::imported_func(Uint16 number)
{
   if (imported_funcs.empty() || number>imported_funcs.size())
   {
      printf("alert! called unknown imported function: %04x\n",number);
      return;
   }

   const ua_conv_imported_item &iitem = imported_funcs[number];

   if (verbose)
      printf("%04x: CALLI %04x: \"%s\"\n",instrp-1,number,iitem.name.c_str());

   if (iitem.name=="babl_menu")
   {
      Uint16 arg1=stack.at(stack.get_stackp());
      Uint16 arg2=stack.at(stack.get_stackp()-1);
      int nr=1;
      while(stack.at(arg2)!=0)
      {
         printf("%u. %s\n",nr++,
            gs.get_string(strblock,stack.at(arg2)).c_str());
         arg2++;
      }

      do
      {
         printf("what do you respond? ");
         char buffer[256];
         fgets(buffer,255,stdin);

         result_register = atoi(buffer);
      } while (result_register<1 || result_register > nr-1);

      printf("response: %u\n\n",result_register);
   }

   if (iitem.name=="babl_fmenu")
   {
      Uint16 arg1=stack.at(stack.get_stackp());
      Uint16 arg2=stack.at(stack.get_stackp()-1);
      Uint16 arg3=stack.at(stack.get_stackp()-2);

      if (verbose)
         printf("babl_fmenu: s[3]=%04x\n",arg3);

      int nr=1;
      while(stack.at(arg2)!=0)
      {
         printf("%u. %s\n",nr++,
            gs.get_string(strblock,stack.at(arg2)).c_str());
         arg2++;
      }

      do
      {
         printf("what do you respond? ");
         char buffer[256];
         fgets(buffer,255,stdin);

         result_register = atoi(buffer);
      } while (result_register<1 || result_register > nr-1);

      printf("response: %u\n\n",result_register);
   }

   if (iitem.name=="take_from_npc")
   {
      Uint16 arg1=stack.at(stack.get_stackp());
      Uint16 arg2=stack.at(stack.get_stackp()-1);

      printf("<NPC gives player an item>\n"
         "arg1=%04x, *arg2=%04x\n\n",arg1,stack.at(arg2));
   }

   if (iitem.name=="set_quest")
   {
      Uint16 arg1=stack.at(stack.get_stackp());
      Uint16 arg2=stack.at(stack.get_stackp()-1);
      Uint16 arg3=stack.at(stack.get_stackp()-2);
      printf("<set_quest: arg1=%04x, *arg2=%04x, *arg3=%04x>\n",
         arg1,stack.at(arg2),stack.at(arg3));
   }

   if (iitem.name=="get_quest")
   {
      Uint16 arg1=stack.at(stack.get_stackp());
      Uint16 arg2=stack.at(stack.get_stackp()-1);
      printf("<get_quest: arg1=%04x, *arg2=%04x>\n",arg1,stack.at(arg2));
   }
}

void ua_conv_debugger::say_op(Uint16 str_id)
{
   if (verbose)
      printf("%04x: SAY (%04x)\n", instrp, str_id);

   printf("%s\n\n", gs.get_string(strblock,str_id).c_str());
}

void ua_conv_debugger::sto_priv(Uint16 at, Uint16 val)
{
   if (verbose)
   {
      if (at<imported_globals.size())
      {
         const ua_conv_imported_item &iitem = imported_globals[at];
         printf("%04x: STO: \"%s\" (%04x) = %04x\n",instrp,iitem.name.c_str(),at,val);
      }
      else
      {
         if (at<glob_reserved)
            printf("%04x: STO: priv[%04x] = %04x\n",instrp,at,val);
//         else
//            printf("%04x: STO: stack[%04x] = %04x\n",instrp,at,val);
      }
   }
}

void ua_conv_debugger::fetchm_priv(Uint16 at)
{
   if (verbose)
   {
      if (at<imported_globals.size())
      {
         const ua_conv_imported_item &iitem = imported_globals[at];
         printf("%04x: FETCHM: \"%s\" (%04x)\n",instrp,iitem.name.c_str(),at);
      }
      else
      {
         if (at<glob_reserved)
            printf("%04x: FETCHM: priv[%04x]\n",instrp,at);
//         else
//            printf("%04x: FETCHM: stack[%04x]\n",instrp,at);
      }
   }
}


// we don't need SDL's main here
#undef main


// main function

int main(int argc, char *argv[])
{
   printf("cnvdbg - ultima underworld 1 conversation script debugger\n\n");

   if (argc<4)
   {
      printf("syntax: cnvdbg <cnv-file> <str-file> <bglobals-file>\n");
      printf("example: cnvdbg \"data\\cnv.ark\" "
         "\"data\\strings.pak\""
         "\"save1\\bglobals.dat\"\n\n");
      return 0;
   }

   printf("conversation-file: %s\n",argv[1]);
   printf("strings-file:      %s\n",argv[2]);
   printf("private globals:   %s\n\n",argv[3]);

   ua_conv_debugger dbg;

#ifndef _DEBUG
   try
#endif
   {
      dbg.init(argv[1],argv[2],argv[3]);

      // start debugging
      dbg.start();
   }
#ifndef _DEBUG
   catch(ua_exception e)
   {
      printf("caught an ua_exception: \"%s\"\n",e.what());
   }
   catch(std::exception e)
   {
      printf("caught an exception: \"%s\"\n",e.what());
   }
   catch(...)
   {
      printf("caught an unknown exception, exiting!\n");
   }
#endif

   return 0;
}
