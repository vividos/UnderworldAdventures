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
-- game.lua - lua underworld script; general game functions
--

-- constants


-- tables


-- functions

-- called on initing underworld
function lua_init_script()

   print ("Lua: lua_init_script() called")

end

-- called when a new game is started, e.g. after character generation
function lua_started_newgame()

   -- init player position

   player_set_pos(32.0, 2.0)
   player_set_angle(90.0)
   player_set_height(24.0)
   player_set_attr(player_attr_maplevel,0)

   -- player_set_attr(player_attr_gender,1)
   -- player_set_attr(player_attr_appearance,1)

end


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


-- called when underworld is destroyed
function lua_done_script()

   print ("Lua: lua_done_script() called")
end


-- called to load lua values from savegame
function lua_savegame_load(savegame,version)

   -- example:
   -- val = savegame_restore_value(savegame);

end


-- called to load lua values from savegame
function lua_savegame_save(savegame)

   -- example:
   -- savegame_store_value(savegame,42)

end


-- game function called on every tick
function lua_game_tick(curtime)

end



level_pos = {
   [0] = { -- starting positions for level 0
      n = 4,
      pos = {
         { x = 32.5, y = 2.5 },  -- abyss doors
         { x = 29.5, y = 18.5 }, -- human enclave
         { x = 11.5, y = 44.5 }, -- green goblin camp
         { x = 56.5, y = 59.5 }, -- gray goblin camp
      }
   },

   [1] = { -- starting positions for level 1
      n = 3,
      pos = {
         { x = 13.5, y = 8.5 },  -- hall of the mountain king
         { x = 60.5, y = 8.5 },  -- shak
         { x = 45.5, y = 58.5 }, -- evil gray goblins
      }
   },

   [2] = { -- starting positions for level 2
      n = 3,
      pos = {
         { x = 11.5, y = 10.5 }, -- gray lizardmen
         { x = 5.5,  y = 1.5 },  -- starting place (stairs)
         { x = 37.5, y = 30.5 }, -- lizardman lair
      }
   },

   [3] = { -- starting positions for level 3
      n = 3,
      pos = {
         { x = 32.5, y = 52.5 }, -- rodrick's hall
         { x = 8.5,  y = 29.5 }, -- knight's home
         { x = 50.5, y = 22.5 }, -- lakshi longtooth (?)
      }
   },

   [4] = { -- starting positions for level 4
      n = 4,
      pos = {
         { x = 52.5, y = 24.5 }, -- ghouls
         { x = 37.5, y = 12.5 }, -- on a box
         { x =  8.5, y = 2.5 },  -- pac-man
         { x = 34.5, y = 47.5 }, -- level stairs
      }
   },

   [5] = { -- starting positions for level 5
      n = 3,
      pos = {
         { x =  6.5, y = 24.5 },  -- golem test
         { x =  3.5, y = 11.5 },  -- seer's storeroom
         { x = 57.5, y = 31.5 },  -- level stairs
      }
   },

   [6] = { -- starting positions for level 6
      n = 1,
      pos = {
         { x =  6.5, y = 30.5 },  -- prison?
      }
   },

   [7] = { -- starting positions for level 7
      n = 1,
      pos = {
         { x = 23.5, y = 49.5 },  -- monster pit?
      }
   },

   [8] = { -- starting positions for level 8
      n = 1,
      pos = {
         { x = 28.5, y = 24.5 },  -- level start
      }
   },
}


function repos_player(newlevel)

   local newlevel = player_get_attr(player_attr_maplevel)

   local targets = level_pos[newlevel]

   if targets == nil
   then
      -- set player to height of current tile
      player_set_height(
         tilemap_get_floor_height(newlevel, player_get_pos() )+0.1 )

      return
   end

   -- find new random pos
   newpos_idx = random(targets.n)

   local newx = targets.pos[newpos_idx].x
   local newy = targets.pos[newpos_idx].y
   local newz = tilemap_get_floor_height(newlevel, newx, newy)+0.1

   player_set_pos( newx, newy )
   player_set_height( newz )

   print( " changing to level " .. newlevel .. ", player position to x = " ..
      newx .. ", y = " .. newy .. " and z = " .. newz )

end


-- called when changing to a new level
function lua_change_level(newlevel)

   -- fun_replace_tilemap_solids()

end
