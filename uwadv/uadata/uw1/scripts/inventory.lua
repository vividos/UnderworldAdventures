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
lua_inv_cat_normal = 0   -- normal inventory object
lua_inv_cat_ring = 1     -- a ring that can be put on a finger
lua_inv_cat_legs = 2     -- armour that can be worn on the legs
lua_inv_cat_chest = 3    -- armour that can be worn on the chest
lua_inv_cat_hands = 4    -- armour that can be worn on the hands
lua_inv_cat_feet = 5     -- all sorts of boots
lua_inv_cat_head = 6     -- all sorts of helmets/crowns

-- combine result values
lua_inv_cmb_failed = 0       -- items couldn't be combined
lua_inv_cmb_dstr_first = 1   -- first (dropped) item is destroyed
lua_inv_cmb_dstr_second = 2  -- second (dropped) item is destroyed
lua_inv_cmb_dstr_both = 3    -- both the two objects are destroyed


-- globals

inv_cmb_recurse_flag = 0


-- tables

-- table with all possible object combinations
-- objects that are destroyed when combined are marked with (*)
inv_cmb_list = {

   -- misc. objects

   -- 0x0095 + 0x0116(*) = 0x0115
   -- "a lit torch" + "a block of incense" = "a block of burning incense"
   { item_id1  =          9*16 +  5, item_id2 = 1*256 +  1*16 +  6,
     result_id = 1*256 +  1*16 +  5, success_code = lua_inv_cmb_dstr_second },

   -- 0x0095 + 0x00b4(*) = 0x00b7
   -- "a lit torch" + "an ear of corn" = "some_popcorn"
   { item_id1  =          9*16 +  5, item_id2 =         11*16 +  4,
     result_id =         11*16 +  7, success_code = lua_inv_cmb_dstr_second },

   -- 0x011c(*) + 0x00d8(*) = 0x012b
   -- "some_strong thread" + "a pole" = "a fishing pole"
   { item_id1  = 1*256 +  1*16 + 12, item_id2 =         13*16 +  8,
     result_id = 1*256 +  2*16 + 11, success_code = lua_inv_cmb_dstr_both },

   -- key of infinity

   -- 0x00e1(*) + 0x00e2(*) = 0x00e6
   -- "the Key of Truth" + "the Key of Love" = "a two part key"
   { item_id1  =         14*16 +  1, item_id2 =         14*16 +  2,
     result_id =         14*16 +  6, success_code = lua_inv_cmb_dstr_both },

   -- 0x00e1(*) + 0x00e3(*) = 0x00e4
   -- "the Key of Truth" + "the Key of Courage" = "a two part key"
   { item_id1  =         14*16 +  1, item_id2 =         14*16 +  3,
     result_id =         14*16 +  4, success_code = lua_inv_cmb_dstr_both },

   -- 0x00e2(*) + 0x00e3(*) = 0x00e5
   -- "the Key of Love" + "the Key of Courage" = "a two part key"
   { item_id1  =         14*16 +  2, item_id2 =         14*16 +  3,
     result_id =         14*16 +  5, success_code = lua_inv_cmb_dstr_both },

   -- 0x00e1(*) + 0x00e5(*) = 0x00e7
   -- "the Key of Truth" + "a two part key" = "the Key of Infinity"
   { item_id1  =         14*16 +  1, item_id2 =         14*16 +  5,
     result_id =         14*16 +  7, success_code = lua_inv_cmb_dstr_both },

   -- 0x00e2(*) + 0x00e4(*) = 0x00e7
   -- "the Key of Love" + "a two part key" = "the Key of Infinity"
   { item_id1  =         14*16 +  2, item_id2 =         14*16 +  4,
     result_id =         14*16 +  7, success_code = lua_inv_cmb_dstr_both },

   -- 0x00e3(*) + 0x00e6(*) = 0x00e7
   -- "the Key of Courage" + "a two part key" = "the Key of Infinity"
   { item_id1  =         14*16 +  3, item_id2 =         14*16 +  6,
     result_id =         14*16 +  7, success_code = lua_inv_cmb_dstr_both },


   -- stop entry
   { item_id1  = -1, item_id2 = -1, result_id = -1, success_code = 0 },
}


-- functions

-- checks if item_id is a container item
function lua_inventory_is_container(item_id)

   ret = nil

   if item_id >= 8*16 + 0 and item_id <= 8*16 + 14
   then
      ret = 1
   end

   return ret
end


-- categorizes item by item id
function lua_inventory_categorize_item(item_id)

   type = lua_inv_cat_normal

   -- ring item; [0x0033; 0x003a] / { 0x0037 }
   if item_id >= 3*16+3 and item_id <= 3*16+10 and item_id ~= 3*16+7
   then
      type = lua_inv_cat_ring
   end

   -- head item; [0x002c; 0x002e] and [0x0030; 0x0032]
   if (item_id >= 2*16+12 and item_id <= 2*16+14) or
      (item_id >= 3*16+0 and item_id <= 3*16+2)
   then
      type = lua_inv_cat_head
   end

   -- chest item; [0x0020; 0x0022]
   if item_id >= 2*16+0 and item_id <= 2*16+2
   then
      type = lua_inv_cat_chest
   end

   -- hands item; [0x0026; 0x0028]
   if item_id >= 2*16+6 and item_id <= 2*16+8
   then
      type = lua_inv_cat_hands
   end

   -- legs item; [0x0023; 0x0025]
   if item_id >= 2*16+3 and item_id <= 2*16+5
   then
      type = lua_inv_cat_legs
   end

   -- feet item; [0x0029; 0x002b] and { 0x002f }
   if (item_id >= 2*16+9 and item_id <= 2*16+11) or item_id == 2*16+15
   then
      type = lua_inv_cat_feet
   end

   return type
end


-- prints "look" description
function lua_inventory_look(item_id)
   ui_print_string("lua_inventory_look(): look not implemented yet!")
end


-- prints "use" description
function lua_inventory_use(item_id)
   ui_print_string("lua_inventory_use(): look not implemented yet!")
end


-- tries to combine items
function lua_inventory_combine_obj(item_id1, item_id2)

   -- initial return values
   local success_code = lua_inv_cmb_failed
   local result_id = 0

   -- check for all combinable items in table
   idx = 1;
   while inv_cmb_list[idx].item_id1 ~= -1 and success_code == lua_inv_cmb_failed
   do

      if inv_cmb_list[idx].item_id1 == item_id1 and
         inv_cmb_list[idx].item_id2 == item_id2
      then
         -- found the right id
         result_id = inv_cmb_list[idx].result_id
         success_code = inv_cmb_list[idx].success_code
      end

      idx = idx + 1
   end

   -- check for combining objects using swapped item id's
   if success_code == lua_inv_cmb_failed and inv_cmb_recurse_flag ~= 1
   then

      inv_cmb_recurse_flag = 1

      -- check with swapped item id's
      success_code, result_id = lua_inventory_combine_obj(item_id2, item_id1)

      inv_cmb_recurse_flag = 0

      -- revers success code when only one item is destroyed
      if success_code == lua_inv_cmb_dstr_second
      then
         success_code = lua_inv_cmb_dstr_first
      end
   end

   return success_code, result_id
end
