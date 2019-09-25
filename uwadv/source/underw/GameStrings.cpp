//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2019 Underworld Adventures Team
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
/// \file GameStrings.cpp
/// \brief game strings loading implementation
//
#include "pch.hpp"
#include "GameStrings.hpp"

bool GameStrings::IsBlockAvail(Uint16 blockId) const
{
   return m_blockSet.find(blockId) != m_blockSet.end();
}

const std::vector<std::string>& GameStrings::GetStringBlock(Uint16 blockId) const
{
   if (!IsBlockAvail(blockId))
   {
      UaTrace("string block %04x cannot be found\n", blockId);

      static std::vector<std::string> dummyBlock;
      return dummyBlock;
   }

   auto iter = m_allStrings.find(blockId);
   return iter->second;
}

std::string GameStrings::GetString(Uint16 blockId, unsigned int stringNumber) const

{
   if (!IsBlockAvail(blockId))
   {
      UaTrace("string block %04x cannot be found\n", blockId);
      return std::string();
   }

   auto block = GetStringBlock(blockId);

   if (stringNumber < block.size())
      return block[stringNumber];
   else
   {
      UaTrace("string %u in block %04x cannot be found\n", stringNumber, blockId);
      return std::string();
   }
}
