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
/// \file Math.hpp
/// \brief math stuff
//
// Note: all angle params should be passed to the functions in degrees, not
// radians.
//
#pragma once

#include <cmath>

#ifdef _MSC_VER
// pragmas for visual c++
#pragma inline_depth( 255 )
#pragma inline_recursion( on )
#pragma auto_inline( on )
#endif

/// pi constant
const double c_pi = 3.141592653589793;

/// converts angle from degree values (0°..360°) to radians (0..2pi)
#define Deg2rad(a) ((double(a) / 180.0) * c_pi)
/// converts angle from randians values to degrees
#define Rad2deg(a) ((double(a) / c_pi) * 180.0)

/// \brief 2D vector class
/// The class represents a 2 dimensional vector with x and y coordinates.
class Vector2d
{
public:
   /// vector elements
   double x, y;

public:
   /// ctor
   Vector2d();
   /// copy ctor
   Vector2d(const Vector2d& vec);
   /// assign operator
   Vector2d& operator=(const Vector2d& vec);

   /// ctor with x and y coordinates
   Vector2d(double ax, double ay);

   /// sets vector by giving vector elements
   void set(double nx, double ny);

   /// sets vector by giving two vectors
   void set(const Vector2d& v1, const Vector2d& v2);

   /// sets vector by giving polar coordinates (angle in degree, not radians)
   void set_polar(double len, double angle);

   /// add assignment
   Vector2d& operator+=(const Vector2d& vec);

   /// subtract assignment
   Vector2d& operator-=(const Vector2d& vec);

   /// multiplication assignment
   Vector2d& operator*=(double sc);

   /// returns length of vector
   double length() const;

   /// normalizes vector
   void normalize();

   /// calculates the inner (dot) product
   double dot(const Vector2d& vec) const;

   /// rotates vector; angle in degree
   void rotate(double angle);
};


/// \brief 3D vector class
/// The class represents a 3 dimensional vector with x, y and z coordinates.
/// It can also represent a point in 3d space, depending on use.
class Vector3d
{
public:
   /// vector elements
   double x, y, z;

public:
   /// ctor
   Vector3d();

   /// copy ctor
   Vector3d(const Vector3d& vec);

   /// ctor with x, y and z coordinates
   Vector3d(double ax, double ay, double az);

   /// assign operator
   Vector3d& operator=(const Vector3d& vec);

   /// add assignment
   Vector3d& operator+=(const Vector3d& vec);

   /// subtract assignment
   Vector3d& operator-=(const Vector3d& vec);

   /// multiplication assignment
   Vector3d& operator*=(double sc);

   /// multiplication assignment with vector
   Vector3d& operator*=(const Vector3d& vec);

   /// division assignment with vector
   Vector3d& operator/=(const Vector3d& vec);

   /// addition
   inline Vector3d operator+(const Vector3d& v) const;

   /// subtraction
   inline Vector3d operator-(const Vector3d& v) const;

   /// scalar multiplication
   inline Vector3d operator*(const double f) const;

   /// unary minus
   inline Vector3d operator-() const;

   /// sets vector by giving vector elements
   void set(double nx, double ny, double nz);

   /// returns length of vector
   double length() const;

   /// normalizes vector
   void normalize();

   /// calculates inner (dot) product
   double dot(const Vector3d& vec) const;

   /// calculates outer (cross) product
   void cross(const Vector3d& first, const Vector3d& second);

   /// rotate vector around x axis
   void rotate_x(double angle);

   /// rotate vector around y axis
   void rotate_y(double angle);

   /// rotate vector around z axis
   void rotate_z(double angle);

   /// rotates vector around axis
   void rotate(const Vector3d& axis, double angle);
};


/// plane in 3d
class Plane3d
{
public:
   /// ctor; takes origin and normal vector
   Plane3d(const Vector3d& the_origin, const Vector3d& the_normal);

   /// ctor; constructs plane from triangle
   Plane3d(Vector3d p1, Vector3d p2, Vector3d p3);

   /// calculates if plane is front-facing to given direction vector
   bool is_front_facing_to(const Vector3d& dir) const;

   /// calculates signed distance to plane
   double signed_dist_to(const Vector3d& point) const;

   Vector3d origin; ///< plane origin
   Vector3d normal; ///< plane normal
   double equation_3;  ///< 3rd plane equation param
};

// inline methods

// Vector2d methods

inline Vector2d::Vector2d()
{
   x = y = 0.0;
}

inline Vector2d::Vector2d(const Vector2d& vec)
{
   x = vec.x;
   y = vec.y;
}

inline Vector2d& Vector2d::operator=(const Vector2d& vec)
{
   x = vec.x;
   y = vec.y;
   return *this;
}

inline Vector2d::Vector2d(double ax, double ay)
{
   x = ax;
   y = ay;
}

inline void Vector2d::set(double nx, double ny)
{
   x = nx;
   y = ny;
}

inline void Vector2d::set(const Vector2d& v1, const Vector2d& v2)
{
   x = v2.x - v1.x;
   y = v2.y - v1.y;
}

inline void Vector2d::set_polar(double len, double angle)
{
   x = len * cos(Deg2rad(angle));
   y = len * sin(Deg2rad(angle));
}

inline Vector2d& Vector2d::operator+=(const Vector2d& vec)
{
   x += vec.x;
   y += vec.y;
   return *this;
}

inline Vector2d& Vector2d::operator-=(const Vector2d& vec)
{
   x -= vec.x;
   y -= vec.y;
   return *this;
}

inline Vector2d& Vector2d::operator*=(double sc)
{
   x *= sc;
   y *= sc;
   return *this;
}

inline double Vector2d::length() const
{
   return sqrt(x*x + y * y);
}

inline void Vector2d::normalize()
{
   (*this) *= (1.0 / length());
}

inline double Vector2d::dot(const Vector2d& vec) const
{
   return x * vec.x + y * vec.y;
}

inline void Vector2d::rotate(double angle)
{
   double temp = x * cos(Deg2rad(angle)) - y * sin(Deg2rad(angle));
   y = x * sin(Deg2rad(angle)) + y * cos(Deg2rad(angle));
   x = temp;
}


// Vector3d methods

inline Vector3d::Vector3d()
{
   x = y = z = 0.0;
}

inline Vector3d::Vector3d(const Vector3d& vec)
   :x(vec.x), y(vec.y), z(vec.z)
{
}

inline Vector3d::Vector3d(double ax, double ay, double az)
   : x(ax), y(ay), z(az)
{
}

inline Vector3d& Vector3d::operator=(const Vector3d& vec)
{
   x = vec.x;
   y = vec.y;
   z = vec.z;
   return *this;
}

inline Vector3d& Vector3d::operator+=(const Vector3d& vec)
{
   x += vec.x;
   y += vec.y;
   z += vec.z;
   return *this;
}

inline Vector3d& Vector3d::operator-=(const Vector3d& vec)
{
   x -= vec.x;
   y -= vec.y;
   z -= vec.z;
   return *this;
}

inline Vector3d& Vector3d::operator*=(double sc)
{
   x *= sc;
   y *= sc;
   z *= sc;
   return *this;
}

inline Vector3d& Vector3d::operator*=(const Vector3d& vec)
{
   x *= vec.x;
   y *= vec.y;
   z *= vec.z;
   return *this;
}

inline Vector3d& Vector3d::operator/=(const Vector3d& vec)
{
   x /= vec.x;
   y /= vec.y;
   z /= vec.z;
   return *this;
}

inline Vector3d Vector3d::operator-(const Vector3d& vec) const
{
   return Vector3d(x - vec.x, y - vec.y, z - vec.z);
}

inline Vector3d Vector3d::operator+(const Vector3d& vec) const
{
   return Vector3d(x + vec.x, y + vec.y, z + vec.z);
}

inline Vector3d Vector3d::operator*(const double d) const
{
   return Vector3d(x*d, y*d, z*d);
}

inline Vector3d Vector3d::operator-() const
{
   return Vector3d(-x, -y, -z);
}


inline void Vector3d::set(double nx, double ny, double nz)
{
   x = nx;
   y = ny;
   z = nz;
}

inline double Vector3d::length() const
{
   return sqrt(x*x + y * y + z * z);
}

inline void Vector3d::normalize()
{
   (*this) *= (1.0 / length());
}

inline double Vector3d::dot(const Vector3d& vec) const
{
   return x * vec.x + y * vec.y + z * vec.z;
}

inline void Vector3d::cross(const Vector3d& first, const Vector3d& second)
{
   set(first.y * second.z - first.z * second.y,
      first.z * second.x - first.x * second.z,
      first.x * second.y - first.y * second.x);
}

inline void Vector3d::rotate_x(double angle)
{
   double y_temp = y, angle_rad = Deg2rad(angle);
   y = y * cos(angle_rad) - z * sin(angle_rad);
   z = y_temp * sin(angle_rad) + z * cos(angle_rad);
}

inline void Vector3d::rotate_y(double angle)
{
   double x_temp = x, angle_rad = Deg2rad(angle);
   x = x * cos(angle_rad) + z * sin(angle_rad);
   z = -x_temp * sin(angle_rad) + z * cos(angle_rad);
}

inline void Vector3d::rotate_z(double angle)
{
   double x_temp = x, angle_rad = Deg2rad(angle);
   x = x * cos(angle_rad) - y * sin(angle_rad);
   y = x_temp * sin(angle_rad) + y * cos(angle_rad);
}

/*! calculates rotated vector using the rotation matrix given at
    http://www.makegames.com/3drotation/
*/
inline void Vector3d::rotate(const Vector3d& axis, double angle)
{
   double c = cos(Deg2rad(angle));
   double s = sin(Deg2rad(angle));
   double t = 1 - c;

   double ax = axis.x, ay = axis.y, az = axis.z;

   set((t*ax*ax + c)*x + (t*ax*ay - s * az)*y + (t*ax*az + s * ay)*z,
      (t*ax*ay + s * az)*x + (t*ay*ay + c)*y + (t*ay*az - s * ax)*z,
      (t*ax*az - s * ay)*x + (t*ay*az + s * ax)*y + (t*az*az + c)*z);
}


// Plane3d methods

inline Plane3d::Plane3d(const Vector3d& the_origin,
   const Vector3d& the_normal)
   :origin(the_origin), normal(the_normal)
{
   equation_3 = -(normal.x*origin.x + normal.y*origin.y + normal.z*origin.z);
}

inline Plane3d::Plane3d(Vector3d p1, Vector3d p2, Vector3d p3)
   : origin(p1)
{
   p2 -= p1;
   p3 -= p1;
   normal.cross(p2, p3);
   normal.normalize();
   equation_3 = -(normal.x*origin.x + normal.y*origin.y + normal.z*origin.z);
}

inline bool Plane3d::is_front_facing_to(const Vector3d& dir) const
{
   double dot = normal.dot(dir);
   return dot <= 0.0;
}

inline double Plane3d::signed_dist_to(const Vector3d& point) const
{
   return point.dot(normal) + equation_3;
}
