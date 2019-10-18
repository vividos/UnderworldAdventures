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
/// \file UnderworldDumper.hpp
/// \brief underworld dumper class
//
#pragma once

#include <string>
#include "GameStrings.hpp"

/// dumper for underworld objects
class UnderworldDumper
{
public:
   /// ctor
   UnderworldDumper()
      :m_command(commandNone),
      m_isUw2(false)
   {
   }

   /// parses arguments
   bool ParseArgs(unsigned int argc, const char** argv);

   /// runs underworld dumper
   void Run();

private:
   /// parses an option
   bool ParseOption(const char* option);

   /// prints help to console
   void PrintHelp() const;

   /// dumps a file; path can possibly have wildcards
   void DumpFile(const std::string& path);

   /// searches all files and dumps each single file
   void DumpWildcardFiles(const std::string& path);

   /// opens a viewer for given file
   void ViewFile(const std::string& filename);

   /// checks for game type to use
   void CheckGameType();

private:
   /// dump command type
   enum DumpCommand
   {
      commandNone = 0,  ///< no command specified
      commandDump,      ///< dumps a single file
      commandView,      ///< starts a viewer for the file
   };

   /// dump command
   DumpCommand m_command;

   /// base path of uw1 or uw2 game
   std::string m_basePath;

   /// command parameter
   std::string m_param;

   /// indicates if game file is from uw2
   bool m_isUw2;

   /// game strings
   GameStrings m_gameStrings;
};
