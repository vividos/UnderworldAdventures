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
 * Mesa (or any other working OpenGL implementation) http://www.mesa3d.org

Additionally, you can install FMOD to hear MIDI music.

Compiling
---------
(Note: if you are building a CVS copy, refer to the 'Remarks' section)

To start compiling, go to the main uwadv directory (containing "configure"),
and run

   ./configure

The configure script should detect all needed things to compile Underworld
Adventures, such as the C++ compiler, paths to X includes/libs, SDL, Mesa and
(if installed) FMOD. Note that FMOD isn't absolutely needed and can be
omitted.

When the script ran through without errors, type:

   make

Now the source code should be compiled (which takes a while), and when
finished, the executable "uwadv" is built. Now type (as root now):

   make install

The built files are now installed into their proper folders.
(The binary is installed into /usr/local/bin by default, the data files
into /usr/local/share/games/uwadv)

To run Underworld Adventures under Linux, make sure that all Ultima
Underworld files and folders have lowercase names, or else the files cannot
be found. There is a perl script called "ua-lowercase-all.pl" in the
"source/linux" folder that does the needed conversion.

More information about running Underworld Adventures can be found in the file
"README.uwadv.txt".

Remarks
-------

Installing FMOD:

   FMOD comes with some header files and a shared library. There's no install
   script, so it's a bit harder to install.

   To install the two header files ("fmod.h" and "fmod_errors.h"), you have to
   be root. Just copy them to "/usr/local/include/" or "/usr/include/". Be
   sure that the two include files have linux line endings. When not, convert
   them using "recode ibmpc..lat1 <file>".

   To install the shared library, you have to copy the file ("libfmod-3.5.so"
   in the latest release at this time) to "/usr/lib/" (or again
   "/usr/local/lib/") and run ldconfig in that directory, to let the linker
   know that a new library is available.


Building from CVS:

   To build from CVS you will need several other tools, including:

    * autoconf 2.53 (or higher)
    * automake 1.5 (or higher)

   To generate the required files, go to the main uwadv directory (containing
   configure.ac), and run

    ./autogen.sh

   Continue by following the 'Compiling' instructions
