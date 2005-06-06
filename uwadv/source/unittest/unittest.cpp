/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2004,2005 Underworld Adventures Team

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
/*! \defgroup unittest Unit Testing

   Unit testing is used to verify that Underworld Adventures and its
   components are working and stay working. There should be a test for all
   classes and functions that are written. If a bug is found, a test should
   be written (if possible) that fails. When the bug is fixed the test should
   has to pass then.

   The library CppUnit is used for writing tests. For more informations,
   please visit http://cppunit.sourceforge.net/

*/

// needed includes
#include "common.hpp"
#include "unittest.hpp"


// global variables

ua_settings settings;


// classes

//! reporter that prints out to debug console
class ua_trace_reporter: public CPPUNIT_NS::Reporter
{
public:
   ua_trace_reporter() : m_num_errors(0), m_num_tests(0){}
   virtual ~ua_trace_reporter(){}

   virtual void error(const char* in_macroName, const char* in_macro, const char* in_file, int in_line)
   {
      m_num_errors++;
      ua_trace("\n%s(%d) : %s(%s);\n", in_file, in_line, in_macroName, in_macro);
   }

   virtual void message(const char* msg)
   {
      ua_trace("\t%s\n", msg);
   }

   virtual void progress(const char* in_className, const char* in_shortTestName)
   {
      m_num_tests++;
      ua_trace("%s::%s\n", in_className, in_shortTestName);
   }

   virtual void printSummary()
   {
      if (m_num_errors > 0)
         ua_trace("There were errors! (%d of %d)\n", m_num_errors, m_num_tests);
      else
         ua_trace("\nOK (%d)\n\n", m_num_tests);
   }

private:
   ua_trace_reporter(const ua_trace_reporter&){}
   const ua_trace_reporter& operator=(const ua_trace_reporter&){ return *this; }

private:
   int m_num_errors;
   int m_num_tests;
};


// ua_testcase methods

ua_settings& ua_testcase::get_settings(){ return settings; }


// ua_test_tempdir methods

ua_test_tempdir::ua_test_tempdir()
{
   pathname = "./temp";

   ua_mkdir(pathname.c_str(), 700);
}

ua_test_tempdir::~ua_test_tempdir()
{
   std::vector<std::string> filelist;
   std::string searchpath(pathname + "/*");
   ua_find_files(searchpath.c_str(), filelist);

   std::vector<std::string>::size_type max = filelist.size();
   for (std::vector<std::string>::size_type i=0; i<max; i++)
      remove(filelist[i].c_str());

//TODO   ua_rmdir("./temp");
}


// global functions

#undef main

//! main function
/*! runs all unit tests registered with the macro CPPUNIT_TEST_SUITE_REGISTRATION

  \return true when all tests pass, false when at least one test failed
*/
int main()
{
   // init settings variable
   std::string uw_path("./");
   settings.set_value(ua_setting_uw_path, uw_path);

   // start unit tests
   ua_trace_reporter reporter;

   int num_errors = CPPUNIT_NS::TestCase::run(&reporter, "");
   reporter.printSummary();

   return num_errors;
}
