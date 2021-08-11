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
/// \file Scaler.cpp
/// \brief Image scaler implementation using hqx
//
#include "pch.hpp"
#include "Scaler.hpp"

extern "C"
{
#include <hqx.h>
}

/// static class instance to init hqx once
static class ScalerInit
{
public:
   ScalerInit()
   {
      hqxInit();
   }
} s_scalerInit;

/// \param scaleFactor scale factor to use; allowed values are 2, 3 and 4
/// \param source source pixel array, in the RGBA format
/// \param dest destination pixel array, in the RGBA format
/// \param sourceWidth source width
/// \param sourceHeight source height
void Scaler::Scale(unsigned int scaleFactor, const Uint32* source, Uint32* dest, Uint32 sourceWidth, Uint32 sourceHeight)
{
   switch (scaleFactor)
   {
   case 2: Scale2x(source, dest, sourceWidth, sourceHeight); break;
   case 3: Scale3x(source, dest, sourceWidth, sourceHeight); break;
   case 4: Scale4x(source, dest, sourceWidth, sourceHeight); break;
   default:
      UaAssertMsg(false, "invalid scale factor");
   }
}

void Scaler::Scale2x(const Uint32* source, Uint32* dest, Uint32 sourceWidth, Uint32 sourceHeight)
{
   hq2x_32(source, dest, (int)sourceWidth, (int)sourceHeight);
}

void Scaler::Scale3x(const Uint32* source, Uint32* dest, Uint32 sourceWidth, Uint32 sourceHeight)
{
   hq3x_32(source, dest, (int)sourceWidth, (int)sourceHeight);
}

void Scaler::Scale4x(const Uint32* source, Uint32* dest, Uint32 sourceWidth, Uint32 sourceHeight)
{
   hq4x_32(source, dest, (int)sourceWidth, (int)sourceHeight);
}
