Underworld Adventures Tools Readme
----------------------------------

There are some tools made while creation of Underworld Adventures that can be
interesting for the common user. The source
code for them resides in the source/tools/ folder.  It can be copied into the same folder
as the .exe files.


animview - Underworld Cutscenes Animation Viewer
------------------------------------------------

animview is a viewer for cutscene animation files stored in the "cuts" folder
of Ultima Underworld. The animation frames can be played back continuously, or
each frame can be viewed separately.

The program is started using the command line, specifying the cutscene file to
play back. In the "cuts" folder, only files that don't have the extension
".n00" are animation files. When using Windows, the file to play back can be
dragged and dropped on the animview.exe to start viewing.

At startup, the first frame of the animation sequence is shown. The user can
toggle continuous playback with the space bar. Pressing the return key steps
through the single frames. The last frame often is the same frame as the first
one.

When an extra parameter "decode" is passed at the end of the command line, the
frames are decoded to tga files instead. They are stored in the current folder
and always have the format anim????.tga. Subsequent calls to animview will
overwrite existent files.

Background info: The cutscene animations were done with Deluxe Paint on Amiga,
using the Deluxe Paint Animator file format (*.anm).



map3ds - Level Map to 3DS format converter [uw1/2]
------------------------------------------

map3ds is a conversion tool to convert Ultima Underworld's level maps to
3ds mesh files with texture info. Each level is output to a separate 3ds
file; textures are written out as .tga files.

The program should be placed in the game's main folder, where the executable
is found. It writes out all .3ds and .tga files into a newly created subfolder
named "levels3ds". Some levels in Ultima Underworld 2 contain no data; those
are recognized and are omitted.

An optional parameter "sepmesh" seperates triangles for ceiling, water and
lava into an own object.


uwdump - Ultima Underworld data dump program
---------------------------------------------

uwdump is a program to extract and show information about many data structures
used in Ultima Underworld. Textual output is always written to standard output
where it can be redirected, e.g. to a text file. The program has the following
syntax:

   uwdump <options> <command> <options>

Options and the command can be in any order, but there can only be one
command. Here is a list of supported commands:

   leveldump <file>
      dumps the level map archive lev.ark given as <basepath><file>; it prints
      an overview of the .ark file block structure, the master object list
      with all fields described in uw-formats.txt, npc extra infos, npc
      inventories, a list of traps, special link ("sp_link") chains and the
      texture mapping. the infos are dumped for every level in the file.

   propdump
      dumps common object properties from <basepath>/data/comobj.dat and
      special object properties from <basepath>/data/objects.dat

The following options are available for all commands:

  -d<basepath>   sets uw1/uw2 path; using current folder when not specified
                 (game type is auto-detected)
  -2             set game type to ultima underworld 2 without further checking

Here are some examples for calling uwdump. It is assumed that the current
folder is the game's main folder:

   uwdump -d c:\uw1\ leveldump data\lev.ark > uw1-leveldump.txt
   uwdump leveldump save1\lev.ark
   uwdump -d c:\uw2\ propdump > uw2-properties.txt


uwmdlview - Ultima Underworld builtin 3d model viewer [uw1/2]
-----------------------------------------------------

uwmdlview shows the 3d models built into the .exe files of both games. It lets
you switch between the objects and switch on/off lighting. The view of the
model can be controlled with the mouse.

To view the models, just start the program in the same folder as one of the
files "uw.exe", "uw2.exe" or "uwdemo.exe". No other files are needed. There
are 32 models available. The view can be controlled with some keys and the mouse:

   action          description
   left-drag       rotates level around center
   right-drag      moves level centerpoint
   up / down key   moves level towards or from the viewer
   page up/down    shows next/previous model
   L               toggles on/off lighting


