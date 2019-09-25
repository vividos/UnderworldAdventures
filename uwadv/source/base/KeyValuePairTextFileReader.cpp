//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2019 Underworld Adventures Team
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
/// \file KeyValuePairTextFileReader.cpp
/// \brief text file reader that splits lines into key and value
//
#include "pch.hpp"
#include "KeyValuePairTextFileReader.hpp"
#include "TextFile.hpp"

using Base::KeyValuePairTextFileReader;

bool KeyValuePairTextFileReader::Next(std::string& key, std::string& value)
{
   std::string line;

   while (m_file.Tell() < m_file.FileLength())
   {
      m_file.ReadLine(line);

      if (SplitKeyValue(line, key, value))
         return true;
   }

   return false;
}

bool KeyValuePairTextFileReader::SplitKeyValue(std::string& line, std::string& key, std::string& value)
{
   if (line.empty())
      return false;

   Base::String::TrimStart(line);

   // comment line?
   if (line.size() == 0 || line.at(0) == '#' || line.at(0) == ';')
      return false;

   // comment somewhere in the line?
   std::string::size_type pos2 = line.find('#');
   if (pos2 != std::string::npos)
   {
      // remove comment
      line.erase(pos2);
   }

   Base::String::TrimEnd(line);

   // empty line?
   if (line.empty())
      return false;

   // replace all '\t' with ' '
   std::string::size_type pos = 0;
   while ((pos = line.find('\t', pos)) != std::string::npos)
      line.replace(pos, 1, " ");

   // there must be at least one space, to separate key from value
   pos = line.find(' ');
   if (pos == std::string::npos)
      return false;

   // retrieve key and value
   key = line.substr(0, pos);
   value = line.substr(pos + 1);

   Base::String::TrimStart(value);

   return true;
}
