# README - Tools for Underworld Adventures #

This is the readme file for the Tools for Underworld Adventures.

Underworld Adventures is available from
   https://vividos.github.io/UnderworldAdventures/

## Tools for Underworld Adventures

This chapter describes the tools that come with the binary distribution of
Underworld Adventures. Note that on Windows, most tools need the file
"SDL2.dll" to work. When copying tools around, also include that file.

Tools that also work with Ultima Underworld 2 have a [uw1/2] next to their
name.

### strpak - Underworld Game Strings Pack/Unpack Tool [uw1/2]

strpak is a tool that lets you unpack the game strings in the file
"strings.pak" to a raw text file. It also can take a raw text file that is
properly formatted (like the output of unpacking) and produce a new packed
game strings file. This can be used for translation of the game strings.

The program needs some parameters to work. Here's the syntax:

    strpak <command> <input-file> <output-file>

`<command>` can either be "unpack" or "pack". Unpacking expects a "strings.pak"-
like file, and writes to the output-file as text. Packing wants a raw text
file as input (see below) and writes a new "strings.pak"-like output file.

The file format for the input file for the "pack" command is the same as the
output of the "unpack" command:

    block: <block-id>; <rest is ignored>
    <number>: string number one
    <number>: this one contains a newline: \n there, it is
    <number>: and this is another one

    block: <block-id>; <rest is ignored>
    <number>: yes, another block

Note that empty lines are ignored, as well as the very first line in the file.
After the block-id, which must be a four-digit hexadecimal number, there must
be a semicolon. A string line contains a number followed by a ':' and exactly
one space. All content that is after that space counts as valid string data.
Newlines are expressed with "\n".

For more info about game strings and how they are used in Ultima Underworld,
especially in the conversations, just look into the "docs/uw-formats.txt" in
the source distribution of Underworld Adventures.

Short example (strpak is placed in the uw_demo folder of the Underworld Demo):

    strpak unpack data/strings.pak uw_demo-strings.txt

Then modify some stuff inside the created text file, e.g. translate strings.

    strpak pack uw_demo-strings.txt data/strings.pak

Be sure to keep a copy of the original "strings.pak" around, in case anything
bad happens.

### xmi2mid - XMIDI to MIDI converter [uw1/2]

xmi2mid is a converter for XMIDI files (*.xmi) that are used in Ultima
Underworld (and many other games) to ordinary midi (*.mid) files. Only the
first track of an XMIDI file is extracted, though. The calling syntax for the
program is:

    xmi2mid <xmi-file> <mid-file>

The program prints out an error if something strange occured, else the midi
file is created.

### luac - Lua Compiler

This is a tool that is used by the Underworld Studio (uastudio.exe) and
Underworld Debugger to compile Lua scripts to binary files. See more at:
https://www.lua.org/manual/5.3/luac.html
