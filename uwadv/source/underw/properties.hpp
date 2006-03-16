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
/*! \file properties.hpp

   \brief object properties

*/

// include guard
#ifndef uwadv_underw_properties_hpp_
#define uwadv_underw_properties_hpp_

// needed includes
#include <vector>

namespace Underworld
{

// structs

//! Object properties common to all objects
/*! \todo this property table is not complete */
struct CommonObjectProperty
{
   //! ctor
   CommonObjectProperty()
      : uiHeight(0),
      uiRadius(0),
      uiMass(0),
      uiQualityClass(0),
      uiQualityType(0),
      bCanHaveOwner(false),
      bCanBeLookedAt(false),
      bCanBePickedUp(false),
      bIsContainer(false)
   {
   }

   Uint8 uiHeight;   //!< object height
   Uint8 uiRadius;   //!< object radius
   Uint16 uiMass;    //!< mass of object in 0.1 kg
   Uint8 uiQualityClass;   //!< quality class
   Uint8 uiQualityType;    //!< quality type
   bool bCanHaveOwner;     //!< indicates if object can have owner ("belonging to ...")
   bool bCanBeLookedAt;    //!< indicates if object can be looked at
   bool bCanBePickedUp;    //!< indicates if object can be picked up
   bool bIsContainer;      //!< indicates if object is a container
};


//! skill type needed for melee weapon
enum EMeleeSkillType
{
   meleeSkillNone=0,    //!< no skill required (may not appear)
   meleeSkillSword=3,   //!< sword skill applied for attacks
   meleeSkillAxe=4,     //!< axe skill
   meleeSkillMace=5,    //!< mace skill
   meleeSkillUnarmed=6, //!< unarmed fighting skill
};

//! Info for melee weapons (IDs 0x0000-0x000f)
/*! \todo this property table is not complete */
struct MeleeWeaponProperty
{
   //! ctor
   MeleeWeaponProperty()
      :uiDamageModSlash(0),
      uiDamageModBash(0),
      uiDamageModStab(0),
      skillType(meleeSkillNone),
      uiDurability(0)
   {
   }

   //! damage modifier for attack type slash
   Uint8 uiDamageModSlash;

   //! damage modifier for attack type bash
   Uint8 uiDamageModBash;

   //! damage modifier for attack type stab
   Uint8 uiDamageModStab;

   //! skill type used for melee weapon
   EMeleeSkillType skillType;

   //! weapon durability
   Uint8 uiDurability;
};

//! Info for ranged weapons (IDs 0x0010-0x001f)
/*! \todo this property table is not complete */
struct RangedWeaponProperty
{
   //! ctor
   RangedWeaponProperty():uiDurability(0){}

   //! weapon durability
   Uint8 uiDurability;
};


//! Armour category
/*! Describes where an item can be worn. */
enum EArmourCategory
{
   armourNone = 0,      //!< wearable as shield; not worn on paperdoll
   armourBodyArmour = 1,//!< body armour
   armourLeggings = 3,  //!< leggings
   armourGloves = 4,    //!< gloves
   armourBoots = 5,     //!< boots
   armourHat = 8,       //!< hat
   armourRing = 9,      //!< ring
};

//! Info for armour and wearables (IDs 0x0020-0x003f)
/*! \todo this property table is not complete */
struct ArmourAndWearableProperty
{
   //! ctor
   ArmourAndWearableProperty()
      : uiProtection(0),
      uiDurability(0),
      category(armourNone)
   {
   }

   Uint8 uiProtection;        //!< protection value
   Uint8 uiDurability;        //!< armour durability
   EArmourCategory category;  //!< armour category (for paperdoll placement)
};

//! Info for critters (IDs 0x0040-0x007f)
/*! \todo this property table is not complete */
struct CritterProperty
{
   //! ctor
   CritterProperty(): uiNpcPower(0){}

   Uint8 uiNpcPower; //!< power of NPC
};


//! Info for container (IDs 0x0080-0x008f)
/*! \todo this property table is not complete */
struct ContainerProperty
{
   //! ctor
   ContainerProperty(): uiCapacity(0), iObjectClassAccepted(-1){}

   Uint8 uiCapacity; //!< capacity in 0.1 stones (0 means unlimited)
   int iObjectClassAccepted; //!< object class (-1 means any)
};


//! Info for light sources (IDs 0x0090-0x009f)
struct LightSourceProperty
{
   //! ctor
   LightSourceProperty(): uiBrightness(0), uiDuration(0){}

   Uint8 uiBrightness;  //!< brightness of light source (4 is max)
   Uint8 uiDuration;    //!< duration in hours (0 means unlimited)
};


//! Info for animated objects (IDs 0x01c0-0x01cf)
/*! \todo this property table is not complete */
struct AnimatedObjectProperty
{
   //! ctor
   AnimatedObjectProperty(): uiStartFrame(0), uiNumberOfFrames(0){}

   Uint8 uiStartFrame;     //!< starting frame of animation
   Uint8 uiNumberOfFrames; //!< number of frames
};



// classes

//! Object properties class
class ObjectProperties
{
public:
   //! ctor
   ObjectProperties(){}

   //! returns common object properties for a specific item
   const CommonObjectProperty& GetCommonProperty(Uint16 uiItemId) const
   {
      UaAssert(uiItemId <= 0x01ff);
      UaAssert(uiItemId < m_vecMeleeWeaponProperties.size());
      return m_vecCommonObjectProperty[uiItemId];
   }

   //! returns melee weapon property for a specific item
   const MeleeWeaponProperty& GetMeleeWeaponProperty(Uint16 uiItemId) const
   {
      UaAssert(uiItemId <= 0x000f);
      UaAssert(uiItemId < m_vecMeleeWeaponProperties.size());
      return m_vecMeleeWeaponProperties[uiItemId];
   }

   //! returns ranged weapon property for a specific item
   const RangedWeaponProperty& GetRangedWeaponProperty(Uint16 uiItemId) const
   {
      UaAssert(uiItemId >= 0x0010 && uiItemId <= 0x001f);
      UaAssert(static_cast<Uint16>(uiItemId-0x0010) < m_vecAnimatedObjectProperties.size());
      return m_vecRangedWeaponProperties[uiItemId-0x0010];
   }

   //! returns armour and wearables property for a specific item
   const ArmourAndWearableProperty& GetArmourAndWearableProperty(Uint16 uiItemId) const
   {
      UaAssert(uiItemId >= 0x0020 && uiItemId <= 0x003f);
      UaAssert(static_cast<Uint16>(uiItemId-0x0020) < m_vecAnimatedObjectProperties.size());
      return m_vecArmourAndWearableProperties[uiItemId-0x0020];
   }

   //! returns critter property for a specific item
   const CritterProperty& GetCritterProperty(Uint16 uiItemId) const
   {
      UaAssert(uiItemId >= 0x0040 && uiItemId <= 0x007f);
      UaAssert(static_cast<Uint16>(uiItemId-0x0040) < m_vecAnimatedObjectProperties.size());
      return m_vecCritterProperties[uiItemId-0x0040];
   }

   //! returns container property for a specific item
   const ContainerProperty& GetContainerProperty(Uint16 uiItemId) const
   {
      UaAssert(uiItemId >= 0x0080 && uiItemId <= 0x008f);
      UaAssert(static_cast<Uint16>(uiItemId-0x0080) < m_vecAnimatedObjectProperties.size());
      return m_vecContainerProperties[uiItemId-0x0080];
   }

   //! returns light source property for a specific item
   const LightSourceProperty& GetLightSourceProperty(Uint16 uiItemId) const
   {
      UaAssert(uiItemId >= 0x0090 && uiItemId <= 0x09f);
      UaAssert(static_cast<Uint16>(uiItemId-0x0090) < m_vecAnimatedObjectProperties.size());
      return m_vecLightSourceProperties[uiItemId-0x0090];
   }

   //! returns animated object property for a specific item
   const AnimatedObjectProperty& GetAnimatedObjectProperty(Uint16 uiItemId) const
   {
      UaAssert(uiItemId >= 0x01c0 && uiItemId <= 0x01cf);
      UaAssert(static_cast<Uint16>(uiItemId-0x01c0) < m_vecAnimatedObjectProperties.size());
      return m_vecAnimatedObjectProperties[uiItemId];
   }

   //! returns vector with common object properties
   std::vector<CommonObjectProperty>& GetVectorCommonObjectProperties(){ return m_vecCommonObjectProperty; }
   //! returns vector with melee weapon properties
   std::vector<MeleeWeaponProperty>& GetVectorMeleeWeaponProperties(){ return m_vecMeleeWeaponProperties; }
   //! returns vector with ranged weapon properties
   std::vector<RangedWeaponProperty>& GetVectorRangedWeaponProperties(){ return m_vecRangedWeaponProperties; }
   //! returns vector with armour and wearables properties
   std::vector<ArmourAndWearableProperty>& GetVectorArmourAndWearableProperties(){ return m_vecArmourAndWearableProperties; }
   //! returns vector with critter properties
   std::vector<CritterProperty>& GetVectorCritterProperties(){ return m_vecCritterProperties; }
   //! returns vector with container properties
   std::vector<ContainerProperty>& GetVectorContainerProperties(){ return m_vecContainerProperties; }
   //! returns vector with light source properties
   std::vector<LightSourceProperty>& GetVectorLightSourceProperties(){ return m_vecLightSourceProperties; }
   //! returns vector with animated object properties
   std::vector<AnimatedObjectProperty>& GetVectorAnimatedObjectProperties(){ return m_vecAnimatedObjectProperties; }

protected:
   //! common object properties
   std::vector<CommonObjectProperty> m_vecCommonObjectProperty;

   //! melee weapon properties
   std::vector<MeleeWeaponProperty> m_vecMeleeWeaponProperties;

   //! ranged weapon properties
   std::vector<RangedWeaponProperty> m_vecRangedWeaponProperties;

   //! armour and wearables properties
   std::vector<ArmourAndWearableProperty> m_vecArmourAndWearableProperties;

   //! critter properties
   std::vector<CritterProperty> m_vecCritterProperties;

   //! container properties
   std::vector<ContainerProperty> m_vecContainerProperties;

   //! light source properties
   std::vector<LightSourceProperty> m_vecLightSourceProperties;

   //! animated object properties
   std::vector<AnimatedObjectProperty> m_vecAnimatedObjectProperties;
};

} // namespace Underworld

#endif
