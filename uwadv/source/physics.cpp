/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

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
/*! \file physics.cpp

   \brief game physics implementation

   used collision detection and response code from this tutorial:
   http://www.peroxide.dk/download/tutorials/tut10/pxdtut10.html

*/

// needed includes
#include "common.hpp"
#include "physics.hpp"
#include "underworld.hpp"
#include "renderer.hpp"

// typedefs

//! collision check data
typedef struct ua_collision_data
{
   ua_vector3d pos;
   ua_vector3d dir;
   ua_vector3d nearest_poly_inter;

   double nearest_dist;
   bool found;
   bool stuck;

} ua_collision_data;


// constants



//! Trick to get smoother CD
const double ua_cd_liftoff   = 0.07;

//! minimum distance
const double ua_physics_min_dist = 0.05;

//! max speed a player can walk, in tiles / second
const double ua_player_max_walk_speed = 2.4;

//! max speed a player can slide left or right
const double ua_player_max_slide_speed = 1.0;

//! max speed a player can rotate, in degrees / second
const double ua_player_max_rotate_speed = 90;

//! max speed a player can rotate the view, in degrees / second
const double ua_player_max_view_rotate_speed = 60;


// helper function prototypes

double ua_physics_intersect_ray_plane(const ua_vector3d& r_orig,
   const ua_vector3d& r_vector, const ua_vector3d& p_orig, const ua_vector3d& p_normal);

double ua_physics_intersect_ray_sphere(const ua_vector3d& r_orig,
   const ua_vector3d& r_vector, const ua_vector3d& s_orig, double s_rad);

bool ua_physics_check_point_in_triangle(const ua_vector3d& point,
   const ua_vector3d& a, const ua_vector3d& b, const ua_vector3d& c);

ua_vector3d ua_physics_closest_point_on_triangle(const ua_vector3d& a,
   const ua_vector3d& b, const ua_vector3d& c, const ua_vector3d& p);

bool ua_physics_insersect_sphere_triangle(ua_vector3d sO, float sR, 
   ua_vector3d a, ua_vector3d b, ua_vector3d c, ua_vector3d& ip);

double ua_physics_dist_point_plane(ua_vector3d point, ua_vector3d pO, ua_vector3d pN);


// ua_physics_model methods

ua_physics_model::ua_physics_model()
:last_evaltime(-0.1)
{
}

void ua_physics_model::init(ua_underworld *uw)
{
   underw = uw;
}

void ua_physics_model::eval_physics(double time)
{
   // evaluate player movement
   {
      ua_player &pl = underw->get_player();

      unsigned int mode = underw->get_player().get_movement_mode();

      if ((mode & ua_move_walk) || (mode & ua_move_slide))
      {
         double speed = ua_player_max_walk_speed*(time-last_evaltime);
         double angle = underw->get_player().get_angle_rot();

         // adjust angle for sliding
         if (mode & ua_move_slide)
         {
            angle -= 90.0;
            speed *= pl.get_movement_factor(ua_move_slide);
            speed *= ua_player_max_slide_speed/ua_player_max_walk_speed;
         }
         else
            speed *= pl.get_movement_factor(ua_move_walk);


         // construct direction vector
         ua_vector3d dir(1.0, 0.0, 0.0);
         dir.rotate_z(angle);
         dir *= speed;

         // position vector
         ua_vector3d pos(pl.get_xpos(),pl.get_ypos(),pl.get_height());
         ua_vector3d ellipsoid = pl.get_ellipsoid();
         pos.z += (ellipsoid.z + ua_cd_liftoff);


         // do object tracking; pos is modified after call
         track_object(pl, pos,dir, false, time);

         // limit height when falling out of the map
         if (pos.z<0.0) pos.z = 0.0;

         // set new player pos
         pl.set_pos(pos.x,pos.y);
         pl.set_height(pos.z - (ellipsoid.z + ua_cd_liftoff));
      }

      // player rotation
      if (mode & ua_move_rotate)
      {
         double angle = ua_player_max_rotate_speed * (time-last_evaltime) *
            pl.get_movement_factor(ua_move_rotate);
         angle += pl.get_angle_rot();

         // keep angle in range [0; 360]
         while(angle>360.0) angle -= 360.0;
         while(angle<0.0) angle += 360.0;

         pl.set_angle_rot(angle);
      }

      // view up/down
      if (mode & ua_move_lookup)
      {
         double viewangle = pl.get_angle_pan();
         viewangle -= ua_player_max_view_rotate_speed * (time-last_evaltime) *
            pl.get_movement_factor(ua_move_lookup);

         double maxangle = underw->have_enhanced_features() ? 45.0 : 75.0;

         // restrict up-down view angle
         if (viewangle < -maxangle) viewangle = -maxangle;
         if (viewangle > maxangle) viewangle = maxangle;

         pl.set_angle_pan(viewangle);
      }


     // If we use the new CD we should apply gravity here nomatter if any keys were pressed..
     // construct direction vector
     ua_vector3d dir = pl.get_fall_velocity();

     // position vector
     ua_vector3d pos(pl.get_xpos(),pl.get_ypos(),pl.get_height());

     ua_vector3d ellipsoid = pl.get_ellipsoid();
     pos.z += (ellipsoid.z + ua_cd_liftoff);


     // do object tracking; pos is modified after call
     track_object(pl, pos,dir, true, time);

     // limit height when falling out of the map
     if (pos.z<0.0) pos.z = 0.0;

     // set new player pos
     pl.set_pos(pos.x,pos.y);
     pl.set_height(pos.z - (ellipsoid.z + ua_cd_liftoff));
   }

   last_evaltime = time;
}

void ua_physics_model::track_object(ua_physics_object& object, ua_vector3d& pos, const ua_vector3d& dir, bool gravity_call, double time)
{
   ua_vector3d dir2(dir);
   // transform to ellipsoid space
   ua_vector3d ellipsoid = object.get_ellipsoid();
   pos /= ellipsoid;
   dir2 /= ellipsoid;

   // call recursive collision response function
   collision_recursion_depth = 0;
   bool collided = collide_with_world(object, pos,dir2);

   if (gravity_call) {
     if (collided) {
      object.reset_fall_velocity();
     } 
     else {
      object.accellerate_fall_velocity(time);
     } 
   }

   // transform back to normal space
   pos *= ellipsoid;
}

bool ua_physics_model::collide_with_world(ua_physics_object& object, ua_vector3d& pos, const ua_vector3d& dir)
{
   // do we need to worry?
   if (dir.length()<ua_physics_min_dist || collision_recursion_depth > 15)
      return true;

   collision_recursion_depth++;

   ua_vector3d dest(pos);
   dest += dir;

   ua_collision_data data;

   // reset the collision package we send to the mesh
   data.pos = pos;
   data.dir = dir;
   data.nearest_dist = -1.0;
   data.found = false;
   data.stuck = false;

   // check triangle mesh collision
   ua_vector3d ellipsoid = object.get_ellipsoid();
   check_collision( object, static_cast<int>(pos.x*ellipsoid.x), static_cast<int>(pos.y*ellipsoid.y), data);
  
   if (data.stuck) {
     // recovery code here:
     ua_vector3d safeSpot = object.pop_safe_spot();
     pos = safeSpot;
     return true; // if stuck we definitly collides
   }
  
   if (!data.found)
   {
      // no collision occured
 
      // move very close to the desired destination
      ua_vector3d dir2(dir);
      dir2.normalize();
      dir2 *= dir.length()-ua_physics_min_dist;

      // push 'pos' as a safe spot:
      object.push_safe_spot(pos);
      
      // return the final position
      pos += dir2;
      return false; // no collision found
   }
   else
   {
      // there was a collision

      // move close to where we hit something
      ua_vector3d newpos(data.pos);

      // only update if we are not already very close
      if (data.nearest_dist >= ua_physics_min_dist)
      {
         ua_vector3d dir2(dir);
         dir2.normalize();
         dir2 *= data.nearest_dist-ua_physics_min_dist;

         newpos += dir2;
      }

      // calculate sliding plane
      ua_vector3d slideplane_origin = data.nearest_poly_inter;
      ua_vector3d slideplane_normal = newpos - data.nearest_poly_inter;

      // project destination point onto the sliding plane

      double len = ua_physics_intersect_ray_plane(dest,
         slideplane_normal, slideplane_origin, slideplane_normal);

      // calculate new destination point on the sliding plane
      ua_vector3d newdest(
         dest.x + len * slideplane_normal.x,
         dest.y + len * slideplane_normal.y,
         dest.z + len * slideplane_normal.z);

      // generate slide vector
      // it becomes our new velocity vector for the next iteration
      ua_vector3d newdir = newdest - data.nearest_poly_inter;
      
      // call the function with the new position and velocity
      collide_with_world(object, newpos,newdir);
      pos = newpos;
      return true; // there was a collision here
   }
}

void ua_physics_model::check_collision(ua_physics_object& object, int xpos, int ypos, ua_collision_data& data)
{
   // retrieve all tile triangles to check
   std::vector<ua_triangle3d_textured> alltriangles;
   {
      for(int i=-1; i<2; i++)
      for(int j=-1; j<2; j++)
         ua_renderer::get_tile_triangles(underw->get_current_level(),
            unsigned(xpos+i),unsigned(ypos+j),alltriangles);
   }

   // also retrieve 3d model triangles
   {
      ua_object_list& objlist = underw->get_current_level().get_mapobjects();

      // get first object link
      Uint16 link = objlist.get_tile_list_start(xpos,ypos);

      while(link != 0)
      {
         ua_object& obj = objlist.get_object(link);
         ua_object_info_ext& extobjinfo = obj.get_ext_object_info();

         ua_vector3d base(
            xpos+(extobjinfo.xpos+0.5)/8.0, ypos+(extobjinfo.ypos+0.5)/8.0,
            extobjinfo.zpos/4.0);

         // get triangles from model manager
         ua_model3d_manager::cur_modelmgr->get_bounding_triangles(
            obj.get_object_info().item_id,base,alltriangles);

         // next object in link chain
         link = obj.get_object_info().link;
      }
   }

   // keep a copy of this as it's needed a few times
   ua_vector3d normdir(data.dir);
   normdir.normalize();
   
   ua_vector3d ellipsoid = object.get_ellipsoid();

   // check all triangles
   unsigned int max = alltriangles.size();
   for(unsigned int i=0; i<max; i++)
   {
      ua_triangle3d_textured& tri = alltriangles[i];

      // get triangle points and convert to ellipsoid space
      ua_vector3d p1(tri.points[0]), p2(tri.points[1]), p3(tri.points[2]);
      p1 /= ellipsoid;
      p2 /= ellipsoid;
      p3 /= ellipsoid;

      // construct plane containing this triangle
      ua_vector3d p_origin(p1), v1(p2), v2(p3);
      v1 -= p1;
      v2 -= p1;

      // determine normal to plane containing polygon
      ua_vector3d p_normal;
      p_normal.cross(v1,v2);
      p_normal.normalize();
           
        
      // calculate sphere intersection point
#ifdef CD_DEBUG_INFO       
      printf("sphere pos (%f,%f,%f)\n", data.pos.x, data.pos.y,data.pos.z); 
#endif      
      ua_vector3d sphere_inter(data.pos);
      sphere_inter -= p_normal;

      // classify point to determine if ellipsoid spans the plane
      bool backside = false;
      {
         ua_vector3d dir(p_origin);
         dir -= sphere_inter;
         double d = dir.dot(p_normal);
         backside = d>0.001;
      }

      // find the plane intersection point
      double dist_plane_inter; // distToPlaneIntersection
      ua_vector3d plane_inter;

      if (backside)
      {
         // plane is embedded in ellipsoid

         // find plane intersection point by shooting a ray from the
         // sphere intersection point along the planes normal
         dist_plane_inter = ua_physics_intersect_ray_plane(
            sphere_inter, p_normal, p_origin, p_normal);

         // calculate plane intersection point
         plane_inter.set(sphere_inter.x + dist_plane_inter * p_normal.x,
                         sphere_inter.y + dist_plane_inter * p_normal.y,
                         sphere_inter.z + dist_plane_inter * p_normal.z);
      }
      else
      {
         // shoot ray along the velocity vector
         dist_plane_inter = ua_physics_intersect_ray_plane(sphere_inter, normdir, p_origin, p_normal);
         
         // calculate plane intersection point
         plane_inter.set(sphere_inter.x + dist_plane_inter * normdir.x,
                         sphere_inter.y + dist_plane_inter * normdir.y,
                         sphere_inter.z + dist_plane_inter * normdir.z);
      }

      // find polygon intersection point; by default we assume its equal to the
      // plane intersection point
      ua_vector3d poly_inter(plane_inter);

      double dist_ell_inter = dist_plane_inter;

#ifdef CD_DEBUG_INFO 
      printf("sphere (%f,%f,%f) - normDir (%f,%f,%f)\n",sphere_inter.x,sphere_inter.y,sphere_inter.z,normdir.x, normdir.y, normdir.z); 
      printf("planeO (%f,%f,%f) - planeNorm (%f,%f,%f)\n", p_origin.x,p_origin.y,p_origin.z, p_normal.x,p_normal.y,p_normal.z);
      printf("triangle (%f,%f,%f) - (%f,%f,%f) - (%f,%f,%f)\n", p1.x,p1.y,p1.z, p2.x,p2.y,p2.z, p3.x, p3.y, p3.z);
#endif

      if (!ua_physics_check_point_in_triangle(plane_inter,p1,p2,p3))
      {
         // Not in triangle - must collide with an edge.. the hard case:
         
         dist_ell_inter = -1.0f; // we assume no hit then..
         int numSubdivisions = 25; 
         
         ua_vector3d moveDist = data.dir;
         moveDist.normalize();
         moveDist *= (data.dir.length() / numSubdivisions);
         ua_vector3d testPos(data.pos);
         int t = 0;
         ua_vector3d ip; // intersection point
         
         
         bool foundCol = false;
         while (t < numSubdivisions && !foundCol) {
            // move sphere a little forward and check for intersection
            testPos = data.pos + (moveDist*t);
           
            if (fabs(ua_physics_dist_point_plane(testPos, p1, p_normal)) <= 1.0f) {
               if (ua_physics_insersect_sphere_triangle(testPos, 1.0f, p1, p2, p3, ip)) {
                 
                 // if t==0 we must be stuck
                 if (t == 0) {
                    data.stuck = true;
                    return;                   
                 }                
                                  
                 dist_ell_inter = (t-1) * moveDist.length();
                 poly_inter = ip;
                 
                 float d = ua_physics_intersect_ray_sphere(poly_inter, -normdir, data.pos, 1.0f);
                 sphere_inter = ip - normdir*dist_ell_inter;
                 foundCol = true;
              }
            }
            t++;
         }
         
         // not in triangle
         /*
         poly_inter = ua_physics_closest_point_on_triangle(p1,p2,p3,plane_inter);
         normdir *= -1;
         dist_ell_inter = ua_physics_intersect_ray_sphere(poly_inter, normdir, data.pos, 1.0);

         if (dist_ell_inter > 0)
         {
      
            // calculate true sphere intersection point
            sphere_inter.set(
               poly_inter.x + dist_ell_inter * normdir.x,
               poly_inter.y + dist_ell_inter * normdir.y,
               poly_inter.z + dist_ell_inter * normdir.z);
         }

         normdir *= -1;
#ifdef CD_DEBUG_INFO
         printf("not in triangle..\n");           
         printf("backray hit sphere at distance %f\n", dist_ell_inter);
#endif
      */
      }
      else {
#ifdef CD_DEBUG_INFO         
        printf("in triangle..\n"); 
#endif        
      }

      // here we do the error checking to see if we got ourself stuck last frame
//      if (CheckPointInSphere(polyIPoint, source, 1.0f)) 
//         colPackage->stuck = TRUE;

      // ok, now we might update the collision data if we hit something
#ifdef CD_DEBUG_INFO       
      printf("dist to ell %f  - velocity length %f\n", dist_ell_inter, data.dir.length());
#endif      
      if (dist_ell_inter > 0 && dist_ell_inter <= data.dir.length() )
      {
         if (!data.found || dist_ell_inter < data.nearest_dist)
         {
#ifdef CD_DEBUG_INFO           
            printf("CD accepted..\n");
#endif            
            // if we are hit we have a closest hit so far; we save the information
            data.nearest_dist = dist_ell_inter;
            data.nearest_poly_inter = poly_inter;
            data.found = true;
         }
      }

   } // end for loop
}


// helper math functions

// ray intersections. All return -1.0 if no intersection, otherwise the distance along the
// ray where the (first) intersection takes place

double ua_physics_intersect_ray_plane(const ua_vector3d& r_orig,
   const ua_vector3d& r_vector, const ua_vector3d& p_orig, const ua_vector3d& p_normal)
{
   double d = -p_normal.dot(p_orig);

   double numer = p_normal.dot(r_orig) + d;
   double denom = p_normal.dot(r_vector);

   if (denom == 0)
      return -1.0; // normal is orthogonal to vector, can't intersect

   return -numer / denom;
}

double ua_physics_intersect_ray_sphere(const ua_vector3d& r_orig,
   const ua_vector3d& r_vector, const ua_vector3d& s_orig, double s_rad)
{
   ua_vector3d q(s_orig);
   q -= r_orig;

   double c = q.length();
   double v = q.dot(r_vector);
   double d = s_rad*s_rad - (c*c - v*v);

   // if there was no intersection, return -1
   if (d < 0.0)
      return -1.0;

   // return distance to the [first] intersecting point
   return (v - sqrt(d));
}


bool ua_physics_insersect_sphere_triangle(ua_vector3d sO, float sR, ua_vector3d a, ua_vector3d b, ua_vector3d c, ua_vector3d& ip) {

  // Check the 3 edges:

  // A -> B
  ua_vector3d rayDir = b - a;
  float edgeLength = rayDir.length();
  rayDir.normalize();
  float dist = ua_physics_intersect_ray_sphere(a,rayDir, sO, sR);
  if (dist > 0 && dist <= edgeLength) {
    ip = a + rayDir*dist;
    return true;
  }

  // B -> C
  rayDir = c - b;
  edgeLength = rayDir.length();
  rayDir.normalize();
  dist = ua_physics_intersect_ray_sphere(b,rayDir, sO, sR);
  if (dist > 0 && dist <= edgeLength) {
    ip = b + rayDir*dist;
    return true;
  }

  // C -> A
  rayDir = a - c;
  edgeLength = rayDir.length();
  rayDir.normalize();
  dist = ua_physics_intersect_ray_sphere(c,rayDir, sO, sR);
  if (dist > 0 && dist <= edgeLength) {
    ip = c + rayDir*dist;
    return true;
  }

  return false;
}



typedef unsigned int uint32;
#define in(a) ((uint32&) a)
bool ua_physics_check_point_in_triangle(const ua_vector3d& point, const ua_vector3d& pa, const ua_vector3d& pb, const ua_vector3d& pc) {

  ua_vector3d e10=pb-pa;
  ua_vector3d e20=pc-pa;
    
  float a,b,c,ac_bb;

  a = e10.dot(e10);
  b = e10.dot(e20);
  c = e20.dot(e20);
  ac_bb=(a*c)-(b*b);

  ua_vector3d vp = ua_vector3d(point.x-pa.x, point.y-pa.y, point.z-pa.z);

  float d = vp.dot(e10);
  float e = vp.dot(e20);

  float x = (d*c)-(e*b);
  float y = (e*a)-(d*b);
  float z = x+y-ac_bb;

  return (( in(z)& ~(in(x)|in(y)) ) & 0x80000000)!=0;
}

double ua_physics_dist_point_plane(ua_vector3d point, ua_vector3d pO, ua_vector3d pN) {
  ua_vector3d dir = pO - point;
  return (dir.dot(pN));
}


void ua_physics_closest_point_on_line(ua_vector3d& point,
   const ua_vector3d& a, const ua_vector3d& b, const ua_vector3d& p)
{
   // determine t (the length of the vector from ‘a’ to ‘p’)
   ua_vector3d c(p); c -= a;
   ua_vector3d v(b); v -= a;

   double d = v.length();
   v.normalize();

   double t = v.dot(c);

   // check to see if ‘t’ is beyond the extents of the line segment
   if (t < 0.0)
   {
      point = a;
      return;
   }

   if (t > d)
   {
      point = b;
      return;
   }

   // return the point between ‘a’ and ‘b’
   // set length of V to t. V is normalized so this is easy
   v *= t;
   point = a;
   point += v;
}

double ua_physics_point_dist(ua_vector3d a, const ua_vector3d& b)
{
   a-=b;
   return a.length();
}

ua_vector3d ua_physics_closest_point_on_triangle(const ua_vector3d& a,
   const ua_vector3d& b, const ua_vector3d& c, const ua_vector3d& p)
{
   ua_vector3d Rab, Rbc, Rca;

   ua_physics_closest_point_on_line(Rab,a,b,p);
   ua_physics_closest_point_on_line(Rbc,b,c,p);
   ua_physics_closest_point_on_line(Rca,c,a,p);

   double dAB = ua_physics_point_dist(Rab,p);
   double dBC = ua_physics_point_dist(Rbc,p);
   double dCA = ua_physics_point_dist(Rca,p);

   double min = dAB;
   ua_vector3d result(Rab);

   if (dBC < min)
   {
      min = dBC;
      result = Rbc;
   }

   if (dCA < min)
      result = Rca;

   return result;
}
