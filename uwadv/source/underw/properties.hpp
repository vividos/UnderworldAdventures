/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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

   \brief object property manager

*/
//! \ingroup underworld

//@{

// include guard
#ifndef uwadv_properties_hpp_
#define uwadv_properties_hpp_

// needed includes
#include <vector>
#include "settings.hpp"


// structs

//! object properties common to all objects
struct ua_common_obj_property
{
   Uint16 radius; //!< object radius
   Uint8 height; //!< object height

   Uint16 mass; //!< mass of object in 0.1 kg

   Uint8 quality_class;
   Uint8 quality_type;

   //! indicates if object can have owner ("belonging to ...")
   bool can_have_owner;

   //! indicates if object can be looked at
   bool can_be_looked_at;

   //! indicates if object can be picked up
   bool can_be_picked_up;

   //! indicates if object is a container
   bool is_container;
};

//! info for melee weapons (ids 0x0000-0x000f)
struct ua_melee_weapon_property
{
   //! damage modifier for attack types
   Uint8 dm_slash,dm_bash,dm_stab;

   //! skill type needed for melee weapon
   Uint8 skill_type;

   //! weapon durability
   Uint8 durability;
};

//! info for ranged weapons (ids 0x0010-0x001f)
struct ua_ranged_weapon_property
{
   //! weapon durability
   Uint8 durability;
};

//! armour category
enum ua_armour_category
{
   ua_armour_none = 0,     //!< also: may not be worn on paperdoll
   ua_armour_body_armour = 1,
   ua_armour_leggings = 3,
   ua_armour_gloves = 4,
   ua_armour_boots = 5,
   ua_armour_hat = 8,
   ua_armour_ring = 9,
};

//! info for armour and wearables (ids 0x0020-0x003f)
struct ua_armour_wearable_property
{
   //! protection value
   Uint8 protection;

   //! armour durability
   Uint8 durability;

   //! armour category (for paperdoll placement)
   ua_armour_category category;
};


// classes

//! object properties class
/*! \todo implement loading all properties */
class ua_object_properties
{
public:
   //! ctor
   ua_object_properties(){}

   //! imports object properties from current uw path
   void import(ua_settings& settings);

   //! returns common object properties about specific object
   inline ua_common_obj_property& get_common_property(Uint16 item_id);

   //! returns properties about armour and wearables
   inline ua_armour_wearable_property& get_armour_property(Uint16 item_id);

protected:
   //! common object properties
   std::vector<ua_common_obj_property> common_properties;

   //! armour and wearables properties
   std::vector<ua_armour_wearable_property> armour_wearable_properties;

   friend class ua_uw_import;
};


// inline methods

inline ua_common_obj_property& ua_object_properties::get_common_property(
   Uint16 item_id)
{
   if (item_id >= common_properties.size())
      item_id = common_properties.size()-1;

   return common_properties[item_id];
}

/*! \todo throw exception when item_id out of range? */
inline ua_armour_wearable_property& ua_object_properties::get_armour_property(
   Uint16 item_id)
{
   item_id-=0x0020;

   if (item_id >= armour_wearable_properties.size())
      item_id = armour_wearable_properties.size()-1;

   return armour_wearable_properties[item_id];
}


#endif
//@}
