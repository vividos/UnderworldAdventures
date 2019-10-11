//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2019 Underworld Adventures Team
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
/// \file PhysicsModel.cpp
/// \brief game physics implementation
//
#include "pch.hpp"
#include "PhysicsModel.hpp"
#include "PhysicsBody.hpp"
#include "CollisionDetection.hpp"

void PhysicsModel::Init(IPhysicsModelCallback* callback)
{
   m_callback = callback;

   // initial params
   SetPhysicsParam(physicsGravity, true);
}

void PhysicsModel::EvaluatePhysics(double elapsedTime)
{
   size_t max = m_trackedBodies.size();
   for (size_t index = 0; index < max; index++)
   {
      PhysicsBody& body = *m_trackedBodies[index];

      body.SetNewElapsedTime(elapsedTime);
      TrackObject(body);
   }
}

/// \param body physics body to track
void PhysicsModel::TrackObject(PhysicsBody& body)
{
   Vector3d pos = body.GetPosition();

   unsigned int xpos = static_cast<unsigned int>(pos.x);
   unsigned int ypos = static_cast<unsigned int>(pos.y);

   // retrieve all tile triangles to check
   std::vector<Triangle3dTextured> allTriangles;

   if (m_callback != NULL)
      m_callback->GetSurroundingTriangles(xpos, ypos, allTriangles);

   CollisionDetection detection{ allTriangles, body };
   detection.TrackObject(body);
}
