Underworld Adventures Developer's Readme
----------------------------------------

This is the uwadv developer's readme file. It explains all stuff that is the
uwadv program, and all associated stuff, as well as more stuff.

Note: this document is slightly out of date!


Table of Contents
-----------------

   1 Introduction
   1.1 Used libraries
   1.2 Code guidelines
   1.3 CVS usage
   1.4 Release procedure
   1.5 Source code documentation
   1.6 "uadata" folder


1 Introduction

   This document is an introduction for developers who want to start
   coding for the Underworld Adventures project or just want to understand the
   source code. The document describes all that a developer has to know. For
   further reference the source code documentation is mentioned.

   Underworld Adventures is done entirely in C++, with a few addon libraries.
   Lua was chosen as the script language to use. For more on Lua interfaces to
   the game, please look into "README.Lua.txt".


1.1 Used libraries

   Some libraries are used throughout the project, to gain some functionality
   that otherwise would be hard to do, or to get a platform independent game.

   * OpenGL 1.1 (or later), http://www.opengl.org/
     3D graphics rendering library

   * SDL 1.2.3 (or later), http://www.libsdl.org/
     a cross-platform support library

   * SDL_Mixer 1.2.4 (or greater), http://www.libsdl.org/projects/SDL_mixer/
     cross-platform audio mixer and playback library

   * Lua 4.0.1, http://www.lua.org/
     scripting language

   * ZZipLib 0.10.65, http://zziplib.sourceforge.net/
     zip file reading library


1.2 Code guidelines

   Here are some code guidelines for the uwadv project that every developer
   has to obey when commiting code via CVS:

   - tabs should be converted to spaces, indendation size is 3 spaces; don't
     reindent code that wasn't changed by you
   - always document your code, use doxygen-style comments for functions
   - don't use "using namespace std;", use e.g. std::string instead
   - use C++ style casts when unsure
   - use std::ostringstream instead of sprintf()
   - use "T* var" and "T& var" for references and pointers (not "T *var" or
     "T &var")
   - throw exceptions on serious errors (e.g. when you absolutely can't
     continue)
   - use SDL functionality where possible, to stay platform independent
   - use SDL data types, such as Uint32, Uint16 when needed
   - don't accumulate all your changes in one cvs commit, separate commits
     that doesn't have something in common


1.3 CVS use

   The source code and related files for Underworld Adventures is stored in
   a CVS repository on SourceForge. You can anonymously access the repository
   via:

      cvs -d:pserver:anonymous@cvs.sourceforge.net:/cvsroot/uwadv login
      password:<return>

      cvs -z9 -d:pserver:anonymous@cvs.sourceforge.net:/cvsroot/uwadv co uwadv

   Developer access (requires SF account and project membership):

      cvs -z9 -d:ext:<username>@cvs.sourceforge.net:/cvsroot/uwadv co uwadv

   Once the project files are checked out and a CVS subfolder does exist, the -d
   option can be omitted. That also means that developer commits for anon-cvs
   checkouts are not possible (you have to copy your changed files over to a
   developer working copy).

   There are some CVS GUIs for Windows:

   * TortoiseCVS: http://www.tortoisecvs.org/
     It works as shell extension for the Windows Explorer and has almost all
     important cvs commands developers need. Unix-to-DOS line ending
     conversion is done automatically.

   * WinCVS: http://www.wincvs.org/
     Explorer-like browser interface for CVS, but a bit cumbersome to set up.
     It has some specialities like version tree graph. Be sure to have
     unchecked the option "Checkout text files with the Unix LF (0xa)" when
     checking out files.

   Occasionally the ChangeLog in the main project dir is updated from the cvs
   log messages. This is done using the perl script "cvs2cl.pl" available
   here:

      http://www.red-bean.com/cvs2cl/

   The complete command to update the ChangeLog file for uwadv is:

      cvs2cl.pl -r -S --no-wrap --gmt


1.4 Release procedure

   Here are the steps for a successful Underworld Adventures release:

   1. Think about the next funny release name

   2. Be sure that the project (uwadv, tools) compiles AND works on all
      systems where build files exist. Be sure to test against uw1 and
      uw_demo. Build systems to test for are: mingw32, mingw32 configure.ac,
      MSVC, Linux configure.ac, Linux RPM

   3. Adjust version numbers in configure.ac and win32/uwadv.nsi

   4. Create latest ChangeLog and commit it

   5. Tag repository using "cvs rtag", using the release name as tag name

   6. Do a "make dist" with a fresh checkout, using the tag name

   7. Upload source package to SourceForge (ftp://upload.sourceforge.net/)

   8. Let the platform porters compile binary versions for distribution

   9. Update Underworld Adventures home page with latest news

   10. Send mails to news sites


1.5 Source code documentation

   The Underworld Adventures source code is heavily commented using
   doxygen-style comments for modules, files, classes, class members, structs,
   enums and typedefs. The source code documentation generated via doxygen can
   be used as reference to understand the source code.

   The source code documentation can be generated with the following commands
   (change into the "docs" folder before):

      doxygen ua-doxygen.cfg

   The html-documentation is created in the folder "html", where "index.html"
   is the entry point to the source code documentation. The file
   "doxygen-warnings.txt" contains the program's warnings.

   doxygen is available from this site:

      http://www.doxygen.org/

   You can install the "dot" tool to generate fancy include, dependency and
   collaboration graphs. The "dot" tool is available at:

      http://www.graphviz.com/

   Be sure to download the "dot" tool and the associated "third-party"
   libraries. Extract the dot.exe and the libraries into the doxygen "bin"
   folder.


1.6 "uadata" folder

   The "uadata" folder contains files that didn't came with the original game
   files of Ultima Underworld, but are needed for the game to run. They can
   contain enhancements to graphics, new audio files, game scripts or new 3D
   models.

   The "Underworld Adventures Resource" file (extension .uar) package the
   contents of the "uadata" tree into one or more files. Name scheme looks
   like this: "uadataXX.uar" (printf syntax: "uadata%02u.uar"). uar files
   essentially are zip files with other extension.

   When the game searches for a "uadata" file, e.g. "uw1/sound/test.wav", it
   first searches the true folder, "uadata/uw1/sound/test.wav". If that
   doesn't exist, all "uadata??.uar" files are tested if the file is in one of
   it. When not found, an exception is thrown.

   The Makefile in the "uadata" folder creates packaged files that have all
   the files from the whole uadata tree in one file. Just do a

      make all

   to build all packaged files that are needed for distribution. For
   developing the raw files can be used.

   Lua scripts can have the extension ".lua" or ".lob". ".lua" files contain
   scripts as text, i.e. not precompiled. ".lob" files are "lua object" files,
   that are lua files compiled with "luac".

   Lua scripts are searched in the same way as other files, but first the
   ".lua" one is searched, then the ".lob". That allows for changes to ".lua"
   files while developing, but for releases the compiled ".lob" files inside
   ".uar" files are distributed only.

   For Lua to C interfaces or other information about Lua with uwadv, look
   into the file "README.Lua.txt".
