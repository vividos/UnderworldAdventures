# README - Manual for Underworld Adventures

Underworld Adventures is a project to recreate Ultima Underworld 1 on modern
operating systems (e.g. Win32, Linux or MacOS), using the original game files.

Underworld Adventures is Copyright (C) 2002,2003,2004,2019,2021 Underworld Adventures Team

Underworld Adventures is available from
   https://vividos.github.io/UnderworldAdventures/

## Content

This Readme file contains the Manual for end users of the Underworld
Adventures project. Please use the table of contents to navigate the document.

[1. Quick start and quick reference](#1-quick-start-and-quick-reference)<br/>
[2. Installing and configuring Underworld Adventures](#2-installing-and-configuring-underworld-adventures)<br/>
[3. Configuring Underworld Adventures](#3-configuring-underworld-adventures)<br/>
[4. Gameplay](#4-gameplay)<br/>

[A. Underworld Adventures Music](#a-underworld-adventures-music)<br/>
[B. Underworld Adventures Debugger](#b-underworld-adventures-debugger)<br/>
[C. Release Notes](#c-release-notes)<br/>

## 1 Quick start and quick reference

You need a copy of the game to run Underworld Adventures. Install the game to
any folder you like and start the `uaconfig` tool. Set the Ultima Underworld
1 folder and close the tool. Start the game by running the `uwadv` program.

In the game, use the WASD keys to run and turn and X to go backwards, or the
left mouse button pressed while moving the mouse cursor in the 3D view. Use
the command buttons on the left to interact with game objects, e.g. for
talking to people or picking up objects.

Enjoy the game and return to this manual if you need more in-depth
information.

## 2 Installing and configuring Underworld Adventures

This chapter explains how to install the games and configure Ultima Underworld
to use the game's resources. To play Underworld Adventures, you definitely
need the original game files.

### 2.1 Install media

Ultima Underworld 1 was originally distributed on install disks, which had to
be installed on a hard drive in order to run the game. Later it was also
released on a CD-ROM distribution called "Electronic Arts Presents CD-Rom
Classics: Ultima Underworld I & II" CD. Nowadays you can get the games on
gog.com, in yet another format.

### 2.2 Running the original installer

The original installer is a 16-bit DOS executable, which doesn't run on modern
64-bit processors anymore. You can install the game using DosBox, though,
which emulates most of the MS-DOS needed, as well as the hard disk. DosBox can
be downloaded here: https://www.dosbox.com/

Run DosBox with the following command, which mounts a part of your hard disk
as drive D:

    dosbox -c "mount d c:\uw1_disc\"

Inside DosBox, you can start the installer by calling the setup exe.
The installed game files should then be available in the mounted folder.

### 2.3 Using the game files directly

This is most straight-forward; just copy the game files (uw.exe, the "data"
and any additional folders) to your hard disk.

### 2.4 gog.com distribution

The gog.com installer extracts several files to your hard disk. The actual
games are stored in a file called "game.gog", which is actually an ISO file
that is mounted when starting the game using DosBox. Open the "game.gog" file
with your favourite ISO tool and extract the files on your hard disk. Note
that the popular 7-zip can also open and extract the file without problems:

    "C:\Program Files\7-Zip\7z.exe" x game.gog -ouw1and2

The actual games are in the "UW" and "UW2" subfolders.

### 2.5 uw_demo download

There is also a demo version of Ultima Underworld 1 available for free on some
Ultima fan download sites. Usually this comes in an `uw_demo.zip` that only
contains a single `uw_demo.exe` file. The exe file is a self-extracting
executable using the LHA compressor. The demo game files can be extracted
using DosBox, as shown above, or using 7-zip:

    "C:\Program Files\7-Zip\7z.exe" x uw_demo.exe -ouw_demo

The files are extracted into the `uw_demo` subfolder.

### 2.5 System requirements

Nowadays, any computer with an on-board graphics card that supports 3D
graphics can be used to run Underworld Adventures. Also, modern CPUs are not
fully used to run the game.

Back in 2004, when the project was started, at least a 600 MHz CPU was needed,
along with a decent 3D graphics card with current OpenGL drivers, e.g. an
Nvidia GeForce card. For sound output a dedicated sound card was often useful.

## 3. Configuring Underworld Adventures

This chapter describes how to set up and configure Underworld Adventures
to run the old Ultima Underworld games.

### 3.1 Tool "uaconfig"

Underworld Adventures is configured using the `uwadv.cfg` file. On Windows,
there is the `uaconfig` tool that helps you configuring that file. The tool
shows a dialog with several fields to edit.

### 3.2 Configuration file

You can also configure Underworld Adventures directly, using the `uwadv.cfg`
file. Just open the file using your favourite text editor. It contains
instructions on top on how to edit the file. Be sure to keep a copy of the
original `uwadv.cfg` file, in case you made a mistake editing the file.

### 3.3 Command line options

The `uwadv` program has some command line options that can be specified. The
syntax of the command line is:

    uwadv {options} [savegame-filename]

An optional save game filename can be specified to directly load a savegame
from a file with `.uas` file extension. The savegame must match the base game.
uw_demo savegames won't work correctly with uw1 savegames and vice versa.

The following options may be specified:

    -h -? -H  --help           Shows help
    -g <game> --game <game>    Starts custom game

Note that custom games are currently not implemented.

### 3.4 Operating system specific installation

This chapter contains operating system specific documentation about Underworld
Adventures.

#### Linux, FreeBSD and other Unices

The game uses timidity for playback of the original MIDI tracks. timidity is a
library that uses a software wavetable for synthesizing audio. The library
requires to have a GUS compatible set of sound patches on your system. By
default, the library looks for these patches on UNIX in
`/usr/local/lib/timidity/`. The patches are available here:
http://www.libsdl.org/projects/SDL_mixer/timidity/timidity.tar.gz


## 4 Gameplay

Underworld Adventures tries to reproduce the old games as faithfully as
possible, including graphics and controls. If you still have the manual for
the game, you can also use this for reference. However, there are a few
differences, which are explained in more detail later.

### Keyboard mapping configuration

Underworld Adventures has a flexible keyboard key mapping configuration that
can be used to remap keys to differnt actions. The key mapping is specified in
a `keymap.cfg` file. The `uwadv.cfg` file specifies the default keymap to be
used:

    custom-keymap %uahome%/keymap.cfg

The keymap.cfg is found in the `data/uadata00.zip` file that comes with the
binary distribution of Underworld Adventures. In order to customize keys, open
the zip archive, go to the `uw1` subfolder and extract the keymap.cfg into the
main folder. You can then adjust the `custom-keymap` setting in `uwadv.cfg` to
point to the new file, e.g.

    custom-keymap ./keymap.cfg

Open the file with your favourite text editor. It contains instructions on top
on how to edit the file. You can always get the original keymap file from the
zip archive, in case you made a mistake editing the file.

### Saving and loading games

One limitation of the Underworld games were the number of savegames one
can use during the game. Underworld Adventures lifts that limitation by
providing practically unlimited number of savegames.

Underworld Adventures introduces a savegame Load/Save screen that is
used to either load a savegame, e.g. when using "Journey Onward" on the
game's screen, or when selecting "Save Game" or "Restore Game" in the
game's builtin menu. The old way of accessing the savegames via buttons
numbered I to IV and savegame descriptions listed in the text scroll isn't
used anymore.

### Quicksave

The current game can quickly be saved using the Quicksave feature. The
key F11 saves the current game to a special Quicksave slot. With F12 one
can restore the previously saved quicksave savegame. The key bindings can be
modified in the keymap.cfg.

### Differences to the original game

While creating Underworld Adventures there were some things that were done
differently than in the original games. Reasons for this are mainly
to simplify the game or to improve it in some way, without losing the spirit
of the old games.

Here is the list of things that are different than in Ultima Underworld 1.
Some of the options can be switched off with the "enable-uwadv-features"
setting in the configure file.

- Up/down view angle can be varied between -75 and 75 degree, as opposed
  to something like 45 degrees in the original (can be switched off).

- Cutscene speech is played a bit too slow (due to some technical reasons
  the audio samples isn't exactly converted to the proper frequency).

- The input cursor in text scrolls (e.g. in conversations) is a vertical
  line, and not a blinking cursor square. The cursor can be moved to any
  position in the input field.


## A. Underworld Adventures Music

Underworld Adventures contains an (incomplete) set of newly arranged music
pieces. This chapter describes the software used and arrangements made in the
rendition of this digital soundtrack for Underworld Adventures.

### Introduction

The original files that came with the game are used in creating a digital
soundtrack. The original .xmi file had to be converted into standard MIDI, and
the tracks parsed according to channel. Based on that - variations are applied
and the arranegent modified, to ad accents and realism to its performance by
the synth.

### Install instructions

The digital music soundtrack is included in the binary packages and are
automatically used by the game.

### Hardware and software used

Pentium 4 - 1.5GHz machine with Soundblaster Audigy Platinum. Internal Audigy
synth is sometimes used for certain tracks.

Main synth is Yamaha S-YXG50 software MIDI emulator. Has over 600 patches
accessible through its banks, fairly realistic sound, and high polyphony.

Arrangements in Cakewalk, recording and reverb in Cool Edit, audio compression
in winLAME.

### Pieces

Here, the most recent renditions of the pieces are described - their
peculiarities, arrangements, features, etc..

#### Introduction Theme

Most elaborate arrangement so far. Number of instruments expanded
from 9 to 14, more specifically - Tuba, Trombone, one more Strings
section, Marimba, and a Taiko Drum. For the most part - same melodic
motifs are given to these instruments as are already played by others,
duplicating in unison, or in some cases taking over a part that originally
was played by another. In a few cases - as with taiko and marimba,
new parts are written for some sections (echo of marimba at the beginning
of tuba solo).  Strings are given a new part at the end of the piece, when
they mimic trumpet melody in canon. All used patches - Yamaha.

#### The Descent

Internally, the original arrangement had a flute patch set up to play
in the lower octaves that would be played by a contrabass or a tuba.
I tried to use other patches of typically low-pitched instruments, such as
a Church Organ, but... the flute just seems to work best. I HIGHLY
reccomend a good subwoofer and loud bass when you listen to this
piece - it's beauty and it's mass is in the bass.

Contrast is given to it's bass by the orchestral harp. The harp was not
bright enough to stand out against the powerful bass. I decided that
the Yamaha patch was a little too "mellow". So I recorded the harp part
played by the SB Audigy synth. The rest of the instruments were done
with Yamaha, an dthen the two parts were mixed down.

#### The Wanderer

In progress.
Arrangement is pretty much the same. Only patches and dynamics
modified. Brass sounds too synthetic. Expect a new rendition in the near
future.

#### Maps & Legends

The piece is short, simple and perfect. Almost no changes were
done to the original arrangement. More appropriate patches were
chosen.


# B. Underworld Adventures Debugger

The Underworld Adventures Debugger is an add-on software to Underworld
Adventures that shows various game states and data and is mainly used for
developing and debugging the game engine and the implemented logic. It's
described here in this manual, even though it is more of a developer tool.

The Debugger can be started while the main game is running. The default key
combination is Alt + D; the key can be modified using a custom keymap.cfg
file.

A future idea is to create a "Underworld Adventures Studio" application that
helps to create new games in the style of Ultima Underworld 1 and 2. The
debugger is designed so that the studio application also uses the debugger to
design the new game.

## Main Window

The main window consists of several windows that show game information. Most
windows are tool windows and can be docked to any side of the main window, by
dragging their title line with the mouse. Some windows are content windows and
display data that needs more screen space. The content windows use the
remaining space of the main window not occupied with tool windows.

The main window also has a menu bar, tool bar and a status bar.

The following tool windows can be shown:

- Player info

The following contnet windows can be shown:

- Object list

Note: The debugger is a work in progress and may not be completely functional.


# C. Release Notes

## Release Notes for "0.10.0-pina-colada", 2021-12-25

This release is the first one in 17 years. It was mainly released to restore,
refactor and improve the old code base that I stopped developing at around
2007. My main goal was to show what the project was like in the 0.9 release,
and what the team accomplished at the time. So keep in mind that most gameplay
elements are still not implemented yet.

This release contains several improvements and bug fixes:

- Refactored the whole code base to compile again and to use modern C++
  language constructs
- Implemented displaying the current map when looking at the map object
- Reactivated displaying 3D models provided by Sam Matthews (servus)
- Objects and critters now use the hqx 4x scaler to improve image quality
- Binaries include the digital music pack, provided by Martin Shetty (QQtis)
- Improved decompiling conversation code
- No need to lowercase game files anymore
- Extended `uwdump` tool to recognize more file types
- Improved Underworld Debugger, including debugging Lua and conversation
  scripts
- Updated all external libraries, like SDL2, Lua, zlib, etc.
- New CMake based build system to help porting the project to other platforms
- Fixed rendering the 3D models, especially the shrine ankh
- Fixed picking up items and containers
- Many more small bugfixes

## Release Notes for "0.9-mojito", 2004-04-10

This release doesn't have many new things. There even are features left out.
The reason for that is a restructuring and refactoring of source code that was
done over the last few months. The release mainly was done to show that the
project is still alive and active. Expect more things to work again in the
future.

Underworld Adventures now has a (not yet complete) manual that you can read.
On Windows it is available from the "Underworld Adventures" program group. For
Linux it is installed into /usr/share/doc/uwadv-0.9_mojito/.

There is a new version of the music pack with a digitally rearranged
soundtrack for Ultima Underworld 1 available. It can be obtained from
http://uwadv.sourceforge.net/, in the download section.

Changes:
* restructured and refactored code
* added config option to show "pixely" user interface
* added splash screen when starting the game
* new savegame format (incompatible to previous versions)
* updated and improved physics code
* added stats and runebag panel

## Release Notes for "0.8-hot-chili", 2003-04-24 

This release has a lot of new features inside. Picking up objects from the
underworld was built in (via the "get"-icon). Lava and water textures are now
animated, giving a nice effect; also NPC's are now animated. Added drawing of
levers, buttons and pull chains (although they lack functionality for now,
since doors are not rendered yet). Bridges are now added to allow the
adventurer to visit more of the underworld.

Finally a savegame feature was added (press CTRL+R or CTRL+S in game),
together with a quicksave (F11) and quickload (F12) functionality. The 3d
models incorporated in the last release were too small, due to a bug, which is
now fixed, too. Some of them can e.g. be found in the rat's room on the first
level, at Lakshi Longtooth's campfire on level 4 or in the abyss' library on
the 6th level. Happy hunting!

Changes:
* added simple NPC animation
* added picking up items from the underworld
* added animated lava and water textures
* added switches, levers and pull chains
* implemented some traps and triggers
* added bridges
* added savegames loading/saving
* added quicksave functionality
* fixed 3d models sizes

## Release Notes for "0.7-muffin", 2003-02-11

In this release there is basic support for conversations. Just click on the
"talk" command button on the left and click on people in the underworld you
see. Inventory interactions with NPC's are not supported yet, and some
conversations may end unexpectedly when an unknown script function is called
internally.

Some objects are replaced by 3d models done by Sam Matthews. Just look out for
them in the underworld.

Changes:
* added support for 3d models for static objects
* conversations work (no inventory interaction yet)
* removed usage of big textures larger than 256x256 (voodoo3 cards
  should work now)

## Release Notes for "0.6-rotworm stew", 2002-11-21

There are some new features in this release: 

Mouse movement mode: You can move around the levels using the mouse. Just
place the mouse cursor over the 3d window and left-click to start moving.
During using mouse movement mode you cannot leave the window, as in the
original game.
 
Looking at objects and walls: You can look at these by right-clicking on them.
For this, you have to be in "default" or "look" command mode. "default" mode
is when no command icons (those on the left side) are on, and "look" mode is
when the "eye" symbol is highlighted.

Level changing: To let the user explore more of the Abyss than just the first
level, there are some keyboard keys to change the levels. Press Alt + PageDown
to descend a level, and Alt + PageUp to move one level up. You are usualy
placed at different interesting spots when changing levels. Users of the
"uw_demo" can use these keys to change positions in the first level.

SPOILER WARNING: Since this is considered cheating, you might see things that
                 you should not see before ending the game. So better don't
                 descend beyond level 8!

Changes:
* added looking at objects in the underworld
* initial rendering of NPC and critter sprites; no animation yet
* added mouse movement
* added customization of key mappings
* added support for digital remakes of the underworld soundtrack
* added support of changing levels via Alt+PageUp and Alt+PageDown
* rewrote collision detection; added gravity
* Tools: improved conversation script decompiling
* Linux: fixed midi playback on Linux
* Linux: added manual pages

## Release Notes for "0.5-butterfinger", 2002-08-22

Changes:
* added character creation
* Linux: finished build system, including binary RPMs
* Windows: added installer and configuration program

## Release Notes for snapshot "0.4b-cheesecake", 2002-08-12

Changes:
* Linux: improved build system

## Release Notes for "0.4-cheesecake", 2002-07-30

Changes:
* added introduction scene
* added acknowledgements screen
* added inventory handling
* added fullscreen mode

## Release Notes for "0.3-poppypastry", 2002-07-08 

Changes:
* added splash screen and start menu
* added initial collision detection

## Release Notes for "0.2-grainroll", 2002-06-20

Changes:
* added rendering objects to the abyss
* textures are rendered correctly now
* Tools: added conversation script decompiler and animation viewer

## Release Notes for 0.1-ricecraecker, 2002-05-05

Changes:
* set up SourceForge project and CVS repository
* player is able to run around on first level of abyss; no clipping yet
* introduction music plays

## Release Notes for "uwadv-stuff-preview", 2002-04-08

Changes:
* made a simple level map viewer
