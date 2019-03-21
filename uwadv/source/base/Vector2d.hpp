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
/// \file Vector2d.hpp
/// \brief 2D vector class
//
// Note: all angle params should be passed to the functions in degrees, not
// radians.
//
#pragma once

#include "Math.hpp"

#ifdef _MSC_VER
// pragmas for Visual C++
#pragma inline_depth( 255 )
#pragma inline_recursion( on )
#pragma auto_inline( on )
#endif

/// \brief 2D vector class
/// The class represents a 2 dimensional vector with x and y coordinates.
class Vector2d
{
public:
   double x; ///< vector x element
   double y; ///< vector y element

public:
   /// ctor
   Vector2d()
      :x(0.0),
      y(0.0)
   {
   }

   /// copy ctor
   Vector2d(const Vector2d& vec)
      :x(vec.x),
      y(vec.y)
   {
   }

   /// move ctor
   Vector2d(const Vector2d&& vec)
      :x(vec.x),
      y(vec.y)
   {
   }

   /// ctor with x and y coordinates
   Vector2d(double _x, double _y)
      :x(_x),
      y(_y)
   {
   }

   // operators

   /// assign operator
   Vector2d& operator=(const Vector2d& vec)
   {
      if (this == &vec)
         return *this;

      x = vec.x;
      y = vec.y;
      return *this;
   }

   /// move assign operator
   Vector2d& operator=(const Vector2d&& vec)
   {
      if (this == &vec)
         return *this;

      x = vec.x;
      y = vec.y;
      return *this;
   }

   /// add assignment
   Vector2d& operator+=(const Vector2d& vec)
   {
      x += vec.x;
      y += vec.y;
      return *this;
   }

   /// subtract assignment
   Vector2d& operator-=(const Vector2d& vec)
   {
      x -= vec.x;
      y -= vec.y;
      return *this;
   }

   /// multiplication assignment
   Vector2d& operator*=(double scalar)
   {
      x *= scalar;
      y *= scalar;
      return *this;
   }

   // operations

   /// sets vector by giving vector elements
   void Set(double _x, double _y)
   {
      x = _x;
      y = _y;
   }

   /// sets vector by giving two vectors
   void Set(const Vector2d& v1, const Vector2d& v2)
   {
      x = v2.x - v1.x;
      y = v2.y - v1.y;
   }

   /// sets vector by giving polar coordinates (angle in degree, not radians)
   void SetPolar(double length, double angle)
   {
      x = length * cos(Deg2rad(angle));
      y = length * sin(Deg2rad(angle));
   }


   /// returns length of vector
   double Length() const
   {
      return sqrt(x * x + y * y);
   }

   /// normalizes vector
   void Normalize()
   {
      operator*=(1.0 / Length());
   }

   /// calculates the inner (dot) product of this and another vector
   double Dot(const Vector2d& vec) const
   {
      return x * vec.x + y * vec.y;
   }

   /// rotates vector; angle in degree
   void Rotate(double angle)
   {
      double temp = x * cos(Deg2rad(angle)) - y * sin(Deg2rad(angle));
      y = x * sin(Deg2rad(angle)) + y * cos(Deg2rad(angle));
      x = temp;
   }
};

// free functions

/// unary minus
inline Vector2d operator-(const Vector2d& vec)
{
   return Vector2d(-vec.x, -vec.y);
};

/// add operator
inline Vector2d operator+(const Vector2d& v1, const Vector2d& v2)
{
   return Vector2d(v1.x + v2.x, v1.y + v2.y);
}

/// subtract operator
inline Vector2d operator-(const Vector2d& v1, const Vector2d& v2)
{
   return Vector2d(v1.x - v2.x, v1.y - v2.y);
}

/// scalar multiplication, vector * scalar
inline Vector2d operator*(const Vector2d& vec, const double scalar)
{
   Vector2d ret(vec);
   ret *= scalar;
   return ret;
}

/// scalar multiplication, scalar * vector
inline Vector2d operator*(const double scalar, const Vector2d& vec)
{
   Vector2d ret(vec);
   ret *= scalar;
   return ret;
}

/// division by scalar
inline Vector2d operator/(const Vector2d& vec, const double scalar)
{
   return Vector2d(vec.x / scalar, vec.y / scalar);
}
