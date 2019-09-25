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
/// \file Properties.hpp
/// \brief object properties
//
#pragma once

#include <vector>

namespace Underworld
{
   /// \brief Object properties common to all objects
   /// \todo this property table is not complete
   struct CommonObjectProperty
   {
      /// ctor
      CommonObjectProperty()
         : m_height(0),
         m_radius(0),
         m_mass(0),
         m_qualityClass(0),
         m_qualityType(0),
         m_canHaveOwner(false),
         m_canBeLookedAt(false),
         m_canBePickedUp(false),
         m_isContainer(false)
      {
      }

      Uint8 m_height;   ///< object height
      Uint8 m_radius;   ///< object radius
      Uint16 m_mass;    ///< mass of object in 0.1 kg
      Uint8 m_qualityClass;   ///< quality class
      Uint8 m_qualityType;    ///< quality type
      bool m_canHaveOwner;     ///< indicates if object can have owner ("belonging to ...")
      bool m_canBeLookedAt;    ///< indicates if object can be looked at
      bool m_canBePickedUp;    ///< indicates if object can be picked up
      bool m_isContainer;      ///< indicates if object is a container
   };


   /// skill type needed for melee weapon
   enum MeleeSkillType
   {
      meleeSkillNone = 0,    ///< no skill required (may not appear)
      meleeSkillSword = 3,   ///< sword skill applied for attacks
      meleeSkillAxe = 4,     ///< axe skill
      meleeSkillMace = 5,    ///< mace skill
      meleeSkillUnarmed = 6, ///< unarmed fighting skill
   };

   /// Info for melee weapons (IDs 0x0000-0x000f)
   /// \todo this property table is not complete
   struct MeleeWeaponProperty
   {
      /// ctor
      MeleeWeaponProperty()
         :m_damageModifierSlash(0),
         m_damageModifierBash(0),
         m_damageModifierStab(0),
         m_skillType(meleeSkillNone),
         m_durability(0)
      {
      }

      /// damage modifier for attack type slash
      Uint8 m_damageModifierSlash;

      /// damage modifier for attack type bash
      Uint8 m_damageModifierBash;

      /// damage modifier for attack type stab
      Uint8 m_damageModifierStab;

      /// skill type used for melee weapon
      MeleeSkillType m_skillType;

      /// weapon durability
      Uint8 m_durability;
   };

   /// Info for ranged weapons (IDs 0x0010-0x001f)
   /// \todo this property table is not complete
   struct RangedWeaponProperty
   {
      /// ctor
      RangedWeaponProperty()
         :m_durability(0)
      {
      }

      /// weapon durability
      Uint8 m_durability;
   };


   /// \brief Armour category
   /// \details Describes where an item can be worn.
   enum ArmourCategory
   {
      armourNone = 0,      ///< wearable as shield; not worn on paperdoll
      armourBodyArmour = 1,///< body armour
      armourLeggings = 3,  ///< leggings
      armourGloves = 4,    ///< gloves
      armourBoots = 5,     ///< boots
      armourHat = 8,       ///< hat
      armourRing = 9,      ///< ring
   };

   /// Info for armour and wearables (IDs 0x0020-0x003f)
   /// \todo this property table is not complete
   struct ArmourAndWearableProperty
   {
      /// ctor
      ArmourAndWearableProperty()
         : m_protection(0),
         m_durability(0),
         m_category(armourNone)
      {
      }

      Uint8 m_protection;        ///< protection value
      Uint8 m_durability;        ///< armour durability
      ArmourCategory m_category;  ///< armour category (for paperdoll placement)
   };

   /// Info for critters (IDs 0x0040-0x007f)
   /// \todo this property table is not complete
   struct CritterProperty
   {
      /// ctor
      CritterProperty()
         :m_npc_power(0)
      {
      }

      Uint8 m_npc_power; ///< power of NPC
   };


   /// Info for container (IDs 0x0080-0x008f)
   /// \todo this property table is not complete
   struct ContainerProperty
   {
      /// ctor
      ContainerProperty()
         :m_capacity(0),
         m_acceptedObjectClass(-1)
      {
      }

      Uint8 m_capacity; ///< capacity in 0.1 stones (0 means unlimited)
      int m_acceptedObjectClass; ///< object class (-1 means any)
   };


   /// Info for light sources (IDs 0x0090-0x009f)
   struct LightSourceProperty
   {
      /// ctor
      LightSourceProperty()
         :m_brightness(0), m_duration(0) {}

      Uint8 m_brightness;  ///< brightness of light source (4 is max)
      Uint8 m_duration;    ///< duration in hours (0 means unlimited)
   };


   /// Info for animated objects (IDs 0x01c0-0x01cf)
   /// \todo this property table is not complete
   struct AnimatedObjectProperty
   {
      /// ctor
      AnimatedObjectProperty()
         :m_startFrame(0),
         m_numberOfFrames(0)
      {
      }

      Uint8 m_startFrame;     ///< starting frame of animation
      Uint8 m_numberOfFrames; ///< number of frames
   };


   /// Object properties class
   class ObjectProperties
   {
   public:
      /// ctor
      ObjectProperties() {}

      /// returns common object properties for a specific item
      const CommonObjectProperty& GetCommonProperty(Uint16 itemId) const
      {
         UaAssert(itemId <= 0x01ff);
         UaAssert(itemId < m_meleeWeaponPropertiesList.size());
         return m_commonObjectPropertyList[itemId];
      }

      /// returns melee weapon property for a specific item
      const MeleeWeaponProperty& GetMeleeWeaponProperty(Uint16 itemId) const
      {
         UaAssert(itemId <= 0x000f);
         UaAssert(itemId < m_meleeWeaponPropertiesList.size());
         return m_meleeWeaponPropertiesList[itemId];
      }

      /// returns ranged weapon property for a specific item
      const RangedWeaponProperty& GetRangedWeaponProperty(Uint16 itemId) const
      {
         UaAssert(itemId >= 0x0010 && itemId <= 0x001f);
         UaAssert(static_cast<Uint16>(itemId - 0x0010) < m_animatedObjectPropertiesList.size());
         return m_rangedWeaponPropertiesList[itemId - 0x0010];
      }

      /// returns armour and wearables property for a specific item
      const ArmourAndWearableProperty& GetArmourAndWearableProperty(Uint16 itemId) const
      {
         UaAssert(itemId >= 0x0020 && itemId <= 0x003f);
         UaAssert(static_cast<Uint16>(itemId - 0x0020) < m_animatedObjectPropertiesList.size());
         return m_armourAndWearablePropertiesList[itemId - 0x0020];
      }

      /// returns critter property for a specific item
      const CritterProperty& GetCritterProperty(Uint16 itemId) const
      {
         UaAssert(itemId >= 0x0040 && itemId <= 0x007f);
         UaAssert(static_cast<Uint16>(itemId - 0x0040) < m_animatedObjectPropertiesList.size());
         return m_critterPropertiesList[itemId - 0x0040];
      }

      /// returns container property for a specific item
      const ContainerProperty& GetContainerProperty(Uint16 itemId) const
      {
         UaAssert(itemId >= 0x0080 && itemId <= 0x008f);
         UaAssert(static_cast<Uint16>(itemId - 0x0080) < m_animatedObjectPropertiesList.size());
         return m_containerPropertiesList[itemId - 0x0080];
      }

      /// returns light source property for a specific item
      const LightSourceProperty& GetLightSourceProperty(Uint16 itemId) const
      {
         UaAssert(itemId >= 0x0090 && itemId <= 0x09f);
         UaAssert(static_cast<Uint16>(itemId - 0x0090) < m_animatedObjectPropertiesList.size());
         return m_lightSourcePropertiesList[itemId - 0x0090];
      }

      /// returns animated object property for a specific item
      const AnimatedObjectProperty& GetAnimatedObjectProperty(Uint16 itemId) const
      {
         UaAssert(itemId >= 0x01c0 && itemId <= 0x01cf);
         UaAssert(static_cast<Uint16>(itemId - 0x01c0) < m_animatedObjectPropertiesList.size());
         return m_animatedObjectPropertiesList[itemId];
      }

      /// returns vector with common object properties
      std::vector<CommonObjectProperty>& GetCommonObjectPropertiesList() { return m_commonObjectPropertyList; }
      /// returns vector with melee weapon properties
      std::vector<MeleeWeaponProperty>& GetMeleeWeaponPropertiesList() { return m_meleeWeaponPropertiesList; }
      /// returns vector with ranged weapon properties
      std::vector<RangedWeaponProperty>& GetRangedWeaponPropertiesList() { return m_rangedWeaponPropertiesList; }
      /// returns vector with armour and wearables properties
      std::vector<ArmourAndWearableProperty>& GetArmourAndWearablePropertiesList() { return m_armourAndWearablePropertiesList; }
      /// returns vector with critter properties
      std::vector<CritterProperty>& GetCritterPropertiesList() { return m_critterPropertiesList; }
      /// returns vector with container properties
      std::vector<ContainerProperty>& GetContainerPropertiesList() { return m_containerPropertiesList; }
      /// returns vector with light source properties
      std::vector<LightSourceProperty>& GetLightSourcePropertiesList() { return m_lightSourcePropertiesList; }
      /// returns vector with animated object properties
      std::vector<AnimatedObjectProperty>& GetAnimatedObjectProperties() { return m_animatedObjectPropertiesList; }

   private:
      /// common object properties
      std::vector<CommonObjectProperty> m_commonObjectPropertyList;

      /// melee weapon properties
      std::vector<MeleeWeaponProperty> m_meleeWeaponPropertiesList;

      /// ranged weapon properties
      std::vector<RangedWeaponProperty> m_rangedWeaponPropertiesList;

      /// armour and wearables properties
      std::vector<ArmourAndWearableProperty> m_armourAndWearablePropertiesList;

      /// critter properties
      std::vector<CritterProperty> m_critterPropertiesList;

      /// container properties
      std::vector<ContainerProperty> m_containerPropertiesList;

      /// light source properties
      std::vector<LightSourceProperty> m_lightSourcePropertiesList;

      /// animated object properties
      std::vector<AnimatedObjectProperty> m_animatedObjectPropertiesList;
   };

} // namespace Underworld
