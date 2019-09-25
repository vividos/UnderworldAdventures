//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Underworld Adventures Team
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
/// \file PropertiesLoader.cpp
/// \brief object property loader
//
#include "pch.hpp"
#include "ResourceManager.hpp"
#include "File.hpp"
#include "Properties.hpp"

using Import::GetBits;
using Underworld::CommonObjectProperty;
using Underworld::MeleeSkillType;
using Underworld::MeleeWeaponProperty;
using Underworld::RangedWeaponProperty;
using Underworld::ArmourCategory;
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

      std::vector<CommonObjectProperty>& propertiesList = properties.GetCommonObjectPropertiesList();
      propertiesList.clear();

      unsigned int maxIndex = (file.FileLength() - 2) / 11;
      UaAssert(maxIndex == 0x0200);

      propertiesList.reserve(maxIndex);
      Uint8 data[11];

      for (unsigned int index = 0; index < maxIndex; index++)
      {
         file.ReadBuffer(data, SDL_TABLESIZE(data));

         CommonObjectProperty prop;
         prop.m_radius = data[1] & 7;
         prop.m_height = data[0];
         prop.m_mass = ((data[2] << 8) | data[1]) >> 4;
         prop.m_qualityClass = GetBits(data[6], 2, 2);
         prop.m_qualityType = GetBits(data[10], 0, 4);
         prop.m_canHaveOwner = GetBits(data[7], 7, 1) != 0;
         prop.m_canBeLookedAt = GetBits(data[10], 4, 1) != 0;
         prop.m_canBePickedUp = GetBits(data[3], 5, 1) != 0;
         prop.m_isContainer = GetBits(data[3], 7, 1) != 0;

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

         std::vector<MeleeWeaponProperty>& propertiesList = properties.GetMeleeWeaponPropertiesList();
         propertiesList.clear();
         propertiesList.reserve(0x10);

         Uint8 data[8];
         for (unsigned int i = 0; i < 0x10; i++)
         {
            file.ReadBuffer(data, SDL_TABLESIZE(data));

            MeleeWeaponProperty prop;
            prop.m_damageModifierSlash = data[0];
            prop.m_damageModifierBash = data[1];
            prop.m_damageModifierStab = data[2];
            prop.m_skillType = static_cast<MeleeSkillType>(data[6]);
            prop.m_durability = data[7];

            UaAssert(prop.m_skillType == Underworld::meleeSkillSword ||
               prop.m_skillType == Underworld::meleeSkillAxe ||
               prop.m_skillType == Underworld::meleeSkillMace ||
               prop.m_skillType == Underworld::meleeSkillUnarmed);

            propertiesList.push_back(prop);
         }
      }

      // ranged weapons table
      {
         file.Seek(0x82L, Base::seekBegin);

         std::vector<RangedWeaponProperty>& propertiesList = properties.GetRangedWeaponPropertiesList();
         propertiesList.clear();
         propertiesList.reserve(0x10);

         Uint8 data[3];
         for (unsigned int i = 0; i < 0x10; i++)
         {
            file.ReadBuffer(data, SDL_TABLESIZE(data));

            RangedWeaponProperty prop;
            prop.m_durability = data[0];

            propertiesList.push_back(prop);
         }
      }

      // armour and wearables table
      {
         file.Seek(0xb2L, Base::seekBegin);

         std::vector<ArmourAndWearableProperty>& propertiesList = properties.GetArmourAndWearablePropertiesList();
         propertiesList.clear();
         propertiesList.reserve(0x20);

         Uint8 data[4];
         for (unsigned int i = 0; i < 0x20; i++)
         {
            file.ReadBuffer(data, SDL_TABLESIZE(data));

            ArmourAndWearableProperty prop;
            prop.m_protection = data[0];
            prop.m_durability = data[1];
            prop.m_category = static_cast<ArmourCategory>(data[3]);

            UaAssert(prop.m_category == Underworld::armourNone ||
               prop.m_category == Underworld::armourBodyArmour ||
               prop.m_category == Underworld::armourLeggings ||
               prop.m_category == Underworld::armourGloves ||
               prop.m_category == Underworld::armourBoots ||
               prop.m_category == Underworld::armourHat ||
               prop.m_category == Underworld::armourRing);

            propertiesList.push_back(prop);
         }
      }

      // critters table
      {
         file.Seek(0x0132L, Base::seekBegin);

         std::vector<CritterProperty>& propertiesList = properties.GetCritterPropertiesList();
         propertiesList.clear();
         propertiesList.reserve(0x40);

         Uint8 data[48];
         for (unsigned int i = 0; i < 0x40; i++)
         {
            file.ReadBuffer(data, SDL_TABLESIZE(data));

            CritterProperty prop;
            prop.m_npc_power = data[5];

            propertiesList.push_back(prop);
         }
      }

      // containers table
      {
         file.Seek(0x0d32L, Base::seekBegin);

         std::vector<ContainerProperty>& propertiesList = properties.GetContainerPropertiesList();
         propertiesList.clear();
         propertiesList.reserve(0x10);

         Uint8 data[3];
         for (unsigned int i = 0; i < 0x10; i++)
         {
            file.ReadBuffer(data, SDL_TABLESIZE(data));

            ContainerProperty prop;
            prop.m_capacity = data[0];
            prop.m_acceptedObjectClass = static_cast<int>(static_cast<Sint8>(data[1]));

            propertiesList.push_back(prop);
         }
      }

      // light source table
      {
         file.Seek(0x0d62L, Base::seekBegin);

         std::vector<LightSourceProperty>& propertiesList = properties.GetLightSourcePropertiesList();
         propertiesList.clear();
         propertiesList.reserve(0x10);

         Uint8 data[2];
         for (unsigned int i = 0; i < 0x10; i++)
         {
            file.ReadBuffer(data, SDL_TABLESIZE(data));

            LightSourceProperty prop;
            prop.m_brightness = data[0];
            prop.m_duration = data[1];

            propertiesList.push_back(prop);
         }
      }

      // todo: jewelry info table

      // animation object table
      {
         file.Seek(0x0da2L, Base::seekBegin);

         std::vector<AnimatedObjectProperty>& propertiesList = properties.GetAnimatedObjectProperties();
         propertiesList.clear();
         propertiesList.reserve(0x10);

         Uint8 data[4];
         for (unsigned int i = 0; i < 0x10; i++)
         {
            file.ReadBuffer(data, SDL_TABLESIZE(data));

            AnimatedObjectProperty prop;
            prop.m_startFrame = data[2];
            prop.m_numberOfFrames = data[3];

            propertiesList.push_back(prop);
         }
      }
   }
}
