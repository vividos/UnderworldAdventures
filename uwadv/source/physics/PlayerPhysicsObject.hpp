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
/// \file PlayerPhysicsObject.hpp
/// \brief player state for physics simulation
//
#pragma once

#include "PhysicsBody.hpp"
#include <map>

namespace Underworld
{
   class Player;
}

/// player movement enum
enum PlayerMovementMode
{
   moveWalk = 1,     ///< walks forward (or backwards, when factor is negative)
   moveRotate = 2,   ///< rotates player left (or right)
   moveLookUpDown = 4, ///< moves player look angle up (or down)
   moveJump = 8,     ///< jumps forward (or factor 0.0 for standing jump)
   moveSlide = 16,   ///< slides right (or left)
   moveFloat = 32,   ///< floats player up (or down)
};

class PlayerPhysicsObject : public PhysicsBody
{
public:
   /// ctor
   PlayerPhysicsObject(Underworld::Player& player, bool enhancedFeatures);

   /// sets and delete movement mode values
   void SetMovementMode(unsigned int set, unsigned int del = 0);

   /// sets movement factor for a given movement type; range is [-1.0; 1.0]
   void SetMovementFactor(PlayerMovementMode mode, double factor);

   /// returns movement mode
   unsigned int GetMovementMode() const
   {
      return m_movementMode;
   }

   /// returns movement factor for given movement mode
   double GetMovementFactor(PlayerMovementMode mode);

   /// does rotation moves
   void RotateMove(double time_elapsed);

   // virtual methods from PhysicsBody
   virtual void SetNewElapsedTime(double timeElapsed) override;
   virtual Vector3d GetPosition() override;
   virtual void SetPosition(Vector3d& pos) override;
   virtual Vector3d GetDirection() override;

   virtual void ResetGravity() override;
   virtual Vector3d GetGravityForce() override;
   virtual void HitFloor() override;

private:
   /// ref to player
   Underworld::Player& m_player;

   /// player ellipsoid
   Vector3d m_ellipsoid;

   /// current movement mode
   unsigned int m_movementMode;

   /// movement factors map
   std::map<PlayerMovementMode, double> m_moveFactors;

   /// current fall time
   double m_fallTime;

   /// player height at fall start
   double m_fallHeightStart;

   /// maximum pan angle
   double m_maxPanAngle;
};
