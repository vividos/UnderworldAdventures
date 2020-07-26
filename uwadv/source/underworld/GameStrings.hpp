//
// Underworld Adventures - an Ultima Underworld remake project
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
/// \file GameStrings.hpp
/// \brief game strings class
//
#pragma once

#include <map>
#include <vector>
#include <string>
#include <set>
#include "Settings.hpp"

namespace Import
{
   class GameStringsImporter;
}

/// \brief Game strings class
/// \details Game strings are contained in blocks that contain a list of strings. Each
/// block contains specific strings, for item descriptions, cutscene text or
/// conversations.
class GameStrings
{
public:
   /// ctor
   GameStrings() {}

   /// returns if block ID is available
   bool IsBlockAvail(Uint16 blockId) const;

   /// returns a whole string block
   const std::vector<std::string>& GetStringBlock(Uint16 blockId) const;

   /// returns a set of all string blocks available
   const std::set<Uint16>& GetStringBlockSet() const
   {
      return m_blockSet;
   }

   /// returns a string from given block
   std::string GetString(Uint16 blockId, size_t stringNumber) const;

private:
   friend Import::GameStringsImporter;

   /// returns a set of all string blocks available
   std::set<Uint16>& GetStringBlockSet()
   {
      return m_blockSet;
   }

   /// returns a set of all string blocks available
   std::map<Uint16, std::vector<std::string>>& GetAllStrings()
   {
      return m_allStrings;
   }

private:
   /// a map with all string blocks
   std::map<Uint16, std::vector<std::string>> m_allStrings;

   /// set with all blocks that are available
   std::set<Uint16> m_blockSet;
};
