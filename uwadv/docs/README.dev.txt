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

   2. Basic uwadv stuff
   2.1. Game utility classes
   2.2. Os porter's guide
   2.3. Core interface
   2.4. Screens explained
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
that otherwise would be hard to gain, or to get a platform independent game.

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
   don't reindent code that wasn't changed
 - always document your code, use doxygen-style comments for functions
 - don't use "using namespace std;", use e.g. std::string instead
 - use C++ style casts when unsure
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


2. Basic uwadv stuff

This chapter explains some basic mechanics of the uwadv project. From here on
it is is useful if you know the original game's file formats, as described in
"uw-formats.txt". It is assumed that the reader has some knowledge of C++ and
object oriented programming.


2.1. Game utility classes

To support the game's inner workings, there are some classes that can be used
to do certain things.

* ua_exception

  todo

  implementation: source/uatypes.hpp

* ua_smart_ptr

  todo

  implementation: source/uatypes.hpp

* ua_vector2d

  todo

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

  todo

  implementation: source/texture.hpp and .cpp, source/resource/texloader.cpp

More utility classes that have to do with the game core can be found in
chapter "2.3. Game core interface".


2.2. Os porter's guide

todo


2.3. Core interface

todo


2.4. Screens explained

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
files from the whole uadata tree in one file. There can be more files


4.2. Lua scripts

Lua scripts can have the extension ".lua" or ".lob". ".lua" files contain
scripts as text, i.e. not precompiled. ".lob" files are "lua object" files,
that is lua files compiled with "luac".

Lua scripts are searched in the same way as other files, but first  the".lua"
one is searched, then the ".lob". That allows for changes to ".lua" files
while developing, but for releases the compiled ".lob" files inside ".uar"
files are distributed only.

For Lua to C interfaces or other informations about Lua with uwadv, look into
the file "README.Lua.txt".
