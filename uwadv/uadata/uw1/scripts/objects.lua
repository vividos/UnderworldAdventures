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
lua_obj_cmb_failed = 0        -- items couldn't be combined
lua_obj_cmb_dstr_first = 1  -- first (dropped) item is destroyed
lua_obj_cmb_dstr_second = 2  -- second (dropped) item is destroyed
lua_obj_cmb_dstr_both = 3     -- both the two objects are destroyed

obj_cmb_recurse_flag = 0

-- tables


-- functions

-- tries to combine items
function lua_obj_combine(item_id1, item_id2)

   -- initial return values
   success_code = lua_obj_cmb_failed
   result_id = 0

   -- check for all combinable items

   if (item_id1 == 256+2*16+11) and (item_id2 == 256+3*16+11)
   then
      success_code = lua_obj_cmb_dstr_second
      result_id = 256+2*16+10
   end


   -- check for swapped objects
   if success_code == lua_obj_cmb_failed and obj_cmb_recurse_flag ~= 1
   then

     obj_cmb_recurse_flag = 1

     success_code, result_id = lua_obj_combine(item_id2, item_id1)

     obj_cmb_recurse_flag = 0

     if success_code == lua_obj_cmb_dstr_second
     then
        success_code = lua_obj_cmb_dstr_first
     end
   end

   return success_code, result_id
end
