//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
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
/// \file playerstate.hpp
/// \brief player state for physics simulation
//
#pragma once

#include "physicsbody.hpp"
#include <map>

namespace Underworld
{
   class Player;
}

/// player movement enum
enum ua_player_movement_mode
{
   ua_move_walk = 1,    ///< walks forward (or backwards, when factor is negative)
   ua_move_rotate = 2, ///< rotates player left (or right)
   ua_move_lookup = 4,  ///< moves player look angle up (or down)
   ua_move_jump = 8,    ///< jumps forward (or factor 0.0 for standing jump)
   ua_move_slide = 16,  ///< slides right (or left)
   ua_move_float = 32,  ///< floats player up (or down)
};

class ua_player_physics_object : public ua_physics_body
{
public:
   /// ctor
   ua_player_physics_object(Underworld::Player& player, bool enhancedFeatures);

   /// sets and delete movement mode values
   void set_movement_mode(unsigned int set, unsigned int del = 0);

   /// sets movement factor for a given movement type; range is [-1.0; 1.0]
   void set_movement_factor(ua_player_movement_mode mode, double factor);

   /// returns movement mode
   unsigned int get_movement_mode() const
   {
      return m_movementMode;
   }

   /// returns movement factor for given movement mode
   double get_movement_factor(ua_player_movement_mode mode);

   /// does rotation moves
   void rotate_move(double time_elapsed);

   // virtual methods from ua_physics_object
   virtual void set_new_elapsed_time(double time_elapsed) override;
   virtual ua_vector3d get_pos() override;
   virtual void set_pos(ua_vector3d& pos) override;
   virtual ua_vector3d get_dir() override;

   virtual void reset_gravity() override;
   virtual ua_vector3d get_gravity_force() override;
   virtual void hit_floor() override;

private:
   /// ref to player
   Underworld::Player& m_player;

   /// player ellipsoid
   ua_vector3d m_ellipsoid;

   /// current movement mode
   unsigned int m_movementMode;

   /// movement factors map
   std::map<ua_player_movement_mode, double> m_moveFactors;

   /// current fall time
   double m_fallTime;

   /// player height at fall start
   double m_fallHeightStart;

   /// maximum pan angle
   double m_maxPanAngle;
};
