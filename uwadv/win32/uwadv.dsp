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
# PROP Output_Dir "../output/release"
# PROP Intermediate_Dir "../output/release/intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\source" /I "..\source\audio" /I "..\source\base" /I "..\source\import" /I "..\source\renderer" /I "..\source\ui" /I "..\source\underw" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "WIN32_EXTRA_LEAN" /D "VC_EXTRALEAN" /D "NOSERVICE" /D "NOMCX" /D "NOIME" /D "NOSOUND" /D "NOCOMM" /D "NOKANJI" /D "NORPC" /D "NOPROXYSTUB" /D "NOTAPE" /D "NOCRYPT" /D "NOIMAGE" /D "_STLP_USE_STATIC_LIB" /YX"common.hpp" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib sdl.lib sdlmain.lib opengl32.lib glu32.lib winmm.lib sdl_mixer.lib zlib.lib uaopt.lib /nologo /subsystem:windows /machine:I386 /libpath:"../output/release/uaopt"

!ELSEIF  "$(CFG)" == "uwadv - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../output/debug"
# PROP Intermediate_Dir "../output/debug/intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /Gm /GX /ZI /Od /I "..\source" /I "..\source\audio" /I "..\source\base" /I "..\source\import" /I "..\source\renderer" /I "..\source\ui" /I "..\source\underw" /D "_DEBUG" /D "HAVE_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "WIN32_EXTRA_LEAN" /D "VC_EXTRALEAN" /D "NOSERVICE" /D "NOMCX" /D "NOIME" /D "NOSOUND" /D "NOCOMM" /D "NOKANJI" /D "NORPC" /D "NOPROXYSTUB" /D "NOTAPE" /D "NOCRYPT" /D "NOIMAGE" /D "_STLP_DEBUG" /D "_STLP_USE_STATIC_LIB" /D "_RTLDLL" /D "_STLP_USE_NEWALLOC" /D "_STLP_DEBUG_ALLOC" /D "_STLP_USE_SYSTEM_ASSERT" /FR /YX"common.hpp" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib sdl.lib sdlmain.lib opengl32.lib glu32.lib winmm.lib sdl_mixer.lib zlib.lib uaopt.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"../output/debug/uaopt"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "uwadv - Win32 Release"
# Name "uwadv - Win32 Debug"
# Begin Group "audio files"

# PROP Default_Filter ""
# Begin Group "audio sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\audio\audio.cpp
# End Source File
# Begin Source File

SOURCE=..\source\audio\midi.cpp
# End Source File
# Begin Source File

SOURCE=..\source\audio\midi_driver\midi_sdl_mixer.cpp
# End Source File
# Begin Source File

SOURCE=..\source\audio\resamp.c
# End Source File
# Begin Source File

SOURCE=..\source\audio\vocresample.cpp
# End Source File
# Begin Source File

SOURCE=..\source\audio\midi_driver\win_midiout.cpp
# End Source File
# Begin Source File

SOURCE=..\source\audio\xmidi.cpp
# End Source File
# End Group
# Begin Group "audio headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\audio\audio.hpp
# End Source File
# Begin Source File

SOURCE=..\source\audio\midi.hpp
# End Source File
# Begin Source File

SOURCE=..\source\audio\midi_driver\midi_sdl_mixer.hpp
# End Source File
# Begin Source File

SOURCE=..\source\audio\resamp.h
# End Source File
# Begin Source File

SOURCE=..\source\audio\midi_driver\win_midiout.h
# End Source File
# Begin Source File

SOURCE=..\source\audio\xmidi.hpp
# End Source File
# End Group
# End Group
# Begin Group "base files"

# PROP Default_Filter ""
# Begin Group "base sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\base\cfgfile.cpp
# End Source File
# Begin Source File

SOURCE=..\source\base\files.cpp
# End Source File
# Begin Source File

SOURCE=..\source\base\keymap.cpp
# End Source File
# Begin Source File

SOURCE=..\source\base\savegame.cpp
# End Source File
# Begin Source File

SOURCE=..\source\base\settings.cpp
# End Source File
# Begin Source File

SOURCE=..\source\base\utils.cpp
# End Source File
# End Group
# Begin Group "base headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\base\cfgfile.hpp
# End Source File
# Begin Source File

SOURCE=..\source\base\files.hpp
# End Source File
# Begin Source File

SOURCE=..\source\base\keymap.hpp
# End Source File
# Begin Source File

SOURCE=..\source\base\savegame.hpp
# End Source File
# Begin Source File

SOURCE=..\source\base\settings.hpp
# End Source File
# Begin Source File

SOURCE=..\source\base\uamath.hpp
# End Source File
# Begin Source File

SOURCE=..\source\base\utils.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\source\common.hpp
# End Source File
# End Group
# Begin Group "ui files"

# PROP Default_Filter ""
# Begin Group "ui sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\ui\cutscene.cpp
# End Source File
# Begin Source File

SOURCE=..\source\ui\font.cpp
# End Source File
# Begin Source File

SOURCE=..\source\ui\image.cpp
# End Source File
# Begin Source File

SOURCE=..\source\ui\imgquad.cpp
# End Source File
# Begin Source File

SOURCE=..\source\ui\mousecursor.cpp
# End Source File
# Begin Source File

SOURCE=..\source\ui\screen.cpp
# End Source File
# Begin Source File

SOURCE=..\source\ui\textscroll.cpp
# End Source File
# Begin Source File

SOURCE=..\source\ui\window.cpp
# End Source File
# End Group
# Begin Group "ui headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\ui\cutscene.hpp
# End Source File
# Begin Source File

SOURCE=..\source\ui\fading.hpp
# End Source File
# Begin Source File

SOURCE=..\source\ui\font.hpp
# End Source File
# Begin Source File

SOURCE=..\source\ui\image.hpp
# End Source File
# Begin Source File

SOURCE=..\source\ui\imgquad.hpp
# End Source File
# Begin Source File

SOURCE=..\source\ui\mousecursor.hpp
# End Source File
# Begin Source File

SOURCE=..\source\ui\screen.hpp
# End Source File
# Begin Source File

SOURCE=..\source\ui\textscroll.hpp
# End Source File
# Begin Source File

SOURCE=..\source\ui\window.hpp
# End Source File
# End Group
# End Group
# Begin Group "main game files"

# PROP Default_Filter ""
# Begin Group "main game sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\win32\debug.cpp
# End Source File
# Begin Source File

SOURCE=..\source\win32\game_win32.cpp
# End Source File
# Begin Source File

SOURCE=..\source\gamecfg.cpp
# End Source File
# Begin Source File

SOURCE=..\source\main.cpp
# End Source File
# Begin Source File

SOURCE=..\source\uwadv.cpp
# End Source File
# End Group
# Begin Group "main game headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\debug.hpp
# End Source File
# Begin Source File

SOURCE=..\source\game_interface.hpp
# End Source File
# Begin Source File

SOURCE=..\source\win32\game_win32.hpp
# End Source File
# Begin Source File

SOURCE=..\source\gamecfg.hpp
# End Source File
# Begin Source File

SOURCE=..\source\uwadv.hpp
# End Source File
# End Group
# Begin Group "win32 resource files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\win32\resource.h
# End Source File
# Begin Source File

SOURCE=..\source\win32\Uus.ico
# End Source File
# Begin Source File

SOURCE=..\source\win32\uwadv_res.rc
# End Source File
# End Group
# End Group
# Begin Group "renderer files"

# PROP Default_Filter ""
# Begin Group "renderer sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\renderer\critter.cpp
# End Source File
# Begin Source File

SOURCE=..\source\renderer\models.cpp
# End Source File
# Begin Source File

SOURCE=..\source\renderer\quadtree.cpp
# End Source File
# Begin Source File

SOURCE=..\source\renderer\renderer.cpp
# End Source File
# Begin Source File

SOURCE=..\source\renderer\renderer_impl.cpp
# End Source File
# Begin Source File

SOURCE=..\source\renderer\tessellator.cpp
# End Source File
# Begin Source File

SOURCE=..\source\renderer\texture.cpp
# End Source File
# End Group
# Begin Group "renderer headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\renderer\critter.hpp
# End Source File
# Begin Source File

SOURCE=..\source\renderer\models.hpp
# End Source File
# Begin Source File

SOURCE=..\source\renderer\quadtree.hpp
# End Source File
# Begin Source File

SOURCE=..\source\renderer\renderer.hpp
# End Source File
# Begin Source File

SOURCE=..\source\renderer\renderer_impl.hpp
# End Source File
# Begin Source File

SOURCE=..\source\renderer\tessellator.hpp
# End Source File
# Begin Source File

SOURCE=..\source\renderer\texture.hpp
# End Source File
# End Group
# End Group
# Begin Group "screen files"

# PROP Default_Filter ""
# Begin Group "screen sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\screens\acknowledgements.cpp
# End Source File
# Begin Source File

SOURCE=..\source\screens\create_character.cpp
# End Source File
# Begin Source File

SOURCE=..\source\screens\cutscene_view.cpp
# End Source File
# Begin Source File

SOURCE=..\source\screens\ingame_ctrls.cpp
# End Source File
# Begin Source File

SOURCE=..\source\screens\ingame_new.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\source\screens\ingame_orig.cpp
# End Source File
# Begin Source File

SOURCE=..\source\screens\save_game.cpp
# End Source File
# Begin Source File

SOURCE=..\source\screens\start_menu.cpp
# End Source File
# Begin Source File

SOURCE=..\source\screens\start_splash.cpp
# End Source File
# Begin Source File

SOURCE=..\source\screens\uwadv_menu.cpp
# End Source File
# End Group
# Begin Group "screen headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\screens\acknowledgements.hpp
# End Source File
# Begin Source File

SOURCE=..\source\screens\create_character.hpp
# End Source File
# Begin Source File

SOURCE=..\source\screens\cutscene_view.hpp
# End Source File
# Begin Source File

SOURCE=..\source\screens\ingame_ctrls.hpp
# End Source File
# Begin Source File

SOURCE=..\source\screens\ingame_new.hpp
# End Source File
# Begin Source File

SOURCE=..\source\screens\ingame_orig.hpp
# End Source File
# Begin Source File

SOURCE=..\source\screens\save_game.hpp
# End Source File
# Begin Source File

SOURCE=..\source\screens\start_menu.hpp
# End Source File
# Begin Source File

SOURCE=..\source\screens\start_splash.hpp
# End Source File
# Begin Source File

SOURCE=..\source\screens\uwadv_menu.hpp
# End Source File
# End Group
# End Group
# Begin Group "underworld files"

# PROP Default_Filter ""
# Begin Group "underworld sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\underw\gamestrings.cpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\geometry.cpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\inventory.cpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\level.cpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\mapnotes.cpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\objects.cpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\pathfinder.cpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\physics.cpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\physicsobject.cpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\player.cpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\underworld.cpp
# End Source File
# End Group
# Begin Group "underworld headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\underw\gamestrings.hpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\geometry.hpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\inventory.hpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\level.hpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\mapnotes.hpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\objects.hpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\pathfinder.hpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\physics.hpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\physicsobject.hpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\player.hpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\properties.hpp
# End Source File
# Begin Source File

SOURCE=..\source\underw\underworld.hpp
# End Source File
# End Group
# End Group
# Begin Group "import files"

# PROP Default_Filter ""
# Begin Group "import sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\import\critloader.cpp
# End Source File
# Begin Source File

SOURCE=..\source\import\cutsloader.cpp
# End Source File
# Begin Source File

SOURCE=..\source\import\fontloader.cpp
# End Source File
# Begin Source File

SOURCE=..\source\import\imageloader.cpp
# End Source File
# Begin Source File

SOURCE=..\source\import\maploader.cpp
# End Source File
# Begin Source File

SOURCE=..\source\import\objloader.cpp
# End Source File
# Begin Source File

SOURCE=..\source\import\playerimport.cpp
# End Source File
# Begin Source File

SOURCE=..\source\import\proploader.cpp
# End Source File
# Begin Source File

SOURCE=..\source\import\stringsloader.cpp
# End Source File
# Begin Source File

SOURCE=..\source\import\texloader.cpp
# End Source File
# Begin Source File

SOURCE=..\source\import\tgaimport.cpp
# End Source File
# Begin Source File

SOURCE=..\source\import\uw2dec.cpp
# End Source File
# End Group
# Begin Group "import headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\import\import.hpp
# End Source File
# Begin Source File

SOURCE=..\source\import\io_endian.hpp
# End Source File
# End Group
# Begin Group "old import stuff"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\resource\codeloader.cpp
# End Source File
# Begin Source File

SOURCE=..\source\resource\modeldecode.cpp
# End Source File
# End Group
# End Group
# Begin Group "conv files"

# PROP Default_Filter ""
# Begin Group "conv sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\conv\codevm.cpp
# End Source File
# Begin Source File

SOURCE=..\source\conv\convgraph.cpp
# End Source File
# End Group
# Begin Group "conv headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\conv\convgraph.hpp
# End Source File
# Begin Source File

SOURCE=..\source\conv\opcodes.hpp
# End Source File
# End Group
# End Group
# Begin Group "scripting files"

# PROP Default_Filter ""
# Begin Group "script sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\script\luascript.cpp
# End Source File
# End Group
# Begin Group "script headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\script\luascript.hpp
# End Source File
# Begin Source File

SOURCE=..\source\script\script.hpp
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE="..\docs\uw-formats.txt"
# End Source File
# End Target
# End Project
