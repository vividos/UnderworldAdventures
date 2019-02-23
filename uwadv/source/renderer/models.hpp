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
/// \file models.hpp
/// \brief 3d models and model manager
//
#pragma once

#include "math.hpp"
#include "triangle3d.hpp"
#include "texture.hpp"
#include "object.hpp"
#include <map>
#include <vector>
#include <memory>

class IGame;

/// \brief 3d model base class
class ua_model3d
{
public:
   /// ctor
   ua_model3d() {}

   /// dtor
   virtual ~ua_model3d() {}

   /// returns unique model name
   const char* get_model_name();

   /// renders model
   virtual void render(const Underworld::Object& obj, ua_texture_manager& texmgr,
      ua_vector3d& base)
   {
      obj; texmgr; base;
   }

   /// returns bounding triangles for collision detection
   virtual void get_bounding_triangles(const Underworld::Object& obj,
      ua_vector3d& base, std::vector<ua_triangle3d_textured>& alltriangles)
   {
      obj; base; alltriangles;
   }
};

/// smart pointer to model object
typedef std::shared_ptr<ua_model3d> ua_model3d_ptr;

/// 3d model manager
class ua_model3d_manager : private Base::ConfigFile
{
public:
   /// ctor
   ua_model3d_manager() {}

   /// init manager
   void init(IGame& game);

   /// returns if a 3d model for a certain item_id is available
   bool model_avail(Uint16 item_id) const;

   /// renders a model
   void render(const Underworld::Object& obj, ua_texture_manager& texmgr,
      ua_vector3d& base);

   /// returns bounding triangles for collision detection with given item_id
   void get_bounding_triangles(const Underworld::Object& obj, ua_vector3d& base,
      std::vector<ua_triangle3d_textured>& alltriangles);

protected:
   /// called to load a specific value
   virtual void load_value(const char* name, const char* value);

protected:
   /// map with all 3d model objects
   std::map<Uint16, ua_model3d_ptr> allmodels;

   /// all builtin models from the exe
   std::vector<ua_model3d_ptr> allbuiltins;
};
