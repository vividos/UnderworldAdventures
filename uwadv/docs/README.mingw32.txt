Underworld Adventures MinGW32 Readme
------------------------------------

This is the Readme file for compiling Underworld Adventures using MinGW32.

MinGW Setup
-----------

First, you should install mingw (and if you like, MSYS) from
http://www.mingw.org/. I recommend using the full distribution, e.g.
MinGW-1.1.tar.gz (or any later version).

To let the system find the make program and the compiler, you should add
something like this to your "autoexec.bat" (or wherever):

   PATH=%PATH%;c:\mingw\bin

In this example, the mingw package is installed to "c:\mingw\".

SDL Setup
---------

SDL 1.2.3 or later is needed to compile Underworld Adventures. Just download
the SDL mingw32 development package from http://www.libsdl.org/. Extract the
containing file "i386-mingw32msvc.tar.gz" to a folder and move the files in
the folders to the mingw folder tree, so that folders match. The SDL headers
should go into "c:\mingw\include\SDL\" (for our example). The SDL libraries
("libsdl.a" and "libsdlmain.a") can go in "c:\mingw\lib\".

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

Compiling
---------

To specify some paths, open the file "Makefile.mingw" in the "source/win32"
folder. If you don't want to use STLport, edit these two lines so that they
look like this:

STLPORT_INCLUDE = 
STLPORT_LIB = 

else let them point to the proper folders. Do the same for the "SDL_INCLUDE"
(and "FMOD_INCLUDE", "FMOD_LIB", if you want FMOD MIDI support).

To compile the project, just type:

   make -f Makefile.mingw

The executable "uwadv.exe" will be built. It additionally needs the SDL.DLL to
run. The DLL is available at http://www.libsdl.org/

To build the tools, type:

   make -f Makefile.mingw tools

More infos about the tools can be found in the file "README.tools.txt". Infos
about how to run Underworld Adventures can be found in the file
"README.uwadv.txt"
