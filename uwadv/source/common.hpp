/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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
/*! \file common.hpp

   \brief commonly used includes and definitions

*/

// include guard
#ifndef __uwadv_common_hpp_
#define __uwadv_common_hpp_


// get rid of warnings
#ifdef WIN32
# pragma warning( disable : 4786 ) // identifier was truncated to '255' characters in the debug information
# pragma warning( disable : 4290 ) // C++ Exception Specification ignored
#endif


// SDL includes
#include "SDL.h"
#include "SDL_opengl.h"


// other includes
#include "uatypes.hpp"


// lua include
extern "C"
{
#include "lua/include/lua.h"
#include "lua/include/lualib.h"
}


// linux config file
#ifdef HAVE_CONFIG_H
#include "linux/config.h"
#endif


#endif
