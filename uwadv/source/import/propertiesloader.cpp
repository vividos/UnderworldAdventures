//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Michael Fink
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
/// \file propertiesloader.cpp
/// \brief object property loader
//
#include "import.hpp"
#include "resourcemanager.hpp"
#include "file.hpp"
#include "properties.hpp"

using Import::GetBits;
using Underworld::CommonObjectProperty;
using Underworld::EMeleeSkillType;
using Underworld::MeleeWeaponProperty;
using Underworld::RangedWeaponProperty;
using Underworld::EArmourCategory;
using Underworld::ArmourAndWearableProperty;
using Underworld::CritterProperty;
using Underworld::ContainerProperty;
using Underworld::LightSourceProperty;
using Underworld::AnimatedObjectProperty;

/// \todo load jewelry info table
void Import::ImportProperties(Base::ResourceManager& resourceManager,
   Underworld::ObjectProperties& properties)
{
   UaTrace("loading properties\n");

   // import common object properties
   {
      Base::SDL_RWopsPtr rwops = resourceManager.GetUnderworldFile(Base::resourceGameUw, "data/comobj.dat");
      Base::File file(rwops);

      std::vector<CommonObjectProperty>& propertiesList = properties.GetVectorCommonObjectProperties();
      propertiesList.clear();

      unsigned int maxIndex = (file.FileLength() - 2) / 11;
      UaAssert(maxIndex == 0x0200);

      propertiesList.reserve(maxIndex);
      Uint8 data[11];

      for (unsigned int index = 0; index < maxIndex; index++)
      {
         file.ReadBuffer(data, SDL_TABLESIZE(data));

         CommonObjectProperty prop;
         prop.uiRadius = data[1] & 7;
         prop.uiHeight = data[0];
         prop.uiMass = ((data[2] << 8) | data[1]) >> 4;
         prop.uiQualityClass = GetBits(data[6], 2, 2);
         prop.uiQualityType = GetBits(data[10], 0, 4);
         prop.bCanHaveOwner = GetBits(data[7], 7, 1) != 0;
         prop.bCanBeLookedAt = GetBits(data[10], 4, 1) != 0;
         prop.bCanBePickedUp = GetBits(data[3], 5, 1) != 0;
         prop.bIsContainer = GetBits(data[3], 7, 1) != 0;

         propertiesList.push_back(prop);
      }
   }

   // import object properties
   {
      Base::SDL_RWopsPtr rwops = resourceManager.GetUnderworldFile(Base::resourceGameUw, "data/objects.dat");
      Base::File file(rwops);

      // melee weapons table
      {
         file.Seek(2L, Base::seekBegin);

         std::vector<MeleeWeaponProperty>& propertiesList = properties.GetVectorMeleeWeaponProperties();
         propertiesList.clear();
         propertiesList.reserve(0x10);

         Uint8 data[8];
         for (unsigned int i = 0; i < 0x10; i++)
         {
            file.ReadBuffer(data, SDL_TABLESIZE(data));

            MeleeWeaponProperty prop;
            prop.uiDamageModSlash = data[0];
            prop.uiDamageModBash = data[1];
            prop.uiDamageModStab = data[2];
            prop.skillType = static_cast<EMeleeSkillType>(data[6]);
            prop.uiDurability = data[7];

            UaAssert(prop.skillType == Underworld::meleeSkillSword ||
               prop.skillType == Underworld::meleeSkillAxe ||
               prop.skillType == Underworld::meleeSkillMace ||
               prop.skillType == Underworld::meleeSkillUnarmed);

            propertiesList.push_back(prop);
         }
      }

      // ranged weapons table
      {
         file.Seek(0x82L, Base::seekBegin);

         std::vector<RangedWeaponProperty>& propertiesList = properties.GetVectorRangedWeaponProperties();
         propertiesList.clear();
         propertiesList.reserve(0x10);

         Uint8 data[3];
         for (unsigned int i = 0; i < 0x10; i++)
         {
            file.ReadBuffer(data, SDL_TABLESIZE(data));

            RangedWeaponProperty prop;
            prop.uiDurability = data[0];

            propertiesList.push_back(prop);
         }
      }

      // armour and wearables table
      {
         file.Seek(0xb2L, Base::seekBegin);

         std::vector<ArmourAndWearableProperty>& propertiesList = properties.GetVectorArmourAndWearableProperties();
         propertiesList.clear();
         propertiesList.reserve(0x20);

         Uint8 data[4];
         for (unsigned int i = 0; i < 0x20; i++)
         {
            file.ReadBuffer(data, SDL_TABLESIZE(data));

            ArmourAndWearableProperty prop;
            prop.uiProtection = data[0];
            prop.uiDurability = data[1];
            prop.category = static_cast<EArmourCategory>(data[3]);

            UaAssert(prop.category == Underworld::armourNone ||
               prop.category == Underworld::armourBodyArmour ||
               prop.category == Underworld::armourLeggings ||
               prop.category == Underworld::armourGloves ||
               prop.category == Underworld::armourBoots ||
               prop.category == Underworld::armourHat ||
               prop.category == Underworld::armourRing);

            propertiesList.push_back(prop);
         }
      }

      // critters table
      {
         file.Seek(0x0132L, Base::seekBegin);

         std::vector<CritterProperty>& propertiesList = properties.GetVectorCritterProperties();
         propertiesList.clear();
         propertiesList.reserve(0x40);

         Uint8 data[48];
         for (unsigned int i = 0; i < 0x40; i++)
         {
            file.ReadBuffer(data, SDL_TABLESIZE(data));

            CritterProperty prop;
            prop.uiNpcPower = data[5];

            propertiesList.push_back(prop);
         }
      }

      // containers table
      {
         file.Seek(0x0d32L, Base::seekBegin);

         std::vector<ContainerProperty>& propertiesList = properties.GetVectorContainerProperties();
         propertiesList.clear();
         propertiesList.reserve(0x10);

         Uint8 data[3];
         for (unsigned int i = 0; i < 0x10; i++)
         {
            file.ReadBuffer(data, SDL_TABLESIZE(data));

            ContainerProperty prop;
            prop.uiCapacity = data[0];
            prop.iObjectClassAccepted = static_cast<int>(static_cast<Sint8>(data[1]));

            propertiesList.push_back(prop);
         }
      }

      // light source table
      {
         file.Seek(0x0d62L, Base::seekBegin);

         std::vector<LightSourceProperty>& propertiesList = properties.GetVectorLightSourceProperties();
         propertiesList.clear();
         propertiesList.reserve(0x10);

         Uint8 data[2];
         for (unsigned int i = 0; i < 0x10; i++)
         {
            file.ReadBuffer(data, SDL_TABLESIZE(data));

            LightSourceProperty prop;
            prop.uiBrightness = data[0];
            prop.uiDuration = data[1];

            propertiesList.push_back(prop);
         }
      }

      // todo: jewelry info table

      // animation object table
      {
         file.Seek(0x0da2L, Base::seekBegin);

         std::vector<AnimatedObjectProperty>& propertiesList = properties.GetVectorAnimatedObjectProperties();
         propertiesList.clear();
         propertiesList.reserve(0x10);

         Uint8 data[4];
         for (unsigned int i = 0; i < 0x10; i++)
         {
            file.ReadBuffer(data, SDL_TABLESIZE(data));

            AnimatedObjectProperty prop;
            prop.uiStartFrame = data[2];
            prop.uiNumberOfFrames = data[3];

            propertiesList.push_back(prop);
         }
      }
   }
}
