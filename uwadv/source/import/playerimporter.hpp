//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2003,2004,2005,2006,2019 Michael Fink
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
/// \file playerimporter.hpp
/// \brief Player import
//
#pragma once

#include "base.hpp"

namespace Base
{
   class ResourceManager;
}

namespace Underworld
{
   class Player;
}

namespace Import
{
   /// imports player
   class PlayerImporter : public Base::NonCopyable
   {
   public:
      /// ctor
      PlayerImporter(Base::ResourceManager& resourceManager)
         :m_resourceManager(resourceManager)
      {
      }

      /// loads player and inventory; path must specify the "data" or a "SaveN" folder
      void LoadPlayer(Underworld::Player& player, const std::string& path, bool initialPlayer);

   private:
      /// resource manager
      Base::ResourceManager& m_resourceManager;
   };

} // namespace Import
