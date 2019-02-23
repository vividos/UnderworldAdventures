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
/// \file renderer.hpp
/// \brief underworld renderer
//
#pragma once

#include "math.hpp"
#include "triangle3d.hpp"
#include "texture.hpp"
#include "settings.hpp"

namespace Underworld
{
   class Underworld;
   class Level;
   class Object;
}

class ua_renderer_impl;
class ua_critter_frames_manager;
class ua_model3d_manager;
struct SDL_Window;

/// underworld renderer
class ua_renderer
{
public:
   /// ctor
   ua_renderer();
   /// dtor
   ~ua_renderer();

   /// initializes renderer
   void init(IGame& game, SDL_Window* window);

   /// cleans up renderer
   void done();

   /// cleans screen with black color and updates frame
   void clear();

   /// returns texture manager
   ua_texture_manager& get_texture_manager();

   /// returns 3d model manager
   ua_model3d_manager& get_model3d_manager();

   /// returns critter frames manager
   ua_critter_frames_manager& get_critter_frames_manager();

   /// sets viewport to render 3d scene
   void set_viewport3d(unsigned int xpos, unsigned int ypos,
      unsigned int width, unsigned int height);

   /// sets up camera for 2d user interface rendering
   void setup_camera2d();

   /// sets up camera for 3d scene rendering
   void setup_camera3d(const ua_vector3d& view_offset, double fov = 90.0,
      double farplane = 16.0);

   /// renders current view of the underworld
   void render_underworld(const Underworld::Underworld& underw);

   /// does selection/picking
   void select_pick(const Underworld::Underworld& underw,
      unsigned int xpos, unsigned int ypos,
      unsigned int& tilex, unsigned int& tiley,
      bool& isobj, unsigned int& id);

   /// swaps screen buffers
   void swap_buffers();

   /// prepares renderer for given level (e.g. when changing levels)
   void prepare_level(Underworld::Level& level);

   /// does renderer-specific tick processing
   void tick(double tickrate);

   /// returns 3d model bounding triangles if a 3d model exists
   void get_model3d_bounding_triangles(unsigned int x, unsigned int y,
      const Underworld::Object& obj,
      std::vector<ua_triangle3d_textured>& alltriangles);

protected:
   /// window
   SDL_Window* m_window;

   /// renderer implementation
   ua_renderer_impl* renderer_impl;

   /// camera view offset
   ua_vector3d view_offset;

   /// field of view in degrees
   double fov;

   /// distance of far plane
   double far_dist;

   /// 3d viewport to use
   GLint viewport[4];

   // constants

   /// near plane distance
   static const double near_dist;
};
