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
-- objects.lua - lua underworld script; object functions
--

-- constants

-- combine result values
lua_obj_cmb_failed = 0       -- items couldn't be combined
lua_obj_cmb_dstr_first = 1   -- first (dropped) item is destroyed
lua_obj_cmb_dstr_second = 2  -- second (dropped) item is destroyed
lua_obj_cmb_dstr_both = 3    -- both the two objects are destroyed

obj_cmb_recurse_flag = 0

-- tables

-- table with all possible object combinations
-- objects that are destroyed when combined are marked with (*)
obj_cmb_list = {

   -- misc. objects

   -- 0x0095 + 0x0116(*) = 0x0115
   -- "a lit torch" + "a block of incense" = "a block of burning incense"
   { item_id1  =          9*16 +  5, item_id2 = 1*256 +  1*16 +  6,
     result_id = 1*256 +  1*16 +  5, success_code = lua_obj_cmb_dstr_second },

   -- 0x0095 + 0x00b4(*) = 0x00b7
   -- "a lit torch" + "an ear of corn" = "some_popcorn"
   { item_id1  =          9*16 +  5, item_id2 =         11*16 +  4,
     result_id =         11*16 +  7, success_code = lua_obj_cmb_dstr_second },

   -- 0x011c(*) + 0x00d8(*) = 0x012b
   -- "some_strong thread" + "a pole" = "a fishing pole"
   { item_id1  = 1*256 +  1*16 + 12, item_id2 =         13*16 +  8,
     result_id = 1*256 +  2*16 + 11, success_code = lua_obj_cmb_dstr_both },

   -- key of infinity

   -- 0x00e1(*) + 0x00e2(*) = 0x00e6
   -- "the Key of Truth" + "the Key of Love" = "a two part key"
   { item_id1  =         14*16 +  1, item_id2 =         14*16 +  2,
     result_id =         14*16 +  6, success_code = lua_obj_cmb_dstr_both },

   -- 0x00e1(*) + 0x00e3(*) = 0x00e4
   -- "the Key of Truth" + "the Key of Courage" = "a two part key"
   { item_id1  =         14*16 +  1, item_id2 =         14*16 +  3,
     result_id =         14*16 +  4, success_code = lua_obj_cmb_dstr_both },

   -- 0x00e2(*) + 0x00e3(*) = 0x00e5
   -- "the Key of Love" + "the Key of Courage" = "a two part key"
   { item_id1  =         14*16 +  2, item_id2 =         14*16 +  3,
     result_id =         14*16 +  5, success_code = lua_obj_cmb_dstr_both },

   -- 0x00e1(*) + 0x00e5(*) = 0x00e7
   -- "the Key of Truth" + "a two part key" = "the Key of Infinity"
   { item_id1  =         14*16 +  1, item_id2 =         14*16 +  5,
     result_id =         14*16 +  7, success_code = lua_obj_cmb_dstr_both },

   -- 0x00e2(*) + 0x00e4(*) = 0x00e7
   -- "the Key of Love" + "a two part key" = "the Key of Infinity"
   { item_id1  =         14*16 +  2, item_id2 =         14*16 +  4,
     result_id =         14*16 +  7, success_code = lua_obj_cmb_dstr_both },

   -- 0x00e3(*) + 0x00e6(*) = 0x00e7
   -- "the Key of Courage" + "a two part key" = "the Key of Infinity"
   { item_id1  =         14*16 +  3, item_id2 =         14*16 +  6,
     result_id =         14*16 +  7, success_code = lua_obj_cmb_dstr_both },


   -- stop entry
   { item_id1  = -1, item_id2 = -1, result_id = -1, success_code = 0 },
}


-- functions

-- tries to combine items
function lua_obj_combine(item_id1, item_id2)

   -- initial return values
   success_code = lua_obj_cmb_failed
   result_id = 0

   -- check for all combinable items in table
   idx = 1;
   while obj_cmb_list[idx].item_id1 ~= -1 and success_code == lua_obj_cmb_failed
   do

      if obj_cmb_list[idx].item_id1 == item_id1 and
         obj_cmb_list[idx].item_id2 == item_id2
      then
         -- found the right id
         result_id = obj_cmb_list[idx].result_id
         success_code = obj_cmb_list[idx].success_code
      end

      idx = idx + 1
   end

   -- check for combining objects using swapped item id's
   if success_code == lua_obj_cmb_failed and obj_cmb_recurse_flag ~= 1
   then

      obj_cmb_recurse_flag = 1

      -- check with swapped item id's
      success_code, result_id = lua_obj_combine(item_id2, item_id1)

      obj_cmb_recurse_flag = 0

      -- revers success code when only one item is destroyed
      if success_code == lua_obj_cmb_dstr_second
      then
         success_code = lua_obj_cmb_dstr_first
      end
   end

   return success_code, result_id
end
