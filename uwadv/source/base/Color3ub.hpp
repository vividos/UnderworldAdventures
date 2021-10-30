//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2021 Underworld Adventures Team
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
/// \file Color3ub.hpp
/// \brief Color class
//
#pragma once

/// an RGB color with unsigned byte type
class Color3ub
{
public:
   /// RGB values; can be used in glColor3ub() calls
   Uint8 m_rgb[3];

   /// ctor; takes individual R G and B values
   Color3ub(Uint8 r, Uint8 g, Uint8 b)
   {
      m_rgb[0] = r;
      m_rgb[1] = g;
      m_rgb[2] = b;
   }

   /// ctor; takes a 0xAARRGGBB value (and ignores alpha channel)
   Color3ub(Uint32 argb)
   {
      m_rgb[0] = (argb >> 16) & 0xFF;
      m_rgb[1] = (argb >> 8) & 0xFF;
      m_rgb[2] = argb & 0xFF;
   }

   /// ctor; takes a pointer to R G and B values
   Color3ub(Uint8* rgb)
   {
      m_rgb[0] = rgb[0];
      m_rgb[1] = rgb[1];
      m_rgb[2] = rgb[2];
   }

   /// left shift assignment operator
   Color3ub& operator <<=(int shift)
   {
      m_rgb[0] <<= shift;
      m_rgb[1] <<= shift;
      m_rgb[2] <<= shift;
      return *this;
   }

   /// right shift assignment operator
   Color3ub& operator >>=(int shift)
   {
      m_rgb[0] >>= shift;
      m_rgb[1] >>= shift;
      m_rgb[2] >>= shift;
      return *this;
   }

   /// calculates color distance of two colors; returns values in [0...1]
   static double CalcColorDistance(const Color3ub& color1, const Color3ub& color2);
};
