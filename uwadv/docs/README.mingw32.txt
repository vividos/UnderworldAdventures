Underworld Adventures MinGW32 Readme
------------------------------------

This is the Readme file for compiling Underworld Adventures using MinGW32.

MinGW Setup
-----------

First, you should install mingw32 and MSYS, available from
http://www.mingw.org/. MSYS is needed for compiling uwadv and STLport. For
mingw32, either get a full distribution, e.g. MinGW-2.0.0-3.exe (or any later
version) or get the packages separately (e.g.):

   gcc-3.2-core-20020817-1
   binutils-2.13-20020903-1
   mingw-runtime-2.2
   w32api-2.0
   make-3.79.1-20010722 

an older set of the files using gcc-3.1 would be:

   gcc-3_1-core-20020516-1
   binutils-2_12_90-20020518-1
   w32api-1.5
   mingw-runtime-2.0
   make-3.79.1-20010722

To let the system find the make program and the compiler, you should add
something like this to your "autoexec.bat" (or wherever):

   PATH=%PATH%;c:\mingw\bin

In this example, the mingw package was extracted to "c:\mingw\". The main
project dir always is the folder with the Makefile.mingw (among others).

SDL/SDL_mixer Setup
-------------------

SDL 1.2.3 or later is needed to compile Underworld Adventures. Just download
the SDL mingw32 development package from http://www.libsdl.org/. Extract the
containing file "i386-mingw32msvc.tar.gz" to a folder and move the files in
the folders to the mingw folder tree, so that folders match. The SDL headers
should go into "c:\mingw\include\SDL\" (for our example). The SDL libraries
("libsdl.a" and "libsdlmain.a") can go in "c:\mingw\lib\".

Additionally, SDL_mixer is needed. The development package can be obtained
from http://www.libsdl.org/projects/SDL_mixer/. The MSVC dev package also
works for mingw32, as the mingw32 seems to be able to link in SDL_mixer.lib.

STLport Setup
-------------

It is optional to install STLport, a good Standard Template Library
implementation, available at http://www.stlport.com/. Note that STLport is not
absolutely needed, but you have to comment out some things in
"Makefile.mingw" later.

Just extract the STLport files into a folder, e.g. "c:\mingw\stl\". To compile
the static library that later is needed, you have to go in the "src" subfolder
and type:

   make -f gcc-mingw32.mak release_static

The library "libstlport_mingw32_static.a" should have been built in the "lib"
folder which is needed later.

zlib Setup
----------

The zlib library to compress and decompress files is also needed. A
development package for zlib-1.1.4 is available here:

   http://uwadv.sourceforge.net/devel/zlib-1.1.4-devel-uwadv.zip

The Readme.txt file in the zip archive explains the contents of the file.

Compiling
---------

To specify some paths, open the file "Makefile.mingw" in the main project dir.

The variable UWADV_PATH contains the path where Underworld Adventures is
installed when doing a "make install" or "make update" (see below for make
targets).

To set the SDL include path, adjust the path after "SDL_INCLUDE". If you don't
want to use STLport, edit these two lines so that they look like this:

STLPORT_INCLUDE = 
STLPORT_LIB = 

To compile, start MSYS and cd to the main project dir. The command to compile
the project looks like this:

   make -f Makefile.mingw <target>

where target can be one of several words:

  target      description
  ------      -----------
  uwadv       builds uwadv.exe and copies a template uwadv.cfg to the main
              project dir
  tools       builds all tools (each tool can be built separately using one of
              these targets: cnvdbg cnvdasm xmi2mid mapdisp animview strpak
  data        builds all needed data files
  install     builds uwadv and data and installs the files in the path
              specified by "UWADV_PATH". overwrites uwadv.cfg
  update      the same as "install" but doesn't copy the uwadv.cfg file
  clean       cleans all built source and data files
  luac        builds the Lua compiler that can be used to verify Lua scripts

To run "uwadv.exe", SDL.dll and SDL_mixer.dll are needed. For the tools, only
SDL.dll is needed. The DLLs are available at http://www.libsdl.org/ or
http://www.libsdl.org/projects/SDL_mixer/

More infos about the tools can be found in the file "README.tools.txt". Infos
about how to run Underworld Adventures can be found in the file
"README.uwadv.txt". Please continue reading there.
