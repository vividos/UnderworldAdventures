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
/*! \file physics.hpp

   \brief game physics model

*/

// include guard
#ifndef __uwadv_physics_hpp_
#define __uwadv_physics_hpp_

// needed includes
#include "uamath.hpp"
#include <vector>


// forward declarations
class ua_underworld;


// classes

class ua_physics_model
{
public:
   //! ctor
   ua_physics_model(){}

   //! inits physics model
   void init(ua_underworld *uw){ underw = uw; }

   //! evaluates player walking
   void walk_player(ua_vector2d &dir);

protected:
   //! current underworld object
   ua_underworld *underw;
};

#endif
