/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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

   adapted collision detection and response tutorial for 2D space from this
   tutorial:
   http://www.peroxide.dk/download/tutorials/tut10/pxdtut10.html

   in 2D, we don't need a transformation from/to ellipsoid space; the player
   sphere really is a circle, and the triangle plane is just a line.

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
#ifdef HAVE_NEW_CD

   ua_vector3d pos;
   ua_vector3d dir;
   ua_vector3d nearest_poly_inter;

   double nearest_dist;
   bool found;
   bool stuck;

#else // HAVE_NEW_CD

   ua_vector2d pos;
   ua_vector2d dir;
   ua_vector2d line_inter;
   ua_vector2d circle_inter; // seems to be not needed
   double nearest_dist;

#endif // HAVE_NEW_CD

} ua_collision_data;


// constants



//! Trick to get smoother CD
const double ua_cd_liftoff   = 0.05;

//! minimum distance
const double ua_physics_min_dist = 0.05;

//! player circle radius
const double ua_player_radius = 0.4;

//! max height the player can step
const double ua_physics_max_step_height = 2.1;

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

         
#ifdef HAVE_NEW_CD

         // construct direction vector
         ua_vector3d dir(1.0, 0.0, 0.0);
         dir.rotate_z(angle);
         dir *= speed;

         // position vector
         ua_vector3d pos(pl.get_xpos(),pl.get_ypos(),pl.get_height()*0.125);
         ua_vector3d ellipsoid = pl.get_ellipsoid();
         pos.z += (ellipsoid.z + ua_cd_liftoff);
              

         // do object tracking; pos is modified after call
         track_object(pl, pos,dir, false, time);

         // limit height when falling out of the map
         if (pos.z<0.0) pos.z = 0.0;

         // set new player pos
         pl.set_pos(pos.x,pos.y);
         pl.set_height((pos.z - (ellipsoid.z + ua_cd_liftoff)) / 0.125);
         
#else // HAVE_NEW_CD

         ua_vector2d dir;
         dir.set_polar(speed,angle);
         ua_vector2d pos(pl.get_xpos(),pl.get_ypos());
         
         // Perform collision detection:
         calc_collision(pos,dir);

         // set new player 2D pos
         pl.set_pos(pos.x,pos.y);

         // Get the map height of the new 2d position:
         double h2 = underw->get_current_level().get_floor_height(pos.x, pos.y);

         double finalHeight = h2;
        
         // cache the player height
         pl.set_height(finalHeight);

#endif // HAVE_NEW_CD
      }

      // player rotation
      if (mode & ua_move_rotate)
      {
         double angle = ua_player_max_rotate_speed * (time-last_evaltime) *
            pl.get_movement_factor(ua_move_rotate);
         pl.set_angle_rot(pl.get_angle_rot()+angle);
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
   
#ifdef HAVE_NEW_CD
     // If we use the new CD we should apply gravity here nomatter if any keys were pressed..  
     // construct direction vector
     ua_vector3d dir = pl.get_fall_velocity();
       
     // position vector
     ua_vector3d pos(pl.get_xpos(),pl.get_ypos(),pl.get_height()*0.125);
    
     ua_vector3d ellipsoid = pl.get_ellipsoid();
     pos.z += (ellipsoid.z + ua_cd_liftoff);
        

     // do object tracking; pos is modified after call
     track_object(pl, pos,dir, true, time);

     // limit height when falling out of the map
     if (pos.z<0.0) pos.z = 0.0;

     // set new player pos
     pl.set_pos(pos.x,pos.y);
     pl.set_height((pos.z - (ellipsoid.z + ua_cd_liftoff)) / 0.125);
   
#endif
   }

   last_evaltime = time;
}

#ifdef HAVE_NEW_CD

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

#else // HAVE_NEW_CD

/*! calculates new position for a given position and direction vector
    all tile walls are checked for collision, and when the player collides
    with a tile wall, we do collision response. the whole function is called
    recursivly then, to check if we now collide with further walls.
*/

void ua_physics_model::calc_collision(ua_vector2d &pos, const ua_vector2d &dir)
{
   if (dir.length()<0.1*ua_physics_min_dist)
      return;

   // prepare info struct
   bool collide = false;
   ua_collision_data data;
   data.pos = pos;
   data.dir = dir;
   data.nearest_dist = -1.0;
  
   // check all lines in all nearby tiles
   for(int tx=-1; tx<2; tx++) for(int ty=-1; ty<2; ty++)
//   int tx=0,ty=0;
   {
      unsigned int tilex = unsigned(pos.x)+tx, tiley = unsigned(pos.y)+ty;
      collide |= check_collision_tile(tilex,tiley,data);
   }

   if (!collide)
   {
      // no collision found; move closer to the destination
      ua_vector2d newdir(dir);
      newdir *= 1.0-ua_physics_min_dist;
      pos+=newdir;

      return;
   }
   else
   {
      // collision detected

      // how far away:
      ua_vector2d newpos(pos);
      ua_vector2d newdir(dir);

      // move close to where we hit something (but not too close! (tm))
      if (data.nearest_dist > ua_physics_min_dist) {
        newdir.normalize();
        newdir *= (data.nearest_dist - ua_physics_min_dist);

        // calc new pos
        newpos += newdir;
      }

      // calc sliding line normal vector
      ua_vector2d sliding;
      sliding.set(data.line_inter,newpos);
      sliding.rotate(90.0);

      // project dir vector onto sliding plane vector
      ua_vector2d proj_dir;
      {
         // project the dir vector onto the line
         // again, assumes that point1 != point2
         double val = sliding.dot(dir) / sliding.length()*sliding.length();

         // new dir vector
         proj_dir = sliding;
         proj_dir *= val;
      }
      newdir = proj_dir;

      // scale new dir a bit (adjust this to get the sliding speed you like! (tm))
      newdir *= 4;


#ifdef HAVE_DEBUG
      static int level=0;

      if (level>10)
         return;

      level++;
#endif
      // check new pos and dir, recursively
      calc_collision(newpos,newdir);

#ifdef HAVE_DEBUG
      level--;
#endif

      pos=newpos;
   }
}

/*! checks for collision with walls for a tile
    all walls are checked, except for the diagonal tiles, where some are
    omitted. all wall "lines" are passed with the right direction, so that
    in check_collision() the normal vector points "into" the tile.
*/
bool ua_physics_model::check_collision_tile(unsigned int tilex,
   unsigned int tiley, ua_collision_data &data)
{
   bool collide = false;

   // check for diagonal tiles
   ua_levelmap_tile &tile = underw->get_current_level().get_tile(tilex,tiley);
   unsigned int checkbits = 1|2|4|8;

   // only check some walls when doing diagonal tiles
   switch(tile.type)
   {
   case ua_tile_diagonal_se: checkbits = 2|8; break;
   case ua_tile_diagonal_sw: checkbits = 2|4; break;
   case ua_tile_diagonal_nw: checkbits = 1|4; break;
   case ua_tile_diagonal_ne: checkbits = 1|8; break;
   }

   // diagonal line collision checks
   switch(tile.type)
   {
   case ua_tile_diagonal_se:
      collide |= check_collision(ua_vector2d(tilex  ,tiley  ),ua_vector2d(tilex+1,tiley+1),data,true);
      break;
   case ua_tile_diagonal_sw:
      collide |= check_collision(ua_vector2d(tilex  ,tiley+1),ua_vector2d(tilex+1,tiley  ),data,true);
      break;
   case ua_tile_diagonal_nw:
      collide |= check_collision(ua_vector2d(tilex+1,tiley+1),ua_vector2d(tilex  ,tiley  ),data,true);
      break;
   case ua_tile_diagonal_ne:
      collide |= check_collision(ua_vector2d(tilex+1,tiley  ),ua_vector2d(tilex  ,tiley+1),data,true);
      break;
   }

   // front, back, left, right line collision check
   if (checkbits&1) collide |= check_collision(ua_vector2d(tilex  ,tiley+1),ua_vector2d(tilex+1,tiley+1),data,true);
   if (checkbits&2) collide |= check_collision(ua_vector2d(tilex+1,tiley  ),ua_vector2d(tilex  ,tiley  ),data,true);
   if (checkbits&4) collide |= check_collision(ua_vector2d(tilex  ,tiley  ),ua_vector2d(tilex  ,tiley+1),data,true);
   if (checkbits&8) collide |= check_collision(ua_vector2d(tilex+1,tiley+1),ua_vector2d(tilex+1,tiley  ),data,true);

   return collide;
}

/*! checks for collision with a line, given by two points.
    the line vector rotated by -90 degree is the normal vector that points
    towards the player.
*/
bool ua_physics_model::check_collision(const ua_vector2d &point1,const ua_vector2d &point2,
   ua_collision_data &data, bool check_height)
{
   // calculate line vector
   ua_vector2d line;
   line.set(point1,point2);

   // calculate normal vector to line (rotated by 90 degrees)
   ua_vector2d norm(-line.y,line.x);
   norm.normalize();
   norm *= -1; // norm faces to the inside of the tile

   // step 0: check if this line has a step that the player cannot climb
   if (check_height)
   {
      // TODO: calculate intersection of player velocity and line and use that point instead of mid-point.
      ua_vector2d mid(line);
      mid *= 0.5;
      mid += point1;

      ua_vector2d testdir(norm);
      testdir *= 0.05;

      // testpt is a point in the tile we wish to climb up to.
      ua_vector2d testpt(mid);
      testpt-=testdir; 

      // get height of player:
      double height1 = underw->get_player().get_height();
      
      // get floor height of step
      ua_level &level = underw->get_current_level();
      double height2 = level.get_floor_height(testpt.x,testpt.y);

      // Is this too big a step up?
      if (height2<height1+ua_physics_max_step_height)
         return false; // no need to test
   }

   // step 1: calculate player circle intersection point
   ua_vector2d circle_inter(norm);
   circle_inter *= -ua_player_radius;
   circle_inter += data.pos;

   // step 2: calculate line intersection point
   ua_vector2d line_inter;
   double dist_to_inters=0.0;
   {
      // check if circle spans the line
      double dist=0.0;
      {
         ua_vector2d pt(circle_inter);
         pt-=point1;

         // calculate distance; assumes that point1 != point2
         double denom = line.x*pt.y - line.y*pt.x;
         dist = -denom/line.length();
      }

      // dist describes the distance and location of the circle intersection
      // point to the line
      // dist > 0 :                       circle in front of line
      // -2*ua_player_radius < dist < 0 : circle spans line
      // dist < -2*ua_player_radius :     circle behind line

      // ignore when behind line
      if (dist<-2*ua_player_radius)
         return false;

      ua_vector2d ray(data.dir);
      if (dist < 0 && dist > -2*ua_player_radius)
      {
         ray = norm;
         ray *= ua_player_radius;
      }

      // find plane intersection point
      ua_vector2d pos1(circle_inter);
      ua_vector2d pos2(pos1);
      pos2+=ray;

      /* we have two lines, g and h (all vars are vectors):

         g: x = point1 + lambda * line
         h: x = circle_inter + mu * ray

         to find the intersection point, we have some restrictions:
         lambda has to stay between 0.0 and 1.0
         mu has to be greater than 0.0

         mu and lambda is calculated via cramer's rule
      */
      double A = line.x;
      double B = line.y;
      double C = pos1.x-pos2.x;
      double D = pos1.y-pos2.y;
      double E = pos1.x-point1.x;
      double F = pos1.y-point1.y;

      double det0 = A*D - B*C;
      double det1 = E*D - C*F;
      double det2 = A*F - B*E;

      if (fabs(det0)<1e-6)
         return false; // lines definitely don't intersect

      double lambda = det1/det0;
      double mu = det2/det0;

      // mu<0 : ray goes into wrong direction
      if (mu<0.0)
         return false;

      line_inter.set(lambda*A + point1.x,lambda*B + point1.y);

      // calculate dist_to_inters
      {
         ua_vector2d temp;
         temp.set(line_inter,circle_inter);
         dist_to_inters = temp.length();

         // check if temp and norm vector point in the opposite direction
         double d = temp.dot(norm);
         if (d<-0.99*dist_to_inters)
            dist_to_inters *= -1.0;
      }

      // step 3: correct line intersection point, if we didn't hit the line
      if (lambda<0.0 || lambda>1.0)
      {
         if (lambda < 0.0) lambda = 0.0;
         if (lambda > 1.0) lambda = 1.0;

         line_inter.set(lambda*A + point1.x,lambda*B + point1.y);

         // step 4: cast ray back to circle, to see if it really hits the circle

         // calculate intersection of ray with circle
         double dist2=0.0;
         {
            ua_vector2d normdir(data.dir);
            normdir.normalize();
            normdir *= -1;

            ua_vector2d q;
            q.set(line_inter,data.pos);
            double c = q.length();
            double v = q.dot(normdir);
            double d = ua_player_radius*ua_player_radius - (c*c - v*v);

            if (d<0.0) return false; // no, doesn't hit
            dist2 = v-sqrt(d);

            // dist2 is the length from circle intersection point to line intersection point
            dist_to_inters = dist2;
         }

         // calculate new circle intersection point
         circle_inter = ray;
         circle_inter.normalize();
         circle_inter *= -dist2;
         circle_inter += line_inter;
      }
   }

   // dont accept collision if dist_to_inters is < 0.. collision is then behind you! (tm)
   if (dist_to_inters<data.dir.length() && dist_to_inters >= 0) 
   {
      if (data.nearest_dist<0.0 || data.nearest_dist > dist_to_inters)
      {
         // closest hit so far
         data.nearest_dist = dist_to_inters;
         data.line_inter = line_inter;
         data.circle_inter = circle_inter;
         return true;
      }
   }

   return false;
}

#endif // HAVE_NEW_CD


#ifdef HAVE_NEW_CD

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

  return (( in(z)& ~(in(x)|in(y)) ) & 0x80000000);
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

#endif // HAVE_NEW_CD
