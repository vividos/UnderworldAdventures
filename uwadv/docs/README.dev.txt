Underworld Adventures Developer's Readme
----------------------------------------

This is the uwadv developer's readme file. It explains all stuff that is the
uwadv program, and all associated stuff, as well as more stuff.


Table of Contents
-----------------

   1. Introduction
   1.1. Used libraries
   1.2. Code guidelines
   1.3. doxygen use
   1.4. CVS use
   1.5. Release procedure

   2. Basic uwadv stuff
   2.1. Game utility classes
   2.2. Os porter's guide
   2.3. Core interface
   2.4. Screens mechanism
   2.5. Resource loading

   3. Underworld object
   3.1. Object members
   3.2. Levelmap
   3.3. Player, paperdoll, inventory
   3.4. Conversations

   4. "uadata" folder
   4.1. Makefile
   4.2. Lua scripts


1. Introduction

Underworld Adventures is done entirely in C++, with a few C addon libraries.
For the script language, Lua was chosen. For more on Lua interfaces to the
game, look into "README.Lua.txt".


1.1. Used libraries

Some libraries are used throughout the project, to gain some functionality
that otherwise would be hard to do, or to get a platform independent game.

* OpenGL 1.1 (or greater)
  3D graphics rendering library
* SDL 1.2.3 (or greater), http://www.libsdl.org/
  a cross-platform support library
* SDL_Mixer 1.2.4 (or greater), http://www.libsdl.org/projects/SDL_mixer/
  cross-platform audio mixer and playback library
* Lua 4.0.1, http://www.lua.org/
  scripting language


1.2. Code guidelines

Here are some code guidelines for the uwadv project that every developer
should consider when commiting code via CVS:

 - tabs should be converted to spaces, indendation size is 3 spaces
   don't reindent code that wasn't changed by you
 - always document your code, use doxygen-style comments for functions
 - don't use "using namespace std;", use e.g. std::string instead
 - use C++ style casts when unsure
 - use std::ostringstream instead of sprintf()
 - use "T* var" and "T& var" for refs and pointers (not "T *var" or "T &var")
 - throw exceptions on serious errors (e.g. when you absolutely can't continue)
 - use SDL functionality where appropriate, to stay platform independent
 - use SDL data types, such as Uint32, Uint16 when needed
 - don't accumulate all your changes in one cvs commit, separate commits that
   doesn't have something in common


1.3. doxygen use

Throughout the code, doxygen-style comments for functions, typedefs, structs
and files are used. To create the doxygen source code documentation, change
into the "docs" folder and type:

   doxygen ua-doxygen.cfg

The html-documentation is created in the folder "html", where "index.html" is
the entry point to the source code documentation. The file
"doxygen-warnings.txt" contains the program's warnings.

doxygen is available from here:

   http://www.doxygen.org/


1.4. CVS use

The source code and related files for Underworld Adventures is stored in CVS,
using the SourceForge CVS repository services. Anonymous access:

   cvs -d:pserver:anonymous@cvs.uwadv.sourceforge.net:/cvsroot/uwadv login
   password:<return>

   cvs -z9 -d:pserver:anonymous@cvs.uwadv.sourceforge.net:/cvsroot/uwadv co uwadv

Developer access (requires SF account and project membership):

   cvs -z9 -d:ext:<username>@cvs.uwadv.sourceforge.net:/cvsroot/uwadv co uwadv

Once the project files are checked out and have a CVS folder, the -d option
can be omitted. That also means that developer commits for anon-cvs checkouts
are not possible.

There are some CVS GUIs for Windows:

* TortoiseCVS: http://www.tortoisecvs.org/
  It works as shell extension for the Windows Explorer and has almost all
  important cvs commands developers need. Unix-to-DOS line ending conversion
  is done automatically.

* WinCVS: http://www.wincvs.org/
  Explorer-like browser interface for CVS, but a bit cumbersome to set up. It
  has some specialities like version tree graph. Be sure to have unchecked the
  option "Checkout text files with the Unix LF (0xa)" when checking out files.

Occasionally the ChangeLog in the main project dir is updated from the cvs log
messages. This is done using the perl script "cvs2cl.pl" available here:

   http://www.red-bean.com/cvs2cl/

The complete command to update the ChangeLog file is:

   cvs2cl.pl -r -S --no-wrap --gmt


1.5. Release procedure

Here are the steps for a successful uwadv release:

1. Think about the next funny release name

2. Be sure that the project (uwadv, tools) compiles AND works on all systems
   where build files exist. be sure to test against uw1 and uw_demo
   for uwadv systems are: mingw32, MSVC, Linux configure.ac, Linux RPM

3. Adjust version numbers in configure.ac and source/win32/uwadv.nsi

4. Create latest ChangeLog and commit it

5. Tag repository using "cvs rtag", using the release name as tag name

6. Do a "make dist", using a fresh checkout using the tag name

7. Upload source package to SourceForge

8. Let the platform porters compile binary versions for distribution

9. Update Underworld Adventures home page with latest news

10. Send mails to news sites


2. Basic uwadv stuff

This chapter explains some basic mechanics of the uwadv project. From here on
it is is useful if you know the original game's file formats, as described in
"uw-formats.txt". It is assumed that the reader has some knowledge of C++ and
object oriented programming.


2.1. Game utility classes

To support the game's inner workings, there are some classes that can be used
to do certain things.

* ua_exception

  This is the standard exception that is used throughout uwadv. It takes a
  std::string as argument, the description of the fatal error. It is derived
  from std::exception, so catch() ua_exception before the parent class.

  implementation: source/uatypes.hpp

* ua_smart_ptr

  The smart pointer template class wraps a normal pointer and adds reference
  counting. when the class is copied (i.e. in an STL container), the reference
  count is incremented, and the pointer is just copied. When the smart pointer
  is destroyed, the real pointer is only deleted when the refcount is
  decremented to zero.

  implementation: source/uatypes.hpp

* ua_vector2d

  The 2d vector class just resembles a 2d vector. Some operators are
  overloaded, and even the 2d inner (or dot) product can be calculated.
  Nothing really exciting here. Angles are, as everywhere in uwadv, in degree.

  implementation: source/uamath.hpp

* ua_image and ua_image_list

  These classes are there to load images stored in *.gr files from the
  original game. ua_image holds one image, while ua_image_list is a list of
  images (as there are more than one image in a *.gr file). Images stored in
  *.byt files can also be loaded.

  The image can be pasted into other ua_image objects, and can be converted to
  a texture. As ua_image objects only store indices into a truecolor palette,
  the texture manager (see below) or a ua_onepalette object is needed for
  conversion.

  implementation: source/image.hpp and .cpp, source/resource/imageloader.cpp

* ua_texture_manager and ua_texture

  The ua_texture class can be used to store and upload textures to OpenGL.
  Textures are usually of size 2^n x 2^m (where n and m are integral numbers).
  The ua_texture class can be used in two ways; a) uploading texture once and
  using it consecutively and b) uploading texture each rendered frame.

  init() has to be called for every ua_texture object. The number of textures
  (which must have the same size) can be specified, as well as the min. and
  mag. filter method. To convert an ua_image to a texture, convert() can be
  called, and the new 32-bit color pixels are stored in the texture (without
  uploading it yet).

  To let OpenGL know which texture to use next (for rendering or uploading),
  call use(). upload() sends the texture to the graphics card (which may slow
  down a bit).

  After the call to use(), the usual OpenGL texture calls can be used.
  get_tex_u() and get_tex_v() can be used to retrieve maximum u and v
  coordinates in texture (e.g. when having a 50x50 image in a 64x64 texture).
  When finished, done() should be called.

  A pointer to a ua_texture_manager (which will be explained in detail later)
  can be passed to init() to keep the texture manager informed about usage of
  the texture.

  implementation: source/texture.hpp and .cpp, source/resource/texloader.cpp

More utility classes that have to do with the game core can be found in
chapter "2.3. Core interface".


2.2. Os porter's guide

uwadv has an abstract class for the game called ua_game, defined in
"source/uwadv.hpp and .cpp". To start a game, derive from that class and
implement all needed virtual methods. For examples, look at the file
"source/win32/source/game_win32.hpp and .cpp" or "source/linux/main.cpp".

These are the functions that can be customized:

   virtual void init();
   virtual void system_message(SDL_SysWMEvent &syswm);
   virtual void done();
   virtual void error_msg(const char *msg);

for derived methods, it is recommended to call the parent's method first.

system_message() is called for a system specific window manager event and can
be used to handle window messages, etc. The ua_game implementation does
nothing.

error_msg() is called to display an error message, e.g. in a message box. The
game exits after this function. The ua_game implementation prints out the
message to stderr.


2.3. Core interface

Here's a list of classes used in the core interface, followed by a more
in-depth look at them:

* ua_settings
* ua_files_manager
* ua_texture_manager
* ua_gamestrings
* ua_audio_interface
* ua_game_core_interface

2.3.1. Core interface classes

* ua_settings

  todo

  implementation: source/settings.hpp and .cpp,
  source/resource/settingsloader.cpp


2.4. Screens mechanism

todo


2.5. Resource loading

todo


3. Underworld object

todo

3.1. Object members

todo


3.2. Levelmap

todo


3.3. Player, paperdoll, inventory

todo


3.4. Conversations

todo


4. "uadata" folder

The "uadata" folder contains files that didn't came with the original game
files of Ultima Underworld, but are needed for the game to run. They can
contain enhancements to graphics, new audio files, game scripts or new 3D
models.

The "Underworld Adventures Resource" file (extension .uar) package the
contents of the "uadata" tree into one or more files. Name scheme looks like
this: "uadataXX.uar" (printf syntax: "uadata%02u.uar"). uar files essentially
are zip files with other extension.

When the game searches for a "uadata" file, e.g. "uw1/sound/test.wav", it
first searches the true folder, "uadata/uw1/sound/test.wav". If that doesn't
exist, all "uadata??.uar" files are tested if the file is in one of it. When
not found, an exception is thrown.


4.1. Makefile

The Makefile in the "uadata" folder creates packaged files that have all the
files from the whole uadata tree in one file. Just do a

   make all

to build all packaged files that are needed for distribution. For developing
the raw files can be used.


4.2. Lua scripts

Lua scripts can have the extension ".lua" or ".lob". ".lua" files contain
scripts as text, i.e. not precompiled. ".lob" files are "lua object" files,
that are lua files compiled with "luac".

Lua scripts are searched in the same way as other files, but first  the".lua"
one is searched, then the ".lob". That allows for changes to ".lua" files
while developing, but for releases the compiled ".lob" files inside ".uar"
files are distributed only.

For Lua to C interfaces or other informations about Lua with uwadv, look into
the file "README.Lua.txt".
