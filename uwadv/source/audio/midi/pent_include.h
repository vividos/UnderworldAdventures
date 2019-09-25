/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2006 Underworld Adventures Team

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
/*! \file pent_include.h

   \brief needed includes and stuff for Pentagram

*/

// include guard
#ifndef uwadv_audio_midi_pent_include_h_
#define uwadv_audio_midi_pent_include_h_

// needed includes
#include "Base.hpp"

#ifdef HAVE_MSVC
#pragma warning(disable: 4100) // 'T' : unreferenced formal parameter
#pragma warning(disable: 4244) // 'initializing' : conversion from 'T' to 'U', possible loss of data
#pragma warning(disable: 4512) // 'T' : assignment operator could not be generated
#pragma warning(disable: 4189) // 'T' : local variable is initialized but not referenced
#pragma warning(disable: 4127) // conditional expression is constant
#pragma warning(disable: 4201) // nonstandard extension used : nameless struct/union
#pragma warning(disable: 4706) // assignment within conditional expression
#endif

#if defined(HAVE_WIN32) && !defined(WIN32)
#define WIN32
#endif

#include "common_types.h"
#include <string>
#include <iostream>
#include <cassert>


#define pout std::cout
#define perr std::cerr

#define FORGET_OBJECT(x) (x = NULL)


#include "String.hpp"

namespace Pentagram
{
   inline int strcasecmp(const char* str1, const char* str2)
   {
      std::string strText1(str1), strText2(str2);
      Base::String::Lowercase(strText1);
      Base::String::Lowercase(strText2);
      return strText1.compare(strText2);
   }
}

#include "IDataSource.h"

#endif
