//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
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
/// \file physicsbody.hpp
/// \brief an object that can be applied to collision/physics
//
#pragma once

#include "uamath.hpp"

/// \brief a body in the world that can be applied to physics
class ua_physics_body
{
public:
   /// dtor
   virtual ~ua_physics_body() {}

   /// sets new elapsed time; used at the begin of tracking this object
   virtual void set_new_elapsed_time(double time_elapsed) { time_elapsed; }

   /// returns body position of the center
   virtual ua_vector3d get_pos() = 0;

   /// sets body position
   virtual void set_pos(ua_vector3d& pos) = 0;

   /// returns body direction
   virtual ua_vector3d get_dir() { return ua_vector3d(0, 0, 0); }

   /// returns ellipsoid of body
   const ua_vector3d& get_ellipsoid() { return ellipsoid; }

   // gravity related

   /// resets gravity when hitting floor
   virtual void reset_gravity() {}

   /// returns gravity vector for object
   virtual ua_vector3d get_gravity_force() { return ua_vector3d(0, 0, 0); }

   /// item hits the floor
   virtual void hit_floor() {}

protected:
   /// body ellipsoid
   ua_vector3d ellipsoid;

   /// indicates if body can move by itself
   bool moving;
};

///// a body that can move around by itself
//class ua_physics_body_moving: public ua_physics_body
//{
//};
