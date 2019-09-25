//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2003,2004,2005,2006,2019 Underworld Adventures Team
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
/// \file Bits.hpp
/// \brief bit handling
//
#pragma once

#include "Base.hpp"

namespace Import
{
   /// retrieves bits from given value
   inline Uint8 GetBits(Uint8 value, unsigned int start, unsigned int count)
   {
      UaAssert(start < 8 && start + count <= 8);
      return static_cast<Uint8>(value >> start) & static_cast<Uint8>((1 << count) - 1);
   }

   /// retrieves bits from given value
   inline Uint16 GetBits(Uint16 value, unsigned int start, unsigned int count)
   {
      UaAssert(start < 16 && start + count <= 16);
      return static_cast<Uint16>(value >> start) & static_cast<Uint16>((1 << count) - 1);
   }

   /// retrieves bits from given value
   inline Uint32 GetBits(Uint32 value, unsigned int start, unsigned int count)
   {
      UaAssert(start < 32 && start + count <= 32);
      return (value >> start) & ((1 << count) - 1);
   }

} // namespace Import
