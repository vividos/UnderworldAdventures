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
/*! \file unittest.hpp

   \brief unit testing main header

*/
/*! \defgroup unittest Unit Testing

   Unit testing is used to verify that Underworld Adventures and its
   components are working and stay working. There should be a test for all
   classes and functions that are written. If a bug is found, a test should
   be written (if possible) that fails. When the bug is fixed the test should
   has to pass then.

   The library CppUnit is used for writing tests. For more informations,
   please visit http://cppunit.sourceforge.net/

   The HAVE_UNITTEST define has to be defined for the project to enable unit
   testing code.

*/
//@{

// include guard
#ifndef uwadv_unittest_hpp_
#define uwadv_unittest_hpp_

#ifdef HAVE_UNITTEST

// CppUnit includes
//#include <cppunit/TestCase.h>


// functions

//! runs all registered unit tests
bool ua_unittest_run();


#endif // HAVE_UNITTEST

#endif
//@}
