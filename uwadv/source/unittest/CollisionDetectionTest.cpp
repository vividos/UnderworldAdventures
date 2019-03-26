//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2019 Michael Fink
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
/// \file CollisionDetectionTest.cpp
/// \brief tests for the CollisionDetection class
//
#include "pch.hpp"
#include "Math.hpp"
#include "Triangle3d.hpp"
#include "physics/PhysicsModel.hpp"
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
   /// \brief collision detection class under test
   /// \details this currently wraps PhysicsModel and wraps IPhysicsModelCallback and should be
   /// refactored to usec by PhysicsModel instead
   class CollisionDetection : private IPhysicsModelCallback
   {
   public:
      /// ctor
      CollisionDetection(const std::vector<Triangle3dTextured>& allTriangles, const PhysicsBody& body)
         :m_allTriangles(allTriangles.begin(), allTriangles.end())
      {
      }

      void TrackObject(PhysicsBody& body)
      {
         PhysicsModel model;
         model.Init(this);
         model.SetPhysicsParam(physicsGravity, true);

         model.TrackObject(body);
      }

   private:
      // virtual methods from IPhysicsModelCallback

      /// returns surrounding triangles on given position
      virtual void GetSurroundingTriangles(unsigned int xpos,
         unsigned int ypos, std::vector<Triangle3dTextured>& allTriangles) override
      {
         allTriangles.assign(m_allTriangles.begin(), m_allTriangles.end());
      }

   private:
      std::vector<Triangle3dTextured> m_allTriangles;
   };

   /// physics body under test
   class TestPhysicsBody : public PhysicsBody
   {
   public:
      /// ctor
      TestPhysicsBody(Vector3d pos, Vector3d dir)
         :m_pos(pos),
         m_dir(dir)
      {
         // test perfect sphere
         m_ellipsoid = Vector3d{ 1.0, 1.0, 1.0 };
      }

      // virtual methods from PhysicsBody

      /// returns position of body
      virtual Vector3d GetPosition() override
      {
         return m_pos;
      }

      /// sets new position of body
      virtual void SetPosition(Vector3d& pos) override
      {
         m_pos = pos;
      }

      /// returns current direction vector of body
      virtual Vector3d GetDirection() override
      {
         return m_dir;
      }


   private:
      /// body position
      Vector3d m_pos;

      /// body direction
      Vector3d m_dir;
   };

   /// \brief CollisionDetection tests
   /// Tests collision detection code
   TEST_CLASS(CollisionDetectionTest)
   {
      /// Tests not colliding physics body and empty list of triangles.
      TEST_METHOD(TestNoCollisionNoTriangles)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured>(), body };

         // run
         detection.TrackObject(body);

         // check
         Assert::AreEqual(0.0, (initialPos + velocity - body.GetPosition()).Length(), 1e-6, L"vector position must be equal");
      }

      /// Tests not colliding physics body with a triangle that is too far away.
      TEST_METHOD(TestNoCollisionTrianglesOutOfReach)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 20.0, 20.0, 20.0 }},
               Vertex3d{Vector3d{ 20.0, 21.0, 20.0 }},
               Vertex3d{Vector3d{ 21.0, 20.0, 20.0 }}}
         }, body};

         // run
         detection.TrackObject(body);

         // check
         Assert::AreEqual(0.0, (initialPos + velocity - body.GetPosition()).Length(), 1e-6, L"vector position must be equal");
      }

      /// Tests not colliding physics body with a triangle that is back facing anyway.
      TEST_METHOD(TestNoCollisionTrianglesBackFacing)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 1.5, -10.0, -10.0 }},
               Vertex3d{Vector3d{ 1.5, 10.0, -10.0 }},
               Vertex3d{Vector3d{ 1.5, 10.0, 10.0 }}
            }
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Assert::AreEqual(0.0, (initialPos + velocity - body.GetPosition()).Length(), 1e-6, L"vector position must be equal");
      }

      /// Tests physics body with no forward velocity.
      TEST_METHOD(TestNoCollisionNoForwardVelocity)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 0.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 20.0, 20.0, 20.0 }},
               Vertex3d{Vector3d{ 20.0, 21.0, 20.0 }},
               Vertex3d{Vector3d{ 21.0, 20.0, 20.0 }}}
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Assert::AreEqual(0.0, (initialPos + velocity - body.GetPosition()).Length(), 1e-6, L"vector position must be equal");
      }

      /// Tests colliding physics body with a triangle inside, without touching an edge or corner
      /// of the triangle. The triangle is at x coordinate 1.5, the sphere at x 0.0 and the
      /// velocity is moving in x direction. The body should hit the sphere at 1.5, 0.0, 0.0.
      TEST_METHOD(TestCollisionTriangleInside)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 1.5, -10.0, -10.0 }},
               Vertex3d{Vector3d{ 1.5, 10.0, 10.0 }},
               Vertex3d{Vector3d{ 1.5, 10.0, -10.0 }}}
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Vector3d expectedPos{ 0.5, 0.0, 0.0 };
         Assert::AreEqual(0.0, (body.GetPosition() - expectedPos).Length(), 0.01,
            L"vector position must be equal the expected position");
      }

      /// Tests colliding physics body with a triangle that is already inside the sphere, however
      /// that happened. The triangle is at x position 0.5. The effect should be that the sphere
      /// can't move further into the triangle.
      TEST_METHOD(TestCollisionTriangleInsideSphere)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 0.75, -10.0, -10.0 }},
               Vertex3d{Vector3d{ 0.75, 10.0, 10.0 }},
               Vertex3d{Vector3d{ 0.75, 10.0, -10.0 }}}
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Vector3d expectedPos{ 0.0, 0.0, 0.0 };
         Assert::AreEqual(0.0, (body.GetPosition() - expectedPos).Length(), 0.01,
            L"vector position must be equal the expected position");
      }

      /// Tests colliding physics body with a triangle inside that is not perpendicular to the
      /// velocity, so that the sphere collides with the triangle and a sliding plane is used to
      /// displace the plane "up".
      TEST_METHOD(TestCollisionTriangleInsideWithSlidingPlane)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 1.0, 10.0, -2.0 }},
               Vertex3d{Vector3d{ 1.0, -10.0, -2.0 }},
               Vertex3d{Vector3d{ 2.0, 0.0, 3.0 }}}
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Assert::IsTrue(body.GetPosition().x > 0.3 && body.GetPosition().x < 1.0,
            L"body x pos must be beyond the 0.3 point, but not the whole velocity vector");
         Assert::AreEqual(0.0, body.GetPosition().y, 0.01, L"y pos must not be modified");
         Assert::IsTrue(body.GetPosition().z > 0.0, L"z pos must be 'up' quite a bit");
      }

      /// Tests colliding physics body with a triangle point. The triangle's point is at x 1.5
      /// and directly in the velocity vector. It is expected that the body hits the point, but is
      /// not moved "up" (in z direction).
      TEST_METHOD(TestCollisionTrianglePoint)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 1.5, 0.0, 0.0 }},
               Vertex3d{Vector3d{ 20.0, 10.0, 0.0 }},
               Vertex3d{Vector3d{ 20.0, -10.0, 0.0 }}}
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Vector3d expectedPos{ 0.5, 0.0, 0.0 };
         Assert::AreEqual(0.0, (body.GetPosition() - expectedPos).Length(), 0.01,
            L"vector position must be equal the expected position");
      }

      /// Tests colliding physics body with a triangle point. This is the variant 2 with the
      /// second triangle point.  Same expectation as in TestCollisionTrianglePoint().
      TEST_METHOD(TestCollisionTrianglePointVariant2)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 20.0, -10.0, 0.0 }},
               Vertex3d{Vector3d{ 1.5, 0.0, 0.0 }},
               Vertex3d{Vector3d{ 20.0, 10.0, 0.0 }}}
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Vector3d expectedPos{ 0.5, 0.0, 0.0 };
         Assert::AreEqual(0.0, (body.GetPosition() - expectedPos).Length(), 0.01,
            L"vector position must be equal the expected position");
      }

      /// Tests colliding physics body with a triangle point. This is the variant 3 with the
      /// third triangle point.  Same expectation as in TestCollisionTrianglePoint().
      TEST_METHOD(TestCollisionTrianglePointVariant3)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 20.0, 10.0, 0.0 }},
               Vertex3d{Vector3d{ 20.0, -10.0, 0.0 }},
               Vertex3d{Vector3d{ 1.5, 0.0, 0.0 }}}
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Vector3d expectedPos{ 0.5, 0.0, 0.0 };
         Assert::AreEqual(0.0, (body.GetPosition() - expectedPos).Length(), 0.01,
            L"vector position must be equal the expected position");
      }

      /// Tests colliding physics body with a triangle edge. The triangle's edge is at x 1.5, and
      /// the triangle is slightly going "up" (the z axis). It is expected that the body hits the
      /// edge, but is not moved "up" (in z direction).
      TEST_METHOD(TestCollisionTriangleEdge)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 1.5, 10.0, 0.0 }},
               Vertex3d{Vector3d{ 1.5, -10.0, 0.0 }},
               Vertex3d{Vector3d{ 20.0, 0.0, 2.0 }}} // last point is moved "up" a bit
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Assert::AreEqual(0.5, body.GetPosition().x, 0.01, L"body x pos must be at the proper point");
         Assert::AreEqual(0.0, body.GetPosition().y, 0.01, L"y pos must not be modified");
         Assert::AreEqual(0.0, body.GetPosition().z, 0.01, L"z pos must not be modified");
      }

      /// Tests colliding physics body with a triangle edge. This is the variant 2 with the second
      /// triangle edge. Same expectation as in TestCollisionTriangleEdge().
      TEST_METHOD(TestCollisionTriangleEdgeVariant2)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 20.0, 0.0, 2.0 }},
               Vertex3d{Vector3d{ 1.5, 10.0, 0.0 }},
               Vertex3d{Vector3d{ 1.5, -10.0, 0.0 }}}
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Assert::AreEqual(0.5, body.GetPosition().x, 0.01, L"body x pos must be at the proper point");
         Assert::AreEqual(0.0, body.GetPosition().y, 0.01, L"y pos must not be modified");
         Assert::AreEqual(0.0, body.GetPosition().z, 0.01, L"z pos must not be modified");
      }

      /// Tests colliding physics body with a triangle edge. This is the variant 3 with the third
      /// triangle edge. Same expectation as in TestCollisionTriangleEdge().
      TEST_METHOD(TestCollisionTriangleEdgeVariant3)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 1.5, -10.0, 0.0 }},
               Vertex3d{Vector3d{ 20.0, 0.0, 2.0 }},
               Vertex3d{Vector3d{ 1.5, 10.0, 0.0 }}}
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Assert::AreEqual(0.5, body.GetPosition().x, 0.01, L"body x pos must be at the proper point");
         Assert::AreEqual(0.0, body.GetPosition().y, 0.01, L"y pos must not be modified");
         Assert::AreEqual(0.0, body.GetPosition().z, 0.01, L"z pos must not be modified");
      }

      /// Tests colliding physics body with a triangle edge that is a bit off, so that the sphere
      /// collides with the triangle and a sliding plane is used to displace the plane "up".
      TEST_METHOD(TestCollisionTriangleEdgeWithSlidingPlane)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 1.5, 10.0, -1.0 }},
               Vertex3d{Vector3d{ 1.5, -10.0, -1.0 }},
               Vertex3d{Vector3d{ 4.0, 0.0, 5.0 }}} // last point is moved "up" a bit
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Assert::IsTrue(body.GetPosition().x > 0.5 && body.GetPosition().x < 1.0,
            L"body x pos must be beyond the 0.5 point, but not the whole velocity vector");
         Assert::AreEqual(0.0, body.GetPosition().y, 0.01, L"y pos must not be modified");
         Assert::IsTrue(body.GetPosition().z > 0.0, L"z pos must be 'up' a bit");
      }

      /// Tests special case: triangles that build a corner with 90 degrees walls; the sphere must
      /// stop directly in the corner when the velocity points to the corner point.
      TEST_METHOD(TestCollisionCorner90DegreesWalls)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 3.0, 3.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 2.0, 2.0, -2.0 }},
               Vertex3d{Vector3d{ 2.0, -2.0, -2.0 }},
               Vertex3d{Vector3d{ 2.0, 2.0, 10.0 }}
            },
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ -2.0, 2.0, -2.0 }},
               Vertex3d{Vector3d{ 2.0, 2.0, -2.0 }},
               Vertex3d{Vector3d{ 2.0, 2.0, 10.0 }}
            }
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Vector3d expectedPos{ 1.0, 1.0, 0.0 };
         Assert::AreEqual(0.0, (body.GetPosition() - expectedPos).Length(), 0.01,
            L"vector position must be equal the expected position");
      }

      /// Tests special case: triangles that build a corner with 90 degrees walls and the sphere
      /// the sphere's velocity doesn't point directly to the corner point.
      TEST_METHOD(TestCollisionCorner90DegreesWallsWithSliding)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 3.0, 3.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 2.0, 3.0, -2.0 }},
               Vertex3d{Vector3d{ 2.0, -2.0, -2.0 }},
               Vertex3d{Vector3d{ 2.0, 3.0, 10.0 }}
            },
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ -2.0, 3.0, -2.0 }},
               Vertex3d{Vector3d{ 2.0, 3.0, -2.0 }},
               Vertex3d{Vector3d{ 2.0, 3.0, 10.0 }}
            }
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Vector3d expectedPos{ 1.0, 2.0, 0.0 };
         Assert::AreEqual(0.0, (body.GetPosition() - expectedPos).Length(), 0.01,
            L"vector position must be equal the expected position");
      }

      /// Tests special case: corner with pointed angle walls; the sphere must stop with touching
      /// both triangles.
      TEST_METHOD(TestCollisionCornerPointedAngleWalls)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 3.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 3.0, 0.0, -2.0 }},
               Vertex3d{Vector3d{ 0.0, -2.0, -2.0 }},
               Vertex3d{Vector3d{ 3.0, 0.0, 10.0 }}
            },
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 0.0, 2.0, -2.0 }},
               Vertex3d{Vector3d{ 3.0, 0.0, -2.0 }},
               Vertex3d{Vector3d{ 3.0, 0.0, 10.0 }}
            }
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Vector3d expectedPos{ 1.2, 0.0, 0.0 };
         Assert::AreEqual(0.0, (body.GetPosition() - expectedPos).Length(), 0.01,
            L"vector position must be equal the expected position");
      }

      /// Tests special case: corner with pointed angle walls; the sphere must stop with touching
      /// both triangles, but since the upper triangle is displaced a bit, the spehre must travel
      /// up a bit on a sliding plane.
      TEST_METHOD(TestCollisionCornerPointedAngleWallsAndSlidingPlane)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 3.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 3.0, 0.0, -2.0 }},
               Vertex3d{Vector3d{ 0.0, -2.0, -2.0 }},
               Vertex3d{Vector3d{ 3.0, 0.0, 10.0 }}
            },
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 0.0, 3.0, -2.0 }},
               Vertex3d{Vector3d{ 3.0, 1.0, -2.0 }},
               Vertex3d{Vector3d{ 3.0, 1.0, 10.0 }}
            }
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Assert::IsTrue(body.GetPosition().y > 0.0, L"y pos must be 'up' a bit");
         Assert::AreEqual(0.0, body.GetPosition().z, 0.01, L"z pos must not be modified");
      }

      /// Tests special case: triangle is parallel to velocity; no collision, just moving along
      /// the triangle without a sliding plane.
      TEST_METHOD(TestCollisionTriangleParallelToVelocity)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ -2.0, -1.0, -10.0 }},
               Vertex3d{Vector3d{ -2.0, -1.0, 10.0 }},
               Vertex3d{Vector3d{ 10.0, -1.0, 0.0 }}}
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Vector3d expectedPos{ 1.0, 0.0, 0.0 };
         Assert::AreEqual(0.0, (body.GetPosition() - expectedPos).Length(), 0.01,
            L"vector position must be equal the expected position");
      }

      /// Tests special case: triangle is almost parallel to velocity; sliding along the triangle.
      TEST_METHOD(TestCollisionTriangleAlmostParallelToVelocity)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ -2.0, -1.0, -10.0 }},
               Vertex3d{Vector3d{ -2.0, -1.0, 10.0 }},
               Vertex3d{Vector3d{ 10.0, -0.9, 0.0 }}} // a little up from y = 1.0, creating an angle
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Vector3d expectedPos{ 1.0, 0.0, 0.0 };
         // note: the error is so minimal that it's below 0.01
         Assert::AreEqual(0.0, (body.GetPosition() - expectedPos).Length(), 0.01,
            L"vector position must be equal the expected position");
      }

      /// Tests special case: sphere that is embedded into triangle; doesn't detect collision.
      TEST_METHOD(TestCollisionTriangleAlreadyEmbeddedIntoSphere)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ -2.0, -0.4, -10.0 }},
               Vertex3d{Vector3d{ -2.0, -0.4, 10.0 }},
               Vertex3d{Vector3d{ 10.0, -0.4, 0.0 }}}
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Vector3d expectedPos{ 1.0, 0.0, 0.0 };
         Assert::AreEqual(0.0, (body.GetPosition() - expectedPos).Length(), 0.01,
            L"vector position must be equal the expected position");
      }

      /// Tests special case: sphere that is currently half embedded in a triangle and leaves it
      /// when following velocity; no collision should be detected.
      TEST_METHOD(TestCollisionTriangleEmbeddedAndLeavesSphere)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 0.5, -0.5, -10.0 }},
               Vertex3d{Vector3d{ 0.5, -0.5, 10.0 }},
               Vertex3d{Vector3d{ -10.0, -0.5, 0.0 }}}
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Vector3d expectedPos{ 1.0, 0.0, 0.0 };
         Assert::AreEqual(0.0, (body.GetPosition() - expectedPos).Length(), 0.01,
            L"vector position must be equal the expected position");
      }

      /// Tests special case: sphere is currently embedded and will hit an edge when following the
      /// velocity vector; sphere is stopped at the edge.
      TEST_METHOD(TestCollisionTriangleEmbeddedAndHitsEdge)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 1.5, -0.5, -10.0 }},
               Vertex3d{Vector3d{ 1.5, -0.5, 10.0 }},
               Vertex3d{Vector3d{ -10.0, -0.5, 0.0 }}}
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Vector3d expectedPos{ 0.62, 0.0, 0.0 };
         Assert::AreEqual(0.0, (body.GetPosition() - expectedPos).Length(), 0.01,
            L"vector position must be equal the expected position");
      }


      /// Tests colliding with two triangles, and the first is further away from the sphere. The
      /// nearer triangle should be used to stop the sphere.
      TEST_METHOD(TestCollisionTwoTrianglesWithDifferentDistances)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 3.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 3.0, 2.0, -2.0 }},
               Vertex3d{Vector3d{ 3.0, -2.0, -2.0 }},
               Vertex3d{Vector3d{ 3.0, 0.0, 2.0 }}
            },
            Triangle3dTextured{0,
               Vertex3d{Vector3d{ 2.0, 2.0, -2.0 }},
               Vertex3d{Vector3d{ 2.0, -2.0, -2.0 }},
               Vertex3d{Vector3d{ 2.0, 0.0, 2.0 }}
            }
         }, body };

         // run
         detection.TrackObject(body);

         // check
         Vector3d expectedPos{ 1.0, 0.0, 0.0 };
         Assert::AreEqual(0.0, (body.GetPosition() - expectedPos).Length(), 0.01,
            L"vector position must be equal the expected position");
      }

      // test is currently ignored, since collision detection gets stuck on some edge
      BEGIN_TEST_METHOD_ATTRIBUTE(TestCollisionWalkingUpAStep)
         TEST_IGNORE()
      END_TEST_METHOD_ATTRIBUTE()

      /// Tests simulating walking up a stair, in multiple time steps
      TEST_METHOD(TestCollisionWalkingUpAStep)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         const double stepHeight = 0.2;
         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0, // triangle where sphere is resting on
               Vertex3d{Vector3d{ 1.5, -1.0, -10.0 }},
               Vertex3d{Vector3d{ 1.5, -1.0, 10.0 }},
               Vertex3d{Vector3d{ -10.0, -1.0, 0.0 }}
            },
            Triangle3dTextured{0, // triangle that builds the stair
               Vertex3d{Vector3d{ 1.5, -1.0 + stepHeight, 10.0 }},
               Vertex3d{Vector3d{ 1.5, -1.0 + stepHeight, -10.0 }},
               Vertex3d{Vector3d{ 1.5, -10.0, 0.0 }}
            },
            Triangle3dTextured{0, // triangle for the elevated floor
               Vertex3d{Vector3d{ 1.5, -1.0 + stepHeight, -10.0 }},
               Vertex3d{Vector3d{ 1.5, -1.0 + stepHeight, 10.0 }},
               Vertex3d{Vector3d{ 10.0, -1.0 + stepHeight, 0.0 }}
            }
         }, body };

         // run until 10 iterations were done, or the sphere has climbed the stair
         unsigned int iteration = 0;
         while (iteration++ < 10 && body.GetPosition().x < 2.5)
            detection.TrackObject(body);

         // check
         Assert::IsTrue(iteration < 10, L"stair must have been climbed with less than 10 iterations");

         Assert::IsTrue(body.GetPosition().x >= 2.5, L"stair must have been climbed successfully");
         Assert::IsTrue(body.GetPosition().y >= stepHeight, L"sphere must have reached step level");
      }

      /// Tests sspecial case: step that's too large to climb
      TEST_METHOD(TestCollisionWalkingUpAStepTooLargeToClimb)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, 0.0, 0.0 };
         TestPhysicsBody body{ initialPos, velocity };

         const double stepHeight = 1.1; // must be more than the sphere radius
         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0, // triangle where sphere is resting on
               Vertex3d{Vector3d{ 1.5, -1.0, -10.0 }},
               Vertex3d{Vector3d{ 1.5, -1.0, 10.0 }},
               Vertex3d{Vector3d{ -10.0, -1.0, 0.0 }}
            },
            Triangle3dTextured{0, // triangle that builds the stair
               Vertex3d{Vector3d{ 1.5, -1.0 + stepHeight, 10.0 }},
               Vertex3d{Vector3d{ 1.5, -1.0 + stepHeight, -10.0 }},
               Vertex3d{Vector3d{ 1.5, -10.0, 0.0 }}
            },
            Triangle3dTextured{0, // triangle for the elevated floor
               Vertex3d{Vector3d{ 1.5, -1.0 + stepHeight, -10.0 }},
               Vertex3d{Vector3d{ 1.5, -1.0 + stepHeight, 10.0 }},
               Vertex3d{Vector3d{ 10.0, -1.0 + stepHeight, 0.0 }}
            }
         }, body };

         // run until 10 iterations were done, or the sphere has climbed the stair
         unsigned int iteration = 0;
         while (iteration++ < 10 && body.GetPosition().x < 2.5)
            detection.TrackObject(body);

         // check
         Assert::AreEqual(11u, iteration, L"loop must have been left after 10 iterations");

         Assert::IsTrue(body.GetPosition().x < 2.5, L"stair must not have been climbed successfully");
      }

      // test is currently ignored, since collision detection gets stuck on some edge
      BEGIN_TEST_METHOD_ATTRIBUTE(TestCollisionWalkingDownAStep)
         TEST_IGNORE()
      END_TEST_METHOD_ATTRIBUTE()

      /// Tests simulating walking down a stair, in multiple time steps
      TEST_METHOD(TestCollisionWalkingDownAStep)
      {
         // set up
         Vector3d initialPos{ 0.0, 0.0, 0.0 };
         Vector3d velocity{ 1.0, -0.5, 0.0 }; // v has y component also
         TestPhysicsBody body{ initialPos, velocity };

         const double stepHeight = 0.6;
         CollisionDetection detection{ std::vector<Triangle3dTextured> {
            Triangle3dTextured{0, // triangle where sphere is resting on
               Vertex3d{Vector3d{ 1.5, -1.0, -10.0 }},
               Vertex3d{Vector3d{ 1.5, -1.0, 10.0 }},
               Vertex3d{Vector3d{ -10.0, -1.0, 0.0 }}
            },
            Triangle3dTextured{0, // triangle that builds the stair
               Vertex3d{Vector3d{ 1.5, -1.0 - stepHeight, 10.0 }},
               Vertex3d{Vector3d{ 1.5, -1.0 - stepHeight, -10.0 }},
               Vertex3d{Vector3d{ 1.5, -10.0, 0.0 }}
            },
            Triangle3dTextured{0, // triangle for the elevated floor
               Vertex3d{Vector3d{ 1.5, -1.0 - stepHeight, -10.0 }},
               Vertex3d{Vector3d{ 1.5, -1.0 - stepHeight, 10.0 }},
               Vertex3d{Vector3d{ 10.0, -1.0 - stepHeight, 0.0 }}
            }
         }, body };

         // run until 10 iterations were done, or the sphere has climbed down the stair
         unsigned int iteration = 0;
         while (iteration++ < 10 && body.GetPosition().y < stepHeight)
            detection.TrackObject(body);

         // check
         Assert::IsTrue(iteration < 10, L"stair must have been climbed with less than 10 iterations");

         Assert::IsTrue(body.GetPosition().x >= 2.5, L"stair must have been climbed down successfully");
         Assert::IsTrue(body.GetPosition().y <= -stepHeight, L"sphere must have reached step level");
      }
   };
} // namespace UnitTest
