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
/// \file StringTest.cpp
/// \brief String test
//
#include "pch.hpp"
#include "String.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
   /// \brief String functions tests
   /// Tests string functions provided by Base::String.
   TEST_CLASS(StringTest)
   {
      /// Tests case changing via Lowercase and Uppercase
      TEST_METHOD(TestCaseChange)
      {
         std::string str1("testAbCd");
         std::string str2(str1);

         Base::String::Lowercase(str1);
         Base::String::Uppercase(str2);

         Assert::IsTrue(str1 == "testabcd");
         Assert::IsTrue(str2 == "TESTABCD");
      }

      /// Tests conversion to unicode and UTF8
      TEST_METHOD(TestConversion)
      {
         const char* c_cstrTestString = "abcd1234";
         const wchar_t* c_cstrTestStringW = L"abcd1234";

         // conversion to unicode
         std::string str1(c_cstrTestString);
         std::wstring wstr1 = Base::String::ConvertToUnicode(str1);

         Assert::IsTrue(wstr1 == c_cstrTestStringW);

         // conversion to UTF8
         std::wstring wstr2(L"\xfeff");
         std::string utf8data = Base::String::ConvertToUTF8(wstr2);
         Assert::IsTrue(utf8data.size() == 3);

         Uint8 data[] = { 0xef, 0xbb, 0xbf };
         Assert::IsTrue(0 == memcmp(data, utf8data.data(), 3));
      }
   };
} // namespace UnitTest
