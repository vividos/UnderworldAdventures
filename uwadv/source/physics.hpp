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
/*! \file physics.hpp

   \brief game physics model

*/

// include guard
#ifndef __uwadv_physics_hpp_
#define __uwadv_physics_hpp_

// needed includes
#include "uamath.hpp"
#include <vector>


// forward declarations
class ua_underworld;
struct ua_collision_data;


// classes

class ua_physics_model
{
public:
   //! ctor
   ua_physics_model();

   //! inits physics model
   void init(ua_underworld *uw){ underw = uw; }

   //! sets speed of player; range: [0.0; 1.0]
   void set_player_speed(double factor);

   //! evaluates player movement
   void eval_player_movement(double time);

protected:
   //! calculates collision and response; recursively called
   void calc_collision(ua_vector2d &pos, const ua_vector2d &dir);

   //! checks a tile for collision with walls
   bool check_collision_tile(unsigned int tilex, unsigned int tiley,
      ua_collision_data &data);

   //! does collision check; returns true when line was hit
   bool check_collision(const ua_vector2d &point1,const ua_vector2d &point2,
      ua_collision_data &data, bool check_height);

protected:
   //! current underworld object
   ua_underworld *underw;

   //! last evaluation time
   double last_evaltime;

   //! current player speed factor
   double player_speed;
};

#endif
