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
/// \file ConvLoader.hpp
/// \brief conversation loader
//
#pragma once

#include "Base.hpp"

namespace Base
{
   class ResourceManager;
   class Settings;
   class File;
}

namespace Underworld
{
   class ConvGlobals;
}

namespace Conv
{
   class CodeVM;
}

namespace Import
{
   /// Loads conversation globals from bglobals.dat or babglobs.dat
   void LoadConvGlobals(Underworld::ConvGlobals& globals,
      Base::ResourceManager& resourceManager, const char* relativeGlobalsFilename);

   /// Loads conversation code into virtual machine
   /// \param conversationSlot the npc_whoami variable of the NPC to talk to
   bool LoadConvCode(Conv::CodeVM& vm, Base::Settings& settings, Base::ResourceManager& resourceManager,
      const char* cnvArkFilename, Uint16 conversationSlot);

   /// Loads imported functions list
   void LoadConvCodeImportedFunctions(Conv::CodeVM& vm, Base::File& file);

} // namespace Import
