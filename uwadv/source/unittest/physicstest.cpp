/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2004,2005 Underworld Adventures Team

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file physicstest.cpp

   \brief unit tests for physics simulation

*/

// needed includes
#include "common.hpp"
#include "CppUnitMini.h"
#include "physics.hpp"


class ua_test_physics_model_body: public ua_physics_body
{
public:
   //! ctor; to keep things simple, test against a sphere
   ua_test_physics_model_body():pos(0,0,0)
   {
      ellipsoid = ua_vector3d(1,1,1);
   }

   virtual ua_vector3d get_pos(){ return pos; };
   virtual void set_pos(ua_vector3d& the_pos){ pos = the_pos; };
   virtual ua_vector3d get_dir(){ return ua_vector3d(0,1,0); }

   virtual void hit_floor()
   {
   }

protected:
   ua_vector3d pos;
};


//! \todo write more tests for math helper functions
class ua_test_physics_model: public CPPUNIT_NS::TestCase,
   public ua_physics_model_callback
{
public:
   // generate test suite
   CPPUNIT_TEST_SUITE(ua_test_physics_model)
      CPPUNIT_TEST(testGetLowestRoot)
      CPPUNIT_TEST(testTrackObjectNoCollision1)
//      CPPUNIT_TEST(testTrackObjectNoCollision2)
      CPPUNIT_TEST(testTrackObjectNoCollision3)
//      CPPUNIT_TEST(testTrackObjectNoCollision4)
      CPPUNIT_TEST(testTrackObjectCollisionPerpendicular)
      CPPUNIT_TEST(testTrackObjectCollisionResponse1)
      CPPUNIT_TEST(testTrackObjectCollisionResponse2)
//      CPPUNIT_TEST(testTrackObjectCollisionResponse3)
   CPPUNIT_TEST_SUITE_END()

   virtual void setUp()
   {
      model.init(this);
//      body.set_ellipsoid(ua_vector3d(1,1,1));
   }

   virtual void tearDown()
   {
   }

   void testGetLowestRoot()
   {
      double new_t = 0.0;

      // equation with no solutions: x^2 + 2x + 2 = 0
      CPPUNIT_ASSERT( false == model.get_lowest_root(1.0, 2.0, 2.0, 0.0, new_t) );

      // equation with 2 double solutions: x^2 - 2x + 1 = 0
      // x1/2 = 1; limit is above, 1 is returned
      CPPUNIT_ASSERT( true == model.get_lowest_root(1.0, -2.0, 1.0, 1.5, new_t) );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, new_t, 1e-6 );

      // equation with 2 solutions: 2x^2 + 2x - 4 = 0
      // x1 = -2, x2 = 1, limit is above, 1 is returned
      CPPUNIT_ASSERT( true == model.get_lowest_root(2.0, 2.0, -4.0, 10.0, new_t) );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, new_t, 1e-6 );

      // equation with 2 solutions: -2x^2 - 4x - 1.5 = 0
      // x1 = -1.5, x2 = -0.5, all two are disregarded
      CPPUNIT_ASSERT( false == model.get_lowest_root(-2.0, -4.0, -1.5, 0.5, new_t) );

      // equation with 2 solutions: x^2 - 2.5x + 1 = 0
      // x1 = 0.5, x2 = 2.0, the lower value is taken
      CPPUNIT_ASSERT( true == model.get_lowest_root(1.0, -2.5, 1.0, 1.9, new_t) );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, new_t, 1e-6 );
   }

/*
   void testMathHelperIntersectRayPlane()
   {
      // testing ua_physics_intersect_ray_plane

      // test plane with z=2
      ua_vector3d plane_origin(2,2,2);
      ua_vector3d plane_normal(0,0,1);

      // test ray 1; parallel to plane
      {
         ua_vector3d ray1_origin(-2,1,5);
         ua_vector3d ray1_vector(1,1,0);

         double dist = ua_physics_intersect_ray_plane(ray1_origin,
            ray1_vector, plane_origin, plane_normal);

         // must not intersect, i.e. return -1.0
         CPPUNIT_ASSERT( fabs(dist+1.0) < 1e-6 );
      }

      // test ray 2; on plane
      {
         ua_vector3d ray2_origin(3,1,2);
         ua_vector3d ray2_vector(2,-2,0);

         double dist = ua_physics_intersect_ray_plane(ray2_origin,
            ray2_vector, plane_origin, plane_normal);

         // must not intersect
         CPPUNIT_ASSERT( fabs(dist+1.0) < 1e-6 );
      }

      // test ray3; hits plane at (-2,-2,2)
      {
         ua_vector3d ray3_origin(-2,1,5);
         ua_vector3d ray3_vector(0,1,1);

         double dist = ua_physics_intersect_ray_plane(ray3_origin,
            ray3_vector, plane_origin, plane_normal);

         // must intersect, at dist -2.0
         CPPUNIT_ASSERT( fabs(dist+3.0) < 1e-6 );

         ua_vector3d hitpos = ray3_vector;
         hitpos *= dist;
         hitpos += ray3_origin;

         hitpos -= ua_vector3d(-2,-2,2);

         // at given point
         CPPUNIT_ASSERT(hitpos.length() < 1e-6);
      }
   }

   void testMathHelperIntersectRaySphere()
   {
      // sphere at (1,1,0) with radius 5
      ua_vector3d sphere_origin(1,1,0);
      double radius = 5.0;

      // test ray 1; doesn't hit sphere
      {
         ua_vector3d ray1_origin(-5,-3,4);
         ua_vector3d ray1_vector(-1,2,1);

         double dist = ua_physics_intersect_ray_sphere(ray1_origin,
            ray1_vector, sphere_origin, radius);

         // must not intersect, i.e. return -1.0
         CPPUNIT_ASSERT( fabs(dist+1.0) < 1e-6 );
      }

      // test ray 1; doesn't hit sphere
      {
         ua_vector3d ray2_origin(1,-2,1);
         ua_vector3d ray2_vector(1,1,-1);

         double dist = ua_physics_intersect_ray_sphere(ray2_origin,
            ray2_vector, sphere_origin, radius);

         // must intersect with dist = 1.56
         CPPUNIT_ASSERT( fabs(dist+1.56) < 0.1 );
      }
   }
*/
   void testTrackObjectNoCollision1()
   {
      ua_vector3d pos(0,0,0);
      ua_vector3d dir(1,0,0);
      ua_vector3d dest(pos);
      dest += dir;

      body.set_pos(pos);

      // test: no triangles, no collision

      // no triangles
      triangles.clear();

      // must not collide with anything
      CPPUNIT_ASSERT( false == model.track_object(body,dir,false) );

      dest -= body.get_pos();

      // must match final position
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, dest.length(), 1e-6 );
   }

   void testTrackObjectNoCollision2()
   {
      ua_vector3d pos(0,0,0);
      ua_vector3d dir(1,0,0);
      ua_vector3d dest(pos);
      dest += dir;

      body.set_pos(pos);

      // test: no collision with a triangle embedded into body and
      // perpendicular to the direction

      // insert a triangle
      ua_triangle3d_textured tri;

      tri.vertices[0].pos = ua_vector3d(0.1, 4.0, 0.0);
      tri.vertices[1].pos = ua_vector3d(0.1, 0.0, 4.0);
      tri.vertices[2].pos = ua_vector3d(0.1,-4.0, 0.0);

      triangles.clear();
      triangles.push_back(tri);

      // to be sure, insert backfacing plane, too
      std::swap(tri.vertices[1].pos,tri.vertices[2].pos);
      triangles.push_back(tri);

      // must not collide with the triangle
      CPPUNIT_ASSERT( false == model.track_object(body,dir,false) );

      dest -= body.get_pos();

      // must match final position
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, dest.length(), 1e-6 );
   }

   void testTrackObjectNoCollision3()
   {
      ua_vector3d pos(0,0,0);
      ua_vector3d dir(1,0,0);
      ua_vector3d dest(pos);
      dest += dir;

      body.set_pos(pos);

      // test: no collision with a triangle embedded into body that is
      // parallel to the direction

      // insert a parallel triangle
      ua_triangle3d_textured tri;

      tri.vertices[0].pos = ua_vector3d( 4.0, 0.1, 0.0);
      tri.vertices[1].pos = ua_vector3d( 0.0, 0.1, 4.0);
      tri.vertices[2].pos = ua_vector3d(-4.0, 0.1, 0.0);

      triangles.clear();
      triangles.push_back(tri);

      // to be sure, insert backfacing plane, too
      std::swap(tri.vertices[1].pos,tri.vertices[2].pos);
      triangles.push_back(tri);

      // must not collide with the triangle
      CPPUNIT_ASSERT( false == model.track_object(body,dir,false) );

      dest -= body.get_pos();

      // must match final position
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, dest.length(), 1e-6 );
   }

   void testTrackObjectNoCollision4()
   {
      ua_vector3d pos(0,0,0);
      ua_vector3d dir(1,0,0);
      ua_vector3d dest(pos);
      dest += dir;

      body.set_pos(pos);

      // test: no collision with a triangle embedded into body that is
      // almost parallel to the direction

      // insert a parallel triangle
      ua_triangle3d_textured tri;

      tri.vertices[0].pos = ua_vector3d( 4.0, 0.2, 0.0);
      tri.vertices[1].pos = ua_vector3d( 0.0, 0.1, 4.0);
      tri.vertices[2].pos = ua_vector3d(-4.0, 0.0, 0.0);

      triangles.clear();
      triangles.push_back(tri);

      // to be sure, insert backfacing plane, too
      std::swap(tri.vertices[1].pos,tri.vertices[2].pos);
      triangles.push_back(tri);

      // must not collide with the triangle
      CPPUNIT_ASSERT( false == model.track_object(body,dir,false) );

      dest -= body.get_pos();

      // must match final position
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, dest.length(), 1e-6 );
   }

   void testTrackObjectCollisionPerpendicular()
   {
      ua_vector3d pos(1,0,0);
      ua_vector3d dir(1,0,0);
      ua_vector3d dest(pos);

      body.set_pos(pos);

      // test: collision with a triangle perpendicular to direction

      // insert a triangle with x=2.5
      // at the next step we'll colliding with it
      ua_triangle3d_textured tri;

      tri.vertices[0].pos = ua_vector3d(2.5, 4.0,-4.0);
      tri.vertices[1].pos = ua_vector3d(2.5,-8.0, 0.0);
      tri.vertices[2].pos = ua_vector3d(2.5, 4.0, 4.0);

      triangles.clear();
      triangles.push_back(tri);

      // must collide with our triangle
      CPPUNIT_ASSERT( true == model.track_object(body,dir,false) );

      // calculate new pos; we halfway hit the plane
      dest.x += 0.5;
      dest -= body.get_pos();

      CPPUNIT_ASSERT( dest.length() < 0.01 );
   }

   void testTrackObjectCollisionResponse1()
   {
      ua_vector3d pos(0,0,0);
      ua_vector3d dir(1,0,0);

      // 'expected' destination point
      ua_vector3d dest(pos);
      dest += dir;

      body.set_pos(pos);

      // test: collision with a triangle that pushes us to y < 0

      // insert a triangle
      ua_triangle3d_textured tri;

      tri.vertices[0].pos = ua_vector3d(4.0,-2.0,-4.0);
      tri.vertices[1].pos = ua_vector3d(4.0,-2.0, 4.0);
      tri.vertices[2].pos = ua_vector3d(0.0, 2.0, 0.0);

      triangles.clear();
      triangles.push_back(tri);

      // must collide with our triangle
      CPPUNIT_ASSERT( true == model.track_object(body,dir,false) );

      // has to push us to the right
      pos = body.get_pos();
      CPPUNIT_ASSERT( pos.y < 0 && pos.x < dest.x );
   }

   void testTrackObjectCollisionResponse2()
   {
      ua_vector3d pos(0,0,0);
      ua_vector3d dir(1,0,0);

      // 'expected' destination point
      ua_vector3d dest(pos);
      dest += dir;

      body.set_pos(pos);

      // test: collision with a triangle edge

      // insert a triangle
      ua_triangle3d_textured tri;

      tri.vertices[0].pos = ua_vector3d(1.5,0.0,0.0);
      tri.vertices[1].pos = ua_vector3d(1.5,0.0,2.0);
      tri.vertices[2].pos = ua_vector3d(2.0,2.0,0.0);

      triangles.clear();
      triangles.push_back(tri);

      // must collide with our triangle, hitting a triangle edge
      CPPUNIT_ASSERT( true == model.track_object(body,dir,false) );

      // has to stop at the edge, at xpos = 0.5
      pos = body.get_pos();
      CPPUNIT_ASSERT( pos.x < dest.x );

      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, pos.x, 0.1 );
   }

   void testTrackObjectCollisionResponse3()
   {
      ua_vector3d pos(0,0,0);
      ua_vector3d dir(1,0,0);

      // 'expected' destination point
      ua_vector3d dest(pos);
      dest += dir;

      body.set_pos(pos);

      // test: collision with a two triangles, stopping the body in the corner

      // insert a triangle
      ua_triangle3d_textured tri1, tri2;

      tri1.vertices[0].pos = ua_vector3d(1.5,0.0,-4.0);
      tri1.vertices[1].pos = ua_vector3d(1.5,0.0, 4.0);
      tri1.vertices[2].pos = ua_vector3d(0.0,3.0, 0.0);

      tri2.vertices[0].pos = ua_vector3d(1.5, 0.0, 4.0);
      tri2.vertices[1].pos = ua_vector3d(1.5, 0.0, -4.0);
      tri2.vertices[2].pos = ua_vector3d(0.0,-3.0, 0.0);

      triangles.clear();
      triangles.push_back(tri1);
      triangles.push_back(tri2);

      // must collide with our triangles
      CPPUNIT_ASSERT( true == model.track_object(body,dir,false) );

      // has to push us in the corner, at y=0
      pos = body.get_pos();

      CPPUNIT_ASSERT( pos.x < dest.x );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, pos.y, 0.01 );
   }

   // callback function to feed physics model with triangles
   virtual void get_surrounding_triangles(unsigned int xpos,
      unsigned int ypos, std::vector<ua_triangle3d_textured>& alltriangles)
   {
      alltriangles.clear();
      alltriangles.insert(alltriangles.begin(), triangles.begin(), triangles.end());
   }

protected:

   ua_physics_model model;

   std::vector<ua_triangle3d_textured> triangles;
   ua_test_physics_model_body body;
};

// register test suite
CPPUNIT_TEST_SUITE_REGISTRATION(ua_test_physics_model);
