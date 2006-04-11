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
/*! \file unittest.hpp

   \brief unit test implementation

*/
/*! \defgroup unittest Unit Testing

   Unit testing is used to verify that Underworld Adventures and its
   components are working and stay working. There should be a test for all
   classes and functions that are written. If a bug is found, a test should
   be written (if possible) that fails. When the bug is fixed the test should
   has to pass then.

   The library CppUnitMini is used for writing tests. It is taken from STLport
   and was modified a bit.

*/

// include guard
#ifndef uwadv_unittest_unittest_hpp_
#define uwadv_unittest_unittest_hpp_

// needed includes
#include "cppunitmini.hpp"
#include "base.hpp"
#include "tempfolder.hpp"

namespace Base
{
class Settings;
}

//! unit tests namespace \ingroup unittest
namespace UnitTest
{

//! unit test case base class
class UnitTestCase: public CppUnitMini::TestCase
{
public:
   //! returns global settings object to use during tests
   Base::Settings& GetTestSettings();

};

} // namespace UnitTest

#endif
