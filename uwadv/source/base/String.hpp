//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019 Underworld Adventures Team
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
/// \file String.hpp
/// \brief string functions
//
#pragma once

#include <string>
#include <vector>

namespace Base
{
   /// \brief String functions
   namespace String
   {
      /// replaces a string in a text with a replacement text
      void Replace(std::string& text, const std::string& textToFind, const std::string& replacementText);

      /// converts a string to lowercase
      void Lowercase(std::string& str);
      /// converts a string to uppercase
      void Uppercase(std::string& str);

      /// trims spaces at start of string
      void TrimStart(std::string& text);

      /// trims spaces at end of string
      void TrimEnd(std::string& text);

      /// converts an UTF-8 string to  unicode string
      std::wstring ConvertToUnicode(const std::string& utf8str);

      /// converts an unicode string to UTF-8 representation
      std::string ConvertToUTF8(const std::wstring& wstr);
   }

} // namespace Base
