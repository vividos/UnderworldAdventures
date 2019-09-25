//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2004,2019 Underworld Adventures Team
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
/// \file UnderworldLoader.cpp
/// \brief underworld loading
//
#include "pch.hpp"
#include "Underworld.hpp"
#include "LevelImporter.hpp"
#include "PlayerImporter.hpp"
#include "ConvLoader.hpp"

void Import::LoadUnderworld(Base::Settings& settings, Base::ResourceManager& resourceManager, Underworld::Underworld& underworld)
{
   LevelImporter levelImporter{ resourceManager };
   switch (settings.GetGameType())
   {
   case Base::gameUw1:
      if (settings.GetBool(Base::settingUw1IsUwdemo))
         levelImporter.LoadUwDemoLevel(underworld.GetLevelList());
      else
         levelImporter.LoadUw1Levels(underworld.GetLevelList());
      break;

   case Base::gameUw2:
      levelImporter.LoadUw2Levels(underworld.GetLevelList());
      break;
   }

   PlayerImporter playerImport{ resourceManager };
   playerImport.LoadPlayer(underworld.GetPlayer(), "data", true);

   Import::LoadConvGlobals(underworld.GetPlayer().GetConvGlobals(), resourceManager, "data", true);
}
