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
-- game.lua - lua underworld script; general game functions
--

-- constants


-- tables


-- functions

-- called on initing underworld
function lua_init_script(this)

   -- store "self" userdata for reference
   self = this

   print ("lua_init_script() called")

   -- init player position

   player_set_pos(self, 32.0, 2.0)
   player_set_angle(self, 90.0)
   player_set_attr(self,player_attr_gender,1)
   player_set_attr(self,player_attr_appearance,1)

end


-- called when underworld is destroyed
function lua_done_script()

   print ("lua_done_script() called")

end


-- game function called on every tick
function lua_game_tick(curtime)

end
