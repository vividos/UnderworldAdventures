--
-- Underworld Adventures - an Ultima Underworld hacking project
-- Copyright (c) 2002 Michael Fink
--
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
--
-- $Id$
--

--
-- inventory.lua - lua underworld script; inventory functions
--

-- constants

-- inventory item category constants
inv_cat_normal = 0   -- normal inventory object
inv_cat_ring = 1     -- a ring that can be put on a finger
inv_cat_legs = 2     -- armour that can be worn on the legs
inv_cat_chest = 3    -- armour that can be worn on the chest
inv_cat_hands = 4    -- armour that can be worn on the hands
inv_cat_feet = 5     -- all sorts of boots
inv_cat_head = 6     -- all sorts of helmets/crowns


-- functions

-- categorizes item by item id
function lua_inventory_categorize_item(item_id)

   type = inv_cat_normal

   -- ring item; [0x0033; 0x003a] / { 0x0037 }
   if item_id >= 3*16+3 and item_id <= 3*16+10 and item_id ~= 3*16+7
   then
      type = inv_cat_ring
   end

   -- head item; [0x002c; 0x002e] and [0x0030; 0x0032]
   if (item_id >= 2*16+12 and item_id <= 2*16+14) or
      (item_id >= 3*16+0 and item_id <= 3*16+2)
   then
      type = inv_cat_head
   end

   -- chest item; [0x0020; 0x0022]
   if item_id >= 2*16+0 and item_id <= 2*16+2
   then
      type = inv_cat_chest
   end

   -- hands item; [0x0026; 0x0028]
   if item_id >= 2*16+6 and item_id <= 2*16+8
   then
      type = inv_cat_hands
   end

   -- legs item; [0x0023; 0x0025]
   if item_id >= 2*16+3 and item_id <= 2*16+5
   then
      type = inv_cat_legs
   end

   -- feet item; [0x0029; 0x002b] and { 0x002f }
   if (item_id >= 2*16+0 and item_id <= 2*16+2) or item_id == 2*16+15
   then
      type = inv_cat_feet
   end

   return type
end
