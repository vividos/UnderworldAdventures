Underworld Adventures Tools Readme
----------------------------------

There are some tools made while creation of Underworld Adventures that can be
interesting for the common user. Here is a description of the tools. The source
code for them resides in the source/tools/ folder. Note that on Windows, most
tools need the file "SDL.dll" to work. It can be copied into the same folder
as the .exe files. Tools that work together with Ultima Underworld 2, too,
have a [uw1/2] next to their name.


mapdisp - Underworld Map Display [uw1/2]
--------------------------------

mapdisp is a program to show an "eagle's eye" view of each level of the
underworld. The user can rotate and move the level to view all details of a
specific level.

The program should be placed in the Ultima Underworld 1, 2, or demo folder,
directly where the original executable file resides.

To rotate the level around the center, drag with the left mouse key (press
down left mouse button anywhere in the screen, move the mouse around, and
release the button again). To move the level around, drag with the right mouse
button. The up and down cursor keys moves towards or from the viewer's point.

Finally, Page Up and Page Down select another level in the abyss, either above
or beyond the current (when using the uw_demo this won't work, since the demo
only has one underworld level stored). The window caption shows infos about
the current level.

mapdisp can be started with the parameter "allmaps"; it then shows all maps
simultaneously as they would be in real space. You can move around the scene
as usual.

Reference Card:

   action            result
   left-drag         rotates level around center
   right-drag        moves level centerpoint
   up / down key     moves level towards or from the viewer
   page up / down    changes underworld level


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


strpak - Underworld Game Strings Pack/Unpack Tool [uw1/2]
-------------------------------------------------

strpak is a tool that lets you unpack the game strings in the file
"strings.pak" to a raw text file. It also can take a raw text file that is
properly formatted (like the output of unpacking) and produce a new packed
game strings file.

The program needs some parameters to work. Here's the syntax:

   strpak <command> <input-file> <output-file>

<command> can either be "unpack" or "pack". Unpacking expects a "strings.pak"-
like file, and writes to the output-file as text. Packing wants a raw text
file as input (see below) and writes a new "strings.pak"-like output file.

The file format for the input file for the "pack" command is the same as the
output of the "unpack" command:

block: <block-id>; <ignored stuff>
<number>: string number one
<number>: this one contains a newline: \n there, it is
<number>: and this is another one
block: <block-id>; <ignored stuff>
<number>: yes, another block

Note that empty lines are ignored, as well as the very first line in the file.
After the block-id, which must be a four-digit hexadecimal number, there must
be a semicolon. A string line contains a number followed by a ':' and exactly
one space. All stuff that is after that space counts as valid string data.
Newlines are expressed as "\n".

For more info about game strings and how they are used in Ultima Underworld,
especially in the conversations, just look into the "docs/uw-formats.txt" in
the source distribution of Underworld Adventures.

Short example (strpak is placed in the uw_demo folder of the Underworld Demo):

   strpak unpack data/strings.pak uw_demo-strings.txt

Then modify some stuff inside the created text file, e.g. translate strings.

   strpak pack uw_demo-strings.txt data/strings.pak

Be sure to keep a copy of the original "strings.pak" around, in case anything
bad happens.


cnvdbg - Underworld Conversation Debugger
-----------------------------------------

cnvdbg is a conversation script debugger for Ultima Underworld 1. Conversation
is done with an assembler-like language that runs in a virtual machine. The
debugger looks and behaves more or less like gdb, the GNU debugger.

The program needs some parameters to work. Here's the syntax:

   cnvdbg <cnv-file> <str-file> <bglobals-file>

<cnv-file> is the file that contains the conversations, usually
"data/cnv.ark". <str-file> is the file where the strings can be extracted.
Most of the time this will be "data/strings.pak". The last parameter is a file
with conversation globals. This can be found in saved game's folders. This
file could be "Save1/bglobals.dat".

If you are in the base folder of the Ultima Underworld installation, you just
could call cnvdbg like this:

   cnvdbg data/cnv.ark data/strings.pak Save1/bglobals.dat

The debugger then should load and initialize, and the debugger command line
should appear, showing a ">", waiting for commands. Here's a list of available
ones (also shows up when you use the command "help"):

   help       h shows this help
   conv n       loads a conversation (conv slot n in decimal, not hex)
   reg        r shows registers
   info       i shows info about the virtual machine, including breakpoints
   list [n]   l lists instructions from code memory 0x<n> on (or from current ip)
   dump       d dumps complete stack
   step       s advances an instruction
   cont       c continues until a breakpoint occurs
   break n    b toggles a breakpoint at code pos 0x<n>
   verbose    v toggles verboseness of debugger
   reset        resets virtual machine (e.g. after an exception)
   exit/quit  x quits the debugger

   short keys can be used for the commands, e.g. 's' for step
   some commands are only available when a conversation is loaded

For more information about the inner workings of the conversation system of
Ultima Underworld, just look into the file "docs/uw-formats.txt" in the source
distribution of Underworld Adventures.


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


xmi2mid - XMIDI to MIDI converter [uw1/2]
---------------------------------

xmi2mid is a converter for XMIDI files (*.xmi) that are used in Ultima
Underworld (and many other games) to ordinary midi (*.mid) files. Only the
first track of an XMIDI file is extracted, though. The calling syntax for the
program is:

   xmi2mid <xmi-file> <mid-file>

The program prints out an error if something strange occured, else the midi
file is created.
