/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2004 Underworld Adventures Team

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
#include "common.hpp"

#ifdef HAVE_UNITTEST

#include "unittest.hpp"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>


// functions

/*! Runes all test suites and test cases that are registered using the
    CppUnit::TestFactoryRegistry class.

    \return true when all tests pass, false when at least one test failed
*/
bool ua_unittest_run()
{
   ua_trace("running unit tests...\n");

   CppUnit::TextUi::TestRunner runner;

   CppUnit::TestFactoryRegistry &registry =
      CppUnit::TestFactoryRegistry::getRegistry();

   runner.addTest( registry.makeTest() );

   bool wasSuccessful = runner.run();
   return wasSuccessful;
}


#endif // HAVE_UNITTEST
