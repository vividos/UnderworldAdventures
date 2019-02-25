//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
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
/// \file Palette256.hpp
/// \brief palette class
//
#pragma once

#include <vector>
#include <memory>
#include "Base.hpp"
#include "Settings.hpp"

/// palette with 256 entries, 4 color values per entry
class Palette256
{
public:
   /// ctor
   Palette256();
   /// copy ctor
   Palette256(const Palette256& lhs);
   /// assignment operator
   const Palette256& operator=(const Palette256& lhs);

   /// set value
   void Set(Uint8 index, Uint8 component, Uint8 value)
   {
      m_palette[index][component] = value;
   }

   /// returns pointer to palette data
   Uint8* Get()
   {
      return &m_palette[0][0];
   }

   /// rotates palette indices
   void Rotate(Uint8 start, Uint8 len, bool forward);

protected:
   /// a GL_RGBA compatible palette
   Uint8 m_palette[256][4];
};

/// smart ptr to palette
typedef std::shared_ptr<class Palette256> Palette256Ptr;
