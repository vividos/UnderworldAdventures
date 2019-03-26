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
/// \file PhysicsModel.hpp
/// \brief game physics model
//
#pragma once

#include "Math.hpp"
#include "Triangle3d.hpp"
#include "PhysicsBody.hpp"
#include <vector>

struct CollisionData;

/// physics parameter
enum PhysicsParam
{
   /// controls if gravity is active in the physics model
   physicsGravity = 0,

   /// last param; not used
   physicsParamMax
};

/// physics model callback interface
class IPhysicsModelCallback
{
public:
   /// returns surrounding triangles on given position
   virtual void GetSurroundingTriangles(unsigned int xpos,
      unsigned int ypos, std::vector<Triangle3dTextured>& allTriangles) = 0;
};

/// physics model class
class PhysicsModel
{
public:
   /// ctor
   PhysicsModel()
      :m_callback(NULL)
   {
   }

   /// inits the physics model
   void Init(IPhysicsModelCallback* callback);

   bool GetPhysicsParam(PhysicsParam param) const
   {
      return m_params[param];
   }

   void SetPhysicsParam(PhysicsParam param, bool value)
   {
      m_params[param] = value;
   }

   /// evaluate physics on objects
   void EvaluatePhysics(double elapsedTime);

   /// tracks object movement using current parameters
   void TrackObject(PhysicsBody& body);

   /// add physics body to track to model
   void AddTrackBody(PhysicsBody* body)
   {
      m_trackedBodies.push_back(body);
   }

   /// removes physics body
   void RemoveTrackBody(PhysicsBody* body)
   {
      auto iter = std::find(m_trackedBodies.begin(), m_trackedBodies.end(), body);
      UaAssert(iter != m_trackedBodies.end());

      if (iter != m_trackedBodies.end())
         m_trackedBodies.erase(iter);
   }

private:
   /// tracks object movement for given direction vector
   bool TrackObject(PhysicsBody& body, Vector3d velocity,
      bool gravityForce = false);

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

protected:
   /// model parameters
   bool m_params[physicsParamMax];

   /// recursion depth for CollideWithWorld()
   int m_collisionRecursionDepth;

   /// callback interface pointer
   IPhysicsModelCallback* m_callback;

   /// list of pointer to bodies tracked by physics model
   std::vector<PhysicsBody*> m_trackedBodies;
};
