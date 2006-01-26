/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2006 Michael Fink

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file stringtest.cpp

   \brief String test

*/

// needed includes
#include "unittest.hpp"
#include "string.hpp"

namespace UnitTest
{

//! String functions test
/*! Tests string functions provided by Base::String. */
class TestString: public UnitTestCase
{
public:
   // generate test suite
   CPPUNIT_TEST_SUITE(TestString)
      CPPUNIT_TEST(TestCaseChange)
      CPPUNIT_TEST(TestConversion)
   CPPUNIT_TEST_SUITE_END()

protected:
   void TestCaseChange();
   void TestConversion();
};

// register test suite
CPPUNIT_TEST_SUITE_REGISTRATION(TestString)

} // namespace UnitTest

// methods

using namespace UnitTest;

/*! Tests case changing via Lowercase and Uppercase */
void TestString::TestCaseChange()
{
   std::string str1("testAbCd");
   std::string str2(str1);

   Base::String::Lowercase(str1);
   Base::String::Uppercase(str2);

   CPPUNIT_ASSERT(str1 == "testabcd");
   CPPUNIT_ASSERT(str2 == "TESTABCD");
}

/*! Tests conversion to unicode and UTF8
    Note: UTF8 conversion test is currently commented out, since it isn't
    implemented.
*/
void TestString::TestConversion()
{
   const char* c_cstrTestString =      "abcd1234";
   const wchar_t* c_cstrTestStringW = L"abcd1234";

   // conversion to unicode
   std::string str1(c_cstrTestString);
   std::wstring wstr1;
   CPPUNIT_ASSERT(true == Base::String::ConvertToUnicode(str1, wstr1));

   CPPUNIT_ASSERT(wstr1 == c_cstrTestStringW);

   // conversion to UTF8
/*
   std::wstring wstr2(L"\xfeff");
   std::vector<Uint8> vData;
   CPPUNIT_ASSERT(true == Base::String::ConvertToUTF8(wstr2, vData));
   CPPUNIT_ASSERT(vData.size() == 3);

   Uint8 auiData[] = { 0xef, 0xbb, 0xbf };
   CPPUNIT_ASSERT(0 == memcmp(auiData, &vData[0], 3));
*/
}
