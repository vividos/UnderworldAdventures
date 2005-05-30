/*
 * Copyright (c) 2003, 2004
 * Zdenek Nemec
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
 * The code was modified to be a real drop-in replacement for CppUnit.
 * Modifications (C) 2005 Michael Fink
 */

#ifndef CPPUNITMINI_H_
#define CPPUNITMINI_H_

#ifdef CPPUNIT_VERSION
#  error You cannot both include CppUnit and CppUnitMini headers!
#endif

#include <cstring>

#define CPPUNIT_NS CppUnitMini

#ifdef _CPPUNWIND
#  define CPPUNIT_MINI_USE_EXCEPTIONS
#endif


namespace CppUnitMini
{
   class Reporter
   {
   public:
      virtual ~Reporter() {}
      virtual void error(const char* /*macroName*/, const char* /*in_macro*/, const char* /*in_file*/, int /*in_line*/){}
      virtual void message(const char* /*msg*/) {}
      virtual void progress(const char* /*in_className*/, const char * /*in_testName*/){}
      virtual void printSummary(){}
   };

   class TestFixture
   {
   public:
      virtual ~TestFixture(){}

      //! \brief Set up context before running a test.
      virtual void setUp(){}

      //! \brief Clean up after the test run.
      virtual void tearDown(){}
   };

   class TestCase : public TestFixture
   {
   public:
      TestCase(){ registerTestCase(this); }
      virtual ~TestCase(){}

      static int run(Reporter* in_reporter = 0, const char* in_testName = "");
      int numErrors() const { return m_numErrors; }

      static void registerTestCase(TestCase* in_testCase)
      {
         in_testCase->m_next = m_root;
         m_root = in_testCase;
      }

      virtual void myRun(const char* /*in_name*/){}

      virtual void error(const char* in_macroName, const char* in_macro, const char* in_file, int in_line)
      {
         m_numErrors++;
         if (m_reporter)
            m_reporter->error(in_macroName, in_macro, in_file, in_line);
      }

      static void message(const char *msg)
      {
         if (m_reporter)
           m_reporter->message(msg);
      }

      bool equalDoubles(double in_expected, double in_real, double in_maxErr)
      {
         double diff = in_expected - in_real;
         if(diff < 0)
            diff = -diff;

         return diff < in_maxErr;
      }

      virtual void progress(const char* in_className, const char* in_functionName)
      {
         m_numTests++;
         if (m_reporter)
            m_reporter->progress(in_className, in_functionName);
      }

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
      static unsigned int m_numErrors;
      static unsigned int m_numTests;

   private:
      static TestCase* m_root;
      TestCase* m_next;

      static Reporter* m_reporter;
   };
}


#define CPPUNIT_TEST_SUITE(X) virtual void myRun(const char *in_name) { char *className = #X;

#if defined CPPUNIT_MINI_USE_EXCEPTIONS
#  define CPPUNIT_TEST(X) if (shouldRunThis(in_name, className, #X)) \
      { try { setUp(); progress(className, #X); X(); tearDown(); } \
      catch(...) { TestCase::error("Test Failed: An Exception was thrown.", #X, __FILE__, __LINE__); } }
#else
#  define CPPUNIT_TEST(X) if (shouldRunThis(in_name, className, #X)) \
      { setUp(); progress(className, #X); X(); tearDown(); }
#endif

#define CPPUNIT_TEST_EXCEPTION(X,Y) if (shouldRunThis(in_name, className, #X)){ progress(className, #X); }
#define CPPUNIT_TEST_SUITE_END() }
#define CPPUNIT_TEST_SUITE_REGISTRATION(X) static X local;

#define CPPUNIT_CHECK(X) if (!(X)){ TestCase::error("CPPUNIT_CHECK", #X, __FILE__, __LINE__); }
#define CPPUNIT_ASSERT(X) if (!(X)){ TestCase::error("CPPUNIT_ASSERT", #X, __FILE__, __LINE__); return; }
#define CPPUNIT_ASSERT_EQUAL(X,Y) if ((X)!=(Y)){ TestCase::error("CPPUNIT_ASSERT_EQUAL", #X","#Y, __FILE__, __LINE__); return; }
#define CPPUNIT_ASSERT_DOUBLES_EQUAL(X,Y,Z) if (!equalDoubles((X),(Y),(Z))){ TestCase::error("CPPUNIT_ASSERT_DOUBLES_EQUAL", #X","#Y","#Z, __FILE__, __LINE__); return; }
#define CPPUNIT_MESSAGE(m) CppUnitMini::TestCase::message(m)

#endif // for header
