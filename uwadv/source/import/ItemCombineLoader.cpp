//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2019 Underworld Adventures Team
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
/// \file ItemCombineLoader.cpp
/// \brief combine items loader
//
#include "pch.hpp"
#include "ResourceManager.hpp"
#include "File.hpp"
#include "Inventory.hpp"

void Import::ImportItemCombineEntries(Base::ResourceManager& resourceManager,
   Underworld::Inventory& inventory)
{
   UaTrace("loading item combine entries\n");

   Base::SDL_RWopsPtr rwops = resourceManager.GetUnderworldFile(Base::resourceGameUw, "data/cmb.dat");
   Base::File file(rwops);

   Uint32 fileLength = file.FileLength();
   unsigned int entries = fileLength / 6;

   for (unsigned int i = 0; i < entries; i++)
   {
      Uint16 data[3];

      for (unsigned int j = 0; j < 3; j++)
         data[j] = file.Read16();

      if (data[0] == 0 || data[1] == 0 || data[2] == 0 ||
         data[0] == 0Xffff || data[1] == 0xffff || data[2] == 0xffff)
         continue;

      bool item1Destroyed = (data[0] & 0x8000) != 0;
      bool item2Destroyed = (data[1] & 0x8000) != 0;

      inventory.AddItemCombineRule(
         data[0] & 0x01ff,
         data[1] & 0x01ff,
         data[2] & 0x01ff,
         (item1Destroyed ? 1 : 0) + (item2Destroyed ? 2 : 0));
   }
}
