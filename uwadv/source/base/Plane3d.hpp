//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Michael Fink
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
/// \file Plane3d.hpp
/// \brief 3D plane class
//
#pragma once

#include "Math.hpp"

#ifdef _MSC_VER
// pragmas for Visual C++
#pragma inline_depth( 255 )
#pragma inline_recursion( on )
#pragma auto_inline( on )
#endif

/// plane in 3d
class Plane3d
{
public:
   /// ctor; takes origin and normal vector
   Plane3d(const Vector3d& the_origin, const Vector3d& the_normal)
      :origin(the_origin), normal(the_normal)
   {
      equation_3 = -normal.Dot(origin);
   }

   /// ctor; constructs plane from triangle
   Plane3d(Vector3d p1, Vector3d p2, Vector3d p3)
      : origin(p1)
   {
      normal = Vector3d::Cross(p2 - p1, p3 - p1);
      normal.Normalize();
      equation_3 = -normal.Dot(origin);
   }

   /// calculates if plane is front-facing to given direction vector
   bool IsFrontFacingTo(const Vector3d& direction) const
   {
      double dot = normal.Dot(direction);
      return dot <= 0.0;
   }

   /// calculates signed distance to plane
   double SignedDistanceTo(const Vector3d& point) const
   {
      return point.Dot(normal) + equation_3;
   }

   Vector3d origin; ///< plane origin
   Vector3d normal; ///< plane normal
   double equation_3;  ///< 3rd plane equation param
};
