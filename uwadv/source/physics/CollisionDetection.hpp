//
// Underworld Adventures - an Ultima Underworld remake project
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
/// \file CollisionDetection.hpp
/// \brief collision detection for physics bodies
//
#pragma once

#include "Math.hpp"
#include "Triangle3d.hpp"
#include <vector>

struct CollisionData;
class PhysicsBody;

/// \brief Collision detection algorithm that implements "collide and slide" along triangles of
/// the level geometry.
class CollisionDetection
{
public:
   /// ctor
   CollisionDetection(const std::vector<Triangle3dTextured>& allTriangles,
      const PhysicsBody& body);

   /// tracks object movement of given physics body
   void TrackObject(PhysicsBody& body);

private:
   /// collides physics body with world and slides it along sliding plane
   bool CollideAndSlide(PhysicsBody& body, Vector3d& pos, Vector3d velocity);

   /// recursive collision response calculation
   bool CollideWithWorld(CollisionData& data, Vector3d& pos,
      const Vector3d& velocity);

   /// checks mesh for collision
   void CheckCollision(CollisionData& data);

   /// checks single triangle for collision
   void CheckTriangle(CollisionData& data, const Vector3d& p1,
      const Vector3d& p2, const Vector3d& p3);

   /// checks collision with single triangle point
   static bool CheckCollisionWithPoint(CollisionData& data,
      const Vector3d& point, double& t, Vector3d& collisionPoint);

   /// checks collision with triangle edge
   static bool CheckCollisionWithEdge(CollisionData& data,
      const Vector3d& p1, const Vector3d& p2, double& t, Vector3d& collisionPoint);

   /// solves quadratic equation and returns solution < t
   static bool GetLowestRoot(double a, double b, double c, double t,
      double& newT);

   /// checks if a given point is inside of a triangle given by 3 points
   static bool CheckPointInTriangle(const Vector3d& point,
      const Vector3d& pa, const Vector3d& pb, const Vector3d& pc);

private:
   /// all triangles, in ellipsoid space
   std::vector<Triangle3dTextured> m_allTriangles;

   /// recursion depth for CollideWithWorld()
   int m_collisionRecursionDepth;
};
