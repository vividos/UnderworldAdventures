/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

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
/*! \file physicsobject.hpp

   \brief an object that can be applied to collision

*/

// include guard
#ifndef uwadv_physicsobject_hpp_
#define uwadv_physicsobject_hpp_

// needed includes
#include "uamath.hpp"


//! physics object class
class ua_physics_object
{
public:
   //! ctor
   ua_physics_object();

   void set_ellipsoid(ua_vector3d size);
   ua_vector3d get_ellipsoid() const;

   void push_safe_spot(ua_vector3d spot);
   ua_vector3d pop_safe_spot(); 
   ua_vector3d get_fall_velocity() const;
   void reset_fall_velocity();
   void accellerate_fall_velocity(double time);

protected:
   ua_vector3d fall_velocity;
   ua_vector3d ellipsoid;

   // Cyclic safe spot buffer:
   ua_vector3d safeSpots[100];
   int safeSpotHead;
   int safeSpotTail;
};

#endif
