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


// typedefs

//! collision check data
typedef struct ua_collision_data
{
   ua_vector2d pos;
   ua_vector2d dir;
   ua_vector2d line_inter;
   ua_vector2d circle_inter; // seems to be not needed

   double nearest_dist;

} ua_collision_data;


// constants

//! minimum distance
const double ua_physics_min_dist = 0.05;

//! player circle radius
const double ua_player_radius = 0.4;

//! max height the player can step
const double ua_physics_max_step_height = 2.1;

//! max speed a player can walk, in tiles / second
const double ua_player_max_walk_speed = 2.4;

//! max speed a player can rotate, in degrees / second
const double ua_player_max_rotate_speed = 90;


// ua_physics_model methods

ua_physics_model::ua_physics_model()
:last_evaltime(-0.1)//,player_speed(1.0)
{
}

/*void ua_physics_model::set_player_speed(double factor)
{
   player_speed = factor>1.0 ? 1.0 : factor;
}*/

/*
   // check for looking up or down
   if (look_up || look_down)
   {
      double viewangle = core->get_underworld().get_player().get_angle_pan();

      viewangle += (look_up ? -1.0 : 1.0)*(viewangle_speed/core->get_tickrate());

      // view angle has to stay between -180 and 180 degree
      while (viewangle > 180.0 || viewangle < -180.0 )
         viewangle = fmod(viewangle-360.0,360.0);

      double maxangle = 45.0;

      if (core->get_settings().get_bool(ua_setting_uwadv_features))
         maxangle = 75.0;

      // restrict up-down view angle
      if (viewangle < -maxangle) viewangle = -maxangle;
      if (viewangle > maxangle) viewangle = maxangle;

      core->get_underworld().get_player().set_angle_pan(viewangle);
   }
*/

void ua_physics_model::eval_player_movement(double time)
{
   ua_player &player = underw->get_player();

   unsigned int mode = underw->get_player().get_movement_mode();

   if (mode & ua_move_walk)
   {
      double speed = ua_player_max_walk_speed*(time-last_evaltime) *
         player.get_movement_factor(ua_move_walk);
      double angle = underw->get_player().get_angle_rot();

      ua_vector2d dir;
      dir.set_polar(speed,angle);
      ua_vector2d pos(player.get_xpos(),player.get_ypos());

      // Get player height before collision:
      double h1 = underw->get_player_height();
      
      // Perform collision detection:
      calc_collision(pos,dir);
   
      // set new player 2D pos
      player.set_pos(pos.x,pos.y);
 
      // Get the map height of the new 2d position:  
      double h2 = underw->get_current_level().get_floor_height(pos.x, pos.y);
 
      double finalHeight = h2;
      // a drop down? Check if player is allowed to fall down
      if (h2 < h1) {
        // TODO:
        // - If edge, check whole player cylinder is over the edge.
        // - if along a slope allow player to "fall"
        
      }

      // cache the player height
      player.set_height(finalHeight);
   }
/*
   if (mode & ua_move_rotate_left)
   {
      double angle = ua_player_max_rotate_speed*(time-last_evaltime);
      player.set_angle_rot(player.get_angle_rot()+angle);
   }

   if (mode & ua_move_rotate_right)
   {
      double angle = ua_player_max_rotate_speed*(time-last_evaltime);
      player.set_angle_rot(player.get_angle_rot()-angle);
   }
*/
   last_evaltime = time;
}

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
      static level=0;

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
