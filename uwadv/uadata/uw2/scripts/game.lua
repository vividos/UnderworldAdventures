--
-- Underworld Adventures - an Ultima Underworld remake project
-- Copyright (c) 2022 Underworld Adventures Team
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
-- game.lua - lua underworld script; general game functions [uw2]
--

-- called when a new game is started, e.g. after character generation
function game_init_new()

   print("Lua: game_init_new() called")

   local info = player.get_info()
   -- the Avatar's bedroom
   info.xpos = 19.5
   info.ypos = 48.5
   info.height = 96.0;
   info.angle = 90.0;
   player.set_info(info)

   player.set_attr(player_attr_maplevel, 0)

end


-- called when changing to a new level
function on_change_level()

end
