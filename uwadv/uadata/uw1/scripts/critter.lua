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
-- critter.lua - critter handling
--


-- constants


-- tables


-- functions

-- evaluates critter condition and does actions
function lua_critter_evaluate(obj_handle)

   -- get object infos
   local objinfo = objlist_get_obj_info(obj_handle)

   -- available functions:
   --  critter_find_path(tox,toy,use_water,use_lava,can_fly)
   --  critter_dist_to(obj_handle)
   --  critter_dist_to_player()
   --  critter_set_move(dir,speed)
   --  critter_set_anim(animtype)
   --  critter_get_npcinfo() -> same as objlist_get_obj_info?
   --  critter_attack_mode(bool on)
   --  critter_set_goal(goal) often used(?)

end
