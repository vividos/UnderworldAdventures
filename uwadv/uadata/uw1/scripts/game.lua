--
-- Underworld Adventures - an Ultima Underworld remake project
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
-- game.lua - lua underworld script; general game functions
--

-- constants


-- tables


-- functions

-- called when a new game is started, e.g. after character generation
function game_init_new()

   print("Lua: game_init_new() called")

   local info = player.get_info()
   info.xpos = 32.0
   info.ypos = 2.0
   info.height = 96.0;
   info.angle = 90.0;
   player.set_info(info)

   player.set_attr(player_attr_maplevel,0)

end


--[[
-- replaces all solid tiles with open ones
function fun_replace_tilemap_solids()

   for xpos = 1, 63
   do
      for ypos = 1, 63
      do
         local tile_handle = tilemap_get_tile(-1, xpos, ypos)

         if tilemap_get_type(tile_handle) == tilemap_type_solid
         then
            tilemap_set_type(tile_handle, tilemap_type_open)
            tilemap_set_floor(tile_handle, 0)
         end
      end
   end
end


-- fun function to count underworld paths
function fun_count_underworld_path()

   local length = 0.0

   for level = 0, 7
   do
      for xpos = 1, 63
      do
         for ypos = 1, 63
         do
            local tile_handle = tilemap_get_tile(level, xpos, ypos)

            -- all normal tiles
            if tilemap_get_type(tile_handle) == tilemap_type_open
            then
               length = length + 1.0
            end

            -- all diagonal tiles
            if tilemap_get_type(tile_handle) == tilemap_type_diagonal_se or
               tilemap_get_type(tile_handle) == tilemap_type_diagonal_sw or
               tilemap_get_type(tile_handle) == tilemap_type_diagonal_nw or
               tilemap_get_type(tile_handle) == tilemap_type_diagonal_ne
            then
               length = length + 0.5
            end

            -- all diagonal tiles
            if tilemap_get_type(tile_handle) == tilemap_type_slope_n or
               tilemap_get_type(tile_handle) == tilemap_type_slope_e or
               tilemap_get_type(tile_handle) == tilemap_type_slope_s or
               tilemap_get_type(tile_handle) == tilemap_type_slope_w
            then
               length = length + 1.2
            end

         end
      end
   end

   print ("Lua: length of paths in underworld: " .. floor(length)/1000 ..
      " km or " .. floor(length/1.60935)/1000 .. " miles (and not 25 :)" )
end
--]]


-- called when changing to a new level
function on_change_level()

   -- fun_replace_tilemap_solids()

end


-- called when user clicked on the compass
function ui_clicked_compass()
   -- TODO implement
end

-- called when user clicked on the vitality flask
function ui_clicked_vitality_flask()
   -- TODO implement
end

-- called when user clicked on the mana flask
function ui_clicked_mana_flask()
   -- TODO implement
end

-- called when user clicked on the gargoyle above the 3D window
function ui_clicked_gargoyle()
   -- TODO implement
end

-- called when user clicked on one of the dragons
function ui_clicked_dragons()
   -- TODO implement
end
