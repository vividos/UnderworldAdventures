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
/*! \file renderer.hpp

   \brief underworld renderer

*/

// include guard
#ifndef uwadv_renderer_hpp_
#define uwadv_renderer_hpp_

// needed includes
#include "uamath.hpp"
#include "texture.hpp"
#include "settings.hpp"


// forward references
class ua_underworld;
class ua_renderer_impl;
class ua_critter_pool;
class ua_model3d_manager;


// classes

//! underworld renderer
class ua_renderer
{
public:
   //! ctor
   ua_renderer();
   //! dtor
   ~ua_renderer();

   //! initializes renderer
   void init(ua_settings& settings);

   //! cleans up renderer
   void done();

   //! cleans screen with black color and updates frame
   void clear();

   //! returns texture manager
   ua_texture_manager& get_texture_manager();

   //! sets up camera for 2d user interface rendering
   void setup_camera2d();

   //! sets up camera for 3d scene rendering
   void setup_camera3d(const ua_vector3d& view_offset, double fov = 90.0,
      double farplane = 16.0);

   //! renders current view of the underworld
   void render_underworld(const ua_underworld& underw);

   //! does selection/picking
   void select_pick(const ua_underworld& underw,
      unsigned int xpos, unsigned int ypos,
      unsigned int& tilex, unsigned int& tiley,
      bool& isobj, unsigned int& id);

/*
   //! renders underworld
   void ua_renderer::render(ua_level& level, ua_vector3d& pos,
      double panangle, double rotangle,
      std::vector<ua_quad_tile_coord>& tilelist);


   //! returns the list of all triangles for a given tile
   static void get_tile_triangles(ua_level& level, unsigned int xpos, unsigned int ypos,
      std::vector<ua_triangle3d_textured>& alltriangles);
*/

protected:
   //! texture manager to use for rendering
   ua_texture_manager texmgr;

   //! critter pool
   ua_critter_pool* critpool;

   //! 3d models manager
   ua_model3d_manager* modelmgr;

   //! renderer implementation
   ua_renderer_impl* renderer_impl;

   //! camera view offset
   ua_vector3d view_offset;

   //! field of view in degrees
   double fov;

   //! distance of far plane
   double far_dist;

   // constants

   //! near plane distance
   static const double near_dist;
};


// inline methods

inline ua_texture_manager& ua_renderer::get_texture_manager()
{
   return texmgr;
}

#endif
