--
-- Underworld Adventures - an Ultima Underworld hacking project
-- Copyright (c) 2002,2003,2004 Underworld Adventures Team
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

--
-- inventory.lua - lua underworld script; inventory functions
--

-- constants

-- combine result values
item_combine_flag_failed = 0       -- items couldn't be combined
item_combine_flag_dstr_first = 1   -- first (dropped) item is destroyed
item_combine_flag_dstr_second = 2  -- second (dropped) item is destroyed
item_combine_flag_dstr_both = 3    -- both the two objects are destroyed


-- globals

item_combine_recurse_flag = 0


-- tables

-- table with all possible object combinations
-- objects that are destroyed when combined are marked with (*)
item_combine_list = {

   -- misc. objects

   -- 0x0095 + 0x0116(*) = 0x0115
   -- "a lit torch" + "a block of incense" = "a block of burning incense"
   { item_id1 = 0x0095, item_id2 = 0x0116, result_id = 0x0115,
     success_code = item_combine_flag_dstr_second },

   -- 0x0095 + 0x00b4(*) = 0x00b7
   -- "a lit torch" + "an ear of corn" = "some_popcorn"
   { item_id1 = 0x0095, item_id2 = 0x00b4, result_id = 0x00b7,
     success_code = item_combine_flag_dstr_second },

   -- 0x011c(*) + 0x00d8(*) = 0x012b
   -- "some_strong thread" + "a pole" = "a fishing pole"
   { item_id1  = 0x011c, item_id2 = 0x00d8, result_id = 0x012b,
     success_code = item_combine_flag_dstr_both },

   -- key of infinity

   -- 0x00e1(*) + 0x00e2(*) = 0x00e6
   -- "the Key of Truth" + "the Key of Love" = "a two part key"
   { item_id1 = 0x00e1, item_id2 = 0x00e2, result_id = 0x00e6,
     success_code = item_combine_flag_dstr_both },

   -- 0x00e1(*) + 0x00e3(*) = 0x00e4
   -- "the Key of Truth" + "the Key of Courage" = "a two part key"
   { item_id1 = 0x00e1, item_id2 = 0x00e3, result_id = 0x00e4,
     success_code = item_combine_flag_dstr_both },

   -- 0x00e2(*) + 0x00e3(*) = 0x00e5
   -- "the Key of Love" + "the Key of Courage" = "a two part key"
   { item_id1 = 0x00e2, item_id2 = 0x00e3, result_id = 0x00e5,
     success_code = item_combine_flag_dstr_both },

   -- 0x00e1(*) + 0x00e5(*) = 0x00e7
   -- "the Key of Truth" + "a two part key" = "the Key of Infinity"
   { item_id1 = 0x00e1, item_id2 = 0x00e5, result_id = 0x00e7,
     success_code = item_combine_flag_dstr_both },

   -- 0x00e2(*) + 0x00e4(*) = 0x00e7
   -- "the Key of Love" + "a two part key" = "the Key of Infinity"
   { item_id1 = 0x00e2, item_id2 = 0x00e4, result_id = 0x00e7,
     success_code = item_combine_flag_dstr_both },

   -- 0x00e3(*) + 0x00e6(*) = 0x00e7
   -- "the Key of Courage" + "a two part key" = "the Key of Infinity"
   { item_id1 = 0x00e3, item_id2 = 0x00e6, result_id = 0x00e7,
     success_code = item_combine_flag_dstr_both },


   -- stop entry
   { item_id1  = -1, item_id2 = -1, result_id = -1, success_code = 0 },
}



-- prints "look" description
function inventory_look(inv_pos)

   print("looking at object in inventory")

   dump_invinfo_table(inv_pos)

   inv_objinfo = inventory.get_info(inv_pos)

   if inv_objinfo.item_id ~= inv_item_none
   then
      look_at_objinfo(inv_objinfo,1)
   end
end


-- prints "use" description
function inventory_use(inv_pos)

   print("using object in inventory")

   dump_invinfo_table(inv_pos)

end


-- tries to combine items
function item_combine(item_id1, item_id2)

   -- initial return values
   local success_code = item_combine_flag_failed
   local result_id = 0

   -- check for all combinable items in table
   idx = 1;
   while item_combine_list[idx].item_id1 ~= -1 and success_code == item_combine_flag_failed
   do

      if item_combine_list[idx].item_id1 == item_id1 and
         item_combine_list[idx].item_id2 == item_id2
      then
         -- found the right id
         result_id = item_combine_list[idx].result_id
         success_code = item_combine_list[idx].success_code
      end

      idx = idx + 1
   end

   -- check for combining objects using swapped item id's
   if success_code == item_combine_flag_failed and item_combine_recurse_flag ~= 1
   then

      item_combine_recurse_flag = 1

      -- check with swapped item id's
      success_code, result_id = lua_inventory_combine_obj(item_id2, item_id1)

      item_combine_recurse_flag = 0

      -- revers success code when only one item is destroyed
      if success_code == item_combine_flag_dstr_second
      then
         success_code = item_combine_flag_dstr_first
      end
   end

   return success_code, result_id
end
