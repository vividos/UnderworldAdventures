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
/// \file PlayerPhysicsObject.cpp
/// \brief player state for physics simulation
//
#include "pch.hpp"
#include "PlayerPhysicsObject.hpp"
#include "Player.hpp"

/// max speed a player can walk, in tiles / second
const double c_playerMaxWalkSpeed = 2.4;

/// max speed a player can slide left or right
const double c_playerMaxSlideSpeed = 1.0;

/// max speed a player can rotate, in degrees / second
const double c_playerMaxRotateSpeed = 90;

/// max speed a player can rotate the view, in degrees / second
const double c_playerMaxViewRotateSpeed = 60;

/// x size of the player m_ellipsoid
const double c_playerEllipsoidX = 0.2;
/// y size of the player m_ellipsoid
const double c_playerEllipsoidY = 0.2;
/// z size of the player m_ellipsoid
const double c_playerEllipsoidZ = 11.9;

PlayerPhysicsObject::PlayerPhysicsObject(Underworld::Player& player, bool enhancedFeatures)
   :m_player(player),
   m_movementMode(0),
   m_fallTime(0.0),
   m_fallHeightStart(0.0),
   m_maxPanAngle(enhancedFeatures ? 45.0 : 75.0)
{
   m_ellipsoid = Vector3d(c_playerEllipsoidX, c_playerEllipsoidY, c_playerEllipsoidZ);

   m_moveFactors[moveWalk] = 0.0;
   m_moveFactors[moveRotate] = 0.0;
   m_moveFactors[moveLookUpDown] = 0.0;
   m_moveFactors[moveJump] = 0.0;
   m_moveFactors[moveSlide] = 0.0;
   m_moveFactors[moveFloat] = 0.0;
}

void PlayerPhysicsObject::SetMovementMode(unsigned int set, unsigned int del)
{
   m_movementMode = (m_movementMode | set) & (~del);
}

void PlayerPhysicsObject::SetMovementFactor(PlayerMovementMode mode, double factor)
{
   m_moveFactors[mode] = factor;
}

double PlayerPhysicsObject::GetMovementFactor(PlayerMovementMode mode)
{
   return m_moveFactors[mode];
}

void PlayerPhysicsObject::RotateMove(double elapsedTime)
{
   unsigned int mode = GetMovementMode();

   // player rotation
   if (mode & moveRotate)
   {
      double angle = c_playerMaxRotateSpeed * elapsedTime *
         GetMovementFactor(moveRotate);
      angle += m_player.GetRotateAngle();

      // keep angle in range [0; 360]
      while (angle > 360.0) angle -= 360.0;
      while (angle < 0.0) angle += 360.0;

      m_player.SetRotateAngle(angle);
   }

   // view up/down
   if (mode & moveLookUpDown)
   {
      double viewangle = m_player.GetPanAngle();
      viewangle -= c_playerMaxViewRotateSpeed * elapsedTime *
         GetMovementFactor(moveLookUpDown);

      // restrict up-down view angle
      if (viewangle < -m_maxPanAngle) viewangle = -m_maxPanAngle;
      if (viewangle > m_maxPanAngle) viewangle = m_maxPanAngle;

      m_player.SetPanAngle(viewangle);
   }
}

void PlayerPhysicsObject::SetNewElapsedTime(double elapsedTime)
{
   m_fallTime += elapsedTime;
}

Vector3d PlayerPhysicsObject::GetPosition()
{
   return Vector3d(m_player.GetXPos(), m_player.GetYPos(), m_player.GetHeight() + c_playerEllipsoidZ / 2.0);
}

void PlayerPhysicsObject::SetPosition(Vector3d& pos)
{
   m_player.SetPos(pos.x, pos.y);
   m_player.SetHeight(pos.z - c_playerEllipsoidZ / 2.0);
}

bool my_movement;

Vector3d PlayerPhysicsObject::GetDirection()
{
   double elapsedTime = 0.05;

   Vector3d dir;

   unsigned int mode = GetMovementMode();

   if ((mode & moveWalk) || (mode & moveSlide))
   {
      double speed = c_playerMaxWalkSpeed * elapsedTime;
      double angle = m_player.GetRotateAngle();

      my_movement = true;

      // adjust angle for sliding
      if (mode & moveSlide)
      {
         angle -= 90.0;
         speed *= GetMovementFactor(moveSlide);
         speed *= c_playerMaxSlideSpeed / c_playerMaxWalkSpeed;
      }
      else
         speed *= GetMovementFactor(moveWalk);

      // construct direction vector
      dir.x = 1.0;
      dir.RotateZ(angle);
      dir *= speed;
   }

   return dir;
}

void PlayerPhysicsObject::ResetGravity()
{
   m_fallTime = 0.0;
   m_fallHeightStart = m_player.GetHeight();
}

Vector3d PlayerPhysicsObject::GetGravityForce()
{
   double gravity = 15.0 * m_fallTime * m_fallTime;

   // limit gravity for falling
   if (gravity > 10.0)
      gravity = 10.0;

   return Vector3d(0.0, 0.0, -gravity);
}

/// \todo hurt player when falling from too high
void PlayerPhysicsObject::HitFloor()
{
   double fallHeight = m_fallHeightStart - m_player.GetHeight();
   UNUSED(fallHeight);
}
