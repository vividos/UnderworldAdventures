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
/*! \file physics.hpp

   \brief game physics model

*/
//! \ingroup underworld

//@{

// include guard
#ifndef uwadv_physics_hpp_
#define uwadv_physics_hpp_

// needed includes
#include "uamath.hpp"
#include "physicsobject.hpp"
#include <vector>


// forward declarations
class ua_underworld;
struct ua_collision_data;


// classes

//! physics model class
class ua_physics_model
{
public:
   //! ctor
   ua_physics_model();

   //! inits physics model
   void init(ua_underworld* uw);

   //! evaluates physics of player and objects
   void eval_physics(double time);


   // functions for tracking objects in 3d space

   //! returns number of objects that are tracked
   unsigned int get_tracking_count();

   //! tracks object with given index
   void track_object(unsigned int index);

protected:
   //! tracks object by position and direction; vectors must be in ellipsoid space
   void track_object(ua_physics_object& object, ua_vector3d& pos,
      const ua_vector3d& dir, bool gravity_call, double time);

   //! recursive collision response calculation
   bool collide_with_world(ua_physics_object& object, ua_vector3d& pos,
      const ua_vector3d& dir);

   //! checks mesh for collision
   void check_collision(ua_physics_object& object, int xpos, int ypos,
      ua_collision_data& data);

protected:
   //! current underworld object
   ua_underworld* underw;

   //! current recursion depth for function collide_with_world()
   unsigned int collision_recursion_depth;

   //! last evaluation time
   double last_evaltime;
};


#endif
//@}
