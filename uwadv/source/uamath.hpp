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
/*! \file uamath.hpp

   math stuff

*/

// include guard
#ifndef __uwadv_uamath_hpp_
#define __uwadv_uamath_hpp_

// needed includes
#include <cmath>

// pragmas for visual c++
#pragma inline_depth( 255 )
#pragma inline_recursion( on )
#pragma auto_inline( on )


// constants

const double ua_pi = 3.141592653589793;


// macros

#define ua_deg2rad(a) ((double(a)/180.0)*ua_pi)
#define ua_rad2deg(a) ((double(a)/ua_pi)*180.0)


// classes

class ua_vector2d
{
public:
   //! vector elements
   double x,y;

public:
   //! ctor
   ua_vector2d(){ x=y=0.0; }
   //! copy ctor
   ua_vector2d(const ua_vector2d &vec){ x=vec.x; y=vec.y; }
   //! assign operator
   ua_vector2d& operator =(const ua_vector2d &vec){ x=vec.x; y=vec.y; return *this; }

   //! ctor with x and y coordinates
   ua_vector2d(double ax, double ay){ x=ax; y=ay; }

   //! sets vector by giving polar coordinates (angle in degree, not radians)
   void set_polar(double len, double angle)
   { x=len*cos(ua_deg2rad(angle)); y=len*sin(ua_deg2rad(angle)); }

   //! add assignment
   ua_vector2d& operator +=(const ua_vector2d &vec){ x+=vec.x; y+=vec.y; return *this; }

   // multiplication assignment
   ua_vector2d& operator *=(double sc){ x*=sc; y*=sc; return *this; }

   //! returns length of vector
   double length(){ sqrt(x*x+y*y); }

   //! normalizes vector
   void normalize(){ (*this)*=(1.0/length()); }
};


#endif
