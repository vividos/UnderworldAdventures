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

-- checks a given handle for triggers and sets it off
function check_handle_trigger(obj_handle)

   objinfo = objlist_get_obj_info(obj_handle)



end


-- handles all triggers that are set off
function lua_trigger_setoff(obj_handle)

   objinfo = objlist_get_obj_info(obj_handle)

   if objinfo.item_id < trig_last or objinfo.item_id > trig_last
   then
      return -- no known trigger to set off
   end

   print( "trigger was set off:\n" )

   dump_objinfo_table(obj_handle)

end
