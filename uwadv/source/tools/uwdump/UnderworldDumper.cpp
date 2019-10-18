//
// Underworld Adventures - an Ultima Underworld remake project
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
/// \file UnderworldDumper.cpp
/// \brief underworld dumper class
//
#include "common.hpp"
#include "UnderworldDumper.hpp"
#include "String.hpp"
#include "FileSystem.hpp"
#include "Settings.hpp"
#include "ResourceManager.hpp"
#include "GameStringsImporter.hpp"
#include <map>
#include <functional>

extern void DumpLevArk(const std::string& filename, const GameStrings& gameStrings, bool isUw2);
extern void DumpObjectProperties(const std::string& filename, const GameStrings& gameStrings, bool isUw2);
extern void DumpUwExe(const std::string& filename, const GameStrings& gameStrings, bool isUw2);

typedef std::function<void(const std::string & filename, const GameStrings & gameStrings, bool isUw2)> T_fileHandler;

std::map<std::string, T_fileHandler> g_dumpFileHandlerMap =
{
   { "lev.ark", DumpLevArk },
   { "comobj.dat", DumpObjectProperties },
   { "uw.exe", DumpUwExe },
   { "uw2.exe", DumpUwExe },
   { "uwdemo.exe", DumpUwExe },
};

bool UnderworldDumper::ParseArgs(unsigned int argc, const char** argv)
{
   bool commandAppeared = false;
   bool needParameter = false;

   for (unsigned int i = 1; i < argc; i++)
   {
      if (strlen(argv[i]) > 1 && argv[i][0] == '-')
      {
         if (!ParseOption(argv[i]))
            return false;
      }
      else if (!commandAppeared) // no command yet?
      {
         commandAppeared = true;
         if (strcmp(argv[i], "dump") == 0)
         {
            m_command = commandDump;
            needParameter = true;
         }
         else if (strcmp(argv[i], "view") == 0)
         {
            m_command = commandView;
            needParameter = true;
         }
         else
            printf("unknown command: %s\n", argv[i]);
      }
      else if (needParameter) // command needs parameter?
      {
         needParameter = false;

         switch (m_command)
         {
         case commandDump:
         case commandView:
            m_param.assign(argv[i]);
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

/// the option string is guaranteed to be at least 2 chars long
bool UnderworldDumper::ParseOption(const char* option)
{
   switch (tolower(option[1]))
   {
   case 'd': // set base path
   {
      m_basePath.assign(&option[2]);

      if (m_basePath.find_last_of("\\/") != m_basePath.size() - 1)
         m_basePath.append(Base::FileSystem::PathSeparator);
   }
   break;

   case '2': // set game type to uw2
      m_isUw2 = true;
      break;

   case 'h': // help
   case '?':
      PrintHelp();
      return false;
   }

   return true;
}

void UnderworldDumper::PrintHelp() const
{
   printf("uwdump help:\n"
      " syntax:\n"
      "   uwdump <options> <command> <options>\n"
      "\n"
      " commands:\n"
      "   dump <filename>    dumps a single file to console; only some files are currently supported\n"
      "   view <filename>    opens a viewer window for the file; again only some files are supported\n"
      "\n"
      " options:\n"
      "   -d<basePath>       sets uw1/uw2 path; using current folder when not specified\n"
      "                      (game type is auto-detected)\n"
      "   -2                 set game type to ultima underworld 2 without further checking\n"
      "\n");
}

void UnderworldDumper::Run()
{
   CheckGameType();

   // execute command
   switch (m_command)
   {
   case commandDump:
   {
      DumpFile(m_param);
   }
   break;

   case commandView:
   {
      ViewFile(m_param);
   }
   break;

   default:
      printf("no command given; show help with parameter -h\n");
      break;
   }
}

void UnderworldDumper::DumpFile(const std::string& path)
{
   if (path.find_first_of('*') != std::string::npos ||
      path.find_first_of('?') != std::string::npos)
   {
      DumpWildcardFiles(path);
      return;
   }

   size_t pos = path.find_last_of(Base::FileSystem::PathSeparator[0]);
   std::string filename = pos == std::string::npos ? path : path.substr(pos + 1);
   Base::String::Lowercase(filename);

   auto iter = g_dumpFileHandlerMap.find(filename);
   if (iter != g_dumpFileHandlerMap.end())
   {
      printf("dumping file %s\n", path.c_str());
      iter->second(path, m_gameStrings, m_isUw2);
      printf("--------------------------------\n\n");
   }
   else
      printf("dumping file %s is currently not supported. Sorry!\n", filename.c_str());
}

void UnderworldDumper::DumpWildcardFiles(const std::string& path)
{
   std::vector<std::string> fileList;
   Base::FileSystem::FindFiles(path, fileList);

   for (const std::string& filename : fileList)
      DumpFile(filename);
}

void UnderworldDumper::ViewFile(const std::string& filename)
{
   printf("viewing file %s is currently not supported. Sorry!", filename.c_str());
}

void UnderworldDumper::CheckGameType()
{
   Base::Settings settings;
   settings.SetValue(Base::settingUnderworldPath, m_basePath);

   Base::ResourceManager resourceManager{ settings };

   // already set uw2?
   if (m_isUw2)
   {
      printf("game type set to ultima underworld 2\n");
      settings.SetGameType(Base::gameUw2);
   }
   else
   {
      resourceManager.DetectGameType(settings);
      m_isUw2 = settings.GetGameType() == Base::gameUw2;

      printf("detected game type: ultima underworld %i\n",
         m_isUw2 ? 2 : 1);
   }

   printf("loading game strings... ");
   Import::GameStringsImporter gameStringsImporter{ m_gameStrings };
   gameStringsImporter.LoadDefaultStringsPakFile(resourceManager);

   printf("done\n\n");
}
