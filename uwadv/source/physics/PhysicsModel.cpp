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
/// used collision detection and response code from this paper:
/// http://www.peroxide.dk/papers/collision/collision.pdf
/// previous document:
/// http://www.peroxide.dk/download/tutorials/tut10/pxdtut10.html
//
#include "common.hpp"
#include "PhysicsModel.hpp"

/// trick to get smoother CD
const double c_cdLiftoff = 0.5;

/// minimum distance
const double c_physicsMinDistance = 0.005;


bool CheckPointInTriangle(const Vector3d& point,
   const Vector3d& pa, const Vector3d& pb, const Vector3d& pc);


/// collision check data
struct CollisionData
{
   /// ellipsoid radius
   Vector3d ellipsoid;

   // information about the move being requested (in eSpace)
   Vector3d dir;        ///< direction
   Vector3d norm_dir;   ///< normalized dir
   Vector3d base_point; ///< base point

   // hit information
   bool found_collision;   ///< indicates if collision was found
   double nearest_dist;    ///< dist to nearest triangle
   Vector3d intersect_point;  ///< direction intersection point
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
   Vector3d effect_force;
   TrackObject(body, effect_force);
}

/// \param body physics body to track
/// \param dir direction vector of object
/// \param gravityForce indicates if we're processing a gravity force
/// \return true if we collided with triangle
bool PhysicsModel::TrackObject(PhysicsBody& body, Vector3d dir,
   bool gravityForce)
{
   // setup collision package for this tracking
   CollisionData data;
   data.ellipsoid = body.GetEllipsoid();
   data.found_collision = false;

   Vector3d pos = body.GetPosition();

   // \todo
   extern bool my_movement;
   if (my_movement)
      UaTrace("old pos: %2.3f / %2.3f / %2.3f\n", pos.x, pos.y, pos.z);

   if (!gravityForce)
      pos.z += 0.5;

   // transform to ellipsoid space
   pos /= data.ellipsoid;
   dir /= data.ellipsoid;

   // call recursive collision response function
   m_collisionRecursionDepth = 0;

   bool collided = CollideWithWorld(data, pos, dir);

   if (gravityForce && collided)
   {
      body.HitFloor();
      body.ResetGravity();
   }

   // transform position back to normal space and set it
   pos *= data.ellipsoid;

   if (!gravityForce)
      pos.z -= 0.5;

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
/// \param dir current direction vector in eSpace
/// \return true when a collision occured
bool PhysicsModel::CollideWithWorld(CollisionData& data,
   Vector3d& pos, const Vector3d& dir)
{
   double min_distance = c_physicsMinDistance;

   // do we need to worry?
   if (m_collisionRecursionDepth > 5)
      return true;

   // set up collision data we send to the mesh
   data.base_point = pos;
   data.norm_dir = data.dir = dir;
   data.norm_dir.normalize();
   data.found_collision = false;

   // check triangle mesh collision (calls the collision routines)
   CheckCollision(data);

   // if no collision we just move along the direction
   if (!data.found_collision)
   {
      pos += dir;
      return false; // no collision
   }

   // collision occured

   Vector3d dest_point(pos); dest_point += dir;
   Vector3d new_basepoint(pos);

   // only update if we are not already very close
   // and if so we only move very close to intersection ...
   // not to the exact spot
   if (data.nearest_dist >= min_distance)
   {
      Vector3d dir2(dir);
      dir2.normalize();
      dir2 *= (data.nearest_dist - min_distance);

      new_basepoint = data.base_point;
      new_basepoint += dir2;

      // adjust polygon intersection point (so sliding
      // plane will be unaffected by the fact that we
      // move slightly less than collision tells us)
      dir2.normalize();
      dir2 *= min_distance;
      data.intersect_point -= dir2;
   }

   // determine the sliding plane
   Vector3d slideplane_normal = new_basepoint;
   slideplane_normal -= data.intersect_point;
   slideplane_normal.normalize();

   Plane3d sliding_plane(data.intersect_point, slideplane_normal);

   // calculate new destination point
   Vector3d new_destpoint = slideplane_normal;
   new_destpoint *= -sliding_plane.signed_dist_to(dest_point);
   new_destpoint += dest_point;

   // generate the slide vector, which will become our new velocity vector for
   // the next iteration
   Vector3d new_dir = new_destpoint;
   new_dir -= data.intersect_point;

   // recurse

   // don't recurse if the new direction is very small
   if (new_dir.length() < min_distance)
   {
      pos = new_basepoint;
      return true;
   }

   // call the function with the new position and velocity
   m_collisionRecursionDepth++;

   CollideWithWorld(data, new_basepoint, new_dir);
   pos = new_basepoint;

   m_collisionRecursionDepth--;

   // recursion call might have not collided with something, but we surely had
   return true;
}

void PhysicsModel::CheckCollision(CollisionData& data)
{
   unsigned int xpos = static_cast<unsigned int>(data.base_point.x*data.ellipsoid.x);
   unsigned int ypos = static_cast<unsigned int>(data.base_point.y*data.ellipsoid.y);

   // retrieve all tile triangles to check
   std::vector<Triangle3dTextured> allTriangles;

   if (m_callback != NULL)
      m_callback->GetSurroundingTriangles(xpos, ypos, allTriangles);

   // check all triangles
   unsigned int max = allTriangles.size();
   for (unsigned int i = 0; i < max; i++)
   {
      Triangle3dTextured& tri = allTriangles[i];

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
   Plane3d triangle_plane(p1, p2, p3);

   // is triangle front-facing to the direction vector?
   // we only check front-facing triangles
   if (!triangle_plane.is_front_facing_to(data.norm_dir))
      return;

   // triangle is front-facing

   // get interval of plane intersection
   double t0, t1;
   bool embedded = false;

   // calculate signed distance from sphere
   // position to triangle plane
   double dist_triangle = triangle_plane.signed_dist_to(data.base_point);

   // cache this as we're going to use it a few times below
   double normal_dot_dir = triangle_plane.normal.dot(data.dir);

   // if sphere is travelling parallel to the plane
   if (normal_dot_dir == 0.0)
   {
      if (fabs(dist_triangle) >= 1.0)
      {
         // sphere is not embedded in plane; no collision possible
         return;
      }
      else
      {
         // sphere is embedded in plane
         // it intersets in the whole range [0..1]
         embedded = true;
         t0 = 0.0;
         t1 = 1.0;
      }
   }
   else
   {
      // N dot D is not 0. calculate intersection interval
      t0 = (-1.0 - dist_triangle) / normal_dot_dir;
      t1 = (1.0 - dist_triangle) / normal_dot_dir;

      // swap so t0 < t1
      if (t0 > t1) std::swap(t0, t1);

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
   Vector3d collision_point;
   bool found = false;
   double t = 1.0;

   // First we check for the easy case - collision inside
   // the triangle. If this happens it must be at time t0
   // as this is when the sphere rests on the front side
   // of the triangle plane. Note, this can only happen if
   // the sphere is not embedded in the triangle plane.
   if (!embedded)
   {
      Vector3d plane_intersect_point;
      plane_intersect_point += data.dir;
      plane_intersect_point *= t0;
      plane_intersect_point += data.base_point;
      plane_intersect_point -= triangle_plane.normal;

      if (CheckPointInTriangle(plane_intersect_point,
         p1, p2, p3))
      {
         found = true;
         t = t0;
         collision_point = plane_intersect_point;
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
      Vector3d dir = data.dir;
      Vector3d base = data.base_point;

      double dir_sq_length = dir.length(); dir_sq_length *= dir_sq_length;
      double a, b, c;
      double new_t;

      // For each vertex or edge a quadratic equation have to
      // be solved. We parameterize this equation as
      // a*t^2 + b*t + c = 0 and below we calculate the
      // parameters a,b and c for each test.

      // check against points
      a = dir_sq_length;

      // P1
      b = 2.0*(dir.dot(base - p1));
      c = (p1 - base).length(); c *= c;
      c -= 1.0;
      if (GetLowestRoot(a, b, c, t, new_t))
      {
         t = new_t;
         found = true;
         collision_point = p1;
      }

      // P2
      b = 2.0*(dir.dot(base - p2));
      c = (p2 - base).length(); c *= c;
      c -= 1.0;
      if (GetLowestRoot(a, b, c, t, new_t))
      {
         t = new_t;
         found = true;
         collision_point = p2;
      }

      // P3
      b = 2.0*(dir.dot(base - p3));
      c = (p3 - base).length(); c *= c;
      c -= 1.0;
      if (GetLowestRoot(a, b, c, t, new_t))
      {
         t = new_t;
         found = true;
         collision_point = p3;
      }

      // check against edges

      // p1 -> p2
      {
         Vector3d edge = p2 - p1;
         Vector3d base_to_vertex = p1 - base;
         double edge_sq_length = edge.length();
         edge_sq_length *= edge_sq_length;
         double edge_dot_dir = edge.dot(dir);
         double edge_dot_base_to_vertex = edge.dot(base_to_vertex);
         double base_to_vertex_sq_length = base_to_vertex.length();
         base_to_vertex_sq_length *= base_to_vertex_sq_length;

         // calculate parameters for equation
         a = edge_sq_length * -dir_sq_length + edge_dot_dir * edge_dot_dir;
         b = edge_sq_length * (2.0*dir.dot(base_to_vertex)) -
            2.0*edge_dot_dir*edge_dot_base_to_vertex;
         c = edge_sq_length * (1 - base_to_vertex_sq_length) +
            edge_dot_base_to_vertex * edge_dot_base_to_vertex;

         // does this swept sphere collide against infinite edge?
         if (GetLowestRoot(a, b, c, t, new_t))
         {
            // check if intersection is within line segment:
            double f = (edge_dot_dir*new_t - edge_dot_base_to_vertex) /
               edge_sq_length;

            if (f >= 0.0 && f < 1.0)
            {
               // intersection took place within segment
               t = new_t;
               found = true;
               collision_point = edge;
               collision_point *= f;
               collision_point += p1;
            }
         }
      }

      // p2 -> p3
      {
         Vector3d edge = p3 - p2;
         Vector3d base_to_vertex = p2 - base;
         double edge_sq_length = edge.length();
         edge_sq_length *= edge_sq_length;
         double edge_dot_dir = edge.dot(dir);
         double edge_dot_base_to_vertex = edge.dot(base_to_vertex);
         double base_to_vertex_sq_length = base_to_vertex.length();
         base_to_vertex_sq_length *= base_to_vertex_sq_length;

         // calculate parameters for equation
         a = edge_sq_length * -dir_sq_length + edge_dot_dir * edge_dot_dir;
         b = edge_sq_length * (2.0*dir.dot(base_to_vertex)) -
            2.0*edge_dot_dir*edge_dot_base_to_vertex;
         c = edge_sq_length * (1 - base_to_vertex_sq_length) +
            edge_dot_base_to_vertex * edge_dot_base_to_vertex;

         // does this swept sphere collide against infinite edge?
         if (GetLowestRoot(a, b, c, t, new_t))
         {
            // check if intersection is within line segment:
            double f = (edge_dot_dir*new_t - edge_dot_base_to_vertex) /
               edge_sq_length;

            if (f >= 0.0 && f < 1.0)
            {
               // intersection took place within segment
               t = new_t;
               found = true;
               collision_point = edge;
               collision_point *= f;
               collision_point += p2;
            }
         }
      }

      // p3 -> p1
      {
         Vector3d edge = p1 - p3;
         Vector3d base_to_vertex = p3 - base;
         double edge_sq_length = edge.length();
         edge_sq_length *= edge_sq_length;
         double edge_dot_dir = edge.dot(dir);
         double edge_dot_base_to_vertex = edge.dot(base_to_vertex);
         double base_to_vertex_sq_length = base_to_vertex.length();
         base_to_vertex_sq_length *= base_to_vertex_sq_length;

         // calculate parameters for equation
         a = edge_sq_length * -dir_sq_length + edge_dot_dir * edge_dot_dir;
         b = edge_sq_length * (2.0*dir.dot(base_to_vertex)) -
            2.0*edge_dot_dir*edge_dot_base_to_vertex;
         c = edge_sq_length * (1 - base_to_vertex_sq_length) +
            edge_dot_base_to_vertex * edge_dot_base_to_vertex;

         // does this swept sphere collide against infinite edge?
         if (GetLowestRoot(a, b, c, t, new_t))
         {
            // check if intersection is within line segment:
            double f = (edge_dot_dir*new_t - edge_dot_base_to_vertex) /
               edge_sq_length;

            if (f >= 0.0 && f < 1.0)
            {
               // intersection took place within segment
               t = new_t;
               found = true;
               collision_point = edge;
               collision_point *= f;
               collision_point += p3;
            }
         }
      }

   } // !found

   // set result
   if (found)
   {
      // distance to collision: 't' is time of collision
      double dist_coll = t * data.dir.length();

      // does this triangle qualify for the closest hit?
      // it does if it's the first hit or the closest
      if (!data.found_collision ||
         dist_coll < data.nearest_dist)
      {
         // collision information necessary for sliding
         data.nearest_dist = dist_coll;
         data.intersect_point = collision_point;
         data.found_collision = true;
      }
   }
}

/// Calculates lowest root of quadratic equation of the form
/// ax^2 + bx + c = 0; the solution that is returned is positive and lower
/// than max_r. Other solutions are disregarded.
bool PhysicsModel::GetLowestRoot(double a, double b, double c,
   double max_r, double& root)
{
   // check if a solution exists
   double determinant = b * b - 4.0*a*c;

   // if determinant is negative it means no solutions
   if (determinant < 0.0)
      return false;

   // calculate the two roots: (if determinant == 0 then
   // x1==x2 but let’s disregard that slight optimization)
   double sqrt_d = sqrt(determinant);
   double r1 = (-b - sqrt_d) / (2 * a);
   double r2 = (-b + sqrt_d) / (2 * a);

   // sort so x1 <= x2
   if (r1 > r2) std::swap(r1, r2);

   // get lowest root
   if (r1 > 0 && r1 < max_r)
   {
      root = r1;
      return true;
   }

   // it is possible that we want x2 - this can happen
   // if x1 < 0
   if (r2 > 0 && r2 < max_r)
   {
      root = r2;
      return true;
   }

   // no (valid) solutions
   return false;
}

/// \todo replace in() macro test with somewhat more portable
bool CheckPointInTriangle(const Vector3d& point,
   const Vector3d& pa, const Vector3d& pb, const Vector3d& pc)
{
   Vector3d e10 = pb - pa;
   Vector3d e20 = pc - pa;

   float a, b, c, ac_bb;

   a = e10.dot(e10);
   b = e10.dot(e20);
   c = e20.dot(e20);
   ac_bb = (a*c) - (b*b);

   Vector3d vp = Vector3d(point.x - pa.x, point.y - pa.y, point.z - pa.z);

   float d = vp.dot(e10);
   float e = vp.dot(e20);

   float x = (d*c) - (e*b);
   float y = (e*a) - (d*b);
   float z = x + y - ac_bb;

#define in(a) ((Uint32&) a)

   return ((in(z)& ~(in(x) | in(y))) & 0x80000000) != 0;
}
