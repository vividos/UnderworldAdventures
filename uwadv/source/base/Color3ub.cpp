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
/// \file Color3ub.cpp
/// \brief Color class
//
#include "pch.hpp"
#include "Color3ub.hpp"

/// maximum value of color distance
const double c_maxColorDistance = std::sqrt(255 * 255 + 255 * 255 + 255 * 255);

double Color3ub::CalcColorDistance(const Color3ub& color1, const Color3ub& color2)
{
   double deltaR = (color2.m_rgb[0] - color1.m_rgb[0]);
   double deltaG = (color2.m_rgb[1] - color1.m_rgb[1]);
   double deltaB = (color2.m_rgb[2] - color1.m_rgb[2]);

   double distance = std::sqrt(
      deltaR * deltaR + deltaG * deltaG + deltaB * deltaB);

   return distance / c_maxColorDistance;
}
