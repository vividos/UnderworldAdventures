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
/*! \file models.hpp

   \brief 3d models and model manager

*/
//! \ingroup renderer

//@{

// include guard
#ifndef uwadv_models_hpp_
#define uwadv_models_hpp_

// needed includes
#include "uamath.hpp"
#include "texture.hpp"
#include "objects.hpp"
#include <map>
#include <vector>


// forward declarations
class ua_game_core_interface;


// classes

//! 3d model base class
class ua_model3d
{
public:
   //! ctor
   ua_model3d(){}

   //! dtor
   virtual ~ua_model3d(){}

   //! returns unique model name
   const char* get_model_name();

   //! renders model
   virtual void render(ua_object& obj, ua_vector3d& base){}

   //! returns bounding triangles for collision detection
   virtual void get_bounding_triangles(ua_object& obj,
      ua_vector3d& base, std::vector<ua_triangle3d_textured>& alltriangles){}
};


//! smart pointer to model object
typedef ua_smart_ptr<ua_model3d> ua_model3d_ptr;


//! 3d model manager
class ua_model3d_manager: private ua_cfgfile
{
public:
   //! ctor
   ua_model3d_manager(){}

   //! init manager
   void init(ua_game_core_interface* core);

   //! returns if a 3d model for a certain item_id is available
   bool model_avail(Uint16 item_id);

   //! renders a model
   void render(ua_object& obj, ua_vector3d& base);

   //! returns bounding triangles for collision detection with given item_id
   void get_bounding_triangles(ua_object& obj, ua_vector3d& base,
      std::vector<ua_triangle3d_textured>& alltriangles);

   //! current model manager instance
   static ua_model3d_manager* cur_modelmgr;

protected:
   //! called to load a specific value
   virtual void load_value(const char* name, const char* value);

protected:
   //! map with all 3d model objects
   std::map<Uint16,ua_model3d_ptr> allmodels;

   //! all builtin models from the exe
   std::vector<ua_model3d_ptr> allbuiltins;

   //! files manager used while loading cfg files
   ua_game_core_interface* core;
};


#endif
//@}
