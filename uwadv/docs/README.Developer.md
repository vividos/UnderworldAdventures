# README for Developers

This is the Readme file for developers. Please use the table of contents to
navigate the document.

[1. Introduction](#1-introduction)<br/>

[2. Compiling Underworld Adventures](#2-compiling-underworld-adventures)<br/>
[2.1 Microsoft Visual Studio on Windows](#2-1-microsoft-visual-studio-on-windows)<br/>

[3. Developing Underworld Adventures](#3-developing-underworld-adventures)<br/>
[3.1 Code guidelines](#3-1-code-guidelines)<br/>
[3.2 Source code repository](#3-2-source-code-repository)<br/>
[3.2.1 Git repository](#3-2-1-git-repository)<br/>
[3.2.2 GitHub issues & pull requests](#3-2-2-github-issues-pull-requests)<br/>
[3.3 Release Checklist](#3-3-release-checklist)<br/>
[3.4 Source code documentation](#3-4-source-code-documentation)<br/>
[3.5 Resource files in the "data" folder](#3-5-resource-files-in-the-data-folder)<br/>

[4. Lua Scripting](#4-lua-scripting)<br/>
[4.1 Lua functions that are called from the game](#4-1-lua-functions-that-are-called-from-the-game)<br/>
[4.2 Game functions that can be called from Lua](#4-2-game-functions-that-can-be-called-from-lua)<br/>
[4.3 Cutscenes control interface](#4-3-cutscenes-control-interface)<br/>
[4.4 Character creation interface](#4-4-character-creation-interface)<br/>

[A. Authors](#a-authors)<br/>

[B. Third-Party libraries and licenses](#b-third-party-libraries-and-licenses)<br/>


## 1. Introduction

This document is an introduction for developers who want to compile from
source code, just want to understand the source code or even want to start
coding for the Underworld Adventures project. The document describes all that
a developer has to know. For further reference the source code documentation
can be used.

Underworld Adventures is developed entirely in C++, with a few external
libraries. Lua is used as the scripting language to use. For more on Lua
scripting and the interfaces to the game, please check out the chapter
"Lua Scripting".


## 2. Compiling Underworld Adventures

There are several platforms on which Underworld Adventures can be built and
run.

### 2.1 Microsoft Visual Studio on Windows

Underworld Adventures can be compiled using the latest Visual Studio version
from Microsoft. As of writing this document, Visual Studio 2019 is the
current version. Any later version should also work. Either Community or
Professional version should work. Install the "C++" workload.

Open the "uwadv.sln" file located in the "uwadv" folder. In the project
workspace, you will see several projects, organized by some solution folders.
Set the "uwadv" project as startup project and select either Debug or Release
configuration. Compile all projects using "Build > Build Solution" or start
debugging with F5, which also builds the relevant projects.

All output files are stored in the "output" folder directly under the base
folder. You see a "bin" folder where the resulting binary files are placed,
an "intermediate" and "lib" folders with built files. The folders are further
subdivided in "Debug" and "Release" folders.

In order to debug the project directly in its created "bin" folder, go to the
"uwadv" project's properties, and in the "Debugging" section set the value for
"Working Directory" to "$(OutDir)" (without the double quotes). This way the
game can find its uwadv.cfg and related resources. A default uwadv.cfg file is
copied to the folder as well, if it doesn't exist yet.

More information about running Underworld Adventures can be found in the file
["README.Manual.md"](README.Manual.md).

### 2.2 CMake on Windows, Linux and macOS

Underworld Adventures can be compiled using the CMake build system, which
supports several platforms. The CMake build system supersedes the automake
based build system and the MinGW makefiles. The minimum CMake version needed
currently is 3.15.


## 3. Developing Underworld Adventures

This chapter contains infos about developing uwadv further.

## 3.1 Code guidelines

Here are some code guidelines for the uwadv project that every developer
should follow when commiting code via Git:

- tabs should be converted to spaces, indendation size is 3 spaces; don't
  reindent code that wasn't changed by you
- always document your code, use doxygen-style comments for functions and
  classes
- don't import whole namespaces, e.g. with `using namespace std;`; instead
  use e.g. `std::string` instead
- use C++ style casts when possible
- use `std::ostringstream` instead of `sprintf()`
- use `T* var` and `T& var` for pointers and references (not `T *var` or
  `T &var`)
- throw exceptions on serious errors (e.g. when you absolutely can't
  continue)
- use SDL functionality where possible, to stay platform independent
- use SDL data types, such as `Uint32`, `Uint16` when needed
- don't accumulate all your changes in one Git commit, separate commits
  that doesn't have something in common

## 3.2 Source code repository

The source code and related files for Underworld Adventures are stored in a
Git repository that is hosted on GitHub.com. You can clone the repository from
the official site:

   https://github.com/vividos/UnderworldAdventures

There are some Git GUIs for Windows; I recommend:

- GitExtensions: https://gitextensions.github.io/

  The tool shows a window with the currently opened repository and the commit
  history. All necessary commands are available as (context) menu items.

### 3.2.1 Git repository

Some guidelines:

- Development happens on the `main` branch.
- All pull requests (PRs) should be based on `main` as well.
- Tag names for versions are in the format `x.x-codename`, e.g.
  `0.9-mojito`.
- Please set Unix-to-DOS line ending option `core.autolf` to `true`.
- You can use branches in PRs, but rebase them on `main` before requesting the
  PR.

### 3.2.2 GitHub issues & pull requests

For bug reports or enhancement wishes, please write a GitHub issue first. Also
write an issue if you want to work on any part of the project, or comment on
an existing ticket. Only then work on the bug or feature, and then submit a
PR.

Pull requests must contain commits that are clear and can be understood from
only reading the diff or by additionally reading the commit text. Be verbose
with the commit text, if needed. Pack code reformatting or code restructuring
in a separate commit, or even a separate pull request if it's large. Follow
the coding guidelines.

If you want to add small amounts of code, or want to make small additions to
documentation (e.g. the [uw-formats.txt](uw-formats.txt) file), just open an
issue and I'll add the change manually. You can also send an email to me;
visit the [Development](https://vividos.github.io/UnderworldAdventures/development.html)
page on how to contact the team.

## 3.3 Release Checklist

Here is the release checklist of steps to do for a successful Underworld
Adventures release:

1. Think about the next funny release name

2. Be sure that the project (uwadv, tools) compiles AND works on all
   systems where build files exist. Be sure to test against `uw1` and
   `uw_demo`. Build systems to test for are: Visual Studio, CMake.

3. Run all unit tests and run the SonarCloud analysis and fix all reported
   bugs

4. Adjust version numbers in the following files:
   - source/doxygen/doxygen.cfg
   - source/version.hpp

5. Update release notes in [README.Manual.md](README.Manual.md) file

6. Add and push a Git tag to the repository, with version and release name as
   tag name.

7. Build the Windows and Linux versions of the source and binary packages,
   using the AppVeyor build service.

8. Upload built packages to GitHub, as a new release

9. Update Underworld Adventures home page with latest news

## 3.4 Source code documentation

The Underworld Adventures source code is commented using doxygen-style
comments for modules, files, classes, class members, structs, enums and
typedefs. The source code documentation generated via doxygen can
be used as reference to understand the source code.

The source code documentation can be generated with the following command
(change into the `uwadv/doxygen´ folder before):

    doxygen doxygen.cfg

If you're using Visual Studio, you can also build the "doxygen" project. The
output of the tools is automatically listed in the "Error List" window.

The HTML documentation is created in the folder "html", where "index.html"
is the entry point to the source code documentation. The file
`doxygen-warnings.txt` contains the program's warnings.

doxygen is available from this site:

    http://www.doxygen.org/

You can install the "dot" tool to generate fancy include, dependency and
collaboration graphs. The "dot" tool is available at:

    http://www.graphviz.org/

Be sure to download the "dot" tool and the associated "third-party"
libraries. Extract the dot.exe and the libraries into the doxygen "bin"
folder.

## 3.5 Resource files in the "data" folder

The "data" folder contains files that didn't came with the original game
files of Ultima Underworld, but are needed for the game to run. They can
contain enhancements to graphics, new audio files, game scripts or new 3D
models.

The "Underworld Adventures Resource" file (file extension `.zip`) package the
contents of the "uadata" source tree into one or more files. Name scheme looks
like this: `uadataXX.zip` (printf syntax: `uadata%02u.zip`).

When the game searches for a "uadata" file, e.g. `uw1/sound/test.wav`, it
first searches the actual folder, `uadata/uw1/sound/test.wav`. If that
doesn't exist, all `uadata??.zip` files are tested if the file is in one of
them. When the file isn't found at all, an exception is thrown.

The Makefile in the "uadata" source folder creates packaged files that have
all the files from the whole uadata tree in one file. Just do a

    make all

to build all packaged files that are needed for distribution. For
developing and debugging, the raw files can also be used.

Lua scripts can have the extension `.lua` or `.lob`. The `.lua` files
contain Lua scripts as text, i.e. not precompiled. `.lob` files are
`lua object` files, Lua scripts that were compiled with the `luac` tool.

Lua scripts are searched in the same way as other files, but first the file
with the `.lua` file extension is searched, then the one with `.lob`. That
allows for changes to `.lua` files while developing. For releases the
compiled `.lob` files inside `uadata??.zip` files are distributed only.


## 4. Lua Scripting

Underworld Adventures uses Lua a scripting language to implement the actual
game logic. Together with the "Underworld Adventures Debugger" this allows for
more flexibility in coding and debugging the game. This chapter describes the
interfaces that Lua scripts can use get and modify game data.

The Lua scripts are usually loaded into the game engine when a new game is
started, e.g. by creating a new character or when loading a save game. Based
on the game prefix (e.g. "uw1" or "uw"), the file
`uadata??.zip/<game-type>/game.cfg` is processed and all Lua scripts specified
with the `load-script` command is loaded.

The two scripts `createchar.lua` and `cutscene.lua` are special scripts used
for character creation and and showing cutscenes and are described in later
chapters.

In this document Lua functions and C functions that are callable from Lua are
listed in this syntax:

    ReturnType Table.FunctionName(ParamType param_name, ParamType, param_name)

When the function doesn't return anything, `ReturnType` is omitted.
When the function isn't stored in a table, `Table` is omitted.
`ParamType` and `ReturnType` can either be Lua types (nil, Integer, Number,
Boolean, Table, etc.) or can be uwadv-specific data types, e.g. tables or
constants. When `Various`is listed, the type of the value depends on the
function.

### 4.1 Lua functions that are called from the game

The game calls the following Lua functions on certain points in the game.
Here's an overview of all functions:

- `game_init_new()`:
  Called by the game when a new game is initialized or a savegame was loaded.

- `on_change_level(Integer new_level)`:
  Called when the player changes to a new underworld level.

- `critter_eval(Integer objectlist_pos)`:
  Evaluates a critter and his actions, e.g. wandering around, combat, chasing
  player or fleeing.

- `trigger_set_off(Integer objectlist_pos)`:
  Called by the game when the player set off a trigger, e.g. a "move trigger".
  The function is only called once when the player is in the trigger's
  distance.

- `object_look(Integer objectlist_pos)`:
  Called when the user looks at an object in the objectlist. Prints the look
  description to the scroll.

- `inventory_look(Integer inventory_pos)`:
  Called when the user looks at an object in the inventory.

- `look_at_wall(Integer texture_id)`:
  Called when the user looks at a wall; the texture ID is passed.

- `object_use(Integer objectlist_pos)`:
  Called when the user tries to use an object. This may start cutscenes,
  ingame animations, a target cursor or other effects.

- `inventory_use(Integer inventory_pos)`:
  Called when the user tries to use an object in the inventory.

- `wall_use(Integer texture_id)`:
  Called when the user tries to "use" a wall; the texture ID is passed.

- `object_get(Integer objectlist_pos)`:
  Called when user tries to pick up object; may call
  `inventory.float_add_item()` to actually pick up the object.

- `object_talk(Integet objectlist_pos)`:
  Tries to speak with the NPC in the given objectlist position.

- `spell_cast()`:
  Starts casting a spell with the runes on the runeshelf. May request a
  target cursor by calling `uw.cursor_target()`.

- `spell_cancel(Integer runeshelf_index)`:
  Cancels a currently active spell displayed on the runeshelf. The index of
  the spell is given.
  
- `ui_clicked_compass()`:
  Called when the user clicked on the compass. Prints game time and how hungry
  the player is.

- `ui_clicked_vitality_flask()`:
  Called when the user clicked on the vitality flask. Prints the current and
  max. vitality value to the scroll.

- `ui_clicked_mana_flask()`:
  Called when the user clicked on the mana flask. Prints the current and max.
  mana value to the scroll.

- `ui_clicked_gargoyle()`:
  Called when the user clicked on the gargoyle eyes above the 3D view. Prints
  the vitality status of the currently targeted NPC.

- `ui_clicked_dragons()`:
  Called when the user clicked on one of the dragons, left or right of the 3D
  view; only for uw1 games.

- `track_creatures()`:
  Performs the "track" skill evaluation and prints a string about possible
  critters in the nearby area on the scroll.

- `Boolean sleep(Boolean is_started)`:
  Called when the user starts sleeping or the sleeping ended (e.g. after
  showing a cutscene). The function returns false when the player cannot sleep
  for a reason. The function may start dream cutscenes by calling
  `uw.show_cutscene()`.

- `target_selected(Integer target_objectlist_pos)`:
  Called when the user selected a target. This can be the result of casting a
  targeted spell. `target_objectlist_pos` is the objectlist position of the
  object that was selected with the target cursor.


### 4.2 Game functions that can be called from Lua

There are several groups of functions that can be called from Lua scripts.
They are organized in tables. The following chapters list all functions.

#### Table `uw`

This table contains common game-related functions:

- `uw.print(String text)`:
  Prints text to the scroll at the bottom of the screen

- `uw.get_string(Integer block, Integer number)`:
  Returns a string from the game strings

- `uw.change_level(Integer newLevel)`:
  Instantly changes to the given underworld level

- `uw.start_conv(Integer npc_obj_pos)`:
  Starts conversation with NPC at given object list position

- `uw.show_cutscene(Integer cutscene_number)`:
  Shows cutscene with given number

- `uw.show_map()`:
  Shows the map view

- `uw.show_ingame_anim(Integer ingame_anim_number)`:
  Shows ingame animation with given number, e.g. anvil or abyss window

- `uw.cursor_use_item(Ìnteger item_id)`:
  Changes cursor with image of item with "item_id". A negative value restores
  the normal cursor. Cancels ther targetting cursor.

- `uw.cursor_target()`:
  Changes mouse cursor to a target cursor, to let the user click on a target.
  When the user finally clicked on a target, TODO
  Calling `uw.cursor_use_item()" cancels targetting.

#### Table `player`

Contains player related functions:

- `Player player.get_info()`:
  Gets player infos as table `Player`. The table looks as follows:

      Player = {
        name = "Gronkey",
        xpos = 32.4,   -- decimal x position
        ypos = 3.1,    -- decimal x position
        height = 96.0, -- height in tilemap
        angle = 90.0,  -- viewing angle, in degrees
      }

- `player.set_info(Player player)`:
  Sets player values from table `Player`; all values from the table are set,
  including the name.

- `Integer player.get_attr(PlayerAttribute attr_type)`:
  Gets single player attribute value. `PlayerAttribute` can be one of these
  values:

  constant                   | remark
  --------                   | ------
  `player_attr_gender`       | 0 means male, 1 female
  `player_attr_handedness`   | 0 means left-handedness
  `player_attr_appearance`   | values from 0..4
  `player_attr_profession`   | values from 0..7
  `player_attr_maplevel`     | current underworld map level
  `player_attr_strength`     |
  `player_attr_dexterity`    |
  `player_attr_intelligence` |
  `player_attr_life`         |
  `player_attr_max_life`     |
  `player_attr_mana`         |
  `player_attr_max_mana`     |
  `player_attr_weariness`    |
  `player_attr_hungriness`   |
  `player_attr_poisoned`     |
  `player_attr_mentalstate`  | drunk, tripping, etc.; 0 means normal
  `player_attr_nightvision`  |
  `player_attr_talks`        |
  `player_attr_kills`        |
  `player_attr_level`        | current experience level
  `player_attr_exp_points`   |
  `player_attr_difficulty`   | 0=easy, 1=normal

- `player.set_attr(PlayerAttribute attr_type, Integer attr_value)`:
  Sets new player attribute value. For `PlayerAttribute` values, see function
  `player.get_attr()'.

- `Integer player.get_skill(PlayerSkill skill_type)`:
  Gets player skill value. `PlayerSkill` can be one of these
  values:

  constant              |
  --------              |
  `player_skill_attack`   |
  `player_skill_defense`  |
  `player_skill_unarmed`  |
  `player_skill_sword`    |
  `player_skill_axe`      |
  `player_skill_mace`     |
  `player_skill_missile`  |
  `player_skill_mana`     |
  `player_skill_lore`     |
  `player_skill_casting`  |
  `player_skill_traps`    |
  `player_skill_search`   |
  `player_skill_track`    |
  `player_skill_sneak`    |
  `player_skill_repair`   |
  `player_skill_charm`    |
  `player_skill_picklock` |
  `player_skill_acrobat`  |
  `player_skill_appraise` |
  `player_skill_swimming` |

- `player.set_skill(PlayerSkill skill_type, Integer skill_value)`:
  Sets player skill value. For `PlayerSkill` values, see function
  `player.get_skill()'.

#### Table `objectlist`

Contains object list related functions:

- `ObjectListInfo objectlist.get_info(Integer objectlist_pos)`:
  Gets object list infos about an object in the object list. When there's no
  object at the given position, `nil` is returned. `ObjectListInfo` is a table
  that looks as follows:

      T = {
         objectlist_pos = 0x0344,  -- object list position

         item_id = 0x0085, -- item ID
         link = 0x0345,    -- object handle to next item (or 0 if end of chain)

         quality = 1,      -- quality value
         owner = 0,        -- owner / special field
         quantity = 0,     -- quantity / property / special link field

         is_enchanted = true, -- is enchanted?
         is_quantity = true,  -- is "special" field a quantity/property?
         is_hidden = false,   -- is object hidden?
         flags = 0,           -- special flags

         xpos = 2,       -- fractional x position in tile (0..7)
         ypos = 7,       -- fractional y position in tile (0..7)
         zpos = 0,       -- z position (flying critters?)
         heading = 7,    -- direction (0..7, dir*45 degree)
         tilex = 32,     -- tile X coordinate
         tiley = 2,      -- tile Y coordinate

         -- these variables are only set when object is a NPC object

         is_npc = true,    -- is true when NPC fields below are available

         npc_hp = 14,      -- vitality of NPC
         npc_goal = 0,     -- goal of NPC (0: wander around, 1: kill gtarg)
         npc_gtarg = 0,    -- goal target value (5 means player)
         npc_level = 2,    -- experience level
         npc_talkedto = true, -- is true when the player already talked to this NPC
         npc_attitude = 0, -- attitude; 0: friendly, 1: mellow, 2: upset, 3: hostile

         npc_xhome = 13,  -- NPC X and Y tile pos of home spot in tilemap
         npc_yhome = 43,

         npc_hunger = 2,  -- indicates hungryness of NPC
         npc_whoami = 0,  -- NPC identity, used in conversations and for names
      }

  The `is_quantity` variable determines if the `quantity` field is a
  `quantity / special` field (when value is `true`), or if it's a
  `special link` (value is `false`). In this case the `quantity` field is an
  objectlist pos to another object associated with the current object. The
  `quality` field is a simple quantity if the value is below 512 (0x0200).

- `objectlist.set_info(ObjectListInfo object_info)`:
  Sets object infos in table `object_info` for the object specified by
  `object_info.objectlist_pos`. The format of the `ObjectListInfo`can be seen
  in the description of `objectlist.get_info()` function above.

- `objectlist.delete(Integer objectlist_pos)`:
  Deletes object from the object list and unlinks it from the tilemap chain it
  is contained in.

- `Integer objectlist.insert(ObjectListInfo object_info)`:
  Inserts a new object into the object list, specified by `object_info`. If
  the `tilex` and `tiley` values are set, the object is added to the given
  tile object chain (otherwise it's e.g. an object linked via the `quantity`
  or the `special link` fields of an object).
  The function returns the new `objectlist_pos` that the object was stored in.

#### Table `inventory`

Contains inventory related functions:

- `InventoryObjectInfo inventory.get_info(Integer inventory_pos)`:
  Gets infos about a single object in inventory. `InventoryObjectInfo` is a
  table that looks as follows:

      InventoryObjectInfo = {
        inventory_pos = 17, -- inventory position

        item_id = 0x0085, -- item ID
        link = 4,         -- next inventory itemlist position (or 0 if last)

        quality = 1,      -- quality value
        owner = 0,        -- owner / special field
        quantity = 0,     -- quantity / property / special link field

        is_enchanted = true, -- is enchanted?
        is_quantity = true,  -- is "special" field a quantity/property?
        is_hidden = false,   -- is object hidden?
        flags = 0,           -- special flags
      }

  Note that when the inventory pos is empty, nil is returned instead.

- `Integer inventory.get_floating_item()`:
  Gets the inventory position of the currently floating item, if any

- `Integer inventory.float_add_item(Integer objectlist_pos)`:
  Moves object list item to the inventory and floats it. When the object is a
  container, then the containing objects are moved as well, recursively.

#### Table `tilemap`

Contains tilemap related functions:

- `TilemapInfo tilemap.get_info(Integer xpos, Integer ypos)`:
  Gets tilemap info for tile on given position. `TilemapInfo` is a table that
  looks as follows:

      TilemapInfo = {
        xpos = 32,
        ypos = 2,
        type = 1,      -- see below
        floor = 96,    -- floor height
        ceiling = 128, -- ceiling height
        slope: 0,      -- only set when a slope tile
      }

  The `type` value can be one of the following constants:

  constant                   | value
  -------------------------- | -----
  `tilemap_type_solid`       | 0
  `tilemap_type_open`        | 1
  `tilemap_type_diagonal_se` | 2
  `tilemap_type_diagonal_sw` | 3
  `tilemap_type_diagonal_nw` | 4
  `tilemap_type_diagonal_ne` | 5
  `tilemap_type_slope_n`     | 6
  `tilemap_type_slope_e`     | 7
  `tilemap_type_slope_s`     | 8
  `tilemap_type_slope_w`     | 9

- `tilemap.set_info(TilemapInfo tilemapInfo)`:
  Sets tilemap info; the values from the `TilemapInfo` table are set for the
  specified tile.

- `Number tilemap.get_floor_height(Number xpos, Number ypos)`:
  Gets floor height on given decimal position, taking slopes into account

- `Integer tilemap.get_objlist_link(Integer xpos, Integer ypos)`:
  Gets first object position in object list for tilemap tile

#### Table `runebag`

Contains runebag access functions:

- `runebag.set(RuneType rune_type, Boolean is_set)`: 
  Sets or clears a rune in the rune bag. `RuneType` can either be a character
  for the rune, or a 1-based index into the rune bag. Note that the 'x' and
  'z' runes don't exist.

- `Boolean runebag.test(RuneType rune_type)`:
  Returns if a rune is available in the rune bag

#### Table `conv`

Contains conversation related functions:

- `Boolean conv.is_avail(Integer conv_slot)`:
  Returns if conversation slot is available

- `conv.get_global(Integer conv_slot, Integer globals_pos)`:
  Gets conversation globals value

- `conv.set_global(Integer conv_slot, Integer globals_pos, Integer value)`:
  Sets conversation globals value

#### Table `quest`

Contains quest flags related functions:

- `Integer quest.get_flag(Integer flag_number)`:
  Returns quest flag by given flag number

- `quest.set_flag(Integer flag_number, Integer flag_value)`:
  Sets quest flag value

#### Table `prop`

The following function allow access to object properties for item IDs that
occur in the game. The object properties can't be modified.

- `CommonObjectProperty prop.get_common(Integer item_id)`:
  Returns common object properties for a given item ID in a table. The table
  looks as follows:

      CommonObjectProperty = {
        item_id = 0x0087,  -- item ID
        height = 4.0,      -- item height (or 0 if no collision check)
        mass = 1.0,        -- mass in stones
        radius = 1.0,      -- object radius (for collision checks)
        quality_class = 0, -- see uw-formats.txt
        quality_type = 1,
        can_have_owner = true,
        can_be_looked_at = true,
        can_be_picked_up = true,
        is_container = true
      }

- `SpecialObjectProperty prop.get_special(Integer item_id)`:
  Returns special object properties for item ID, depending on the object type

  TODO


### 4.3 Cutscenes control interface

TODO

### 4.4 Character creation interface

The cutscenes are stored in the script `cutscene.lua`. There are two Lua
functions that are called from the game:

- `cuts_init(Integer cutscene)`
  Initializes showing cutscene sequence, using given cutscene number. The
  function returns nothing.

- `cuts_tick(Number time)`
  Called for every game tick (usually 20 times per second); `time` contains
  the time in seconds elapsed since start of the cutscene. The function
  returns nothing.

There is one game function that can be called from the Lua script:

- `cuts_do_action(Integer action_code, Various action_value)`
  action_code is one of the values below; `action_value` can be one of the
  following actions:

  action_code | description | action_value type
  ----------- | ----------- | -----------------
  cuts_finished           | indicates that cutscene is at it's end       | nil
  cuts_set_string_block   | sets game strings block to use               | nil
  cuts_sound_play         | plays a sound, e.g. "sound/XX.voc"           | String
  cuts_text_set_color     | sets new text color; value is color index    | Integer
  cuts_text_fadein        | fades in text; value is string number        | Integer
  cuts_text_fadeout       | fades out text                               | nil
  cuts_text_show          | shows text instantly; value is string number | Integer
  cuts_text_hide          | hides text instantly                         | nil
  cuts_anim_fadein        | loads animation and fades in first frame     | String
  cuts_anim_show          | loads animation and starts it, e.g. "cs000.n02" | String
  cuts_anim_set_stopframe | sets frame to stop at; value is stopframe    | Integer
  cuts_anim_fadeout       | fades out current animation frame            | nil
  cuts_anim_stop          | stops animation looping                      | nil
  cuts_anim_hide          | hides animation                              | nil
  cuts_anim_continue      | continues animation after stop               | nil

  When the type column contains `nil`, no value is necessary to be passed to
  the function (use nil in that case). The function returns nothing.

Here's a detailed description of every `action_code`:

- `cuts_finished`:
  Indicates that cutscene is at it's end; the screen immediately gives back
  control to the previous screen, so do some fade-out before doing this
  action.

- `cuts_set_string_block`:
  Sets game string block that should be used for cutscene text. Game strings
  can be extracted from the game with the "strpak" tool. Usually string
  blocks for cutscenes begin at `0x0c00`.

- `cuts_sound_play`:
  Starts playing a sound from the "sounds" folder. The action_value contains
  the name of the sound file, without the extension ".voc", and without
  paths.

- `cuts_text_set_color`:
  Sets new subtitle text color. `action_value` contains the new palette
  index. For text strings, the first game palette is used.

- `cuts_text_fadein`:
  Fades in a subtitle text string; the `action_value` contains the string
  number in the current string block.

- `cuts_text_fadeout`:
  Fades out the currently shown text.

- `cuts_text_show`:
  Shows a subtitle text string; almost the same as `cuts_text_fadein`, but
  without fading in. The text is shown instantly.

- `cuts_text_hide`:
  Hides the text instantly.

- `cuts_anim_fadein`:
  Fades in a cutscene animation. `action_value` contains the animation
  filename, without folder (e.g. "cs401.n01").

- `cuts_anim_show`:
  Instantly shows a cutscene animation. `action_value` again contains the
  animation filename.

- `cuts_anim_set_stopframe`:
  Sets a stopframe for the current animation; animation is stopped when the
  frame in `action_value` is reached. A negative value disables the stopframe
  feature.

- `cuts_anim_fadeout`:
  Fades out current animation. The animation stops, and the current frame is
  used for the fadeout.

- `cuts_anim_stop`:
  Stops the animation at the currently shown frame.

- `cuts_anim_hide`:
  Hides the animation instantly.

- `cuts_anim_continue`:
  Continues a stopped animation (either with `cuts_anim_set_stopframe` or
  `cuts_anim_stop`. It doesn't clear the stopframe.

The code in cutscene.lua already contains an implementation that reads the
cutscene sequence and their actions from a Lua table; the cutscenes can be
defined using this table then.

Cutscenes can be started in the other Lua scripts by calling
`uw.show_cutscene()`, specifying the number of the cutscene to show.


## A. Authors

This chapter lists all authors and contributors of Underworld Adventures.

- Michael Fink (vividos; code, documentation)
- Willem Jan Palenstijn (wjp; Linux port)
- Dirk Manders (phlask; Lua cutscenes and character creation)
- Jim Cameron (hairyjim; inspiration)
- Kasper Fauerby (Telemachos; ideas and collision detection code)
- Martin Gircys-Shetty (QQtis; music)
- Ryan Nunn (Colourless Dragon; XMIDI and Windows MidiOut driver)
- Tels (SDL_mixer MIDI driver)
- Cuneyt Cuneyitoglu (perl script to convert uw1 files/folders to lowercase)
- Radoslaw Grzanka (Debian port)
- Sam Matthews (servus; ideas, 3D models)
- Marc A. Pelletier (Coren; ideas)


## B. Third-Party libraries and licenses

Some third-party libraries are used throughout the project. These are listed
here.

### Exult & Pentagram

The project is using MIDI audio source code from the
[Exult](https://github.com/exult/exult) project.

The Exult source code is licensed using the
[GNU General Public License 2.0 License](https://github.com/exult/exult/blob/master/COPYING).

### SDL2 & SDL_Mixer

The project is using the [SDL2 library](https://www.libsdl.org/) to implement
the cross-platform game application.

The project is using the
[SDL_Mixer library](https://www.libsdl.org/projects/SDL_mixer/) to implement
cross-platform audio mixer and playback.

The SDL2 and SDL_Mixer libraries are licensed using the
[zlib license](https://www.libsdl.org/license.php).

### Lua

The project is using the [Lua scripting language](https://www.lua.org/) to
implement part of the game logic.
The Lua scripting language library is licensed under the
[MIT license](https://www.lua.org/license.html).

> Copyright © 1994–2018 Lua.org, PUC-Rio.
>
> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 

### ZZIPlib

The project is using the
[ZZIPlib library](https://github.com/gdraheim/zziplib) to read zip archives.
The library is licensed under the
[GNU LIBRARY GENERAL PUBLIC LICENSE, Version 2](https://github.com/gdraheim/zziplib/blob/master/COPYING.LIB)

### hqx

The project is using the [hqx scaler](https://github.com/grom358/hqx/)
implementation from grom358 to scale textures and object graphics.
The library is licensed under the
[GNU Lesser General Public License v2.1](https://github.com/grom358/hqx/blob/master/COPYING)
