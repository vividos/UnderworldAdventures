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
#include "unittest.hpp"
#include "physics.hpp"
#include "import.hpp"
#include "level.hpp"
#include "geometry.hpp"


class ua_test_physics_model_body: public ua_physics_body
{
public:
   //! ctor; to keep things simple, test against a sphere
   ua_test_physics_model_body():pos(0,0,0)
   {
      ellipsoid = ua_vector3d(1,1,1);
   }

   void set_ellipsoid(ua_vector3d new_ellipsoid){ ellipsoid = new_ellipsoid; }

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
class ua_test_physics_model: public ua_testcase,
   public ua_physics_model_callback
{
public:
   // set up test suite
   CPPUNIT_TEST_SUITE(ua_test_physics_model)
      CPPUNIT_TEST(test_get_lowest_root)
      //CPPUNIT_TEST(test_math_helper_intersect_ray_plane)
      //CPPUNIT_TEST(test_math_helper_intersect_ray_sphere)
      CPPUNIT_TEST(test_check_point_in_triangle)
      CPPUNIT_TEST(test_track_object_no_collision1)
      CPPUNIT_TEST(test_track_object_no_collision2)
      CPPUNIT_TEST(test_track_object_no_collision3)
      CPPUNIT_TEST(test_track_object_no_collision4)
      CPPUNIT_TEST(test_track_object_collision_perpendicular)
      CPPUNIT_TEST(test_track_object_collision_response1)
      CPPUNIT_TEST(test_track_object_collision_response2)
      CPPUNIT_TEST(test_track_object_collision_response3)
      CPPUNIT_TEST(test_stuck_level0_floor)
   CPPUNIT_TEST_SUITE_END()

protected:

   virtual void setUp()
   {
      model.init(this);
      body.set_ellipsoid(ua_vector3d(1,1,1));
   }

   // callback function to feed physics model with triangles
   virtual void get_surrounding_triangles(unsigned int /*xpos*/,
      unsigned int /*ypos*/, std::vector<ua_triangle3d_textured>& alltriangles)
   {
      alltriangles.clear();
      alltriangles.insert(alltriangles.begin(), triangles.begin(), triangles.end());
   }

   void test_get_lowest_root();
   //void test_math_helper_intersect_ray_plane();
   //void test_math_helper_intersect_ray_sphere();
   void test_check_point_in_triangle();
   void test_track_object_no_collision1();
   void test_track_object_no_collision2();
   void test_track_object_no_collision3();
   void test_track_object_no_collision4();
   void test_track_object_collision_perpendicular();
   void test_track_object_collision_response1();
   void test_track_object_collision_response2();
   void test_track_object_collision_response3();
   void test_stuck_level0_floor();

protected:
   ua_physics_model model;

   std::vector<ua_triangle3d_textured> triangles;

   ua_test_physics_model_body body;
};

// register test suite
CPPUNIT_TEST_SUITE_REGISTRATION(ua_test_physics_model);

void ua_test_physics_model::test_get_lowest_root()
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
void ua_test_physics_model::test_math_helper_intersect_ray_plane()
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

void ua_test_physics_model::test_math_helper_intersect_ray_sphere()
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

void ua_test_physics_model::test_check_point_in_triangle()
{
   extern bool ua_physics_check_point_in_triangle(const ua_vector3d& point,
      const ua_vector3d& pa, const ua_vector3d& pb, const ua_vector3d& pc);

   // TODO
//   ua_physics_check_point_in_triangle();
}

void ua_test_physics_model::test_track_object_no_collision1()
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

void ua_test_physics_model::test_track_object_no_collision2()
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

void ua_test_physics_model::test_track_object_no_collision3()
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

void ua_test_physics_model::test_track_object_no_collision4()
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

void ua_test_physics_model::test_track_object_collision_perpendicular()
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

void ua_test_physics_model::test_track_object_collision_response1()
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

void ua_test_physics_model::test_track_object_collision_response2()
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

void ua_test_physics_model::test_track_object_collision_response3()
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

/*! This function tests for players that get stuck when walking up slopes.
*/
void ua_test_physics_model::test_stuck_level0_floor()
{
//   ua_vector3d pos(14.756371115970936, 3.6415141816119467, 76.699086166258056);
//   double rotangle = 94.499999999999076;
//   ua_vector3d pos(14.445312500000000, 5.2773437500000000, 89.539062500000000);
   ua_vector3d pos(14.445312500000000, 5.3952129322154407, 102.38201595772354);
   double rotangle = 90.000000000000000;
   double speed = 0.120000000000000;
   ua_vector3d player_ellipsoid(0.2, 0.2, 11.9); // player ellipsoid

   body.set_pos(pos);
   body.set_ellipsoid(player_ellipsoid);

   ua_vector3d dir(1.0, 0.0, 0.0);
   dir.rotate_z(rotangle);
   dir *= speed;

   // load levels
   {
      std::vector<ua_level> levels;
      ua_uw1_import uw1_import;
      uw1_import.load_levelmaps(levels, get_settings(), "data/");

      CPPUNIT_ASSERT(9 == levels.size());

      // collect all triangles
      ua_geometry_provider prov(levels[0]);
      {
         unsigned int xmin, xmax, ymin, ymax;
         xmin = unsigned(pos.x)-1;
         xmax = xmin + 3;

         ymin = unsigned(pos.y)-1;
         ymax = ymin + 3;

         for (unsigned int x=xmin; x<xmax; x++)
         for (unsigned int y=ymin; y<ymax; y++)
            prov.get_tile_triangles(x, y, triangles);
      }

      ua_trace("collected %u triangles\n", triangles.size());
   }

   // collides with wall
   CPPUNIT_ASSERT(true == model.track_object(body, dir, false) );

   ua_vector3d pos2(body.get_pos());

   // must have moved forward "speed" units
   pos2 -= pos;
   CPPUNIT_ASSERT_DOUBLES_EQUAL(speed, pos2.length(), 0.01);
}
