//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2019,2022 Underworld Adventures Team
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
/// \file CollisionDetection.cpp
/// \brief collision detection for physics bodies
/// \details used collision detection and response code from this paper:
/// http://www.peroxide.dk/papers/collision/collision.pdf
/// and fixes to the algorithm from this paper:
/// https://arxiv.org/ftp/arxiv/papers/1211/1211.0059.pdf
//
#include "pch.hpp"
#include "CollisionDetection.hpp"
#include "PhysicsBody.hpp"
#include "Plane3d.hpp"
#include <algorithm>

using Physics::CollisionDetection;
using Physics::CollisionData;

// define this to get verbose debug output for collision detection
#undef DEBUG_OUTPUT

/// minimum distance
const double c_physicsMinDistance = 0.005;

/// collision data
struct CollisionData
{
   /// default ctor
   CollisionData()
      :foundCollision(false),
      nearestDistance(1e20), // should be "infinite enough"
      debugOutput(false)
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

   bool debugOutput; ///< indicates if debug output should be made
};

CollisionDetection::CollisionDetection(const std::vector<Triangle3dTextured>& allTriangles,
   const PhysicsBody& body)
   :m_allTriangles(allTriangles.begin(), allTriangles.end()),
   m_collisionRecursionDepth(0)
{
   const Vector3d& ellipsoid = body.GetEllipsoid();
   for (auto& triangle : m_allTriangles)
   {
      triangle.m_vertices[0].pos /= ellipsoid;
      triangle.m_vertices[1].pos /= ellipsoid;
      triangle.m_vertices[2].pos /= ellipsoid;
   }
}

/// tracks object movement using current parameters
void CollisionDetection::TrackObject(PhysicsBody& body)
{
   Vector3d pos = body.GetPosition();
   Vector3d dir = body.GetDirection();

#if defined(HAVE_DEBUG) && defined(DEBUG_OUTPUT)
   bool debugOutput = dir.Length() > 1e-6 && dir.z >= 0;
#else
   bool debugOutput = false;
#endif

   if (debugOutput)
      UaTrace("tracking physics body at pos (%2.3f / %2.3f / %2.3f), velocity (%2.3f / %2.3f / %2.3f), length=%1.3f\n",
         pos.x, pos.y, pos.z,
         dir.x, dir.y, dir.z, dir.Length());

   // apply direction
   CollideAndSlide(body, pos, dir);

   if (body.IsGravityActive())
   {
      // apply gravity
      bool hitFloor = CollideAndSlide(body, pos, body.GetGravityForce());
      if (hitFloor)
      {
         body.HitFloor();
         body.ResetGravity();
      }
   }

   // apply effect forces, e.g. jump traps, up vector for bouncing balls
   Vector3d effectForce;
   CollideAndSlide(body, pos, effectForce);

   // limit height when falling out of the map
   if (pos.z < 0.0)
      pos.z = 0.0;

   body.SetPosition(pos);

   if (debugOutput)
      UaTrace("finished tracking physics body, new pos (%2.3f / %2.3f / %2.3f)\n\n", pos.x, pos.y, pos.z);
}

bool CollisionDetection::CollideAndSlide(PhysicsBody& body, Vector3d& pos, Vector3d velocity)
{
   // setup collision package for this tracking
   CollisionData data;
   data.ellipsoid = body.GetEllipsoid();
   data.foundCollision = false;
#if defined(HAVE_DEBUG) && defined(DEBUG_OUTPUT)
   data.debugOutput = velocity.Length() > 1e-6 && velocity.z >= 0;
#endif

   UaAssert(data.ellipsoid.Length() > 1e-6); // ellipsoid must not be of zero size

   // transform to ellipsoid space
   pos /= data.ellipsoid;
   velocity /= data.ellipsoid;

   // call recursive collision response function
   m_collisionRecursionDepth = 0;

   bool collided = CollideWithWorld(data, pos, velocity);

   // transform position back to normal space
   pos *= data.ellipsoid;

   return collided;
}

/// \param data collision data package
/// \param pos current position in eSpace
/// \param velocity current direction vector in eSpace
/// \return true when a collision occured
bool CollisionDetection::CollideWithWorld(CollisionData& data,
   Vector3d& pos, const Vector3d& velocity)
{
   if (data.debugOutput)
      UaTrace("  iteration %i\n", m_collisionRecursionDepth);

   double minDistance = c_physicsMinDistance;

   // do we need to worry?
   if (m_collisionRecursionDepth > 5)
      return true;

   // set up collision data we send to the mesh
   data.basePoint = pos;
   data.normalizedVelocity = data.velocity = velocity;
   data.normalizedVelocity.Normalize();
   data.foundCollision = false;

   if (data.debugOutput)
      UaTrace("velocity in espace (%2.3f / %2.3f / %2.3f)\n\n", velocity.x, velocity.y, velocity.z);

   // check triangle mesh collision (calls the collision routines)
   CheckCollision(data);

   // if no collision we just move along the direction
   if (!data.foundCollision)
   {
      pos += velocity;

      if (data.debugOutput)
         UaTrace("  no collision; exiting\n");
      return false; // no collision
   }

   // collision occured

   Vector3d destinationPoint = pos + velocity;
   Vector3d newBasePoint = pos;

   // calculate touch point and new base point (code from arxiv paper, chapter III.A.)
   Vector3d touchPoint;
   {
      touchPoint = data.basePoint + data.nearestDistance * data.normalizedVelocity;

      double distance = data.normalizedVelocity.Length() * data.nearestDistance;

      double shortDistance = std::max(distance - c_physicsMinDistance, 0.0);

      Vector3d nearPoint = data.basePoint + shortDistance * data.normalizedVelocity;

      newBasePoint = nearPoint;
   }

   // determine the sliding plane
   Vector3d slidePlaneNormal = touchPoint - data.intersectPoint;
   slidePlaneNormal.Normalize();

   Plane3d slidingPlane(data.intersectPoint, slidePlaneNormal);

   // calculate new destination point
   Vector3d newDestinationPoint =
      -slidingPlane.SignedDistanceTo(destinationPoint) * slidePlaneNormal + destinationPoint;

   // generate the slide vector, which will become our new velocity vector for
   // the next iteration
   Vector3d newVelocity = newDestinationPoint - data.intersectPoint;

   if (data.debugOutput)
      UaTrace("  new base point (%2.3f / %2.3f / %2.3f), new velocity (%2.3f / %2.3f / %2.3f)\n",
         newBasePoint.x, newBasePoint.y, newBasePoint.z,
         newVelocity.x, newVelocity.y, newVelocity.z);

   // recurse

   // don't recurse if the new direction is very small
   if (newVelocity.Length() < minDistance)
   {
      pos = newBasePoint;
      if (data.debugOutput)
         UaTrace("  stop recursing, velocity length below min. distance\n");

      return true;
   }

   m_collisionRecursionDepth++;

   // call the function with the new position and velocity
   pos = newBasePoint;
   return CollideWithWorld(data, pos, newVelocity);
}

void CollisionDetection::CheckCollision(CollisionData& data)
{
   // check all triangles; already in ellipsoid space
   size_t max = m_allTriangles.size();
   for (size_t index = 0; index < max; index++)
   {
      if (data.debugOutput)
         UaTrace("    checking triangle %i...", index);

      const Triangle3dTextured& tri = m_allTriangles[index];

      CheckTriangle(data,
         tri.m_vertices[0].pos,
         tri.m_vertices[1].pos,
         tri.m_vertices[2].pos);
   }
}

/// Checks triangle for collision; triangle vertices must already be in
/// ellipsoid space.
void CollisionDetection::CheckTriangle(CollisionData& data,
   const Vector3d& p1, const Vector3d& p2, const Vector3d& p3)
{
   // construct plane containing this triangle
   Plane3d trianglePlane(p1, p2, p3);

   // is triangle front-facing to the direction vector?
   // we only check front-facing triangles
   if (!trianglePlane.IsFrontFacingTo(data.normalizedVelocity))
   {
      if (data.debugOutput)
         UaTrace("not front-facing\n");
      return;
   }

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
         if (data.debugOutput)
            UaTrace("velocity parallel to triangle, but not embedded\n");
         return;
      }
      else
      {
         // sphere is embedded in plane
         // it intersects in the whole range [0..1]
         isEmbedded = true;
         t0 = 0.0;
         t1 = 1.0;

         if (data.debugOutput)
            UaTrace("sphere embedded in triangle plane\n");
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
         if (data.debugOutput)
            UaTrace("no collision possible intersection, at t0=%1.3f, t1=%1.3f\n", t0, t1);
         return;
      }

      // clamp to [0,1]
      if (t0 < 0.0) t0 = 0.0;
      if (t1 < 0.0) t1 = 0.0;
      if (t0 > 1.0) t0 = 1.0;
      if (t1 > 1.0) t1 = 1.0;

      if (data.debugOutput)
         UaTrace("sphere colliding with triangle plane at %1.3f and %1.3f\n", t0, t1);
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

         if (data.debugOutput)
            UaTrace("      sphere collides with triangle inside, at (%2.3f / %2.3f / %2.3f), t=%1.3f\n",
               collisionPoint.x, collisionPoint.y, collisionPoint.z, t);
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
      // For each vertex or edge a quadratic equation have to
      // be solved. We parameterize this equation as
      // a*t^2 + b*t + c = 0 and below we calculate the
      // parameters a, b and c for each test.

      // check against points
      found |= CheckCollisionWithPoint(data, p1, t, collisionPoint);
      found |= CheckCollisionWithPoint(data, p2, t, collisionPoint);
      found |= CheckCollisionWithPoint(data, p3, t, collisionPoint);

      // check against edges
      found |= CheckCollisionWithEdge(data, p1, p2, t, collisionPoint);
      found |= CheckCollisionWithEdge(data, p2, p3, t, collisionPoint);
      found |= CheckCollisionWithEdge(data, p3, p1, t, collisionPoint);

      if (found && data.debugOutput)
         UaTrace("      sphere collides with triangle edge or point, at (%2.3f / %2.3f / %2.3f), t=%1.3f\n",
            collisionPoint.x, collisionPoint.y, collisionPoint.z, t);

   }

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

bool CollisionDetection::CheckCollisionWithPoint(CollisionData& data,
   const Vector3d& point, double& t, Vector3d& collisionPoint)
{
   double velocityLengthSquared = data.velocity.Length();
   velocityLengthSquared *= velocityLengthSquared;

   double a = velocityLengthSquared;
   double b = 2.0 * data.velocity.Dot(data.basePoint - point);
   double c = (point - data.basePoint).Length();
   c *= c;
   c -= 1.0;

   double newT;
   if (GetLowestRoot(a, b, c, t, newT))
   {
      t = newT;
      collisionPoint = point;
      return true;
   }

   return false;
}

bool CollisionDetection::CheckCollisionWithEdge(CollisionData& data,
   const Vector3d& p1, const Vector3d& p2, double& t, Vector3d& collisionPoint)
{
   Vector3d edge = p2 - p1;
   Vector3d baseToVertex = p1 - data.basePoint;
   double edgeLengthSquared = edge.Length();
   edgeLengthSquared *= edgeLengthSquared;
   double edgeDotVelocity = edge.Dot(data.velocity);
   double edgeDotBaseToVertex = edge.Dot(baseToVertex);
   double baseToVertexLengthSquared = baseToVertex.Length();
   baseToVertexLengthSquared *= baseToVertexLengthSquared;

   // calculate parameters for equation
   double velocityLengthSquared = data.velocity.Length();
   velocityLengthSquared *= velocityLengthSquared;

   double a = edgeLengthSquared * -velocityLengthSquared + edgeDotVelocity * edgeDotVelocity;
   double b = edgeLengthSquared * (2.0 * data.velocity.Dot(baseToVertex)) -
      2.0 * edgeDotVelocity * edgeDotBaseToVertex;
   double c = edgeLengthSquared * (1 - baseToVertexLengthSquared) +
      edgeDotBaseToVertex * edgeDotBaseToVertex;

   // does this swept sphere collide against infinite edge?
   double newT;
   if (GetLowestRoot(a, b, c, t, newT))
   {
      // check if intersection is within line segment:
      double f = (edgeDotVelocity * newT - edgeDotBaseToVertex) /
         edgeLengthSquared;

      if (f >= 0.0 && f < 1.0)
      {
         // intersection took place within segment
         t = newT;
         collisionPoint = f * edge + p1;
         return true;
      }
   }

   return false;
}

/// Calculates lowest root of quadratic equation of the form
/// ax^2 + bx + c = 0; the solution that is returned is positive and lower
/// than maxR. Other solutions are disregarded.
bool CollisionDetection::GetLowestRoot(double a, double b, double c,
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
   Vector3d cp1 = Vector3d::Cross(b - a, p1 - a);
   Vector3d cp2 = Vector3d::Cross(b - a, p2 - a);

   return cp1.Dot(cp2) >= 0;
}

bool CollisionDetection::CheckPointInTriangle(const Vector3d& point,
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
   ac_bb = (a * c) - (b * b);

   Vector3d vp = Vector3d(point.x - pa.x, point.y - pa.y, point.z - pa.z);

   float d = vp.Dot(e10);
   float e = vp.Dot(e20);

   float x = (d * c) - (e * b);
   float y = (e * a) - (d * b);
   float z = x + y - ac_bb;

#define in(a) ((Uint32&) a)

   return ((in(z) & ~(in(x) | in(y))) & 0x80000000) != 0;
}
