--
-- Underworld Adventures - an Ultima Underworld hacking project
-- Copyright (c) 2002,2003 Underworld Adventures Team
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
-- traps.lua - trap handling
--

-- constants

-- trap item_id's
trap_damage = 384       -- a_damage trap
trap_teleport = 385     -- a_teleport trap
trap_arrow = 386        -- a_arrow trap
trap_do = 387           -- a_do trap
trap_pit = 388          -- a_pit trap
trap_change_terrain = 389  -- a_change terrain trap
trap_spelltrap = 390    -- a_spelltrap
trap_create_obj = 391   -- a_create object trap
trap_door = 392         -- a_door trap
trap_ward = 393         -- a_ward trap
trap_tell = 394         -- a_tell trap
trap_delete_obj = 395   -- a_delete object trap
trap_inventory = 396    -- an_inventory trap
trap_set_var = 397      -- a_set variable trap
trap_check_var = 398    -- a_check variable trap
trap_combination = 399  -- a_combination trap
trap_text_string = 400  -- a_text string trap

trap_first = trap_damage
trap_last = trap_text_string

-- tables


-- functions

-- sets off effect of a trap
function trap_set_off(obj_handle)

   dump_objinfo_table(obj_handle)

   objinfo = objlist_get_obj_info(obj_handle)

   if objinfo.item_id == trap_damage
   then
      -- a_damage trap

   elseif objinfo.item_id == trap_teleport
   then
      -- a_teleport trap

      local newlevel = player_get_attr(player_attr_maplevel)

      if objinfo.zpos ~= 0
      then
         -- change level
         newlevel = objinfo.zpos-1

         underw_change_level(newlevel)
      end

      player_set_pos(objinfo.quality+0.5, objinfo.owner+0.5)

      player_set_height(
         tilemap_get_floor_height(newlevel, player_get_pos() )+0.1 )

   elseif objinfo.item_id == trap_text_string
   then
      -- a_text string trap
      local str_id = player_get_attr(player_attr_maplevel) * 64 +
         objinfo.owner

      ui_print_string( ui_get_gamestring(9, str_id) )

   else
      print( "set off unknown trap:\n" )
      dump_objinfo_table(obj_handle)
   end

end
