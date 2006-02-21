/*
 * Copyright (c) 2003, 2004 Zdenek Nemec
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */
/*
 * The code was modified to be used in Underworld Adventures.
 * Modifications (C) 2005,2006 Michael Fink
 */
/*! \file cppunitmini.hpp

   \brief CppUnitMini header

*/

// include guard
#ifndef uwadv_unittest_cppunitmini_hpp_
#define uwadv_unittest_cppunitmini_hpp_

// needed includes
#include <cstring>

#ifdef _CPPUNWIND
#  define CPPUNIT_MINI_USE_EXCEPTIONS
#endif

//! namespace for CppUnitMini \ingroup unittest
namespace CppUnitMini
{
   //! reporter class
   class Reporter
   {
   public:
      //! dtor
      virtual ~Reporter(){}
      //! called when an error is detected
      virtual void error(const char* /*macroName*/, const char* /*in_macro*/, const char* /*in_file*/, int /*in_line*/){}
      //! called for CPPUNIT_MESSAGE
      virtual void message(const char* /*msg*/){}
      //! called to write progress message at start of test function
      virtual void progress(const char* /*in_className*/, const char * /*in_testName*/){}
      //! called to print a summary at the end
      virtual void printSummary(){}
   };

   //! test fixture
   class TestFixture
   {
   public:
      //! dtor
      virtual ~TestFixture(){}

      //! set up context before running a test
      virtual void setUp(){}

      //! clean up after the test run
      virtual void tearDown(){}
   };

   //! test case
   class TestCase : public TestFixture
   {
   public:
      //! ctor
      TestCase():m_next(NULL){ registerTestCase(this); }
      //! dtor
      virtual ~TestCase(){}

      //! runs all registered tests
      static int run(Reporter* in_reporter = 0, const char* in_testName = "");
      //! returns number of errors
      int numErrors() const { return m_iErrors; }

      //! registers a test case in the test case registry
      static void registerTestCase(TestCase* in_testCase)
      {
         in_testCase->m_next = m_root;
         m_root = in_testCase;
      }

      //! runs all tests of a test case
      virtual void myRun(const char* /*in_name*/){}

      //! hands errors to reporter
      virtual void error(const char* in_macroName, const char* in_macro, const char* in_file, int in_line)
      {
         m_iErrors++;
         if (m_reporter)
            m_reporter->error(in_macroName, in_macro, in_file, in_line);
      }

      //! hands messages to reporter
      static void message(const char *msg)
      {
         if (m_reporter)
           m_reporter->message(msg);
      }

      //! checks if double values are equal
      bool equalDoubles(double in_expected, double in_real, double in_maxErr)
      {
         double diff = in_expected - in_real;
         if( diff < 0)
            diff = -diff;

         return diff < in_maxErr;
      }

      //! called when doing progress
      virtual void progress(const char* in_className, const char* in_functionName)
      {
         m_iTests++;
         if (m_reporter)
            m_reporter->progress(in_className, in_functionName);
      }

      //! checks if a given test should be run
      bool shouldRunThis(const char* in_desiredTest, const char* in_className, const char* in_functionName)
      {
         if ((in_desiredTest) && (in_desiredTest[0] != '\0'))
         {
            const char *ptr = strstr(in_desiredTest, "::");
            if (ptr)
            {
               return (strncmp(in_desiredTest, in_className, strlen(in_className)) == 0)
                  && (strncmp(ptr + 2, in_functionName, strlen(in_functionName)) == 0);
            }
            return (strcmp(in_desiredTest, in_className) == 0);
         }
         return true;
      }

   protected:
      //! number of errors so far
      static unsigned int m_iErrors;
      //! number of tests ran so far
      static unsigned int m_iTests;

   private:
      //! start of test case registry single-linked list
      static TestCase* m_root;
      //! next test case
      TestCase* m_next;

      //! reporter
      static Reporter* m_reporter;
   };
}


// macros

//! starts a test suite
#define CPPUNIT_TEST_SUITE(X) virtual void myRun(const char *in_name) { char *className = #X; (in_name); (className);

#if defined CPPUNIT_MINI_USE_EXCEPTIONS
//! adds a test to the test suite
#  define CPPUNIT_TEST(X) if (shouldRunThis(in_name, className, #X)) \
      { try { setUp(); progress(className, #X); X(); tearDown(); } \
      catch(...) { TestCase::error("Test Failed: An Exception was thrown.", #X, __FILE__, __LINE__); } }
#else
//! adds a test to the test suite
#  define CPPUNIT_TEST(X) if (shouldRunThis(in_name, className, #X)) \
      { setUp(); progress(className, #X); X(); tearDown(); }
#endif

//! ends a test suite
#define CPPUNIT_TEST_SUITE_END() }

//! registers a test suite in the test suite registry
#define CPPUNIT_TEST_SUITE_REGISTRATION(X) static X local;

//! checks a condition and reports error in case; continues test function
#define CPPUNIT_CHECK(X) if (!(X)){ TestCase::error("CPPUNIT_CHECK", #X, __FILE__, __LINE__); }

//! checks a condition and reports error in case; returns from test function
#define CPPUNIT_ASSERT(X) if (!(X)){ TestCase::error("CPPUNIT_ASSERT", #X, __FILE__, __LINE__); return; }

//! checks if two values are equal and reports error when they are not
#define CPPUNIT_ASSERT_EQUAL(X,Y) if ((X)!=(Y)){ TestCase::error("CPPUNIT_ASSERT_EQUAL", #X","#Y, __FILE__, __LINE__); return; }

//! checks if two double values are equal, given an epsilon value how far they may differ
#define CPPUNIT_ASSERT_DOUBLES_EQUAL(X,Y,Z) if (!equalDoubles((X),(Y),(Z))){ TestCase::error("CPPUNIT_ASSERT_DOUBLES_EQUAL", #X","#Y","#Z, __FILE__, __LINE__); return; }

//! prints out an informative message
#define CPPUNIT_MESSAGE(m) CppUnitMini::TestCase::message(m)

//! always reports an error; same as CPPUNIT_ASSERT(false)
#define CPPUNIT_ASSERT_FALSE() { TestCase::error("CPPUNIT_ASSERT", "false", __FILE__, __LINE__); return; }

#endif
