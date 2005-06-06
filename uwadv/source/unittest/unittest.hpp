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
/*! \file unittest.hpp

   \brief unit test implementation

*/
//! \ingroup unittest

//@{

// include guard
#ifndef uwadv_unittest_hpp_
#define uwadv_unittest_hpp_

// needed includes
#include "CppUnitMini.h"
#include "settings.hpp"


class ua_testcase: public CPPUNIT_NS::TestCase
{
public:
   ua_settings& get_settings();

};

class ua_test_tempdir
{
public:
   ua_test_tempdir();
   ~ua_test_tempdir();

   std::string get_pathname() const { return pathname; }
private:
   std::string pathname;
};


#endif
//@}
