Underworld Adventures Tools Readme
----------------------------------

There are some tools made while creation of Underworld Adventures, that can be
interesting to the common user. Here is a description of the tools. The source
code to them reside in the source/tools/ folder. Note that on Windows, most
programs need the file "SDL.dll" to work. It can be copied into the same
folder as the .exe file.


mapdisp - Underworld Map Display
--------------------------------

mapdisp is a program to show an "eagle's eye" view of each level of the
underworld. The user can rotate and move the level to view all details of a
specific level.

The program should be placed in the Ultima Underworld 1 (or demo) folder,
directly where the original file "uw.exe" resides.

To rotate the level around the center, drag with the left mouse key (press
down left mouse button anywhere in the screen, move the mouse around, and
release the button again). To move the level around, drag with the right mouse
button. The up and down cursor keys moves towards or from the viewer's point.

Finally, Page Up and Page Down select another level in the abyss, either above
or beyond the current (when using the uw_demo, this won't work, since the demo
only has one underworld level stored).

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

Background info: The cutscene animations were done with Deluxe Paint on Amiga,
using the Deluxe Paint Animator file format (*.anm).


strpak - Underworld Game Strings Pack/Unpack Tool
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

Be sure to keep a copy of the origina "strings.pak" around, in case anything
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


cnvdasm - Conversation Disassembler and Decompiler
--------------------------------------------------

cnvdasm is a disassembler and decompiler for the Ultima Underworld
assembler-like conversation script code. The disassembler part just decodes
every opcode and prints a assembler source listing. The decompiler instead
tries to recognize code structures and produces C-like code.

The program has the following calling syntax:

   cnvdasm <command> <cnv-archive> <conv-slot>

The command can either be "dasm" which only performs disassembling the conv
code, or "dec" which performs decompiling to C-like code. The cnv-archive
usually is a file called "cnv.ark" found in the "data" folder of Underworld.
The conv-slot specifies the conversation slot that should be decoded. Not all
slots actually contain code.

Again, for more info about the conversation system, look into the file
"docs/uw-formats.txt" in the source distribution of Underworld Adventures.


xmi2mid - XMIDI to MIDI converter
---------------------------------

xmi2mid is a converter for XMIDI files (*.xmi) that are used in Ultima
Underworld (and many other games) to ordinary midi (*.mid) files. Only the
first track of an XMIDI file is extracted, though. The calling syntax for the
program is:

   xmi2mid <xmi-file> <mid-file>

The program prints out an error, if something strange occured, else the midi
file is created.
