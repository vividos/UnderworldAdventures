/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2003,2004 Underworld Adventures Team

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
/*! \file uwdump.cpp

   \brief uw1 and uw2 dump program

*/

// needed includes
#include "common.hpp"
#include "levark.hpp"
#include "prop.hpp"
#include "mdldump.hpp"
#include "cnvdec.hpp"


// enums

enum uwdump_command
{
   cmd_nop=0,
   cmd_leveldec,//!< level decoding
   cmd_objprop, //!< object properties dumping
   cmd_mdldump, //!< builtin models dumping
   cmd_cnvdec,  //!< conversation code decompiling
};


// global variables

bool is_uw2 = false;
uwdump_command command = cmd_nop;
std::string basepath;
std::string param;


// functions

void parse_args(unsigned int argc, const char** argv)
{
   bool cmd_appeared = false;
   bool need_param = false;

   for(unsigned int i=1; i<argc; i++)
   {
      if (strlen(argv[i])>1 && argv[i][0]=='-')
      {
         // we have an option
         switch(tolower(argv[i][1]))
         {
         case 'd': // set base path
            {
               if (strlen(&argv[i][2])==0)
                  basepath.assign(argv[++i]);
               else
                  basepath.assign(&argv[i][2]);

               if (basepath.find_last_of("\\/")!=basepath.size()-1)
                  basepath.append("/");
            }
            break;

         case '2': // set game type to uw2
            is_uw2 = true;
            break;

         case 'h': // help
         case '?':
            printf("uwdump help:\n"
               " syntax:\n"
               "   uwdump <options> <command> <options>\n"
               "\n"
               " commands:\n"
               "   leveldump <file>    dumps <basepath><file> as lev.ark\n"
               "   propdump            dumps data/comobj.dat and objects.dat\n"
               "   mdldump             dumps builtin model data\n"
               "   cnvdec <slot>       decompiles conv. code\n"
               "\n"
               " options:\n"
               "   -d<basepath>   sets uw1/uw2 path; using current folder when not specified\n"
               "                  (game type is auto-detected)\n"
               "   -2             set game type to ultima underworld 2 without further checking\n"
               "\n");
            exit(0); // plain ugly, but I'm too lazy now
            break;
         }
      }
      else
      if (!cmd_appeared) // no command yet?
      {
         cmd_appeared = true;
         if (strcmp(argv[i],"leveldump")==0)
         {
            command = cmd_leveldec;
            need_param = true;
         }
         else
         if (strcmp(argv[i],"propdump")==0)
         {
            command = cmd_objprop;
            need_param = true;
         }
         else
         if (strcmp(argv[i],"mdldump")==0)
         {
            command = cmd_mdldump;
            need_param = true;
         }
         else
         if (strcmp(argv[i],"cnvdec")==0)
         {
            command = cmd_cnvdec;
            need_param = true;
         }
         else
            printf("unknown command: %s\n",argv[i]);
      }
      else
      if (need_param) // command needs parameter?
      {
         need_param = false;

         switch(command)
         {
         case cmd_leveldec:
            param.assign(argv[i]);
            break;

         case cmd_cnvdec:
            param.assign(argv[i]);
            break;
         }
      }
      else
         printf("unknown parameter: %s\n",argv[i]);
   }
}

// checks for game type to use
void check_game_type()
{
   // already set uw2?
   if (is_uw2)
   {
      printf("game type set to ultima underworld 2\n");
      return;
   }

   // check for uw.exe, uwdemo.exe or ultimau1.exe in <basepath>
   std::string filename1(basepath),filename2(basepath),filename3(basepath);
   filename1.append("uw.exe");
   filename2.append("uwdemo.exe");
   filename3.append("ultimau1.exe");

   if (ua_file_exists(filename1.c_str()) ||
       ua_file_exists(filename2.c_str()) ||
       ua_file_exists(filename3.c_str()) )
   {
      is_uw2 = false;
      printf("detected game type: ultima underworld 1\n");
      return;
   }

   // check for uw2.exe
   filename1.assign(basepath);
   filename1.append("uw2.exe");

   if (ua_file_exists(filename1.c_str()))
   {
      is_uw2 = true;
      printf("detected game type: ultima underworld 2\n");
      return;
   }

   // no game files found
   printf("no game type detected; assuming ultima underworld 1\n");
   is_uw2 = false;
}


// main function

#undef main

extern "C"
int main(char argc, char* argv[])
{
   printf("uwdump - ultima underworld data dump program\n"
      "Copyright (c) 2003,2004 Michael Fink\n\n");

   basepath.assign("./");

   parse_args(argc,const_cast<const char**>(argv));

   check_game_type();
   printf("\n");

   // execute command
   switch(command)
   {
   case cmd_leveldec: // lev.ark decoding
      {
         ua_dump_level_archive levdump;
         levdump.start(basepath,param);
      }
      break;

   case cmd_objprop: // object properties decoding
      {
         ua_dump_obj_properties objprop;
         objprop.start(basepath);
      }
      break;

   case cmd_mdldump: // builtin model dumping
      {
         ua_dump_builtin_models mdldump;
         mdldump.start(basepath);
      }
      break;

   case cmd_cnvdec: // conversation code decompiling
      {
         Uint16 slot = (Uint16)strtol(param.c_str(), NULL, 10);

         ua_conv_code_decompiler cnvdec;
         cnvdec.start(basepath, slot);
      }
      break;

   default:
      printf("no command given; show help with parameter -h\n");
      break;
   }

   return 0;
}


// fake functions to get linking to work

#include "files.hpp"

SDL_RWops* ua_files_manager::get_uadata_file(const char* relpath)
{
   return NULL;
}
