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

### mapdisp - Underworld Map Display [uw1/2]

mapdisp is a program to show an "eagle's eye" view of each level of the
underworld. The user can rotate and move the camera to view all details of a
specific level.

The program should be placed in the Ultima Underworld 1, 2, or demo folder,
directly where the original executable file resides.

You can move around the camera viewpoint either with the cursor keys or with
WASD keys like in the game. When left-clicking on the view, the camera angle
can be changed. The camera always moves in the view direction.

Page Up and Page Down keys select another level in the abyss, either above
or beyond the current level (when using the uw_demo this won't work, since the
demo only has one underworld level stored). The window caption shows infos about
the current level.

Screenshots can be created by pressing the "print screen" key. A file
`mapdisp-shot.png` is created in the executable's folder. When holding down
the Shift key while pressing "print screen", a large screenshot is taken and
stored.

mapdisp can be started with the parameter "allmaps"; it then shows all maps
simultaneously as they would be in real space. You can move around the scene
as usual.

Reference Card:

    action                 result
    left-drag              rotates camera view
    WASD, cursor keys      moves level towards or from the viewer
    page up / down         changes underworld level
    print screen           takes a screenshot
    shift + print screen   takes a large screenshot


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


### uwdump - Ultima Underworld data dump program

uwdump is a program to extract and show information about many data structures
used in Ultima Underworld. Textual output is always written to standard output
where it can be redirected, e.g. to a text file. The program has the following
syntax:

    uwdump <options> <command> <options>

Options and the command can be in any order, but there can only be one
command. Here is a list of supported commands:

`dump <file>`

Dumps the specified file. Depending on the filename, different information is
dumped. The file path can contain wildcards to dump many or all files at once.
Note that not all files are currently supported for dumping.

`view <file>`

Views a specified file with a dedicated viewer, e.g. images or 3D models. Note
that only some files are supported for viewing.

The following options are available for all commands:

    -d<basepath>  sets uw1/uw2 path; using current folder when not specified
                  (game type is auto-detected)
    -2            set game type to ultima underworld 2 without further checking

Here are some examples for calling uwdump. It is assumed that the current
folder is the game's main folder:

    uwdump -d c:\uw1\ dump c:\uw1\data\lev.ark > uw1-leveldump.txt
    uwdump dump save1\lev.ark
    uwdump -d . dump data\comobj.dat > uw1-properties.txt


### convdbg - Underworld Conversation Debugger

convdbg is a conversation script debugger for Ultima Underworld. Conversation
is done with an assembler-like language that runs in a virtual machine. The
debugger looks and behaves more or less like gdb, the GNU debugger.

The program needs some parameters to work. Here's the syntax:

    convdbg <underworld-path> <bglobals-file>

`<underworld-path>` is the path to the Ultima Underworld 1 or 2 folder, ending
with a backslasl. The parameter `<bglobals-file>` is the relative file path
to the conversation globals, e.g. the default `"data\babglobs.dat"` or a
`bglobals.dat` from a savegame folder.

If you are in the install folder of Ultima Underworld, you can just call
convdbg like this:

    convdbg .\ Save1/bglobals.dat

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

### convdec - Underworld Conversation Decompiler

convdbg is a conversation script decompiler for Ultima Underworld.
Conversation is done with an assembler-like language that runs in a virtual
machine. The tool tooks the assembler opcodes and tries to find functions,
expressions and structured programming constructs (like if, switch and while
loops). The tool also resolves the string indices used and so lets the user
better understand the conversation code.

The program needs some parameters to work. Here's the syntax:

    convdbg <underworld-path> <slotnumber> {--show-disasm}

The `underworld-path` specifies the path to the original Ultima Underworld 1
or 2 game files. The Ultima Underworld Demo also works.

The slot number specifies the conversation slot to use for decompiling. Slot
numbers can be derived from the conversation names stored in string block 7,
starting at string 16. So subtract 16 from the conversation name index to
specify the slot. If you specify a star ('*' without the quotes), all
available conversation slots are decoded sequentially.

The --show-disasm is optional and can be specified as last parameter, in order
to also show disassembly for the decoded conversations. Normally only the
decompiled conversation language is shown. With this switch, the conversation
position and opcodes are shown, interleaved with the decompiled code.

The decompiled conversation code looks a lot like C code, but isn't thought
to be compiled with a C compiler. It is merely used to better show the user
what each conversation does, what internal functions are called and how the
conversation flow is.

### xmi2mid - XMIDI to MIDI converter [uw1/2]

xmi2mid is a converter for XMIDI files (*.xmi) that are used in Ultima
Underworld (and many other games) to ordinary midi (*.mid) files. Only the
first track of an XMIDI file is extracted, though. The Underworld xmi files
all have only one track anyway. The calling syntax for the
program is:

    xmi2mid <xmi-file> <mid-file> {conversion-option}

The third parameter {conversion-option} is optional and can be one of the
following switches:

    --mt32-to-gm    Converts from MT-32 to General MIDI patches
    --mt32-to-gs    Converts to GS patches
    --mt32-to-gs127 Converts to GS patches with a MT-32 patch bank installed
                    in Bank 127
    --gs127-to-gs   Converts from GS patches using Bank 127 to one that
                    doesn't
    --noconvert     No conversion is done

The default option is `--mt32-to-gm` when the parameter is not specified.

The program prints out an error if something strange occured, else the midi
file is created.

### luac - Lua Compiler

This is a tool that is used by the Underworld Studio (uastudio.exe) and
Underworld Debugger to compile Lua scripts to binary files. See more at:
https://www.lua.org/manual/5.3/luac.html
