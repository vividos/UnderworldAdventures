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
   lua_track
   lua_sleep

   lua_objlist_default_action
   lua_objlist_talk
   lua_objlist_get
   lua_objlist_look
   lua_objlist_use
   lua_objlist_use_target
   lua_objlist_drag
   lua_objlist_walk_over

   lua_cast_spell
   lua_cast_target

   lua_inventory_is_container
   lua_inventory_categorize_item
   lua_inventory_look
   lua_inventory_use
   lua_inventory_combine_obj

   lua_savegame_load
   lua_savegame_save

   (lua_player_hit_floor)
   (lua_player_hit_wall)

registered C functions to call from Lua:

   player_get_attr
   player_set_attr
   player_get_skill
   player_set_skill
   player_get_pos
   player_set_pos
   player_get_height
   player_set_height
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
   tilemap_get_ceiling
   tilemap_set_ceiling
   tilemap_get_floor_height
   tilemap_get_objlist_start

   inventory_rune_avail
   inventory_rune_add

   quest_get_flag
   quest_set_flag

   conv_is_avail
   conv_get_conv_global
   conv_set_conv_global

   ui_start_conv
   ui_show_cutscene
   ui_print_string
   ui_get_gamestring
   ui_show_ingame_anim
   ui_cursor_use_item
   ui_cursor_target


2.1.2. Functions provided by Lua scripts

* lua_init_script()
  return values: none

  the first function that is called when all scripts are loaded.

* lua_done_script()
  return values: none

  does some cleanup

* lua_game_tick(curtime)
  return values: none

  is called for every game tick for which the underworld Lua scripts should
  run. "curtime" is the current underworld time in seconds since start of the
  underworld object.

* lua_track()
  return values: none

  performs the "track" skill evaluation and prints a string about possible
  critters in the nearby area on the scroll.

* lua_sleep(start)
  return values: success

  performs sleeping. when "start" is positive, sleeping is to be started, else
  sleeping ended. "success" is "nil" when the player cannot sleep for a
  reason. function may start dream cutscenes.

* lua_objlist_default_action(obj_handle)
  return values: none

  performs the "default" action for object with given handle.

* lua_objlist_talk(obj_handle)
  return values: none

  tries to speak with the object with given handle.

* lua_objlist_get(obj_handle)
  return values: none

  tries to pick up object with given handle.

* lua_objlist_look(obj_handle)
  return values: none

  prints the look description for a given object.

* lua_objlist_use(obj_handle)
  return values: none

  uses an object; may call "ui_cursor_use_item" to get a target for using the
  item.

* lua_objlist_use_target(target_obj_handle)
  return values: none

  called when user selected a target object to use the current object on.

* lua_objlist_drag(obj_handle)
  return values: none

  called when user "drags" an object with the mouse.

* lua_objlist_walk_over(obj_handle)
  return values: none

  called when the player walks over an object. "obj" is the position in the
  objlist.

* lua_cast_spell()
  return values: none

  starts casting a spell with the runes on the runeshelf. may request a "target"
  cursor.

* lua_cast_target(target_obj)
  return values: none

  finishes casting of "target" spells. "target_obj" is the object handle for
  the object selected with the target cursor.

* lua_inventory_is_container(item_id)
  return values: is_cont

  determines if given item is a container; returns nil when not

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

* lua_inventory_look(item_pos)
  return values: none

* lua_inventory_use(item_pos)
  return values: none

* lua_inventory_combine_obj(item_id1, item_id2)
  return values: success_code, new_item_id

  tries to combine the two items given by their item_id's. The second item is
  treated as the item dropped onto the first. Possible values for
  "success_code" are:

  - lua_inv_cmb_failed        items couldn't be combined
  - lua_inv_cmb_dstr_first    first (existing) item is destroyed
  - lua_inv_cmb_dstr_second   second (dropped) item is destroyed
  - lua_inv_cmb_dstr_both     both the two objects are destroyed

  in any case but "lua_inv_cmb_failed" the "new_item_id" contains the item id
  of the newly created object.

* lua_savegame_load(savegame,version)
  return values: none

  restores lua values from the savegame that is to be loaded. "version"
  indicates the version of the savegame. "savegame" is a userdata value that
  has to be passed to the savegame_* functions. the function should skip
  loading values that are stored with an earlier version.

* lua_savegame_save(savegame)
  return values: none

  saves the lua values that have to be persisted. "savegame" is a userdata
  value that has to be passed to the savegame_* functions.


2.1.3. Registered C functions


2.1.3.1. Player access functions

* player_get_attr(attr_type)
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

  - player_attr_life
  - player_attr_max_life
  - player_attr_mana
  - player_attr_max_mana

  - player_attr_weariness
  - player_attr_hungriness
  - player_attr_poisoned
  - player_attr_mentalstate       drunk, tripping, etc.; 0 means normal
  - player_attr_nightvision

  - player_attr_talks
  - player_attr_kills
  - player_attr_level             current experience level
  - player_attr_exp_points

  - ua_attr_difficulty            0=easy, 1=normal

* player_set_attr(attr_type, attr_value)
  return values: none

  sets player attribute. "attr_type" can be one of the constants from
  player_get_attr.

* player_get_skill(skill_type)
  return values: skill_value

  - player_skill_attack
  - player_skill_defense
  - player_skill_unarmed
  - player_skill_sword
  - player_skill_axe
  - player_skill_mace
  - player_skill_missile
  - player_skill_mana
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

* player_set_skill(skill_type, skill_value)
  return values: none

  sets player skill. "skill_type" can be one of the constants from
  player_get_skill.

* player_get_pos()
  return values: xpos, ypos

  returns current player position.

* player_set_pos(xpos, ypos)
  return values: none

  sets new player position.

* player_get_height()
  return values: height

  returns current player height

* player_set_height(height)

  sets new player height

* player_get_angle()
  return values: angle

  returns rotation view angle of the player.

* player_set_angle(angle)

  sets new rotation view angle.


2.1.3.2. Object list access functions

Object infos are stored in a table that may look like this:

T = {
   item_id = 0,    -- item ID
   enchanted = 1,  -- 1 if enchanted, 0 if not
   is_link = 1,    -- is "special" field a link?

   zpos = 0,       -- z position (flying critters?)
   dir = 7,        -- direction (0..7, dir*45 degree)
   ypos = 0.2,     -- fractional y position in tile
   xpos = 0.7,     -- fractional x position in tile

   quality = 1,    -- quality value
   handle_next,    -- object handle to next item (or 0 if end of chain)
   owner = 0,      -- owner / special field
   quantity = 0,   -- quantity / link / special property field

   data = {        -- data value array
      size = 2,
      [0] = 1, [1] = 42
   },
}

The "quantity" variable can be a link field for special objects (when is_link
is 1), e.g. for container content or NPC/critter inventory. It also can serve
as special property field (see uw-formats.txt for more).
The "data" array contains misc. data for that particular item type. The
content of these fields are not yet specified.

* objlist_get_obj_info(obj_handle)
  return values: objinfo

  returns all infos about an object in the object list in the table "objinfo".

* objlist_set_obj_info(obj_handle, objinfo)
  return values: none

  sets object infos in table "objinfo" for a given object handle.

* objlist_remove_obj(obj_handle)
  return values: none

  removes object from the object list and unlinks it.

* objlist_obj_is_npc(obj_handle)
  return values: is_npc

  determines if an object is a NPC; when not, "is_npc" is nil.

* objlist_insert_obj(tile_handle, objinfo, xpos, ypos)
  return values: none

  inserts object into a tile given by "tile_handle"; infos about the object is
  in "objinfo", and "xpos" and "ypos" are coordinates relative to the tile
  (i.e. clamped to [0.0 1.0]).


2.1.3.3. Tilemap access functions

* tilemap_get_tile(level, xpos, ypos)
  return values: tile_handle

  returns a tile handle for a given level and position. "level" can be -1 and
  is a placeholder for the current level.

* tilemap_get_type(tile_handle)
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

* tilemap_set_type(tile_handle, tile_type)
  return values: none

  sets tile type. "tile_type" can be one of the values for "tilemap_get_type"

* tilemap_get_floor(tile_handle)
  return values: floor_height

  returns floor height.

* tilemap_set_floor(tile_handle, floor_height)
  return values: none

  sets floor height.

* tilemap_get_ceiling(tile_handle)
  return values: ceiling_height

  returns ceiling height.

* tilemap_set_ceiling(tile_handle, ceiling_height)
  return values: none

  sets ceiling height.

* tilemap_get_floor_height(level, xpos, ypos)
  return values: height

  returns height on a specific tilemap position; xpos and ypos can be
  fractional values

* tilemap_get_objlist_start(tile_handle)
  return values: obj_handle

  returns handle to the first object in object list


2.1.3.4. Inventory access functions

* inventory_rune_avail(rune)
  return values: avail

  returns nil when the given rune is not available. "rune" can be either a
  string where the first char is taken as the rune letter. if "rune" is a
  number, it is used as aphabetical index value. note that there only are
  24 runes, X and Z doesn't exist.

* inventory_rune_add(rune)
  return values: none

  adds rune to the runebag. the parameter "rune" is treated the same as in
  "inventory_rune_avail".

todo


2.1.3.5. Quest flags functions

* quest_get_flag(flag_nr)
  return values: flag_value

  returns a quest flag value by given number.

* quest_set_flag(flag_nr, flag_value)
  return values: none

  sets a quest flag value.


2.1.3.6. Conversation flags functions

* conv_is_avail(slot)
  return values: is_avail

  returns nil if a conversation slot is not available

* conv_get_global(slot,pos)
  return values: value

  retrieves conversation global value, for conv. slot "slot", variable index
  "pos".

* conv_set_global(slot,pos,value)
  return values: none

  sets a conversation global value, for conv. slot "slot", variable index
  "pos", to "value".


2.1.3.7. User Interface callback functions

* ui_start_conv(conv_slot)
  return values: none

  starts conversation with NPC using conversation slot "conv_slot".

* ui_show_cutscene(cutscene)
  return values: none

  shows a cutscene with number "cutscene"

* ui_print_string(text)
  return values: none

  prints a string "text" to the message scroll

* ui_get_gamestring(block, num)
  return values: string

  returns a string from given string block and number

* ui_show_ingame_anim(anim)
  return values: none

  shows ingame animation like death animation, anvil, etc.

* ui_cursor_use_item(item_id)
  return values: none

  changes cursor with image of item with "item_id". a negative value restores
  normal cursor. cancels targetting cursor.

* ui_cursor_target()
  return values: none

  changes mouse cursor to a target cursor, to let the user click on a target.
  call e.g. "lua_cast_target" to finally cast target spells, or call
  "ui_cursor_use_item" to cancel targetting.


2.1.3.8. Savegame functions

* savegame_store_value(savegame,value)
  return values: none

  stores numerical value in savegame. "savegame" is a userdata value from
  either "lua_savegame_save" or "lua_savegame_load". note that 32 bits
  are spent before the decimal place, and 16 bits after the decimal place.

* savegame_restore_value(savegame)
  return values: value

  restores a numerical value from the savegame returned with "value"


2.2. Cutscenes control interface

The controls for all cutscenes are placed into a Lua script, so that the
cutscenes are not hard-coded into C code. The cutscenes script is placed in
"uadata/<game-name>/scripts/cutscene.lua", and must have some functions
defined.

Lua functions to call from C:

* cuts_init(cutscene)
  return values: none

  tells the cutscene script the actual cutscene to play. Here's a list of all
  values for "cutscene":
  
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

* cuts_do_action(actioncode,actionvalue)

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

todo
