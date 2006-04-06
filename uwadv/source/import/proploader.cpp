/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2002,2003,2004,2005,2006 Michael Fink

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file proploader.cpp

   \brief object property loader

*/

// needed includes
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


/*! \todo load jewelry info table */
void Import::ImportProperties(Base::ResourceManager& resourceManager,
   Underworld::ObjectProperties& properties)
{
   UaTrace("loading properties\n");

   // import common object properties
   {
      Base::SDL_RWopsPtr rwops = resourceManager.GetUnderworldFile(Base::resourceGameUw, "data/comobj.dat");
      Base::File file(rwops);

      std::vector<CommonObjectProperty>& vecProp = properties.GetVectorCommonObjectProperties();
      vecProp.clear();

      unsigned int uiMax = (file.FileLength()-2) / 11;
      UaAssert(uiMax == 0x0200);

      vecProp.reserve(uiMax);
      Uint8 auiData[11];

      for (unsigned int ui=0; ui<uiMax; ui++)
      {
         file.ReadBuffer(auiData, SDL_TABLESIZE(auiData));

         CommonObjectProperty prop;
         prop.uiRadius = auiData[1] & 7;
         prop.uiHeight = auiData[0];
         prop.uiMass = ((auiData[2]<<8) | auiData[1]) >> 4;
         prop.uiQualityClass = GetBits(auiData[6],  2, 2);
         prop.uiQualityType =  GetBits(auiData[10], 0, 4);
         prop.bCanHaveOwner =  GetBits(auiData[7],  7, 1) != 0;
         prop.bCanBeLookedAt = GetBits(auiData[10], 4, 1) != 0;
         prop.bCanBePickedUp = GetBits(auiData[3],  5, 1) != 0;
         prop.bIsContainer =   GetBits(auiData[3],  7, 1) != 0;

         vecProp.push_back(prop);
      }
   }

   // import object properties
   {
      Base::SDL_RWopsPtr rwops = resourceManager.GetUnderworldFile(Base::resourceGameUw, "data/objects.dat");
      Base::File file(rwops);

      // melee weapons table
      {
         file.Seek(2L, Base::seekBegin);

         std::vector<MeleeWeaponProperty>& vecProp = properties.GetVectorMeleeWeaponProperties();
         vecProp.clear();
         vecProp.reserve(0x10);

         Uint8 auiData[8];
         for (unsigned int i=0; i<0x10; i++)
         {
            file.ReadBuffer(auiData, SDL_TABLESIZE(auiData));

            MeleeWeaponProperty prop;
            prop.uiDamageModSlash = auiData[0];
            prop.uiDamageModBash = auiData[1];
            prop.uiDamageModStab = auiData[2];
            prop.skillType = static_cast<EMeleeSkillType>(auiData[6]);
            prop.uiDurability = auiData[7];

            UaAssert(prop.skillType == Underworld::meleeSkillSword ||
               prop.skillType == Underworld::meleeSkillAxe ||
               prop.skillType == Underworld::meleeSkillMace ||
               prop.skillType == Underworld::meleeSkillUnarmed);

            vecProp.push_back(prop);
         }
      }

      // ranged weapons table
      {
         file.Seek(0x82L, Base::seekBegin);

         std::vector<RangedWeaponProperty>& vecProp = properties.GetVectorRangedWeaponProperties();
         vecProp.clear();
         vecProp.reserve(0x10);

         Uint8 auiData[3];
         for (unsigned int i=0; i<0x10; i++)
         {
            file.ReadBuffer(auiData, SDL_TABLESIZE(auiData));

            RangedWeaponProperty prop;
            prop.uiDurability = auiData[0];

            vecProp.push_back(prop);
         }
      }

      // armour and wearables table
      {
         file.Seek(0xb2L, Base::seekBegin);

         std::vector<ArmourAndWearableProperty>& vecProp = properties.GetVectorArmourAndWearableProperties();
         vecProp.clear();
         vecProp.reserve(0x20);

         Uint8 auiData[4];
         for (unsigned int i=0; i<0x20; i++)
         {
            file.ReadBuffer(auiData, SDL_TABLESIZE(auiData));

            ArmourAndWearableProperty prop;
            prop.uiProtection = auiData[0];
            prop.uiDurability = auiData[1];
            prop.category = static_cast<EArmourCategory>(auiData[3]);

            UaAssert(prop.category == Underworld::armourNone ||
               prop.category == Underworld::armourBodyArmour ||
               prop.category == Underworld::armourLeggings ||
               prop.category == Underworld::armourGloves ||
               prop.category == Underworld::armourBoots ||
               prop.category == Underworld::armourHat ||
               prop.category == Underworld::armourRing);

            vecProp.push_back(prop);
         }
      }

      // critters table
      {
         file.Seek(0x0132L, Base::seekBegin);

         std::vector<CritterProperty>& vecProp = properties.GetVectorCritterProperties();
         vecProp.clear();
         vecProp.reserve(0x40);

         Uint8 auiData[48];
         for (unsigned int i=0; i<0x40; i++)
         {
            file.ReadBuffer(auiData, SDL_TABLESIZE(auiData));

            CritterProperty prop;
            prop.uiNpcPower = auiData[5];

            vecProp.push_back(prop);
         }
      }

      // containers table
      {
         file.Seek(0x0d32L, Base::seekBegin);

         std::vector<ContainerProperty>& vecProp = properties.GetVectorContainerProperties();
         vecProp.clear();
         vecProp.reserve(0x10);

         Uint8 auiData[3];
         for (unsigned int i=0; i<0x10; i++)
         {
            file.ReadBuffer(auiData, SDL_TABLESIZE(auiData));

            ContainerProperty prop;
            prop.uiCapacity = auiData[0];
            prop.iObjectClassAccepted = static_cast<int>(static_cast<Sint8>(auiData[1]));

            vecProp.push_back(prop);
         }
      }

      // light source table
      {
         file.Seek(0x0d62L, Base::seekBegin);

         std::vector<LightSourceProperty>& vecProp = properties.GetVectorLightSourceProperties();
         vecProp.clear();
         vecProp.reserve(0x10);

         Uint8 auiData[2];
         for (unsigned int i=0; i<0x10; i++)
         {
            file.ReadBuffer(auiData, SDL_TABLESIZE(auiData));

            LightSourceProperty prop;
            prop.uiBrightness = auiData[0];
            prop.uiDuration = auiData[1];

            vecProp.push_back(prop);
         }
      }

      // todo: jewelry info table

      // animation object table
      {
         file.Seek(0x0da2L, Base::seekBegin);

         std::vector<AnimatedObjectProperty>& vecProp = properties.GetVectorAnimatedObjectProperties();
         vecProp.clear();
         vecProp.reserve(0x10);

         Uint8 auiData[4];
         for (unsigned int i=0; i<0x10; i++)
         {
            file.ReadBuffer(auiData, SDL_TABLESIZE(auiData));

            AnimatedObjectProperty prop;
            prop.uiStartFrame = auiData[2];
            prop.uiNumberOfFrames = auiData[3];

            vecProp.push_back(prop);
         }
      }
   }
}
