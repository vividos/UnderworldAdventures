//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Underworld Adventures Team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file common.hpp
/// \brief commonly used includes and definitions
//
#pragma once

// auto-generated config file
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// get rid of warnings
#ifdef _MSC_VER
# pragma warning(disable: 4244) // conversion from 'x' to 'y', possible loss of data
# pragma warning(disable: 4100) // unreferenced formal parameter
#endif

#ifndef UNUSED
/// Use this to mark any (potentially) unused function parameter names in the
/// function body.
#define UNUSED(x) ((void)(x))
#endif

// SDL includes
#include <SDL2/SDL.h>

// uwadv includes
#include "Base.hpp"
#include "Math.hpp"
#include "Settings.hpp"
#include "GameInterface.hpp"
#include "IUserInterface.hpp"
