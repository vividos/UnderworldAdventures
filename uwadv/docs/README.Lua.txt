Underworld Adventures Lua Readme File
-------------------------------------

This file describes all interfaces for the scripting language Lua to the
Underworld Adventures game and it's objects (especially the ua_underworld
object that represents the whold underworld).


Contents

1. Introduction
2. Reference
2.1. ua_underworld interfaces
2.2. cutscenes control interface


1. Introduction

Lua is available from http://www.lua.org.
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


2.1. ua_underworld interfaces

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

The "actionvalue" variable always must be passed to the function. For actions
that doesn't use the value, it can be set to anything.

