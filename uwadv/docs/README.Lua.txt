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


1. Introduction

Lua is available from http://www.lua.org/.
uwadv currently uses Lua version 4.0.1.

To build a compiler that translates the Lua scripts (extension .lua) into
compiled Lua object files (extension .lob), go into the folder
"source/lua/src/luac" and type:

   make

This should work using Linux or mingw32. It produces the program "luac" that
can be placed in any folder in the PATH env variable, or directly into the
"uadata" folder.

To compile all scripts and eventually package all stuff in the "uadata" into
one resource file, just go into the "uadata" folder and type:

   make

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

   player_get_stat
   player_get_attr
   player_get_skill
   player_get_pos
   player_get_angle
   player_set_stat
   player_set_attr
   player_set_skill
   player_set_pos
   player_set_angle

   objlist_get_obj_info

   tilemap_get_tile_info
   tilemap_set_tile_height

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

  the first function that is called when all scripts are loaded. the userdata
  value "self" is passed which is the handle to the underworld object. every
  function that calls a registered C function has to pass the self parameter
  as the first parameter, unless noted otherwise.

* lua_done_script()

  does some cleanup

* lua_game_tick(curtime)

  is called for every game tick for which the underworld Lua scripts should
  run. "curtime" is the current underworld time in seconds since start of the
  underworld object.

* lua_obj_look_at(item_id)

  performs the "look at" action for a given item id.

* lua_obj_use(inv_pos)

  performs the "use" action for a given object. "inv_pos" is the inventory
  itemlist position. generally, only objects that are in the inventory can be
  used with that function. if the object must be used together with another
  object, a "object" cursor is requested. no return value.

* lua_obj_walk_over(obj)

  called when the player walks over an object. "obj" is the position in the
  objlist. no return value.

* lua_obj_combine(item_id1,item_id2)

  tries to combine the two items given by their item_id's. the three return
  values are: "success_code,new_item_id". Possible values for the
  "success_code" are:

  - lua_obj_cmb_failed        items couldn't be combined
  - lua_obj_cmb_dstr_first    first item is destroyed
  - lua_obj_cmb_dstr_second   second item is destroyed
  - lua_obj_cmb_dstr_all      all two objects are destroyed

  in any case but "lua_obj_cmb_failed" the "new_item_id" contains the item id
  of the newly created object.

* lua_obj_get
* lua_obj_can_talk
* lua_obj_can_use
* lua_obj_can_get
* lua_player_hit_floor(speed)

  called when the player hits the floor. "speed" is the speed in
  height_unit/second. no return value.

* lua_player_hit_wall

* lua_inventory_categorize_item(item_id)

  categorizes an item in respect to the inventory paperdoll. the single return
  value can be one of the following category constants:

  - lua_inv_cat_normal   normal object, cannot be worn on paperdoll
  - lua_inv_cat_ring     a ring that can be put on a finger
  - lua_inv_cat_legs     armour that can be worn on the legs
  - lua_inv_cat_chest    armour that can be worn on the chest
  - lua_inv_cat_hands    armour that can be worn on the hands
  - lua_inv_cat_feet     all sorts of boots
  - lua_inv_cat_head     all sorts of helmets/crowns

* lua_track()

  performs the "track" skill evaluation and prints a string about possible
  critters in the nearby area on the scroll. no return value.

* lua_sleep()

  performs sleeping. may start dream cutscenes. no return value.

* lua_cast_spell(spell_id)

  starts casting a spell with "spell_id". may request a "target" cursor. no
  return value.

* lua_cast_target(target_obj)

  finishes casting of "target" spells. "target_obj" is the object selected
  with the target cursor. no return value.

* lua_savegame_load(version)

  restores lua values from the savegame that is to be loaded. "version"
  indicates the version of the savegame.

* lua_savegame_save()

  saves the lua values that have to be persisted. no return value.


2.1.3. Registered C functions

todo


2.2. Cutscenes control interface

The controls for all cutscenes are placed into a Lua script, so that the
cutscenes are not hard-coded into C code. The cutscenes script is placed in
"uadata/<game-name>/scripts/cutscene.lua", and must have some functions
defined.

Lua functions to call from C:

* cuts_init(self,cutscene)

  tells the cutscene script the "self" userdata, needed for all calls back to
  the C language. "cutscene" is the number of the cutscene that should be
  shown. Here's a list of all cutscenes:
  
  0: introduction
  1: endgame
  2: ...
  todo

* cuts_tick(time)

  is called for every game tick. Game ticks are constant over time, as opposed
  to rendered frames per second (which might change). "time" contains the
  current time from start of the cutscene animation, in seconds.

C functions to call from Lua:

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
