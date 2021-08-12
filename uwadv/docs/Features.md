# Underworld Adventures Features

This file lists all features of Underworld Adventures and their state of
implementation. The following GitHub emojis are used for state of completion:

emoji | meaning
----- | -------
:heavy_check_mark: | feature is complete
:soon:             | Feature is almost complete
:warning:          | feature is not completed yet
:x:                | feature is not implemented yet

# User Interface

## Screens

Feature | State
------- | -----
Underworld Adventures menu screen | :x:
Start splash / menu screen        | :heavy_check_mark:
Create character screen           | :heavy_check_mark:
Cutscene screen                   | :heavy_check_mark:
Load/save game screen             | :heavy_check_mark:
Ingame screen                     | :warning:
Map view                          | :x:
Conversation screen               | :warning:
Acknowledgements screen           | :heavy_check_mark:
End game screen                   | :x:

## Rendering 

Feature | State
------- | -----
Rendering Tilemap                      | :heavy_check_mark:
Rendering Objects                      | :heavy_check_mark:
Rendering and animating critters       | :warning:
Rendering special objects, doors, etc. | :warning:
Animating tile textures                | :x:
Rendering Underworld 3D models         | :warning:
Rendering VRML98 3D models             | :soon:
2D scaler support (hqx)                | :x:
Optimized rendering using VBO/VBA/texture atlas | :x:

## Keyboard & mouse commands

Feature | State
------- | -----
Moving around with keyboard and mouse | :heavy_check_mark:
Easy movement mode                    | :x:
User Interface keyboard control       | :x:

## Audio & Music

Feature | State
------- | -----
Playing back MIDI music           | :heavy_check_mark:
Playing back digital music        | :heavy_check_mark:
Digital music pieces for uw1      | :warning:
Playing speech sound files        | :heavy_check_mark:
Sound effects for uw1             | :x:
Use uw2 sound effects in uw1 game | :x:
Sound effects for uw2             | :x:

# Gameplay

Feature | State
------- | -----
Support for Ultima Underworld Demo | :warning:
Support for Ultima Underworld 1    | :warning:
Support for Ultima Underworld 2    | :x:
Support for custom games           | :x:

## Player

Feature | State
------- | -----
Vitality           | :x:
Mana               | :x:
Game time          | :x:
Hunger             | :x:
Jumping            | :x:
Swimming           | :x:
Levitating         | :x:
Tracking           | :x:
Lore               | :x:
Death, silver seed | :x:

## Mobiles / Critters

Feature | State
------- | -----
Talking      | :soon:
Bumping NPCs | :x:
Attitude     | :x:
Pathfinding  | :x:
Stealing     | :x:
Tracking     | :x:
Lore         | :x:
Combat       | :x:

## World interaction

Feature | State
------- | -----
Looking at tilemap, objects | :soon:
Picking up objects          | :warning:
Putting down objects        | :x:
Throwing objects            | :x:
Inventory handling          | :soon:
Combining objects           | :x:
Using objects               | :x:
Casting spells              | :x:
Modifying tilemap           | :x:
Triggers & traps            | :warning:
Collision detection         | :soon:

## Conversations

Feature | State
------- | -----
Talking, menu selection         | :heavy_check_mark:
Respond with text               | :x:
Bartering                       | :x:
World interaction (doors, etc.) | :x:

# Tools

Feature | State
------- | -----
uaconfig: configuring game                 | :heavy_check_mark:
strpak: extracting and creating .pak files | :heavy_check_mark:
strpak: merging .pak files                 | :x:
uwdump: dumping all file types             | :warning:
uwdump: viewing some file types            | :x:
mapdisp: displaying maps                   | :soon:
convdbg: debug conversations               | :heavy_check_mark:
convdec: decompile conversations           | :warning:
xmi2mid: exporting .xmi files              | :heavy_check_mark:

## Underworld Debugger

Feature | State
------- | -----
Reading game.cfg                      | :x:
2D Tilemap                            | :soon:
3D Tilemap with editor                | :x:
Code debugger for Lua                 | :x:
Code debugger for conversation script | :x:

## Underworld Adventures Studio

The studio application shares most of its implementation with with Underworld
Debugger, so you will find the most features there.

Feature | State
------- | -----
Creating new projects | :x:
Loading projects      | :x:
Exporting projects    | :x:

# Platform

## Misc.

Feature | State
------- | -----
Load data from in zip archives     | :heavy_check_mark:
Load data from uw_demo lhasfx .exe | :x:
Load data from game.gog ISO file   | :x:
Support portuguese translation     | :x:
Support russian translation        | :x:

## Projects

Feature | State
------- | -----
Visual Studio project       | :heavy_check_mark:
CMake projects              | :warning:
Debian package              | :warning:
AppVeyor build              | :heavy_check_mark:
SonarCloud, cppcheck checks | :heavy_check_mark:
Source code documentation   | :heavy_check_mark:
