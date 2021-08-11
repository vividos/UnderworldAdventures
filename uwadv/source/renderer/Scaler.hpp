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
/// \file Scaler.hpp
/// \brief Image scaler implementation using hqx
//
#pragma once

/// Scaler for 32-bit bitmaps, which uses hqx for scaling.
/// \see https://en.wikipedia.org/wiki/Hqx
class Scaler
{
public:
   /// Scales 32-bit RGBA bitmap using scale factor
   static void Scale(unsigned int scaleFactor, const Uint32* source, Uint32* dest, Uint32 sourceWidth, Uint32 sourceHeight);

   /// Scales 32-bit RGBA bitmap to 2x size; dest must be 4 times the size of source
   static void Scale2x(const Uint32* source, Uint32* dest, Uint32 sourceWidth, Uint32 sourceHeight);

   /// Scales 32-bit RGBA bitmap to 3x size; dest must be 9 times the size of source
   static void Scale3x(const Uint32* source, Uint32* dest, Uint32 sourceWidth, Uint32 sourceHeight);

   /// Scales 32-bit RGBA bitmap to 4x size; dest must be 16 times the size of source
   static void Scale4x(const Uint32* source, Uint32* dest, Uint32 sourceWidth, Uint32 sourceHeight);
};
