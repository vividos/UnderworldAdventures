Underworld Adventures Linux Readme
----------------------------------

This is the Readme file for Linux and all other similar operating systems,
such as FreeBSD, MacOS X etc.

Prerequisites
-------------

To compile and install Underworld Adventures, you need several things. These
are:

 * GCC 2.95.2 or higher
 * SDL 1.2.3 (or higher), available at http://www.libsdl.org/
 * SDL_mixer 1.2.4 (or higher), available at http://www.libsdl.org/projects/SDL_mixer/
 * Mesa (or any other working OpenGL implementation) http://www.mesa3d.org

To hear Midi music using SDL_mixer, you additionally need the sound patches
available on the SDL_mixer home page.

Compiling
---------
(Note: if you are building a CVS copy, refer to the 'Remarks' section)

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

To run Underworld Adventures under Linux, make sure that all Ultima
Underworld files and folders have lowercase names, or else the files cannot
be found. There is a perl script called "ua-lowercase-all.pl" in the
"source/linux" folder that does the needed conversion.

More information about running Underworld Adventures can be found in the file
"README.uwadv.txt".

Remarks
-------

Uninstalling:

   If you built and installed Underworld Adventures from source, you can
   uninstall it by running (as root)

      make uninstall


Building from CVS:

   To build from CVS you will need several other tools, including:

    * autoconf 2.53 (or higher)
    * automake 1.6.3 (or higher)
    * libtool 1.4.1 (or higher)

   To generate the required files, go to the main uwadv directory (containing
   configure.ac), and run

    ./autogen.sh

   Continue by following the 'Compiling' instructions.
