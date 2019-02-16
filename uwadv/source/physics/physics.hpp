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
/// \file physics.hpp
/// \brief game physics model
//
#pragma once

#include "uamath.hpp"
#include "triangle3d.hpp"
#include "physicsbody.hpp"
#include <vector>

struct ua_collision_data;

/// physics parameter
enum ua_physics_param
{
   /// controls if gravity is active in the physics model
   ua_physics_gravity = 0,

   /// last param; not used
   ua_physics_param_max
};

/// physics model callback interface
class ua_physics_model_callback
{
public:
   /// returns surrounding triangles on given position
   virtual void get_surrounding_triangles(unsigned int xpos,
      unsigned int ypos, std::vector<ua_triangle3d_textured>& allTriangles) = 0;
};

/// physics model class
class ua_physics_model
{
public:
   /// ctor
   ua_physics_model()
      :callback(NULL)
   {
   }

   /// inits the physics model
   void init(ua_physics_model_callback* callback);

   bool get_physics_param(ua_physics_param param) const
   {
      return params[param];
   }

   void set_physics_param(ua_physics_param param, bool value)
   {
      params[param] = value;
   }

   /// evaluate physics on objects
   void eval_physics(double time_elapsed);

   /// tracks object movement using current parameters
   void track_object(ua_physics_body& body);

   /// add physics body to track to model
   void add_track_body(ua_physics_body* body)
   {
      tracked_bodies.push_back(body);
   }

private:
   /// tracks object movement for given direction vector
   bool track_object(ua_physics_body& body, ua_vector3d dir,
      bool gravity_force = false);

   /// recursive collision response calculation
   bool collide_with_world(ua_collision_data& data, ua_vector3d& pos,
      const ua_vector3d& dir);

   /// checks mesh for collision
   void check_collision(ua_collision_data& data);

   /// checks single triangle for collision
   void check_triangle(ua_collision_data& data, const ua_vector3d& p1,
      const ua_vector3d& p2, const ua_vector3d& p3);

   /// solves quadratic equation and returns solution < t
   bool get_lowest_root(double a, double b, double c, double t,
      double& new_t);

protected:
   /// model parameters
   bool params[ua_physics_param_max];

   /// recursion depth for collide_with_world()
   int collision_recursion_depth;

   /// callback interface pointer
   ua_physics_model_callback* callback;

   /// list of pointer to bodies tracked by physics model
   std::vector<ua_physics_body*> tracked_bodies;

   // friend to test class
   friend class ua_test_physics_model;
};
