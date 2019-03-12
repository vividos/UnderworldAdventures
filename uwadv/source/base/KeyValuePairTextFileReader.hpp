//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2019 Michael Fink
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
/// \file KeyValuePairTextFileReader.hpp
/// \brief text file reader that splits lines into key and value
//
#pragma once

namespace Base
{
   class TextFile;

   /// text file reader that splits lines into key and value
   class KeyValuePairTextFileReader
   {
   public:
      /// ctor
      KeyValuePairTextFileReader(Base::TextFile& file)
         :m_file(file)
      {
      }

      /// returns next key and value, or false when at end of file
      bool Next(std::string& key, std::string& value);

   private:
      /// splits a text file line into key and value, when found
      bool SplitKeyValue(std::string& line, std::string& key, std::string& value);

   private:
      /// text file to read
      Base::TextFile& m_file;
   };

} // namespace Base
