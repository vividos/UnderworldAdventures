//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2022 Underworld Adventures Team
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
/// \file PhysicsBody.hpp
/// \brief an object that can be applied to collision/physics
//
#pragma once

#include "Math.hpp"

namespace Physics
{
   /// \brief a body in the world that can be applied to physics
   class PhysicsBody
   {
   public:
      /// dtor
      virtual ~PhysicsBody() {}

      /// sets new elapsed time; used at the begin of tracking this object
      virtual void SetNewElapsedTime(double elapsedTime) { UNUSED(elapsedTime); }

      /// returns body position of the center
      virtual Vector3d GetPosition() const = 0;

      /// sets body position
      virtual void SetPosition(const Vector3d& pos) = 0;

      /// returns body direction
      virtual Vector3d GetDirection() const { return Vector3d(0, 0, 0); }

      /// returns ellipsoid of body
      const Vector3d& GetEllipsoid() const { return m_ellipsoid; }

      // gravity related

      /// returns if gravity should be checked for this body
      virtual bool IsGravityActive() const { return true; }

      /// resets gravity when hitting floor
      virtual void ResetGravity() {}

      /// returns gravity vector for object
      virtual Vector3d GetGravityForce() const { return Vector3d(0, 0, 0); }

      /// item hits the floor
      virtual void HitFloor() {}

   protected:
      /// body ellipsoid
      Vector3d m_ellipsoid;

      /// indicates if body can move by itself
      bool m_canMove;
   };

} // namespace Physics
