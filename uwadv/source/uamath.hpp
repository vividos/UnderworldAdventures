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
/// \file uamath.hpp
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
const double ua_pi = 3.141592653589793;

/// converts angle from degree values (0°..360°) to radians (0..2pi)
#define ua_deg2rad(a) ((double(a)/180.0)*ua_pi)
/// converts angle from randians values to degrees
#define ua_rad2deg(a) ((double(a)/ua_pi)*180.0)

/// \brief 2D vector class
/// The class represents a 2 dimensional vector with x and y coordinates.
class ua_vector2d
{
public:
   /// vector elements
   double x, y;

public:
   /// ctor
   ua_vector2d();
   /// copy ctor
   ua_vector2d(const ua_vector2d& vec);
   /// assign operator
   ua_vector2d& operator=(const ua_vector2d& vec);

   /// ctor with x and y coordinates
   ua_vector2d(double ax, double ay);

   /// sets vector by giving vector elements
   void set(double nx, double ny);

   /// sets vector by giving two vectors
   void set(const ua_vector2d& v1, const ua_vector2d& v2);

   /// sets vector by giving polar coordinates (angle in degree, not radians)
   void set_polar(double len, double angle);

   /// add assignment
   ua_vector2d& operator+=(const ua_vector2d& vec);

   /// subtract assignment
   ua_vector2d& operator-=(const ua_vector2d& vec);

   /// multiplication assignment
   ua_vector2d& operator*=(double sc);

   /// returns length of vector
   double length() const;

   /// normalizes vector
   void normalize();

   /// calculates the inner (dot) product
   double dot(const ua_vector2d& vec) const;

   /// rotates vector; angle in degree
   void rotate(double angle);
};


/// \brief 3D vector class
/// The class represents a 3 dimensional vector with x, y and z coordinates.
/// It can also represent a point in 3d space, depending on use.
class ua_vector3d
{
public:
   /// vector elements
   double x, y, z;

public:
   /// ctor
   ua_vector3d();

   /// copy ctor
   ua_vector3d(const ua_vector3d& vec);

   /// ctor with x, y and z coordinates
   ua_vector3d(double ax, double ay, double az);

   /// assign operator
   ua_vector3d& operator=(const ua_vector3d& vec);

   /// add assignment
   ua_vector3d& operator+=(const ua_vector3d& vec);

   /// subtract assignment
   ua_vector3d& operator-=(const ua_vector3d& vec);

   /// multiplication assignment
   ua_vector3d& operator*=(double sc);

   /// multiplication assignment with vector
   ua_vector3d& operator*=(const ua_vector3d& vec);

   /// division assignment with vector
   ua_vector3d& operator/=(const ua_vector3d& vec);

   /// addition
   inline ua_vector3d operator+(const ua_vector3d& v) const;

   /// subtraction
   inline ua_vector3d operator-(const ua_vector3d& v) const;

   /// scalar multiplication
   inline ua_vector3d operator*(const double f) const;

   /// unary minus
   inline ua_vector3d operator-() const;

   /// sets vector by giving vector elements
   void set(double nx, double ny, double nz);

   /// returns length of vector
   double length() const;

   /// normalizes vector
   void normalize();

   /// calculates inner (dot) product
   double dot(const ua_vector3d& vec) const;

   /// calculates outer (cross) product
   void cross(const ua_vector3d& first, const ua_vector3d& second);

   /// rotate vector around x axis
   void rotate_x(double angle);

   /// rotate vector around y axis
   void rotate_y(double angle);

   /// rotate vector around z axis
   void rotate_z(double angle);

   /// rotates vector around axis
   void rotate(const ua_vector3d& axis, double angle);
};


/// plane in 3d
class ua_plane3d
{
public:
   /// ctor; takes origin and normal vector
   ua_plane3d(const ua_vector3d& the_origin, const ua_vector3d& the_normal);

   /// ctor; constructs plane from triangle
   ua_plane3d(ua_vector3d p1, ua_vector3d p2, ua_vector3d p3);

   /// calculates if plane is front-facing to given direction vector
   bool is_front_facing_to(const ua_vector3d& dir) const;

   /// calculates signed distance to plane
   double signed_dist_to(const ua_vector3d& point) const;

   ua_vector3d origin; ///< plane origin
   ua_vector3d normal; ///< plane normal
   double equation_3;  ///< 3rd plane equation param
};

// inline methods

// ua_vector2d methods

inline ua_vector2d::ua_vector2d()
{
   x = y = 0.0;
}

inline ua_vector2d::ua_vector2d(const ua_vector2d& vec)
{
   x = vec.x;
   y = vec.y;
}

inline ua_vector2d& ua_vector2d::operator=(const ua_vector2d& vec)
{
   x = vec.x;
   y = vec.y;
   return *this;
}

inline ua_vector2d::ua_vector2d(double ax, double ay)
{
   x = ax;
   y = ay;
}

inline void ua_vector2d::set(double nx, double ny)
{
   x = nx;
   y = ny;
}

inline void ua_vector2d::set(const ua_vector2d& v1, const ua_vector2d& v2)
{
   x = v2.x - v1.x;
   y = v2.y - v1.y;
}

inline void ua_vector2d::set_polar(double len, double angle)
{
   x = len * cos(ua_deg2rad(angle));
   y = len * sin(ua_deg2rad(angle));
}

inline ua_vector2d& ua_vector2d::operator+=(const ua_vector2d& vec)
{
   x += vec.x;
   y += vec.y;
   return *this;
}

inline ua_vector2d& ua_vector2d::operator-=(const ua_vector2d& vec)
{
   x -= vec.x;
   y -= vec.y;
   return *this;
}

inline ua_vector2d& ua_vector2d::operator*=(double sc)
{
   x *= sc;
   y *= sc;
   return *this;
}

inline double ua_vector2d::length() const
{
   return sqrt(x*x + y * y);
}

inline void ua_vector2d::normalize()
{
   (*this) *= (1.0 / length());
}

inline double ua_vector2d::dot(const ua_vector2d& vec) const
{
   return x * vec.x + y * vec.y;
}

inline void ua_vector2d::rotate(double angle)
{
   double temp = x * cos(ua_deg2rad(angle)) - y * sin(ua_deg2rad(angle));
   y = x * sin(ua_deg2rad(angle)) + y * cos(ua_deg2rad(angle));
   x = temp;
}


// ua_vector3d methods

inline ua_vector3d::ua_vector3d()
{
   x = y = z = 0.0;
}

inline ua_vector3d::ua_vector3d(const ua_vector3d& vec)
   :x(vec.x), y(vec.y), z(vec.z)
{
}

inline ua_vector3d::ua_vector3d(double ax, double ay, double az)
   : x(ax), y(ay), z(az)
{
}

inline ua_vector3d& ua_vector3d::operator=(const ua_vector3d& vec)
{
   x = vec.x;
   y = vec.y;
   z = vec.z;
   return *this;
}

inline ua_vector3d& ua_vector3d::operator+=(const ua_vector3d& vec)
{
   x += vec.x;
   y += vec.y;
   z += vec.z;
   return *this;
}

inline ua_vector3d& ua_vector3d::operator-=(const ua_vector3d& vec)
{
   x -= vec.x;
   y -= vec.y;
   z -= vec.z;
   return *this;
}

inline ua_vector3d& ua_vector3d::operator*=(double sc)
{
   x *= sc;
   y *= sc;
   z *= sc;
   return *this;
}

inline ua_vector3d& ua_vector3d::operator*=(const ua_vector3d& vec)
{
   x *= vec.x;
   y *= vec.y;
   z *= vec.z;
   return *this;
}

inline ua_vector3d& ua_vector3d::operator/=(const ua_vector3d& vec)
{
   x /= vec.x;
   y /= vec.y;
   z /= vec.z;
   return *this;
}

inline ua_vector3d ua_vector3d::operator-(const ua_vector3d& vec) const
{
   return ua_vector3d(x - vec.x, y - vec.y, z - vec.z);
}

inline ua_vector3d ua_vector3d::operator+(const ua_vector3d& vec) const
{
   return ua_vector3d(x + vec.x, y + vec.y, z + vec.z);
}

inline ua_vector3d ua_vector3d::operator*(const double d) const
{
   return ua_vector3d(x*d, y*d, z*d);
}

inline ua_vector3d ua_vector3d::operator-() const
{
   return ua_vector3d(-x, -y, -z);
}


inline void ua_vector3d::set(double nx, double ny, double nz)
{
   x = nx;
   y = ny;
   z = nz;
}

inline double ua_vector3d::length() const
{
   return sqrt(x*x + y * y + z * z);
}

inline void ua_vector3d::normalize()
{
   (*this) *= (1.0 / length());
}

inline double ua_vector3d::dot(const ua_vector3d& vec) const
{
   return x * vec.x + y * vec.y + z * vec.z;
}

inline void ua_vector3d::cross(const ua_vector3d& first, const ua_vector3d& second)
{
   set(first.y * second.z - first.z * second.y,
      first.z * second.x - first.x * second.z,
      first.x * second.y - first.y * second.x);
}

inline void ua_vector3d::rotate_x(double angle)
{
   double y_temp = y, angle_rad = ua_deg2rad(angle);
   y = y * cos(angle_rad) - z * sin(angle_rad);
   z = y_temp * sin(angle_rad) + z * cos(angle_rad);
}

inline void ua_vector3d::rotate_y(double angle)
{
   double x_temp = x, angle_rad = ua_deg2rad(angle);
   x = x * cos(angle_rad) + z * sin(angle_rad);
   z = -x_temp * sin(angle_rad) + z * cos(angle_rad);
}

inline void ua_vector3d::rotate_z(double angle)
{
   double x_temp = x, angle_rad = ua_deg2rad(angle);
   x = x * cos(angle_rad) - y * sin(angle_rad);
   y = x_temp * sin(angle_rad) + y * cos(angle_rad);
}

/*! calculates rotated vector using the rotation matrix given at
    http://www.makegames.com/3drotation/
*/
inline void ua_vector3d::rotate(const ua_vector3d& axis, double angle)
{
   double c = cos(ua_deg2rad(angle));
   double s = sin(ua_deg2rad(angle));
   double t = 1 - c;

   double ax = axis.x, ay = axis.y, az = axis.z;

   set((t*ax*ax + c)*x + (t*ax*ay - s * az)*y + (t*ax*az + s * ay)*z,
      (t*ax*ay + s * az)*x + (t*ay*ay + c)*y + (t*ay*az - s * ax)*z,
      (t*ax*az - s * ay)*x + (t*ay*az + s * ax)*y + (t*az*az + c)*z);
}


// ua_plane3d methods

inline ua_plane3d::ua_plane3d(const ua_vector3d& the_origin,
   const ua_vector3d& the_normal)
   :origin(the_origin), normal(the_normal)
{
   equation_3 = -(normal.x*origin.x + normal.y*origin.y + normal.z*origin.z);
}

inline ua_plane3d::ua_plane3d(ua_vector3d p1, ua_vector3d p2, ua_vector3d p3)
   : origin(p1)
{
   p2 -= p1;
   p3 -= p1;
   normal.cross(p2, p3);
   normal.normalize();
   equation_3 = -(normal.x*origin.x + normal.y*origin.y + normal.z*origin.z);
}

inline bool ua_plane3d::is_front_facing_to(const ua_vector3d& dir) const
{
   double dot = normal.dot(dir);
   return dot <= 0.0;
}

inline double ua_plane3d::signed_dist_to(const ua_vector3d& point) const
{
   return point.dot(normal) + equation_3;
}
