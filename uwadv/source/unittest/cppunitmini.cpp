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

#include "cppunitmini.hpp"

namespace CppUnitMini
{
   unsigned int TestCase::m_numErrors = 0;
   unsigned int TestCase::m_numTests = 0;

   TestCase* TestCase::m_root = 0;
   Reporter* TestCase::m_reporter = 0;

   int TestCase::run(Reporter* in_reporter, const char* in_testName)
   {
      TestCase::m_reporter = in_reporter;

      m_numErrors = 0;
      m_numTests = 0;

      TestCase* tmp = m_root;
      while(tmp != 0)
      {
         tmp->myRun(in_testName);
         tmp = tmp->m_next;
      }
      return m_numErrors;
   }
}
