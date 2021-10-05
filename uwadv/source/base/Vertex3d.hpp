//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019,2021 Underworld Adventures Team
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
/// \file Vertex3d.hpp
/// \brief triangle vertex class
//
#pragma once

#include "Vector3d.hpp"

/// \brief vertex in 3D space
/// Represents a vertex in 3D space, which forms a triangle. The vertex
/// additionally has texture coordinates associated with them.
struct Vertex3d
{
   /// struct ctor
   Vertex3d()
      :u(0.0),
      v(0.0)
   {
   }

   /// ctor, setting vertex elements
   Vertex3d(const Vector3d& pos_, double u_ = 0.0, double v_ = 0.0)
      :pos(pos_),
      u(u_),
      v(v_)
   {
   }

   /// vertex position
   Vector3d pos;

   /// texture coordinates
   double u, v;
};
