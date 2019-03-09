//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2003,2004,2019 Underworld Adventures Team
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
/// \file uwdump.cpp
/// \brief uw1 and uw2 dump program
//
#include "common.hpp"
#include "FileSystem.hpp"
#include "levark.hpp"
#include "prop.hpp"
#include "mdldump.hpp"
#include "GameStringsImporter.hpp"

enum DumpCommand
{
   commandNone = 0,
   commandLevelDecoding,///< level decoding
   commandObjectProperties, ///< object properties dumping
   commandModelDump, ///< builtin models dumping
};

bool isUw2 = false;
DumpCommand command = commandNone;
std::string basePath;
std::string param;

bool ParseArgs(unsigned int argc, const char** argv)
{
   bool commandAppeared = false;
   bool needParameter = false;

   for (unsigned int i = 1; i < argc; i++)
   {
      if (strlen(argv[i]) > 1 && argv[i][0] == '-')
      {
         // we have an option
         switch (tolower(argv[i][1]))
         {
         case 'd': // set base path
         {
            if (strlen(&argv[i][2]) == 0)
               basePath.assign(argv[++i]);
            else
               basePath.assign(&argv[i][2]);

            if (basePath.find_last_of("\\/") != basePath.size() - 1)
               basePath.append(Base::FileSystem::PathSeparator);
         }
         break;

         case '2': // set game type to uw2
            isUw2 = true;
            break;

         case 'h': // help
         case '?':
            printf("uwdump help:\n"
               " syntax:\n"
               "   uwdump <options> <command> <options>\n"
               "\n"
               " commands:\n"
               "   leveldump <file>    dumps <basePath><file> as lev.ark\n"
               "   propdump            dumps data/comobj.dat and objects.dat\n"
               "   mdldump             dumps builtin model data\n"
               "\n"
               " options:\n"
               "   -d<basePath>   sets uw1/uw2 path; using current folder when not specified\n"
               "                  (game type is auto-detected)\n"
               "   -2             set game type to ultima underworld 2 without further checking\n"
               "\n");
            return false;
         }
      }
      else if (!commandAppeared) // no command yet?
      {
         commandAppeared = true;
         if (strcmp(argv[i], "leveldump") == 0)
         {
            command = commandLevelDecoding;
            needParameter = true;
         }
         else if (strcmp(argv[i], "propdump") == 0)
         {
            command = commandObjectProperties;
            needParameter = true;
         }
         else if (strcmp(argv[i], "mdldump") == 0)
         {
            command = commandModelDump;
            needParameter = true;
         }
         else
            printf("unknown command: %s\n", argv[i]);
      }
      else if (needParameter) // command needs parameter?
      {
         needParameter = false;

         switch (command)
         {
         case commandLevelDecoding:
            param.assign(argv[i]);
            break;

         default:
            break;
         }
      }
      else
         printf("unknown parameter: %s\n", argv[i]);
   }

   return true;
}

/// checks for game type to use
void CheckGameType()
{
   // already set uw2?
   if (isUw2)
   {
      printf("game type set to ultima underworld 2\n");
      return;
   }

   // check for uw.exe, uwdemo.exe or ultimau1.exe in <basePath>
   std::string filename1(basePath), filename2(basePath), filename3(basePath);
   filename1.append("uw.exe");
   filename2.append("uwdemo.exe");
   filename3.append("ultimau1.exe");

   if (Base::FileSystem::FileExists(filename1) ||
      Base::FileSystem::FileExists(filename2) ||
      Base::FileSystem::FileExists(filename3))
   {
      isUw2 = false;
      printf("detected game type: ultima underworld 1\n");
      return;
   }

   // check for uw2.exe
   filename1.assign(basePath);
   filename1.append("uw2.exe");

   if (Base::FileSystem::FileExists(filename1))
   {
      isUw2 = true;
      printf("detected game type: ultima underworld 2\n");
      return;
   }

   // no game files found
   printf("no game type detected; assuming ultima underworld 1\n");
   isUw2 = false;
}

int main(int argc, char* argv[])
{
   printf("uwdump - Ultima Underworld data dump program\n"
      "Copyright (c) 2003,2004,2019 Michael Fink\n\n");

   basePath.assign("./");

   if (!ParseArgs(argc, const_cast<const char**>(argv)))
      return -1;

   CheckGameType();
   printf("\n");

   // execute command
   switch (command)
   {
   case commandLevelDecoding: // lev.ark decoding
   {
      DumpLevelArchive levdump;
      levdump.start(basePath, param, isUw2);
   }
   break;

   case commandObjectProperties: // object properties decoding
   {
      DumpObjectProperties objprop;
      objprop.start(basePath);
   }
   break;

   case commandModelDump: // builtin model dumping
   {
      DumpBuiltinModels mdldump;
      mdldump.start(basePath, isUw2);
   }
   break;

   default:
      printf("no command given; show help with parameter -h\n");
      break;
   }

   return 0;
}
