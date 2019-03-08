//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019 Michael Fink
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
/// \file String.cpp
/// \brief string functions implementation
//
#include "Base.hpp"
#include "String.hpp"
#include <algorithm>
#include <cctype>
#include <codecvt>

void Base::String::Lowercase(std::string& str)
{
   std::transform(str.begin(), str.end(), str.begin(),
      [](char c) -> char { return static_cast<char>(std::tolower(c)); });
}

void Base::String::Uppercase(std::string& str)
{
   std::transform(str.begin(), str.end(), str.begin(),
      [](char c) -> char { return static_cast<char>(std::toupper(c)); });
}

std::wstring Base::String::ConvertToUnicode(const std::string& utf8str)
{
   std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
   return conv.from_bytes(utf8str);
}

std::string Base::String::ConvertToUTF8(const std::wstring& wstr)
{
   std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
   return conv.to_bytes(wstr);
}
