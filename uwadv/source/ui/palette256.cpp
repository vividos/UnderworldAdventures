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
/// \file palette256.cpp
/// \brief palette class
//
#include "common.hpp"
#include "palette256.hpp"

Palette256::Palette256()
{
   memset(pal, 0, sizeof(pal));
}

Palette256::Palette256(const Palette256& pal2)
{
   if (&pal2 == this)
      return;

   memcpy(pal, pal2.pal, sizeof(pal));
}

const Palette256& Palette256::operator=(const Palette256& pal2)
{
   if (&pal2 == this)
      return *this;

   memcpy(pal, pal2.pal, sizeof(pal));

   return *this;
}

void Palette256::rotate(Uint8 start, Uint8 len, bool forward)
{
   Uint8 save[4];
   if (forward)
   {
      memcpy(save, &pal[start], sizeof(pal[0]));
      memmove(&pal[start], &pal[start + 1], (len - 1) * sizeof(pal[0]));
      memcpy(pal[start + len - 1], save, sizeof(pal[0]));
   }
   else
   {
      memcpy(save, &pal[start + len - 1], sizeof(pal[0]));
      memmove(&pal[start + 1], &pal[start], (len - 1) * sizeof(pal[0]));
      memcpy(pal[start], save, sizeof(pal[0]));
   }
}
