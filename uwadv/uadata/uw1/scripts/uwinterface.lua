--
-- Underworld Adventures - an Ultima Underworld remake project
-- Copyright (c) 2002,2019 Underworld Adventures Team
--
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but he implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
--

--
-- uwinterface.lua - lua underworld script; interface constants
--

-- constants

-- player constants
-- these should be in sync with the enums in Player.hpp

-- attributes
player_attr_gender=0
player_attr_handedness=1
player_attr_appearance=2
player_attr_profession=3
player_attr_maplevel=4
player_attr_strength=5
player_attr_dexterity=6
player_attr_intelligence=7
player_attr_life=8
player_attr_max_life=9
player_attr_mana=10
player_attr_max_mana=11
player_attr_weariness=12
player_attr_hungriness=13
player_attr_poisoned=14
player_attr_mentalstate=15
player_attr_nightvision=16
player_attr_talks=17
player_attr_kills=18
player_attr_level=19
player_attr_exp_points=20
player_attr_difficulty=21

-- skills
player_skill_attack=0
player_skill_defense=1
player_skill_unarmed=2
player_skill_sword=3
player_skill_axe=4
player_skill_mace=5
player_skill_missile=6
player_skill_mana=7
player_skill_lore=8
player_skill_casting=9
player_skill_traps=10
player_skill_search=11
player_skill_track=12
player_skill_sneak=13
player_skill_repair=14
player_skill_charm=15
player_skill_picklock=16
player_skill_acrobat=17
player_skill_appraise=18
player_skill_swimming=19


-- tilemap constants
-- these should be in sync with the enums in Level.hpp

-- tilemap types
tilemap_type_solid = 0
tilemap_type_open = 1
tilemap_type_diagonal_se = 2
tilemap_type_diagonal_sw = 3
tilemap_type_diagonal_nw = 4
tilemap_type_diagonal_ne = 5
tilemap_type_slope_n = 6
tilemap_type_slope_e = 7
tilemap_type_slope_s = 8
tilemap_type_slope_w = 9


-- inventory constants
inventory_slot_no_item = 0xffff
