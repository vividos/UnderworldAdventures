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
/*! \file physicsobject.cpp

   \brief an object that can be applied to collision

*/

// needed includes
#include "common.hpp"
#include "physicsobject.hpp"


//! default initial gravity pull
const double ua_default_gravity_pull = -0.03*8;
const double ua_max_gravity_pull = -0.5*8; // TODO - tweak


// ua_physics_object methods

ua_physics_object::ua_physics_object()
{
   safeSpotHead = -1;
   safeSpotTail = -1;

   reset_fall_velocity();
}


void ua_physics_object::set_ellipsoid(ua_vector3d size)
{
   ellipsoid = size;
}

ua_vector3d ua_physics_object::get_ellipsoid() const
{
   return ellipsoid;
}


void ua_physics_object::push_safe_spot(ua_vector3d spot)
{
   // first insertion
   if (safeSpotHead == -1)
   {
      safeSpotHead = 0;
      safeSpotTail = 0;
      safeSpots[safeSpotHead] = spot;
   }
   else
   {
      // only insert new spot if it's far enough away from the last inserted one
      double dist = (safeSpots[safeSpotHead] - spot).length();
      if (dist < 0.1)
         return;

      // move tail if needed (full buffer)
      if ((safeSpotHead + 1) % 100 == safeSpotTail)
         safeSpotTail = (safeSpotTail + 1) % 100;

      safeSpotHead = (safeSpotHead + 1) % 100;
      safeSpots[safeSpotHead] = spot;
   }
}

ua_vector3d ua_physics_object::pop_safe_spot()
{
   ua_vector3d spot(0,0,0);

   if (safeSpotHead > 0)
   {
      spot = safeSpots[safeSpotHead];

      // Check we have not reached the tail yet.
      if (safeSpotHead != safeSpotTail)
      {
         safeSpotHead--;
         if (safeSpotHead < 0)
            safeSpotHead = safeSpotHead + 100;
      }
   }

   return spot;
}

ua_vector3d ua_physics_object::get_fall_velocity() const
{
   return fall_velocity;
}

void ua_physics_object::reset_fall_velocity()
{
   fall_velocity = ua_vector3d(0.0, 0.0, ua_default_gravity_pull);
}

void ua_physics_object::accellerate_fall_velocity(double time)
{
   // TODO - not at all physically correct.. just a quick hack
   fall_velocity.z += ua_default_gravity_pull;
   if (fall_velocity.z < ua_max_gravity_pull)
      fall_velocity.z = ua_max_gravity_pull;
}
