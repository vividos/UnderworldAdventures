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
/// \file Palette256.cpp
/// \brief palette class
//
#include "common.hpp"
#include "Palette256.hpp"

Palette256::Palette256()
{
   memset(m_palette, 0, sizeof(m_palette));
}

Palette256::Palette256(const Palette256& lhs)
{
   if (&lhs == this)
      return;

   memcpy(m_palette, lhs.m_palette, sizeof(m_palette));
}

const Palette256& Palette256::operator=(const Palette256& lhs)
{
   if (&lhs == this)
      return *this;

   memcpy(m_palette, lhs.m_palette, sizeof(m_palette));

   return *this;
}

void Palette256::Rotate(Uint8 start, Uint8 len, bool forward)
{
   Uint8 save[4];
   if (forward)
   {
      memcpy(save, &m_palette[start], sizeof(m_palette[0]));
      memmove(&m_palette[start], &m_palette[start + 1], (len - 1) * sizeof(m_palette[0]));
      memcpy(m_palette[start + len - 1], save, sizeof(m_palette[0]));
   }
   else
   {
      memcpy(save, &m_palette[start + len - 1], sizeof(m_palette[0]));
      memmove(&m_palette[start + 1], &m_palette[start], (len - 1) * sizeof(m_palette[0]));
      memcpy(m_palette[start], save, sizeof(m_palette[0]));
   }
}
