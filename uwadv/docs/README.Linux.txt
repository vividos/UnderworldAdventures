Underworld Adventures Linux Readme
----------------------------------

This is the Readme file for Linux and all other similar operating systems,
such as FreeBSD, MacOS X etc.

Prerequisites
-------------

To compile and install Underworld Adventures, you need several things. These
are:

 * GCC 2.95.2 or higher
 * a working X window system
 * SDL 1.2.3 (or higher), available at http://www.libsdl.org/
 * Mesa (or another working OpenGL implementation) http://www.mesa3d.org

Additionally, you can install FMOD to hear MIDI music.

Compiling
---------

To start compiling, go into the folder source/linux, and type:

   ./configure

The configure script should detect all needed things to compile Underworld
Adventures, such as the C++ compiler, paths to X includes/libs, SDL, Mesa and
(if installed) FMOD. Note that FMOD isn't absolutely needed and can be
omitted.

When the script ran through without errors, type:

   cd ..
   make

Now the source code should be compiled (which takes a while), and when
finished, the executable "uwadv" is built. Now type (as root now):

   make install

The appropriate files are now installed into their proper folders. More infos
about running Underworld Adventures is in the file README.uwadv.txt

Remarks
-------

Installing FMOD:

   FMOD comes with some header files and a shared library. There's no install
   script, so it's a bit harder to install it.

   To install the two header files ("fmod.h" and "fmod_errors.h"), you have to
   be root. Just copy them to /usr/local/include/ or /usr/include/

   To install the shared library, you have to copy the file ("libfmod-3.5.so"
   in the latest release at this time) to /usr/lib/ (or again /usr/local/lib/)
   and run ldconfig in that directory, to let the linker know that a new
   library is available.
