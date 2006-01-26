/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2004,2005,2006 Michael Fink

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
/*! \file unittest.cpp

   \brief unit test implementation

*/

// needed includes
#include "unittest.hpp"

// global variables

//Base::Settings settings;


namespace Detail
{

//! reporter that prints out to debug console
class TraceReporter:
   public CppUnitMini::Reporter,
   public Base::NonCopyable
{
public:
   //! ctor
   TraceReporter() : m_iErrors(0), m_iTests(0){}
   //! dtor
   virtual ~TraceReporter(){}

   virtual void error(const char* in_macroName, const char* in_macro, const char* in_file, int in_line)
   {
      m_iErrors++;
      UaTrace("\n%s(%d) : %s(%s);\n", in_file, in_line, in_macroName, in_macro);
   }

   virtual void message(const char* msg)
   {
      UaTrace("\t%s\n", msg);
   }

   virtual void progress(const char* in_className, const char* in_shortTestName)
   {
      m_iTests++;
      UaTrace("%s::%s\n", in_className, in_shortTestName);
   }

   virtual void printSummary()
   {
      if (m_iErrors > 0)
         UaTrace("There were errors! (%d of %d)\n", m_iErrors, m_iTests);
      else
         UaTrace("\nOK (%d)\n\n", m_iTests);
   }

private:
   //! number of errors so far
   int m_iErrors;

   //! number of tests run so far
   int m_iTests;
};

}

// UnitTestCase methods

//Settings& UnitTestCase::GetSettings(){ return settings; }


// global functions

#undef main

//! main function
/*! runs all unit tests registered with the macro CPPUNIT_TEST_SUITE_REGISTRATION

  \return true when all tests pass, false when at least one test failed
*/
int main()
{
   // init settings variable
//   std::string strUwPath("./");
//   settings.SetValue(settingUnderworldPath, strUwPath);

   // start unit tests
   Detail::TraceReporter reporter;

   int iNumErrors = CppUnitMini::TestCase::run(&reporter, "");
   reporter.printSummary();

   return iNumErrors;
}
