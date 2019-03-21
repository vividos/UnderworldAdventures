//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Michael Fink
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
/// \file Math.hpp
/// \brief math stuff
//
// Note: all angle params should be passed to the functions in degrees, not
// radians.
//
#pragma once

#include <cmath>

/// pi constant
const double c_pi = 3.141592653589793;

/// converts angle from degree values (0°..360°) to radians (0..2pi)
#define Deg2rad(a) ((double(a) / 180.0) * c_pi)
/// converts angle from randians values to degrees
#define Rad2deg(a) ((double(a) / c_pi) * 180.0)

#include "Vector2d.hpp"
#include "Vector3d.hpp"
