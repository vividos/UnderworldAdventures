//
// Underworld Adventures - an Ultima Underworld hacking project
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
/// \details used collision detection and response code from this paper:
/// http://www.peroxide.dk/papers/collision/collision.pdf
/// previous document:
/// http://www.peroxide.dk/download/tutorials/tut10/pxdtut10.html
//
#include "pch.hpp"
#include "PhysicsModel.hpp"
#include "Plane3d.hpp"

/// trick to get smoother CD
const double c_cdLiftoff = 0.5;

/// minimum distance
const double c_physicsMinDistance = 0.005;


bool CheckPointInTriangle(const Vector3d& point,
   const Vector3d& pa, const Vector3d& pb, const Vector3d& pc);


/// collision data
struct CollisionData
{
   /// default ctor
   CollisionData()
      :foundCollision(false),
      nearestDistance(1e20) // should be "infinite enough"
   {
   }

   /// ellipsoid radius
   Vector3d ellipsoid;

   // information about the move being requested (in eSpace)
   Vector3d velocity;            ///< velocity direction
   Vector3d normalizedVelocity;  ///< normalized velocity
   Vector3d basePoint;           ///< base point

   // hit information
   bool foundCollision;       ///< indicates if collision was found
   double nearestDistance;    ///< dist to nearest triangle
   Vector3d intersectPoint;   ///< intersection point of collision
};


void PhysicsModel::Init(IPhysicsModelCallback* callback)
{
   m_callback = callback;
   m_collisionRecursionDepth = 0;

   // initial params
   SetPhysicsParam(physicsGravity, true);
}

void PhysicsModel::EvaluatePhysics(double elapsedTime)
{
   unsigned int max = m_trackedBodies.size();
   for (unsigned int i = 0; i < max; i++)
   {
      PhysicsBody& body = *m_trackedBodies[i];

      body.SetNewElapsedTime(elapsedTime);
      TrackObject(body);
   }
}

void PhysicsModel::TrackObject(PhysicsBody& body)
{
   // apply direction
   TrackObject(body, body.GetDirection());

   if (GetPhysicsParam(physicsGravity))
   {
      // apply gravity
      TrackObject(body, body.GetGravityForce(), true);
   }

   // apply effect forces, e.g. jump traps, up vector for bouncing balls
   Vector3d effectForce;
   TrackObject(body, effectForce);
}

/// \param body physics body to track
/// \param velocity velocity direction vector of object
/// \param gravityForce indicates if we're processing a gravity force
/// \return true if we collided with triangle
bool PhysicsModel::TrackObject(PhysicsBody& body, Vector3d velocity,
   bool gravityForce)
{
   // setup collision package for this tracking
   CollisionData data;
   data.ellipsoid = body.GetEllipsoid();
   data.foundCollision = false;

   UaAssert(data.ellipsoid.Length() > 1e-6); // ellipsoid must not be of zero size

   Vector3d pos = body.GetPosition();

   // \todo
   extern bool my_movement;
   if (my_movement)
      UaTrace("old pos: %2.3f / %2.3f / %2.3f\n", pos.x, pos.y, pos.z);

   //if (!gravityForce)
   //   pos.z += 0.5;

   // transform to ellipsoid space
   pos /= data.ellipsoid;
   velocity /= data.ellipsoid;

   // call recursive collision response function
   m_collisionRecursionDepth = 0;

   bool collided = CollideWithWorld(data, pos, velocity);

   if (gravityForce && collided)
   {
      body.HitFloor();
      body.ResetGravity();
   }

   // transform position back to normal space and set it
   pos *= data.ellipsoid;

   //if (!gravityForce)
   //   pos.z -= 0.5;

   if (my_movement)
      UaTrace("new pos: %2.3f / %2.3f / %2.3f\n", pos.x, pos.y, pos.z);
   my_movement = false;

   // limit height when falling out of the map
   if (pos.z < 0.0) pos.z = 0.0;

   body.SetPosition(pos);

   return collided;
}

/// \param data collision data package
/// \param pos current position in eSpace
/// \param velocity current direction vector in eSpace
/// \return true when a collision occured
bool PhysicsModel::CollideWithWorld(CollisionData& data,
   Vector3d& pos, const Vector3d& velocity)
{
   double minDistance = c_physicsMinDistance;

   // do we need to worry?
   if (m_collisionRecursionDepth > 5)
      return true;

   // set up collision data we send to the mesh
   data.basePoint = pos;
   data.normalizedVelocity = data.velocity = velocity;
   data.normalizedVelocity.Normalize();
   data.foundCollision = false;

   // check triangle mesh collision (calls the collision routines)
   CheckCollision(data);

   // if no collision we just move along the direction
   if (!data.foundCollision)
   {
      pos += velocity;
      return false; // no collision
   }

   // collision occured

   Vector3d destinationPoint = pos + velocity;
   Vector3d newBasePoint = pos;

   // only update if we are not already very close
   // and if so we only move very close to intersection ...
   // not to the exact spot
   if (data.nearestDistance >= minDistance)
   {
      Vector3d velocity2(velocity);
      velocity2.Normalize();
      velocity2 *= data.nearestDistance - minDistance;

      newBasePoint = data.basePoint + velocity2;

      // adjust polygon intersection point (so sliding
      // plane will be unaffected by the fact that we
      // move slightly less than collision tells us)
      velocity2.Normalize();
      velocity2 *= minDistance;
      data.intersectPoint -= velocity2;
   }

   // determine the sliding plane
   Vector3d slidePlaneNormal = newBasePoint - data.intersectPoint;
   slidePlaneNormal.Normalize();

   Plane3d slidingPlane(data.intersectPoint, slidePlaneNormal);

   // calculate new destination point
   Vector3d newDestinationPoint =
      -slidingPlane.SignedDistanceTo(destinationPoint) * slidePlaneNormal + destinationPoint;

   // generate the slide vector, which will become our new velocity vector for
   // the next iteration
   Vector3d newVelocity = newDestinationPoint - data.intersectPoint;

   // recurse

   // don't recurse if the new direction is very small
   if (newVelocity.Length() < minDistance)
   {
      pos = newBasePoint;
      return true;
   }

   // call the function with the new position and velocity
   m_collisionRecursionDepth++;

   CollideWithWorld(data, newBasePoint, newVelocity);
   pos = newBasePoint;

   m_collisionRecursionDepth--;

   // recursion call might have not collided with something, but we surely had
   return true;
}

void PhysicsModel::CheckCollision(CollisionData& data)
{
   unsigned int xpos = static_cast<unsigned int>(data.basePoint.x * data.ellipsoid.x);
   unsigned int ypos = static_cast<unsigned int>(data.basePoint.y * data.ellipsoid.y);

   // retrieve all tile triangles to check
   std::vector<Triangle3dTextured> allTriangles;

   if (m_callback != NULL)
      m_callback->GetSurroundingTriangles(xpos, ypos, allTriangles);

   // check all triangles
   unsigned int max = allTriangles.size();
   for (unsigned int index = 0; index < max; index++)
   {
      Triangle3dTextured& tri = allTriangles[index];

      // get triangle points and convert to ellipsoid space
      tri.m_vertices[0].pos /= data.ellipsoid;
      tri.m_vertices[1].pos /= data.ellipsoid;
      tri.m_vertices[2].pos /= data.ellipsoid;

      CheckTriangle(data,
         tri.m_vertices[0].pos,
         tri.m_vertices[1].pos,
         tri.m_vertices[2].pos);
   }
}

/// Checks triangle for collision; triangle vertices must already be in
/// ellipsoid space.
void PhysicsModel::CheckTriangle(CollisionData& data,
   const Vector3d& p1, const Vector3d& p2, const Vector3d& p3)
{
   // construct plane containing this triangle
   Plane3d trianglePlane(p1, p2, p3);

   // is triangle front-facing to the direction vector?
   // we only check front-facing triangles
   if (!trianglePlane.IsFrontFacingTo(data.normalizedVelocity))
      return;

   // triangle is front-facing

   // get interval of plane intersection
   double t0, t1;
   bool isEmbedded = false;

   // calculate signed distance from sphere
   // position to triangle plane
   double distanceToTriangle = trianglePlane.SignedDistanceTo(data.basePoint);

   // cache this as we're going to use it a few times below
   double normalDotVelocity = trianglePlane.normal.Dot(data.velocity);

   // if sphere is travelling parallel to the plane
   if (normalDotVelocity == 0.0)
   {
      if (fabs(distanceToTriangle) >= 1.0)
      {
         // sphere is not embedded in plane; no collision possible
         return;
      }
      else
      {
         // sphere is embedded in plane
         // it intersects in the whole range [0..1]
         isEmbedded = true;
         t0 = 0.0;
         t1 = 1.0;
      }
   }
   else
   {
      // N dot D is not 0; calculate intersection interval
      t0 = (-1.0 - distanceToTriangle) / normalDotVelocity;
      t1 = (1.0 - distanceToTriangle) / normalDotVelocity;

      // swap so t0 < t1
      if (t0 > t1)
         std::swap(t0, t1);

      // check that at least one result is within range
      if (t0 > 1.0 || t1 < 0.0)
      {
         // both t values are outside values [0,1]
         // no collision possible
         return;
      }

      // clamp to [0,1]
      if (t0 < 0.0) t0 = 0.0;
      if (t1 < 0.0) t1 = 0.0;
      if (t0 > 1.0) t0 = 1.0;
      if (t1 > 1.0) t1 = 1.0;
   }

   // OK, at this point we have two time values t0 and t1
   // between which the swept sphere intersects with the
   // triangle plane. If any collision is to occur it must
   // happen within this interval.
   Vector3d collisionPoint;
   bool found = false;
   double t = 1.0;

   // First we check for the easy case - collision inside
   // the triangle. If this happens it must be at time t0
   // as this is when the sphere rests on the front side
   // of the triangle plane. Note, this can only happen if
   // the sphere is not embedded in the triangle plane.
   if (!isEmbedded)
   {
      Vector3d planeIntersectionPoint;
      planeIntersectionPoint = t0 * data.velocity + data.basePoint - trianglePlane.normal;

      if (CheckPointInTriangle(planeIntersectionPoint, p1, p2, p3))
      {
         found = true;
         t = t0;
         collisionPoint = planeIntersectionPoint;
      }
   }

   // If we haven't found a collision already we'll have to
   // sweep sphere against points and edges of the triangle.
   // Note: A collision inside the triangle (the check above)
   // will always happen before a vertex or edge collision!
   // This is why we can skip the swept test if the above
   // gives a collision!

   if (!found)
   {
      // some commonly used terms
      Vector3d velocity = data.velocity;
      Vector3d base = data.basePoint;

      double velocityLengthSquared = velocity.Length();
      velocityLengthSquared *= velocityLengthSquared;
      double a, b, c;
      double new_t;

      // For each vertex or edge a quadratic equation have to
      // be solved. We parameterize this equation as
      // a*t^2 + b*t + c = 0 and below we calculate the
      // parameters a,b and c for each test.

      // check against points
      a = velocityLengthSquared;

      // P1
      b = 2.0 * (velocity.Dot(base - p1));
      c = (p1 - base).Length(); c *= c;
      c -= 1.0;
      if (GetLowestRoot(a, b, c, t, new_t))
      {
         t = new_t;
         found = true;
         collisionPoint = p1;
      }

      // P2
      b = 2.0 * (velocity.Dot(base - p2));
      c = (p2 - base).Length(); c *= c;
      c -= 1.0;
      if (GetLowestRoot(a, b, c, t, new_t))
      {
         t = new_t;
         found = true;
         collisionPoint = p2;
      }

      // P3
      b = 2.0 * (velocity.Dot(base - p3));
      c = (p3 - base).Length(); c *= c;
      c -= 1.0;
      if (GetLowestRoot(a, b, c, t, new_t))
      {
         t = new_t;
         found = true;
         collisionPoint = p3;
      }

      // check against edges

      // p1 -> p2
      {
         Vector3d edge = p2 - p1;
         Vector3d baseToVertex = p1 - base;
         double edgeLengthSquared = edge.Length();
         edgeLengthSquared *= edgeLengthSquared;
         double edgeDotVelocity = edge.Dot(velocity);
         double edgeDotBaseToVertex = edge.Dot(baseToVertex);
         double baseToVertexLengthSquared = baseToVertex.Length();
         baseToVertexLengthSquared *= baseToVertexLengthSquared;

         // calculate parameters for equation
         a = edgeLengthSquared * -velocityLengthSquared + edgeDotVelocity * edgeDotVelocity;
         b = edgeLengthSquared * (2.0 * velocity.Dot(baseToVertex)) -
            2.0*edgeDotVelocity * edgeDotBaseToVertex;
         c = edgeLengthSquared * (1 - baseToVertexLengthSquared) +
            edgeDotBaseToVertex * edgeDotBaseToVertex;

         // does this swept sphere collide against infinite edge?
         if (GetLowestRoot(a, b, c, t, new_t))
         {
            // check if intersection is within line segment:
            double f = (edgeDotVelocity * new_t - edgeDotBaseToVertex) /
               edgeLengthSquared;

            if (f >= 0.0 && f < 1.0)
            {
               // intersection took place within segment
               t = new_t;
               found = true;
               collisionPoint = edge;
               collisionPoint *= f;
               collisionPoint += p1;
            }
         }
      }

      // p2 -> p3
      {
         Vector3d edge = p3 - p2;
         Vector3d baseToVertex = p2 - base;
         double edgeLengthSquared = edge.Length();
         edgeLengthSquared *= edgeLengthSquared;
         double edgeDotVelocity = edge.Dot(velocity);
         double edgeDotBaseToVertex = edge.Dot(baseToVertex);
         double baseToVertexLengthSquared = baseToVertex.Length();
         baseToVertexLengthSquared *= baseToVertexLengthSquared;

         // calculate parameters for equation
         a = edgeLengthSquared * -velocityLengthSquared + edgeDotVelocity * edgeDotVelocity;
         b = edgeLengthSquared * (2.0 * velocity.Dot(baseToVertex)) -
            2.0*edgeDotVelocity * edgeDotBaseToVertex;
         c = edgeLengthSquared * (1 - baseToVertexLengthSquared) +
            edgeDotBaseToVertex * edgeDotBaseToVertex;

         // does this swept sphere collide against infinite edge?
         if (GetLowestRoot(a, b, c, t, new_t))
         {
            // check if intersection is within line segment:
            double f = (edgeDotVelocity * new_t - edgeDotBaseToVertex) /
               edgeLengthSquared;

            if (f >= 0.0 && f < 1.0)
            {
               // intersection took place within segment
               t = new_t;
               found = true;
               collisionPoint = edge;
               collisionPoint *= f;
               collisionPoint += p2;
            }
         }
      }

      // p3 -> p1
      {
         Vector3d edge = p1 - p3;
         Vector3d baseToVertex = p3 - base;
         double edgeLengthSquared = edge.Length();
         edgeLengthSquared *= edgeLengthSquared;
         double edgeDotVelocity = edge.Dot(velocity);
         double edgeDotBaseToVertex = edge.Dot(baseToVertex);
         double baseToVertexLengthSquared = baseToVertex.Length();
         baseToVertexLengthSquared *= baseToVertexLengthSquared;

         // calculate parameters for equation
         a = edgeLengthSquared * -velocityLengthSquared + edgeDotVelocity * edgeDotVelocity;
         b = edgeLengthSquared * (2.0 * velocity.Dot(baseToVertex)) -
            2.0 * edgeDotVelocity * edgeDotBaseToVertex;
         c = edgeLengthSquared * (1 - baseToVertexLengthSquared) +
            edgeDotBaseToVertex * edgeDotBaseToVertex;

         // does this swept sphere collide against infinite edge?
         if (GetLowestRoot(a, b, c, t, new_t))
         {
            // check if intersection is within line segment:
            double f = (edgeDotVelocity * new_t - edgeDotBaseToVertex) /
               edgeLengthSquared;

            if (f >= 0.0 && f < 1.0)
            {
               // intersection took place within segment
               t = new_t;
               found = true;
               collisionPoint = edge;
               collisionPoint *= f;
               collisionPoint += p3;
            }
         }
      }

   } // !found

   // set result
   if (found)
   {
      // distance to collision: 't' is time of collision
      double distanceToCollision = t * data.velocity.Length();

      // does this triangle qualify for the closest hit?
      // it does if it's the first hit or the closest
      if (!data.foundCollision ||
         distanceToCollision < data.nearestDistance)
      {
         // collision information necessary for sliding
         data.nearestDistance = distanceToCollision;
         data.intersectPoint = collisionPoint;
         data.foundCollision = true;
      }
   }
}

/// Calculates lowest root of quadratic equation of the form
/// ax^2 + bx + c = 0; the solution that is returned is positive and lower
/// than maxR. Other solutions are disregarded.
bool PhysicsModel::GetLowestRoot(double a, double b, double c,
   double maxR, double& root)
{
   // check if a solution exists
   double determinant = b * b - 4.0 * a * c;

   // if determinant is negative it means no solutions
   if (determinant < 0.0)
      return false;

   // calculate the two roots: (if determinant == 0 then
   // x1==x2 but let’s disregard that slight optimization)
   double sqrtD = sqrt(determinant);
   double r1 = (-b - sqrtD) / (2 * a);
   double r2 = (-b + sqrtD) / (2 * a);

   // sort so x1 <= x2
   if (r1 > r2)
      std::swap(r1, r2);

   // get lowest root
   if (r1 > 0 && r1 < maxR)
   {
      root = r1;
      return true;
   }

   // it is possible that we want x2 - this can happen
   // if x1 < 0
   if (r2 > 0 && r2 < maxR)
   {
      root = r2;
      return true;
   }

   // no (valid) solutions
   return false;
}

bool IsSameSide(const Vector3d& p1,
   const Vector3d& p2, const Vector3d& a, const Vector3d& b)
{
   Vector3d cp1; cp1.Cross(b - a, p1 - a);
   Vector3d cp2; cp2.Cross(b - a, p2 - a);

   return cp1.Dot(cp2) >= 0;
}

bool CheckPointInTriangle(const Vector3d& point,
   const Vector3d& pa, const Vector3d& pb, const Vector3d& pc)
{
   return IsSameSide(point, pa, pb, pc) &&
      IsSameSide(point, pb, pa, pc) &&
      IsSameSide(point, pc, pa, pb);
}

/// \todo replace in() macro test with somewhat more portable
bool CheckPointInTriangleFast(const Vector3d& point,
   const Vector3d& pa, const Vector3d& pb, const Vector3d& pc)
{
   Vector3d e10 = pb - pa;
   Vector3d e20 = pc - pa;

   float a, b, c, ac_bb;

   a = e10.Dot(e10);
   b = e10.Dot(e20);
   c = e20.Dot(e20);
   ac_bb = (a*c) - (b*b);

   Vector3d vp = Vector3d(point.x - pa.x, point.y - pa.y, point.z - pa.z);

   float d = vp.Dot(e10);
   float e = vp.Dot(e20);

   float x = (d*c) - (e*b);
   float y = (e*a) - (d*b);
   float z = x + y - ac_bb;

#define in(a) ((Uint32&) a)

   return ((in(z)& ~(in(x) | in(y))) & 0x80000000) != 0;
}
