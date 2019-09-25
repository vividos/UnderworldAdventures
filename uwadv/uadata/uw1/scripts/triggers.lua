--
-- Underworld Adventures - an Ultima Underworld remake project
-- Copyright (c) 2002,2003,2004,2005 Underworld Adventures Team
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
-- triggers.lua - trigger handling
--

-- constants

-- trigger item_id's
trig_move    = 0x01a0  -- a_move trigger
trig_pick_up = 0x01a1  -- a_pick up trigger
trig_use     = 0x01a2  -- a_use trigger
trig_look    = 0x01a3  -- a_look trigger
trig_step_on = 0x01a4  -- a_step on trigger
trig_open    = 0x01a5  -- an_open trigger
trig_unlock  = 0x01a6  -- an_unlock trigger

trig_first  = trig_move
trig_last   = trig_unlock

-- tables


-- functions

-- checks a given handle for a given trigger type and sets it off
function trigger_check_handle(trig_id,objpos)

   objinfo = objlist.get_info(objpos)

   -- do we have a special linked object?
   if objinfo.is_quantity ~= 0
   then
      return
   end

   -- check if special linked obj is trigger
   objinfo2 = objlist.get_info(objinfo.quantity)

   if objinfo2.item_id == trig_id and objinfo2.is_quantity == 0
   then
      print( "trigger set off: " .. uw.get_string(4,objinfo2.item_id))

      trap_set_off(objinfo2.quantity,objpos)
   end

end


-- handles all triggers that are set off
function trigger_set_off(objpos)

   dump_objinfo_table(objpos)

   local objinfo = objlist.get_info(objpos)

   if objinfo.item_id < trig_first or objinfo.item_id > trig_last
   then
      return -- no known trigger to set off
   end

   trap_set_off(objinfo.quantity,objpos)

end
