/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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
/*! \file renderer_impl.hpp

   \brief renderer implementation class

*/
//! \ingroup renderer

//@{

// include guard
#ifndef uwadv_renderer_impl_hpp_
#define uwadv_renderer_impl_hpp_

// needed includes
#include "uamath.hpp"
#include "level.hpp"
#include "critter.hpp"
#include "models.hpp"
#include "quadtree.hpp"
#include "geometry.hpp"


// classes

//! renderer implementation
class ua_renderer_impl
{
public:
   //! ctor
   ua_renderer_impl();

   //! renders underworld level at given player pos and angles
   void render(const ua_level& level, ua_vector3d pos,
      double panangle, double rotangle, double fov);

   //! returns texture manager
   ua_texture_manager& get_texture_manager(){ return texmgr; }

   //! returns critter frames manager
   ua_critter_frames_manager& get_critter_frames_manager(){ return critter_manager; }

   //! returns 3d models manager
   ua_model3d_manager& get_ua_model3d_manager(){ return model_manager; }

   //! renders the objects of a tile
   void render_objects(const ua_level& level, unsigned int x, unsigned int y);

protected:
   //! renders a single object
   void render_object(const ua_level& level, const ua_object& obj,
      unsigned int x, unsigned int y);

   //! renders a billboarded sprite
   void render_sprite(ua_vector3d base, double width, double height,
      bool ignore_upvector, double u, double v,
      double move_u=0.0, double move_v=0.0);

/*
   //! renders decal
   void render_decal(const ua_object& obj, unsigned int x, unsigned int y);

   //! renders tmap object
   void render_tmap_obj(const ua_object& obj, unsigned int x, unsigned int y);

   //! draws a billboarded quad
   void draw_billboard_quad(ua_vector3d base,
      double quadwidth, double quadheight,
      double u1,double v1,double u2,double v2);
*/
protected:
   //! texture manager
   ua_texture_manager texmgr;

   //! critter frames manager
   ua_critter_frames_manager critter_manager;

   //! 3d models manager
   ua_model3d_manager model_manager;

   //! indicates if in selection (picking) mode
   bool selection_mode;

   //! billboard right and up vectors
   ua_vector3d bb_right, bb_up;
};


//! level tile renderer class
class ua_level_tile_renderer: public ua_quad_callback
{
public:
   //! ctor
   ua_level_tile_renderer(const ua_level& my_level, ua_renderer_impl& my_renderer_impl)
      :level(my_level), renderer_impl(my_renderer_impl), geom(my_level){}

   //! callback function to render tile
   virtual void visible_tile(unsigned int xpos, unsigned int ypos);

protected:
   //! ref to level
   const ua_level& level;

   //! geometry provider for level
   ua_geometry_provider geom;

   //! ref back to renderer implementation
   ua_renderer_impl& renderer_impl;
};


#endif
//@}
