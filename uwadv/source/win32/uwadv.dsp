# Microsoft Developer Studio Project File - Name="uwadv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=uwadv - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "uwadv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "uwadv.mak" CFG="uwadv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "uwadv - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "uwadv - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "uwadv - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../output/release"
# PROP Intermediate_Dir "../../output/release/intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "WIN32_EXTRA_LEAN" /D "VC_EXTRALEAN" /D "NOSERVICE" /D "NOMCX" /D "NOIME" /D "NOSOUND" /D "NOCOMM" /D "NOKANJI" /D "NORPC" /D "NOPROXYSTUB" /D "NOTAPE" /D "NOCRYPT" /D "NOIMAGE" /D "_STLP_USE_STATIC_LIB" /YX"common.hpp" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib sdl.lib sdlmain.lib opengl32.lib glu32.lib winmm.lib sdl_mixer.lib zlib.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "uwadv - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../output/debug"
# PROP Intermediate_Dir "../../output/debug/intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /Gm /GX /ZI /Od /I ".." /D "_DEBUG" /D "HAVE_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "WIN32_EXTRA_LEAN" /D "VC_EXTRALEAN" /D "NOSERVICE" /D "NOMCX" /D "NOIME" /D "NOSOUND" /D "NOCOMM" /D "NOKANJI" /D "NORPC" /D "NOPROXYSTUB" /D "NOTAPE" /D "NOCRYPT" /D "NOIMAGE" /D "_STLP_DEBUG" /D "_STLP_USE_STATIC_LIB" /D "_RTLDLL" /D "_STLP_USE_NEWALLOC" /D "_STLP_DEBUG_ALLOC" /D "_STLP_USE_SYSTEM_ASSERT" /YX"common.hpp" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib sdl.lib sdlmain.lib opengl32.lib glu32.lib winmm.lib sdl_mixer.lib zlib.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "uwadv - Win32 Release"
# Name "uwadv - Win32 Debug"
# Begin Group "main game files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\common.hpp
# End Source File
# Begin Source File

SOURCE=..\uamath.hpp
# End Source File
# Begin Source File

SOURCE=..\uatypes.hpp
# End Source File
# Begin Source File

SOURCE=..\uwadv.cpp
# End Source File
# Begin Source File

SOURCE=..\uwadv.hpp
# End Source File
# End Group
# Begin Group "resource files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\source\Uus.ico
# End Source File
# End Group
# Begin Group "lua scripting files"

# PROP Default_Filter ""
# Begin Group "lua scripts"

# PROP Default_Filter "*.lua"
# Begin Source File

SOURCE=..\..\uadata\uw1\scripts\cutscene.lua
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Group
# Begin Group "win32 specific files"

# PROP Default_Filter ""
# Begin Group "win32 sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\source\game_win32.cpp
# End Source File
# Begin Source File

SOURCE=.\source\main.cpp
# End Source File
# Begin Source File

SOURCE=.\source\uwadv.rc
# End Source File
# End Group
# Begin Group "win32 headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\source\game_win32.hpp
# End Source File
# Begin Source File

SOURCE=.\source\resource.h
# End Source File
# End Group
# End Group
# Begin Group "audio specific files"

# PROP Default_Filter ""
# Begin Group "audio sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\audio\audio.cpp
# End Source File
# Begin Source File

SOURCE=..\audio\midi_driver\linux_sdl_mixer.cpp
# End Source File
# Begin Source File

SOURCE=..\audio\midi.cpp
# End Source File
# Begin Source File

SOURCE=..\audio\midi_driver\uni_fmod.cpp
# End Source File
# Begin Source File

SOURCE=..\audio\midi_driver\win_midiout.cpp
# End Source File
# Begin Source File

SOURCE=..\audio\xmidi.cpp
# End Source File
# End Group
# Begin Group "audio headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\audio\midi_driver\linux_sdl_mixer.h
# End Source File
# Begin Source File

SOURCE=..\audio\midi.hpp
# End Source File
# Begin Source File

SOURCE=..\audio\midi_driver\uni_fmod.h
# End Source File
# Begin Source File

SOURCE=..\audio\midi_driver\win_midiout.h
# End Source File
# Begin Source File

SOURCE=..\audio\xmidi.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\audio.hpp
# End Source File
# End Group
# Begin Group "library files"

# PROP Default_Filter ""
# Begin Group "zziplib sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\resource\zziplib\SDL_rwops_zzip.c
# ADD CPP /I "..\resource\zziplib"
# End Source File
# Begin Source File

SOURCE="..\resource\zziplib\zzip-dir.c"
# ADD CPP /I "..\resource\zziplib"
# End Source File
# Begin Source File

SOURCE="..\resource\zziplib\zzip-err.c"
# ADD CPP /I "..\resource\zziplib"
# End Source File
# Begin Source File

SOURCE="..\resource\zziplib\zzip-file.c"
# ADD CPP /I "..\resource\zziplib"
# End Source File
# Begin Source File

SOURCE="..\resource\zziplib\zzip-info.c"
# ADD CPP /I "..\resource\zziplib"
# End Source File
# Begin Source File

SOURCE="..\resource\zziplib\zzip-io.c"
# ADD CPP /I "..\resource\zziplib"
# End Source File
# Begin Source File

SOURCE="..\resource\zziplib\zzip-stat.c"
# ADD CPP /I "..\resource\zziplib"
# End Source File
# Begin Source File

SOURCE="..\resource\zziplib\zzip-zip.c"
# ADD CPP /I "..\resource\zziplib"
# End Source File
# End Group
# Begin Group "zziplib headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\resource\zziplib\SDL_rwops_zzip.h
# End Source File
# Begin Source File

SOURCE="..\resource\zziplib\zzip-conf.h"
# End Source File
# Begin Source File

SOURCE="..\resource\zziplib\zzip-file.h"
# End Source File
# Begin Source File

SOURCE="..\resource\zziplib\zzip-io.h"
# End Source File
# Begin Source File

SOURCE="..\resource\zziplib\zzip-msvc.h"
# End Source File
# Begin Source File

SOURCE="..\resource\zziplib\zzip-stdint.h"
# End Source File
# Begin Source File

SOURCE=..\resource\zziplib\zzip.h
# End Source File
# Begin Source File

SOURCE=..\resource\zziplib\zzipformat.h
# End Source File
# Begin Source File

SOURCE=..\resource\zziplib\zziplib.h
# End Source File
# End Group
# Begin Group "lua headers"

# PROP Default_Filter ""
# Begin Group "lua internal headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lua\src\lapi.h
# End Source File
# Begin Source File

SOURCE=..\lua\src\lcode.h
# End Source File
# Begin Source File

SOURCE=..\lua\src\ldebug.h
# End Source File
# Begin Source File

SOURCE=..\lua\src\ldo.h
# End Source File
# Begin Source File

SOURCE=..\lua\src\lfunc.h
# End Source File
# Begin Source File

SOURCE=..\lua\src\lgc.h
# End Source File
# Begin Source File

SOURCE=..\lua\src\llex.h
# End Source File
# Begin Source File

SOURCE=..\lua\src\llimits.h
# End Source File
# Begin Source File

SOURCE=..\lua\src\lmem.h
# End Source File
# Begin Source File

SOURCE=..\lua\src\lobject.h
# End Source File
# Begin Source File

SOURCE=..\lua\src\lopcodes.h
# End Source File
# Begin Source File

SOURCE=..\lua\src\lparser.h
# End Source File
# Begin Source File

SOURCE=..\lua\src\lstate.h
# End Source File
# Begin Source File

SOURCE=..\lua\src\lstring.h
# End Source File
# Begin Source File

SOURCE=..\lua\src\ltable.h
# End Source File
# Begin Source File

SOURCE=..\lua\src\ltm.h
# End Source File
# Begin Source File

SOURCE=..\lua\src\lundump.h
# End Source File
# Begin Source File

SOURCE=..\lua\src\lvm.h
# End Source File
# Begin Source File

SOURCE=..\lua\src\lzio.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\lua\include\lauxlib.h
# End Source File
# Begin Source File

SOURCE=..\lua\include\lua.h
# End Source File
# Begin Source File

SOURCE=..\lua\include\luadebug.h
# End Source File
# Begin Source File

SOURCE=..\lua\include\lualib.h
# End Source File
# End Group
# Begin Group "lua sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lua\src\lapi.c
# ADD CPP /I "..\lua\include"
# End Source File
# Begin Source File

SOURCE=..\lua\src\lcode.c
# ADD CPP /I "..\lua\include"
# End Source File
# Begin Source File

SOURCE=..\lua\src\ldebug.c
# ADD CPP /I "..\lua\include"
# End Source File
# Begin Source File

SOURCE=..\lua\src\ldo.c
# ADD CPP /I "..\lua\include"
# End Source File
# Begin Source File

SOURCE=..\lua\src\lfunc.c
# ADD CPP /I "..\lua\include"
# End Source File
# Begin Source File

SOURCE=..\lua\src\lgc.c
# ADD CPP /I "..\lua\include"
# End Source File
# Begin Source File

SOURCE=..\lua\src\llex.c
# ADD CPP /I "..\lua\include"
# End Source File
# Begin Source File

SOURCE=..\lua\src\lmem.c
# ADD CPP /I "..\lua\include"
# End Source File
# Begin Source File

SOURCE=..\lua\src\lobject.c
# ADD CPP /I "..\lua\include"
# End Source File
# Begin Source File

SOURCE=..\lua\src\lparser.c
# ADD CPP /I "..\lua\include"
# End Source File
# Begin Source File

SOURCE=..\lua\src\lstate.c
# ADD CPP /I "..\lua\include"
# End Source File
# Begin Source File

SOURCE=..\lua\src\lstring.c
# ADD CPP /I "..\lua\include"
# End Source File
# Begin Source File

SOURCE=..\lua\src\ltable.c
# ADD CPP /I "..\lua\include"
# End Source File
# Begin Source File

SOURCE=..\lua\src\ltests.c
# ADD CPP /I "..\lua\include"
# End Source File
# Begin Source File

SOURCE=..\lua\src\ltm.c
# ADD CPP /I "..\lua\include"
# End Source File
# Begin Source File

SOURCE=..\lua\src\lundump.c
# ADD CPP /I "..\lua\include"
# End Source File
# Begin Source File

SOURCE=..\lua\src\lvm.c
# ADD CPP /I "..\lua\include"
# End Source File
# Begin Source File

SOURCE=..\lua\src\lzio.c
# ADD CPP /I "..\lua\include"
# End Source File
# End Group
# End Group
# Begin Group "screen specific files"

# PROP Default_Filter ""
# Begin Group "screens sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\screens\acknowledgements.cpp
# End Source File
# Begin Source File

SOURCE=..\screens\conversation.cpp
# End Source File
# Begin Source File

SOURCE=..\screens\create_character.cpp
# End Source File
# Begin Source File

SOURCE=..\screens\cutscene_view.cpp
# End Source File
# Begin Source File

SOURCE=..\screens\ingame_orig.cpp
# End Source File
# Begin Source File

SOURCE=..\screens\map_view.cpp
# End Source File
# Begin Source File

SOURCE=..\screens\start_menu.cpp
# End Source File
# Begin Source File

SOURCE=..\screens\start_splash.cpp
# End Source File
# End Group
# Begin Group "screens headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\screens\acknowledgements.hpp
# End Source File
# Begin Source File

SOURCE=..\screens\conversation.hpp
# End Source File
# Begin Source File

SOURCE=..\screens\create_character.hpp
# End Source File
# Begin Source File

SOURCE=..\screens\cutscene_view.hpp
# End Source File
# Begin Source File

SOURCE=..\screens\ingame_orig.hpp
# End Source File
# Begin Source File

SOURCE=..\screens\map_view.hpp
# End Source File
# Begin Source File

SOURCE=..\screens\start_menu.hpp
# End Source File
# Begin Source File

SOURCE=..\screens\start_splash.hpp
# End Source File
# End Group
# End Group
# Begin Group "underworld files"

# PROP Default_Filter ""
# Begin Group "underworld source files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\inventory.cpp
# End Source File
# Begin Source File

SOURCE=..\level.cpp
# End Source File
# Begin Source File

SOURCE=..\objects.cpp
# End Source File
# Begin Source File

SOURCE=..\physics.cpp
# End Source File
# Begin Source File

SOURCE=..\player.cpp
# End Source File
# Begin Source File

SOURCE=..\underworld.cpp
# End Source File
# End Group
# Begin Group "underworld header files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\inventory.hpp
# End Source File
# Begin Source File

SOURCE=..\level.hpp
# End Source File
# Begin Source File

SOURCE=..\objects.hpp
# End Source File
# Begin Source File

SOURCE=..\physics.hpp
# End Source File
# Begin Source File

SOURCE=..\player.hpp
# End Source File
# Begin Source File

SOURCE=..\underworld.hpp
# End Source File
# End Group
# End Group
# Begin Group "conv code files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\conv\codevm.cpp
# End Source File
# Begin Source File

SOURCE=..\conv\codevm.hpp
# End Source File
# End Group
# Begin Group "game support files"

# PROP Default_Filter ""
# Begin Group "game support sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\cutscene.cpp
# End Source File
# Begin Source File

SOURCE=..\files.cpp
# End Source File
# Begin Source File

SOURCE=..\font.cpp
# End Source File
# Begin Source File

SOURCE=..\image.cpp
# End Source File
# Begin Source File

SOURCE=..\keymap.cpp
# End Source File
# Begin Source File

SOURCE=..\quadtree.cpp
# End Source File
# Begin Source File

SOURCE=..\texture.cpp
# End Source File
# End Group
# Begin Group "game support headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\core.hpp
# End Source File
# Begin Source File

SOURCE=..\cutscene.hpp
# End Source File
# Begin Source File

SOURCE=..\files.hpp
# End Source File
# Begin Source File

SOURCE=..\font.hpp
# End Source File
# Begin Source File

SOURCE=..\gamestrings.hpp
# End Source File
# Begin Source File

SOURCE=..\image.hpp
# End Source File
# Begin Source File

SOURCE=..\keymap.hpp
# End Source File
# Begin Source File

SOURCE=..\quadtree.hpp
# End Source File
# Begin Source File

SOURCE=..\screen.hpp
# End Source File
# Begin Source File

SOURCE=..\settings.hpp
# End Source File
# Begin Source File

SOURCE=..\texture.hpp
# End Source File
# End Group
# End Group
# Begin Group "resource loading sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\resource\codeloader.cpp
# End Source File
# Begin Source File

SOURCE=..\resource\cutsloader.cpp
# End Source File
# Begin Source File

SOURCE=..\resource\fontloader.cpp
# End Source File
# Begin Source File

SOURCE=..\resource\fread_endian.hpp
# End Source File
# Begin Source File

SOURCE=..\resource\gamestrings.cpp
# End Source File
# Begin Source File

SOURCE=..\resource\imageloader.cpp
# End Source File
# Begin Source File

SOURCE=..\resource\maploader.cpp
# End Source File
# Begin Source File

SOURCE=..\resource\objloader.cpp
# End Source File
# Begin Source File

SOURCE=..\resource\settings.cpp
# End Source File
# Begin Source File

SOURCE=..\resource\texloader.cpp
# End Source File
# End Group
# Begin Source File

SOURCE="..\..\docs\uw-formats.txt"
# End Source File
# End Target
# End Project
