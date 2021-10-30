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
/// \file String.cpp
/// \brief string functions implementation
//
#include "pch.hpp"
#include "String.hpp"
#include <algorithm>
#include <cctype>
#include <codecvt>
#include <cstdarg>

std::string Base::String::Format(const char* format, ...)
{
   va_list args;
   va_start(args, format);

   int length = vsnprintf(nullptr, 0, format, args);

   std::vector<char> buffer;
   buffer.resize(length + 1, 0);

   vsnprintf(buffer.data(), length + 1, format, args);
   va_end(args);

   buffer.push_back(0);

   return std::string(buffer.data());
}

/// \see https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
void Base::String::Replace(std::string& text, const std::string& textToFind, const std::string& replacementText)
{
   if (textToFind.empty())
      return;
   size_t startPos = 0;
   while ((startPos = text.find(textToFind, startPos)) != std::string::npos)
   {
      text.replace(startPos, textToFind.length(), replacementText);
      startPos += replacementText.length(); // In case 'replacementText' contains 'textToFind', like replacing 'x' with 'yx'
   }
}

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

void Base::String::TrimStart(std::string& text)
{
   for (; !text.empty() && isspace(text.at(0));)
      text.erase(0, 1);
}

void Base::String::TrimEnd(std::string& text)
{
   while (!text.empty())
   {
      size_t length = text.size() - 1;
      if (isspace(text.at(length)))
         text.erase(length);
      else
         break;
   }
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
