# README - Manual for Underworld Adventures #

Underworld Adventures is a project to recreate Ultima Underworld 1 on modern
operating systems (e.g. Win32, Linux or MacOS), using the original game files.

Underworld Adventures is Copyright (C) 2002,2003,2004,2019 Underworld Adventures Team

Underworld Adventures is available from
   https://vividos.github.io/UnderworldAdventures/

## Content

This Readme file contains the Manual for end users of the Underworld
Adventures project. Please use the table of contents to navigate the document.

[1. Quick start and quick reference](#1-quick-start-and-quick-reference)<br/>
[2. Installing and configuring Underworld Adventures](#2-installing-and-configuring-underworld-adventures)<br/>
[3. Configuring Underworld Adventures](#3-configuring-underworld-adventures)<br/>
[4. Gameplay](#4-gameplay)<br/>

[A. Underworld Adventures Music](#b-underworld-adventures-music)<br/>
[B. Underworld Adventures Debugger](#c-underworld-adventures-debugger)<br/>

## 1 Quick start and quick reference

You need a copy of the game to run Underworld Adventures. Install the game to
any folder you like and start the uaconfig.exe tool. Set the Ultima Underworld
1 folder and close the tool. Start the game by running uwadv.exe.

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
be downloaded here: TODO

Run DosBox with the following command, which mounts a part of your hard disk
as drive D:

    dosbox TODO

The installed game files should then be available in the mounted folder.

### 2.3 Using the game files directly

This is most straight-forward; just copy the game files (uw.exe, the "data"
and any additional folders) to your hard disk.

### 2.4 gog.com distribution

The gog.com installer extracts several files to your hard disk. The actual
games are stored in a file called "game.gog", which is actually an ISO file
that is mounted when starting the game using DosBox. Open the "game.gog" file
with your favourite ISO tool and extract the files on your hard disk. Note
that the popular 7-zip can also open and extract the file without problems.

### 2.5 System requirements

Nowadays, any computer with an on-board graphics card that supports 3D
graphics can be used to run Underworld Adventures. Also, modern CPUs are not
fully used to run the game.

Back in 2004, when the project was started, at least a 600 MHz CPU was needed,
along with a decent 3D-Graphics card with current OpenGL drivers, e.g. an
Nvidia GeForce. For sound output a dedicated sound card was often useful.

## 3. Configuring Underworld Adventures

This chapter describes how to set up and configure Underworld Adventures
to run the old Ultima Underworld games.

### 3.1 Tool "uaconfig"

TODO

### 3.2 Configuration file

TODO

### 3.3 Command line options

TODO

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

TODO

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
