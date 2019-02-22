//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2004,2019 Underworld Adventures Team
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
/// \file model_builtin.hpp
/// \brief builtin models
//
#pragma once

#include "models.hpp"

/// builtin model class
class ua_model3d_builtin : public ua_model3d
{
public:
   /// ctor
   ua_model3d_builtin() {}
   /// dtor
   virtual ~ua_model3d_builtin() {}

   /// renders model
   virtual void render(const Underworld::Object& obj, ua_texture_manager& texmgr,
      ua_vector3d& base);

   /// returns bounding triangles for collision detection
   virtual void get_bounding_triangles(const Underworld::Object& obj,
      ua_vector3d& base, std::vector<ua_triangle3d_textured>& alltriangles);

protected:
   /// model extents
   ua_vector3d extents;

   /// all triangles
   std::vector<ua_triangle3d_textured> triangles;

   // friend decoding function
   friend bool ua_model_decode_builtins(const char* filename,
      std::vector<ua_model3d_ptr>& allmodels, bool dump);
};

/// special model class
class ua_model3d_special : public ua_model3d
{
public:
   /// ctor
   ua_model3d_special() {}
   /// dtor
   virtual ~ua_model3d_special() {}

   /// renders model
   virtual void render(const Underworld::Object& obj, ua_texture_manager& texmgr,
      ua_vector3d& base);

   /// returns bounding triangles for collision detection
   virtual void get_bounding_triangles(const Underworld::Object& obj,
      ua_vector3d& base, std::vector<ua_triangle3d_textured>& alltriangles);
};
