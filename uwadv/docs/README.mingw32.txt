Underworld Adventures MinGW32 Readme
------------------------------------

This is the Readme file for compiling Underworld Adventures using MinGW.

MinGW Setup
-----------

To compile Underworld Adventures, you need MinGW and MSYS (a Linux-like
shell for Windows). The two packages are available at http://www.mingw.org/.
For MinGW, it's best to take a full distribution (e.g. MinGW-2.0.0-3.exe or
any later version).

To let the system find the compiler, you should add something like this to
your "autoexec.bat":

   PATH=%PATH%;c:\mingw\bin

You could also change the PATH variable assignment in /etc/profile using MSYS.
All further examples in this file assume that MinGW was installed to
"c:\mingw\".

Needed files
------------

To compile Underworld Adventures, you also need some files for the libraries
that are used. A package containing all these files is available here (in the
"uwadv-devel" section):

http://sourceforge.net/project/showfiles.php?group_id=50987

Just download the latest "uwadv-mingw32-installer-setup-?.zip" and install the
files according to the Readme-File that is included in the package.

STLport Setup
-------------

It is optional to install STLport 5.0, a good Standard C++ Library
implementation, available at http://www.stlport.com/. Note that STLport is not
absolutely needed, but you have to comment out some things in
"Makefile.mingw" later.

Just extract the STLport files into a folder, e.g. "c:\mingw\stlport\". To compile
the static library that is needed later, you have to go in the "build\lib"
subfolder and type:

   make -f gcc.mak release-static

The library "libstlport_static_r50.a" (which is needed later) should be
built and put into the "lib" folder.

Compiling
---------

To specify some paths, open the file "Makefile.mingw" in the main project dir.

The variable UWADV_PATH contains the path where Underworld Adventures is
installed when doing a "make install" or "make update" (see below for make
targets).

The variable MINGW_PATH should be set to the base path of the MinGW
installation. It is mainly to specify the SDL include path.

There are some settings that can be set to "yes" or "no". These are:

  WITH_CONSOLE    enables separate console output and disables writing of the
                  stdout.txt and stderr.txt file. default: "no"
  WITH_DEBUGGING  enables some experimental code for developers. Basically
                  defines HAVE_DEBUG in the source code. default: "no"
  WITH_STLPORT    enables linking against the static STLport library (see
                  above). Be sure to also set STLPORT_PATH. default: "yes"

To compile, start MSYS and change ("cd") to the main project dir. The command
to compile the project looks like this:

   make -f Makefile.mingw <target>

where target can be one of several words:

  target      description
  ------      -----------
  uwadv       builds uwadv.exe and copies a template uwadv.cfg to the main
              project dir
  tools       builds all tools (each tool can be built separately using one of
              these targets: cnvdbg xmi2mid mapdisp animview strpak
  data        builds all needed data files
  install     builds uwadv and data and installs the files in the path
              specified by "UWADV_PATH". overwrites uwadv.cfg
  update      the same as "install" but doesn't copy the uwadv.cfg file
  clean       cleans all built source and data files
  luac        builds the Lua compiler that can be used to verify Lua scripts
  toolsinstall installs all tools in a "tools" subfolder of "UWADV_PATH"

To run "uwadv.exe", SDL.dll and SDL_mixer.dll are needed. For the tools, only
SDL.dll is needed. The DLLs are available at http://www.libsdl.org/ or
http://www.libsdl.org/projects/SDL_mixer/

More infos about the tools can be found in the file "README.tools.txt". Infos
about how to run Underworld Adventures can be found in the file
"README.uwadv.txt". Please continue reading there.

Using autoconf and automake
---------------------------

If you're adventurous, you can try compiling Underworld Adventures using the
autoconf and automake tools. You need the msysDTK-packages as well as the
autoconf, automake and libtool packages extracted over your existing MinGW
installation. Continue with the instructions in README.Linux.txt under the
"Remarks" section. Good Luck!

Note: when this error occurs:

   NONE:0: /bin/m4: Expecting line feed in frozen file

try converting the file <mingw-path>/share/autoconf/autoconf/autoconf.m4f to
use unix-style line endings, e.g. dos2unix.
