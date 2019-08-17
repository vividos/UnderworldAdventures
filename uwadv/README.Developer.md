# README for Developers

This is the Readme file for developers. Please use the table of contents to
navigate the document.

[1. Introduction](#1-introduction)<br/>

[2. Compiling Underworld Adventures](#2-compiling-underworld-adventures)<br/>
[2.1 Microsoft Visual Studio on Windows](#2-1-microsoft-visual-studio-on-windows)<br/>
[2.2 gcc/clang on Linux](#2-2-gcc-clang-on-linux)<br/>
[2.2.1 Building from Git](#2-2-1-building-from-git)<br/>
[2.2.2 Building from source package](#2-2-2-building-from-source-package)<br/>
[2.3 MinGW on Windows](#2-3-mingw-on-windows)<br/>
[2.4 Other operating systems](#2-4-other-operating-systems)<br/>

[3. Developing Underworld Adventures](#3-developing-underworld-adventures)<br/>
[3.1 Code guidelines](#3-1-code-guidelines)<br/>
[3.2 Source code repository](#3-2-source-code-repository)<br/>
[3.2.1 Git repository](#3-2-1-git-repository)<br/>
[3.2.2 GitHub issues & pull requests](#3-2-2-github-issues-pull-requests)<br/>
[3.3 Release Checklist](#3-3-release-checklist)<br/>
[3.4 Source code documentation](#3-4-source-code-documentation)<br/>
[3.5 Resource files in the "data" folder](#3-5-resource-files-in-the-data-folder)<br/>

[4. Lua Scripting](#4-lua-scripting)<br/>

[A. Authors](#a-authors)<br/>

[B. Third-Party libraries and licenses](#b-third-party-libraries-and-licenses)<br/>

## 1. Introduction

This document is an introduction for developers who want to compile from
source code, just wait to understand the source code or event want to start
coding for the Underworld Adventures project. The document describes all that
a developer has to know. For further reference the source code documentation
can be used.

Underworld Adventures is developed entirely in C++, with a few external
libraries. Lua was chosen as the script language to use. For more on Lua
interfaces to the game, please look into the chapter "Lua Scripting".


## 2. Compiling Underworld Adventures

There are several platforms on which Underworld Adventures can run.

### 2.1 Microsoft Visual Studio on Windows

Underworld Adventures can be compiled using the latest Visual Studio version
from Microsoft. As of writing this document, Visual Studio 2019 is the
current version. Any later version should also work. Either Community or
Professional version should work. Install the "C++" workload.

Open the "uwadv.sln" file located in this folder. You see several projects.
Set the "uwadv" project as startup project and select either Debug or Release
configuration. Compile all projects using "Build > Build Solution" or start
debugging with F5, which also builds the relevant projects.

All output files are stored in the "output" folder directly beneath this
folder. You see a "bin" folder where the resulting binary files are placed,
an "intermediate" and "lib" folders with built files. The folders are further
subdivided in "Debug" and "Release" folders.

In order to debug the project directly in its created "bin" folder, go to the
"uwadv" project's properties, and in the "Debugging" section set the value for
"Working Directory" to "$(OutDir)" (without the double quotes). This way the
game can find its uwadv.cfg and related resources.

More information about running Underworld Adventures can be found in the file
"README.Manual.txt".

### 2.2 gcc/clang on Linux

To compile and install Underworld Adventures, you need several development
packages. These are:

 - GCC 7.0 or higher, or
 - CLang 5.0 or higher
 - SDL2 2.0.9 (or higher)
 - SDL_mixer 2.0.5 (or higher)

To hear Midi music using SDL_mixer, you additionally need the sound patches
available on the SDL_mixer home page.

All the packages can be installed using the package manager of your Linux
distribution of choice.

Depending on if you build from scratch using the Git repository, or if you're
using a source package, read the following respective sub topics.

#### 2.2.1 Building from Git

(Note that you can skip this part if you are using a source package).

To build from Git you will need several other tools, including:

 - autoconf 2.53 (or higher)
 - automake 1.6.3 (or higher)
 - libtool 1.4.1 (or higher)

To generate the required files, go to the main uwadv directory (containing
configure.ac), and run

    ./autogen.sh

Once the command is finished, you have the same files as when using the source
package.

#### 2.2.2 Building from source package

To start compiling, go to the main uwadv directory (containing "configure"),
and run

    ./configure

The configure script should detect all needed things to compile Underworld
Adventures, such as the C++ compiler, paths to SDL, SDL_mixer and the OpenGL
include file. There are several options that can be passed to the configure
script:

    --with-uw1=DIR          directory where UW1 is installed
    --disable-tools         don't build the tools
    --enable-debug          enable debugging (disables optimizations)

When the configure script is finished, type:

    make

Now the source code should be compiled (which takes a while), and when
finished, the executable "uwadv" is built. Now type (as root now):

    make install

The built files are now installed into their proper folders.
(The binary is installed into /usr/local/bin/ by default, the data files
into /usr/local/share/games/uwadv/)

More information about running Underworld Adventures can be found in the file
"README.Manual.txt".

Uninstalling: If you built and installed Underworld Adventures from source,
you can uninstall it by running (as root):

    make uninstall

### 2.3 MinGW on Windows

MinGW stands for "Minimalist Gnu for Windows" and can be obtained in various
versions, e.g. MinGW32 or MinGW64. The binary distributions come with a Gnu
bash console called MSYS and can be used to build Underworld Adventures.

#### Setup

To let the system find the compiler, you should add something like this to
your "autoexec.bat":

   PATH=%PATH%;c:\mingw\bin

You could also change the PATH variable assignment in /etc/profile using MSYS.
All further examples in this file assume that MinGW was installed to
"c:\mingw\".

#### Compiling

To specify some paths, open the file "Makefile.mingw" in the main project
folder.

The variable `UWADV_PATH` contains the path where Underworld Adventures is
installed when performing a "make install" or "make update" (see below for
make targets).

The variable `MINGW_PATH` should be set to the base path of the MinGW
installation. It is mainly to specify the SDL include path.

There are some settings that can be set to "yes" or "no". These are:

- WITH_CONSOLE: enables separate console output and disables writing of the
                stdout.txt and stderr.txt file. default: "no"
- WITH_DEBUGGING: enables some experimental code for developers. Basically
                  defines `HAVE_DEBUG` in the source code. default: "no"

To compile, start MSYS and change ("cd") to the main project dir. The command
to compile the project looks like this:

    make -f Makefile.mingw <target>

where `target` can be one of several words:

- uwadv:       builds uwadv.exe and copies a template uwadv.cfg to the main
               project dir
- tools:       builds all tools (each tool can be built separately using one
               of these targets: `cnvdbg xmi2mid mapdisp animview strpak`)
- data:        builds all needed data files
- install:     builds uwadv and data and installs the files in the path
               specified by "UWADV_PATH". overwrites uwadv.cfg
- update:      the same as "install" but doesn't copy the uwadv.cfg file
- clean:       cleans all built source and data files
- luac:        builds the Lua compiler that can be used to verify Lua scripts
- toolsinstall: installs all tools in a "tools" subfolder of "UWADV_PATH"

#### Using autoconf and automake

If you're adventurous, you can try compiling Underworld Adventures using the
autoconf and automake tools. You need the msysDTK-packages as well as the
autoconf, automake and libtool packages extracted over your existing MinGW
installation. Continue with the instructions in chapter 2.3. Good Luck!

### 2.4 Other operating systems

Other Unix based operating system (FreeBSD, MacOS X etc.) may work like on
Linux, but no guarantees if it works. If you want to port the project to a new
platform, please contact the project team.

## 3. Developing Underworld Adventures

## 3.1 Code guidelines

Here are some code guidelines for the uwadv project that every developer
should follow when commiting code via Git:

- tabs should be converted to spaces, indendation size is 3 spaces; don't
  reindent code that wasn't changed by you
- always document your code, use doxygen-style comments for functions
- don't use "using namespace std;", use e.g. std::string instead
- use C++ style casts when possible
- use std::ostringstream instead of sprintf()
- use "T* var" and "T& var" for references and pointers (not "T *var" or
  "T &var")
- throw exceptions on serious errors (e.g. when you absolutely can't
  continue)
- use SDL functionality where possible, to stay platform independent
- use SDL data types, such as Uint32, Uint16 when needed
- don't accumulate all your changes in one Git commit, separate commits
  that doesn't have something in common

## 3.2 Source code repository

The source code and related files for Underworld Adventures are stored in a
Git repository that is hosted on GitHub.com. You can clone the repository from
the official site:

   https://github.com/vividos/UnderworldAdventures

There are some Git GUIs for Windows; I recommend:

* GitExtensions: https://gitextensions.github.io/

  The tool shows a window with the currently opened repository and the commit
  history. Almost all necessary commands are available as 

### 3.2.1 Git repository

Some guidelines:

- Development happens on the `master` branch.
- All pull requests should be based on `master` as well.
- Tag names are in the format `x.x-codename`, e.g. `0.9-mojito`.
- Please set Unix-to-DOS line ending option `core.autolf` to `true`.
- You can use branches in PRs, but rebase them on master before requesting the
  PR.

### 3.2.2 GitHub issues & pull requests

For bug reports or enhancement wishes, please write a GitHub issue. Also write
an issue if you want to work on any part of the project, or comment an
existing ticket. Only then work on the bug or feature, and then submit a PR.

Pull requests must contain commits that are clear and can be understood from
only reading the diff or by additionally reading the commit text. Be verbose
with the commit text, if needed. Pack code reformatting or code restructuring
in a separate commit, or even a separate pull request if it's large. Follow
the coding guidelines.

If you want to add small amounts of code, or want to make small additions to
documentation (e.g. the uw-formats.txt file), just open an issue and I'll add
the change manually. You can also send an email to me; visit the
[Development](https://vividos.github.io/UnderworldAdventures/development.html)
page.

## 3.3 Release Checklist

Here is the release checklist of steps to do for a successful Underworld
Adventures release:

1. Think about the next funny release name

2. Be sure that the project (uwadv, tools) compiles AND works on all
   systems where build files exist. Be sure to test against uw1 and
   uw_demo. Build systems to test for are: mingw32, mingw32 configure.ac,
   MSVC, Linux configure.ac, Linux RPM

3. Adjust version numbers in configure.ac, source/Makefile.mingw,
   source/doxygen/doxygen.cfg, source/win32/uwadv_res.rc,
   source/win32/uadebug/uadebug_res.rc and
   source/win32/uaconfig/uaconfig_res.rc

4. Update ChangeLog file and commit it

5. Add and push a Git tag to the repository, with version and release name as
   tag name.

6. Build the Windows and Linux versions of the source and binary packages,
   using the AppVeyor build service.

7. Upload built packages to GitHub, as a new release

9. Update Underworld Adventures home page with latest news

## 3.4 Source code documentation

The Underworld Adventures source code is heavily commented using
doxygen-style comments for modules, files, classes, class members, structs,
enums and typedefs. The source code documentation generated via doxygen can
be used as reference to understand the source code.

The source code documentation can be generated with the following commands
(change into the "doxygen" folder before):

    doxygen doxygen.cfg

If you're using Visual Studio, you can also build the "doxygen" project. The
output of the tools is automatically listed in the "Error List" window.

The HTML documentation is created in the folder "html", where "index.html"
is the entry point to the source code documentation. The file
"doxygen-warnings.txt" contains the program's warnings.

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

The "Underworld Adventures Resource" file (extension .zip) package the
contents of the "uadata" source tree into one or more files. Name scheme looks
like this: "uadataXX.zip" (printf syntax: "uadata%02u.zip").

When the game searches for a "uadata" file, e.g. "uw1/sound/test.wav", it
first searches the true folder, "uadata/uw1/sound/test.wav". If that
doesn't exist, all "uadata??.zip" files are tested if the file is in one of
it. When not found, an exception is thrown.

The Makefile in the "uadata" source folder creates packaged files that have
all the files from the whole uadata tree in one file. Just do a

    make all

to build all packaged files that are needed for distribution. For
developing and debugging the raw files can be used.

Lua scripts can have the extension ".lua" or ".lob". ".lua" files contain
scripts as text, i.e. not precompiled. ".lob" files are "lua object" files,
that are lua files compiled with "luac".

Lua scripts are searched in the same way as other files, but first the
".lua" one is searched, then the ".lob". That allows for changes to ".lua"
files while developing, but for releases the compiled ".lob" files inside
"uadata??.zip" files are distributed only.


## 4. Lua Scripting

For Lua to C interfaces or other information about Lua with uwadv, look
into the file "README.Lua.txt".


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
- Sam Matthews (servus; ideas)
- Marc A. Pelletier (Coren; ideas)


## B. Third-Party libraries and licenses

Some third-party libraries are used throughout the project, to gain some
functionality that otherwise would be hard to do, or to get a platform
independent game.

### SDL2 & SDL_Mixer

The project is using the [SDL2 library](https://www.libsdl.org/) to implement
the cross-platform game.

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
