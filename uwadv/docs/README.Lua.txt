Underworld Adventures Lua Readme File
-------------------------------------

This file describes all interfaces for the scripting language Lua to the
Underworld Adventures game and it's objects (especially the ua_underworld
object that represents the whold underworld). No knowledge of other
programming languages or internals of the uwadv program is needed.


Contents

1. Introduction
2.0. Reference
2.1. ua_underworld interfaces
2.2. cutscenes control interface
2.3. character creation interface


1. Introduction

Lua is available from http://www.lua.org/.
uwadv currently uses Lua version 4.0.1.

To compile all scripts and eventually package all stuff in the "uadata" into
one resource file, just go into the main project dir and type:

   make data

The Underworld Adventures Resource file uadata00.uar should have been built
then.


2.0. Reference

This is a reference for all functions that can be called from the main program
or from within the Lua script.


2.1. ua_underworld interfaces

The following Lua interface functions provide functionality for the calling
program that should not be hard-coded into the program. In turn the Lua
scripts have access to most underworld objects.

On initialization of the ua_underworld object, all Lua scripts are loaded from
the "uadata" folder. The file "uadata/<game-type>/scripts/ua-scripts.txt"
contains the names of all the scripts to load. They have no ".lua" or ".lob"
extension but a relative path to the "uadata" folder.


2.1.1. Index of callable functions

Lua functions callable from C:

   lua_init_script
   lua_done_script
   lua_game_tick
   lua_obj_look_at
   lua_obj_use
   lua_obj_walk_over
   lua_obj_combine
   lua_obj_get
   lua_obj_can_talk
   lua_obj_can_use
   lua_obj_can_get
   lua_player_hit_floor
   lua_player_hit_wall
   lua_inventory_categorize_item
   lua_track
   lua_sleep
   lua_cast_spell
   lua_savegame_load
   lua_savegame_save

registered C functions to call from Lua:

   player_get_attr
   player_set_attr
   player_get_skill
   player_set_skill
   player_get_pos
   player_set_pos
   player_get_angle
   player_set_angle

   objlist_get_obj_info
   objlist_set_obj_info
   objlist_remove_obj
   objlist_obj_is_npc
   objlist_insert_obj

   tilemap_get_tile
   tilemap_get_type
   tilemap_set_type
   tilemap_get_floor
   tilemap_set_floor
   tilemap_get_automap_visible
   tilemap_set_automap_visible
   tilemap_get_objlist_start

   inventory_rune_avail
   inventory_rune_add

   conv_get_global
   conv_set_global

   ui_print_string
   ui_cursor_use_item
   ui_cursor_target
   ui_start_conv
   ui_show_cutscene
   ui_show_ingame_anim
   ui_get_rand
   ui_persist_put
   ui_persist_get


2.1.2. Functions provided by Lua scripts

* lua_init_script(self)
  return values: none

  the first function that is called when all scripts are loaded. the userdata
  value "self" is passed which is the handle to the underworld object. every
  function that calls a registered C function has to pass the self parameter
  as the first parameter, unless noted otherwise.

* lua_done_script()
  return values: none

  does some cleanup

* lua_game_tick(curtime)
  return values: none

  is called for every game tick for which the underworld Lua scripts should
  run. "curtime" is the current underworld time in seconds since start of the
  underworld object.

* lua_obj_look_at(item_id)
  return values: none

  performs the "look at" action for a given item id.

* lua_obj_use(inv_pos)
  return values: none

  performs the "use" action for a given object. "inv_pos" is the inventory
  itemlist position. generally, only objects that are in the inventory can be
  used with that function. if the object must be used together with another
  object, a "object" cursor is requested.

* lua_obj_walk_over(obj)
  return values: none

  called when the player walks over an object. "obj" is the position in the
  objlist.

* lua_obj_combine(item_id1, item_id2)
  return values: success_code, new_item_id

  tries to combine the two items given by their item_id's. The second item is
  treated as the item dropped onto the first. Possible values for
  "success_code" are:

  - lua_obj_cmb_failed        items couldn't be combined
  - lua_obj_cmb_dstr_first    first (existing) item is destroyed
  - lua_obj_cmb_dstr_second   second (dropped) item is destroyed
  - lua_obj_cmb_dstr_both     both the two objects are destroyed

  in any case but "lua_obj_cmb_failed" the "new_item_id" contains the item id
  of the newly created object.

* lua_obj_get
* lua_obj_can_talk
* lua_obj_can_use
* lua_obj_can_get
* lua_player_hit_floor(speed)
  return values: none

  called when the player hits the floor. "speed" is the speed in
  height_unit/second.

* lua_player_hit_wall

* lua_inventory_categorize_item(item_id)
  return values: category

  categorizes an item in respect to the inventory paperdoll. the return value
  "category" can be one of the following category constants:

  - lua_inv_cat_normal   normal object, cannot be worn on paperdoll
  - lua_inv_cat_ring     a ring that can be put on a finger
  - lua_inv_cat_legs     armour that can be worn on the legs
  - lua_inv_cat_chest    armour that can be worn on the chest
  - lua_inv_cat_hands    armour that can be worn on the hands
  - lua_inv_cat_feet     all sorts of boots
  - lua_inv_cat_head     all sorts of helmets/crowns

* lua_track()
  return values: none

  performs the "track" skill evaluation and prints a string about possible
  critters in the nearby area on the scroll.

* lua_sleep()
  return values: none

  performs sleeping. may start dream cutscenes.

* lua_cast_spell(spell_id)
  return values: none

  starts casting a spell with "spell_id". may request a "target" cursor.

* lua_cast_target(target_obj)
  return values: none

  finishes casting of "target" spells. "target_obj" is the object selected
  with the target cursor.

* lua_savegame_load(version)
  return values: none

  restores lua values from the savegame that is to be loaded. "version"
  indicates the version of the savegame.

* lua_savegame_save()
  return values: none

  saves the lua values that have to be persisted.


2.1.3. Registered C functions

For all functions, the self parameter must be passed.


2.1.3.1. Player access functions

* player_get_attr(self, attr_type)
  return values: attr_value

  returns a specific player attribute. "attr_type" can be one of the
  constants:

  - player_attr_gender            0 means male
  - player_attr_handedness        0 means left-handedness
  - player_attr_appearance        values from 0..4
  - player_attr_profession        values from 0..7

  - player_attr_maplevel          current underworld map level

  - player_attr_strength
  - player_attr_dexterity
  - player_attr_intelligence
  - player_attr_attack
  - player_attr_defense

  - player_attr_life
  - player_attr_max_life
  - player_attr_mana
  - player_attr_max_mana

  - player_attr_weariness
  - player_attr_hungriness
  - player_attr_poisoned

  - player_attr_talks
  - player_attr_kills
  - player_attr_level             current experience level
  - player_attr_exp_points

* player_set_attr(self, attr_type, attr_value)
  return values: none

  sets player attribute. "attr_type" can be one of the constants from
  player_get_attr.

* player_get_skill(self, skill_type)
  return values: skill_value

  - player_skill_unarmed
  - player_skill_sword
  - player_skill_axe
  - player_skill_mace
  - player_skill_missile
  - player_skill_lore
  - player_skill_casting
  - player_skill_traps
  - player_skill_search
  - player_skill_track
  - player_skill_sneak
  - player_skill_repair
  - player_skill_charm
  - player_skill_picklock
  - player_skill_acrobat
  - player_skill_appraise
  - player_skill_swimming

* player_set_skill(self, skill_type, skill_value)
  return values: none

  sets player skill. "skill_type" can be one of the constants from 
  player_get_skill.

* player_get_pos(self)
  return values: xpos, ypos

  returns current player position.

* player_set_pos(self, xpos, ypos)
  return values: none

  sets new player position.

* player_get_angle(self)
  return values: angle

  returns view angle of the player.

* player_set_angle(self, angle)

  sets new view angle.


2.1.3.2. Object list access functions

* objlist_get_obj_info(self, obj_handle)
  return values: item_id, quantity, obj_handle_next, obj_handle_special

  returns all infos about an object in the object list. "obj_handle_next" is a
  handle to the next object, or 0. "obj_handle_special" is an object handle to
  another object related to the object (e.g. contents of NPC's inventory, or a
  door to the given lock).

* objlist_set_obj_info(self, obj_handle, item_id, quantity)
  return values: none

  sets object info for a given object handle

* objlist_remove_obj(self, obj_handle)
  return values: none

  removed object from the object list and unlinks it.

* objlist_obj_is_npc(self, obj_handle)
  return values: is_npc

  determines if an object is a NPC; when no, "is_npc" is nil.

* objlist_insert_obj
  todo


2.1.3.3. Tilemap access functions

* tilemap_get_tile(self, level, xpos, ypos)
  return values: tile_handle

  returns a tile handle for a given level and position. "level" can be -1 and
  is a placeholder for the current level.

* tilemap_get_type(self, tile_handle)
  return values: tile_type

  returns tile type for a given tile handle. "tile_type" can be:

  - tilemap_type_solid
  - tilemap_type_open
  - tilemap_type_diagonal_se
  - tilemap_type_diagonal_sw
  - tilemap_type_diagonal_nw
  - tilemap_type_diagonal_ne
  - tilemap_type_slope_n
  - tilemap_type_slope_e
  - tilemap_type_slope_s
  - tilemap_type_slope_w

* tilemap_set_type(self, tile_handle, tile_type)
  return values: none

  sets tile type. "tile_type" can be one of the values for "tilemap_get_type"

* tilemap_get_floor(self, tile_handle)
  return values: floor_height

  returns floor height.

* tilemap_set_floor(self, tile_handle, floor_height)
  return values: none

  sets floor height.

* tilemap_get_automap_visible(self, tile_handle)
  return values: visible

  returns if a tile is visible on the automap. returns nil when not visible.

* tilemap_set_automap_visible(self, tile_handle, visible)
  return values: none

* tilemap_get_objlist_start(self, tile_handle)
  return values: obj_handle

  returns handle to the first object in object list


2.1.3.4.

todo


2.2. Cutscenes control interface

The controls for all cutscenes are placed into a Lua script, so that the
cutscenes are not hard-coded into C code. The cutscenes script is placed in
"uadata/<game-name>/scripts/cutscene.lua", and must have some functions
defined.

Lua functions to call from C:

* cuts_init(self,cutscene)
  return values: none

  tells the cutscene script the "self" userdata, needed for all calls back to
  the C language. "cutscene" is the number of the cutscene that should be
  shown. Here's a list of all cutscenes:
  
  0: introduction
  1: endgame
  2: ...
  todo

* cuts_tick(time)
  return values: none

  is called for every game tick. Game ticks are constant over time, as opposed
  to rendered frames per second (which might change). "time" contains the
  current time from start of the cutscene animation, in seconds.

registered C functions to call from Lua:

* cuts_do_action(self,actioncode,actionvalue)

  does some action in the cutscene, controlled by the value in "actioncode".
  "actionvalue" can contain additional informations that may be needed for a
  particular action. Here's a list of possible action codes:

  - cuts_finished
    indicates that cutscene is at it's end; the screen immediately gives back
    control to the previous screen, so do some fade-out before doing this
    action.

  - cuts_set_string_block
    sets game string block that should be used for cutscene text. Game strings
    can be extracted from the game with the "strpak" tool. Usually string
    blocks for cutscenes begin at 0x0c00.

  - cuts_sound_play
    starts playing a sound from the "sounds" folder. The actionvalue contains
    the name of the sound file, without the extension ".voc", and without
    paths.

  - cuts_text_set_color
    sets new subtitle text color. "actionvalue" contains the new palette
    index. For text strings, the first game palette is used.

  - cuts_text_fadein
    fades in a subtitle text string; the "actionvalue" contains the string
    number in the current string block.

  - cuts_text_fadeout
    fades out the currently shown text.

  - cuts_text_show
    shows a subtitle text string; almost the same as "cuts_text_fadein", but
    without fading in. The text is shown instantly.

  - cuts_text_hide
    hides the text instantly.

  - cuts_anim_fadein
    fades in a cutscene animation. "actionvalue" contains the animation
    filename, without folder (e.g. "cs401.n01").

  - cuts_anim_show
    instantly shows a cutscene animation. "actionvalue" again contains the
    animation filename.

  - cuts_anim_set_stopframe
    sets a stopframe for the current animation; animation is stopped when the
    frame in "actinvalue" is reached. A negative value disables the stopframe
    feature.

  - cuts_anim_fadeout
    fades out current animation. The animation stops, and the current frame is
    taken for the fadeout.

  - cuts_anim_stop
    stops the animation at the currently shown frame.

  - cuts_anim_hide
    hides the animation instantly.

  - cuts_anim_continue
    continues a stopped animation (either with "cuts_anim_set_stopframe" or
    "cuts_anim_stop". It doesn't clear the stopframe.

The "actionvalue" variable always must be passed to the function. For actions
that doesn't use the value, it can be set to anything.


2.3. Character creation interface
