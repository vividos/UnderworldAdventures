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
#include "settings.hpp"
#include "filesystem.hpp"

// global variables

//! settings common for all unit tests
Base::Settings g_settings;


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
         UaTrace("\nThere were errors! (%d of %d)\n", m_iErrors, m_iTests);
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

Base::Settings& UnitTest::UnitTestCase::GetTestSettings(){ return g_settings; }


// global functions

// undef main that points to SDL_main entry point; ensures console is shown
#undef main

//! main function
/*! runs all unit tests registered with the macro CPPUNIT_TEST_SUITE_REGISTRATION

  \return true when all tests pass, false when at least one test failed
*/
int main()
{
   UaTrace("Underworld Adventures Unit Tests\n\n");

   // init global settings variable
   try
   {
      Base::LoadSettings(g_settings);
   }
   catch(Base::Exception& e)
   {
      UaTrace("Exception while config file loading: %s\n", e.what());
      return 1;
   }

   // check settings if they are right
   std::string strFilename = g_settings.GetString(Base::settingUw1Path) + "/uw.exe";
   if (!Base::FileSystem::FileExists(strFilename))
   {
      UaTrace("The Ultima Underworld 1 files cannot be found in %s\n"
         "Make sure to have a properly configured uwadv.cfg file!\n",
         strFilename.c_str());
      return 1;
   }

   strFilename = g_settings.GetString(Base::settingUw2Path) + "/uw2.exe";
   if (!Base::FileSystem::FileExists(strFilename))
   {
      UaTrace("The Ultima Underworld 2 files cannot be found in %s\n"
         "Make sure to have a properly configured uwadv.cfg file!\n",
         strFilename.c_str());
      return 1;
   }

   UaTrace("\nRunning tests...\n\n");

   // start unit tests
   Detail::TraceReporter reporter;

   int iNumErrors = CppUnitMini::TestCase::run(&reporter, "");
   reporter.printSummary();

   return iNumErrors;
}
