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
-- triggers.lua - trigger handling
--

-- constants

-- trigger item_id's
trig_move = 416     -- a_move trigger
trig_pick_up = 417  -- a_pick up trigger
trig_use = 418      -- a_use trigger
trig_look = 419     -- a_look trigger
trig_step_on = 420  -- a_step on trigger
trig_open = 421     -- an_open trigger
trig_unlock = 422   -- an_unlock trigger

trig_first = trig_move
trig_last = trig_unlock

-- tables


-- functions

-- checks a given handle for a given trigger type and sets it off
function trig_check_handle(trig_id,obj_handle)

   objinfo = objlist_get_obj_info(obj_handle)

   -- do we have a special linked object?
   if objinfo.is_quantity ~= 0
   then
      return
   end

   -- check if special linked obj is trigger
   objinfo2 = objlist_get_obj_info(objinfo.quantity)

   if objinfo2.item_id == trig_id and objinfo2.is_quantity == 0
   then
      print( "trigger set off: " .. ui_get_gamestring(4,objinfo2.item_id))

      trap_set_off(objinfo2.quantity,obj_handle)
   end

end


-- handles all triggers that are set off
function lua_trigger_set_off(obj_handle)

   dump_objinfo_table(obj_handle)

   local objinfo = objlist_get_obj_info(obj_handle)

   if objinfo.item_id < trig_first or objinfo.item_id > trig_last
   then
      return -- no known trigger to set off
   end

   trap_set_off(objinfo.quantity,obj_handle)

end
