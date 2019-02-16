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
/// \file playerstate.cpp
/// \brief player state for physics simulation
//
#include "common.hpp"
#include "playerphysics.hpp"
#include "player.hpp"

/// max speed a player can walk, in tiles / second
const double ua_player_max_walk_speed = 2.4;

/// max speed a player can slide left or right
const double ua_player_max_slide_speed = 1.0;

/// max speed a player can rotate, in degrees / second
const double ua_player_max_rotate_speed = 90;

/// max speed a player can rotate the view, in degrees / second
const double ua_player_max_view_rotate_speed = 60;

/// x size of the player m_ellipsoid
const double ua_ellipsoid_x = 0.2;
/// y size of the player m_ellipsoid
const double ua_ellipsoid_y = 0.2;
/// z size of the player m_ellipsoid
const double ua_ellipsoid_z = 11.9;

ua_player_physics_object::ua_player_physics_object(Underworld::Player& player, bool enhancedFeatures)
   :m_player(player),
   m_ellipsoid(ua_ellipsoid_x, ua_ellipsoid_y, ua_ellipsoid_z),
   m_movementMode(0),
   m_fallTime(0.0),
   m_fallHeightStart(0.0),
   m_maxPanAngle(enhancedFeatures ? 45.0 : 75.0)
{
   m_moveFactors[ua_move_walk] = 0.0;
   m_moveFactors[ua_move_rotate] = 0.0;
   m_moveFactors[ua_move_lookup] = 0.0;
   m_moveFactors[ua_move_jump] = 0.0;
   m_moveFactors[ua_move_slide] = 0.0;
   m_moveFactors[ua_move_float] = 0.0;
}

void ua_player_physics_object::set_movement_mode(unsigned int set, unsigned int del)
{
   m_movementMode = (m_movementMode | set) & (~del);
}

void ua_player_physics_object::set_movement_factor(ua_player_movement_mode mode, double factor)
{
   m_moveFactors[mode] = factor;
}

double ua_player_physics_object::get_movement_factor(ua_player_movement_mode mode)
{
   return m_moveFactors[mode];
}

void ua_player_physics_object::rotate_move(double elapsedTime)
{
   unsigned int mode = get_movement_mode();

   // player rotation
   if (mode & ua_move_rotate)
   {
      double angle = ua_player_max_rotate_speed * elapsedTime *
         get_movement_factor(ua_move_rotate);
      angle += m_player.GetRotateAngle();

      // keep angle in range [0; 360]
      while (angle > 360.0) angle -= 360.0;
      while (angle < 0.0) angle += 360.0;

      m_player.SetRotateAngle(angle);
   }

   // view up/down
   if (mode & ua_move_lookup)
   {
      double viewangle = m_player.GetPanAngle();
      viewangle -= ua_player_max_view_rotate_speed * elapsedTime *
         get_movement_factor(ua_move_lookup);

      // restrict up-down view angle
      if (viewangle < -m_maxPanAngle) viewangle = -m_maxPanAngle;
      if (viewangle > m_maxPanAngle) viewangle = m_maxPanAngle;

      m_player.SetPanAngle(viewangle);
   }
}

void ua_player_physics_object::set_new_elapsed_time(double elapsedTime)
{
   m_fallTime += elapsedTime;
}

ua_vector3d ua_player_physics_object::get_pos()
{
   return ua_vector3d(m_player.GetXPos(), m_player.GetYPos(), m_player.GetHeight());
}

void ua_player_physics_object::set_pos(ua_vector3d& pos)
{
   m_player.SetPos(pos.x, pos.y);
   m_player.SetHeight(pos.z);
}

bool my_movement;

ua_vector3d ua_player_physics_object::get_dir()
{
   double elapsedTime = 0.05;

   ua_vector3d dir;

   unsigned int mode = get_movement_mode();

   if ((mode & ua_move_walk) || (mode & ua_move_slide))
   {
      double speed = ua_player_max_walk_speed * elapsedTime;
      double angle = m_player.GetRotateAngle();

      my_movement = true;

      // adjust angle for sliding
      if (mode & ua_move_slide)
      {
         angle -= 90.0;
         speed *= get_movement_factor(ua_move_slide);
         speed *= ua_player_max_slide_speed / ua_player_max_walk_speed;
      }
      else
         speed *= get_movement_factor(ua_move_walk);

      // construct direction vector
      dir.x = 1.0;
      dir.rotate_z(angle);
      dir *= speed;
   }

   return dir;
}

void ua_player_physics_object::reset_gravity()
{
   m_fallTime = 0.0;
   m_fallHeightStart = m_player.GetHeight();
}

ua_vector3d ua_player_physics_object::get_gravity_force()
{
   double gravity = 15.0 * m_fallTime * m_fallTime;

   // limit gravity for falling
   if (gravity > 10.0)
      gravity = 10.0;

   return ua_vector3d(0.0, 0.0, -gravity);
}

/// \todo hurt player when falling from too high
void ua_player_physics_object::hit_floor()
{
   double fallHeight = m_fallHeightStart - m_player.GetHeight();
   fallHeight;
}
