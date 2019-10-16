//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Underworld Adventures Team
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
/// \file ConfigFile.hpp
/// \brief Config file class
//
#pragma once

#include <map>
#include <string>

namespace Base
{
   class TextFile;

   /// map type to hold key/value pairs
   typedef std::map<std::string, std::string> ConfigValueMap;

   /// \brief Config file class
   /// Supports reading and writing configuration files, stored as text files
   /// that have "key" and "value" pairs. "keys" are always unique throughout a
   /// config file. When a key appears twice while loading the file, the second
   /// value is used.
   ///
   /// The file might look as follows:<pre>
   /// ; one-line comment
   /// key1 value1
   /// key2 value2
   /// </pre>
   /// Keys and values are separated with at least one whitespace. When saving
   /// files one space character is put between them. Comments are preserved
   /// during writing; comments that are on a line with a key/value pair are
   /// written to a new line.
   class ConfigFile
   {
   public:
      /// ctor
      ConfigFile()
      {
      }

      /// dtor
      ~ConfigFile()
      {
      }

      /// loads a config file by filename
      void Load(const std::string& filename);

      /// loads a config file
      void Load(Base::TextFile& file);

      /// creates a new config file using the original file as template
      void Save(const std::string& originalFilename, const std::string& newFilename);

      /// returns map with all config values
      ConfigValueMap& GetValueMap() { return m_mapValues; }

   private:
      /// map with all config key/value pairs
      ConfigValueMap m_mapValues;
   };

} // namespace Base
