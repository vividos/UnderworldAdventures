Microsoft Visual C++ 2005 Express Setup Readme
----------------------------------------------

This file guides you to compile Underworld Adventures using the freely available
Visual C++ 2005 Express Edition from Microsoft (vc8express for short).

Content
-------

1. Install the prerequisites
2. Compile the program




Install the prerequisites
-------------------------

The following packages and libraries are needed to compile Underworld Adventures with
vc8express and build an installer for Windows:

SDL 1.2.9 (or any later version)
SDL_mixer 1.2.6 (or any later version)
zlib 1.2.3 (or any later version)
STLport 5.0.1 (optional)


Install SDL
-----------

SDL comes in precompiled form with headers used for compiling. Go to

   http://www.libsdl.org/

and go to the "SDL 1.2" section. Download a package named like this:

   SDL-devel-1.x.x-VC6.zip

It will support vc8express, too. Extract the files to your favourite folder
(e.g. where your third party packages all reside). The folder "SDL-1.x.x" should have
some subfolders, namely "include", "lib" and "docs" (which you can safely remove).
Go into vc8express and open the "Options" dialog:

   Tools | Options | Projects and Solutions | VC++ Directories

Select the "Include files" in the directories combobox and add the path of SDL "include"
subfolder pathname to the end of the list. Do the same for the "Library files" and add
the SDL "lib" subfolder name to the end of the list.

Install SDL-Mixer
-----------------

SDL-Mixer also comes in precompiled form, although from this URL:

   http://www.libsdl.org/projects/SDL_mixer/

Just with the SDL setup, grab a file named

   SDL_mixer-devel-1.x.x-VC6.zip

and extract it to your favourite folder. Again there are an "include" and "lib" folder.
Just as with SDL, add the "include" and "lib" folder to the project options as
include and library directories.

Note:
For playing back Ogg Vorbis files you need another DLL. Fortunately you don't have to
compile anything. Just go to

   http://www.rarewares.org/ogg.html

and grab a package called "ogg vorbis dlls using libVorbis v1.x.x". The package should
contain a file called "libvorbis.dll" that is needed later when setting up Underworld
Adventures for running. You also may need another dll called "libmmd.dll" (a math
functions library).


Build and install zlib
----------------------

Unfortunately zlib isn't available as precompiled package, but it's rather easy to
create the necessary library files. Go to

   http://www.gzip.org/zlib/

and download the latest zlib source code. You can take the "zipfile format".
Extract all files to your favourite folder again and go into the subfolder

   contrib\vstudio\vc8

There are some project and solution files. Open the "zlibvc.sln" file with vc8express.
On the "Standard" toolbar ensure that the combobox items "Release" and "Win32" are
selected.

Since Underworld Adventures is built with the "Multi-threaded DLL" runtime, zlib also
has to be built with that option, or else linker errors occur. Go th the project settings
for the "zlibstat" project, under "C/C++", "Code Generation" and make sure the "Runtime
Library" option uses the /MD (or /MDd for debug builds) switch.

In the Solution Explorer right-click on "zlibstat" and click on
"Build". The following file is built:

   contrib\vstudio\vc8\x86\ZlibStatRelease\zlibstat.lib

This file is later used for linking.

Note: When you already set up your vc8 express for using STLport, you have to add the
define _STLP_NO_IOSTREAMS to the project settings, under "C/C++", "Preprocessor", to
prevent that the static lib introduces STLport as a dependency. Normally zlib doesn't
use STLport, but for some unknown reason (a bug maybe?) the autolink feature of
STLport pulls in its lib dependency.

For convenience, create a "zlib-1.x.x" folder with two subfolders "include" and "lib"
in your favourite packages folder. Put the files "zlib.h" and "zconf.h" in the "include"
folder, and the "zlibstat.lib" file into the "lib" folder. Throw away the zlib build
folder, it isn't needed anymore.

For completeness add the "include" and "lib" folders as previously described in the
SDL setup.






