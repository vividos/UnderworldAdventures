# Microsoft Developer Studio Project File - Name="toolslib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=toolslib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "toolslib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "toolslib.mak" CFG="toolslib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "toolslib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "toolslib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "toolslib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../output/release/toolslib"
# PROP Intermediate_Dir "../output/release/toolslib/intermediate"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\source" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "toolslib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../output/debug/toolslib"
# PROP Intermediate_Dir "../output/debug/toolslib/intermediate"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\source" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "toolslib - Win32 Release"
# Name "toolslib - Win32 Debug"
# Begin Group "uwadv source files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\resource\cfgfile.cpp
# End Source File
# Begin Source File

SOURCE=..\source\resource\codeloader.cpp
# End Source File
# Begin Source File

SOURCE=..\source\conv\codevm.cpp
# End Source File
# Begin Source File

SOURCE=..\source\conv\convgraph.cpp
# End Source File
# Begin Source File

SOURCE=..\source\cutscene.cpp
# End Source File
# Begin Source File

SOURCE=..\source\resource\cutsloader.cpp
# End Source File
# Begin Source File

SOURCE=..\source\resource\gamestrings.cpp
# End Source File
# Begin Source File

SOURCE=..\source\image.cpp
# End Source File
# Begin Source File

SOURCE=..\source\resource\imageloader.cpp
# End Source File
# Begin Source File

SOURCE=..\source\imgquad.cpp
# End Source File
# Begin Source File

SOURCE=..\source\resource\model\importwrl.cpp
# End Source File
# Begin Source File

SOURCE=..\source\level.cpp
# End Source File
# Begin Source File

SOURCE=..\source\resource\maploader.cpp
# End Source File
# Begin Source File

SOURCE=..\source\resource\modeldecode.cpp
# End Source File
# Begin Source File

SOURCE=..\source\models.cpp
# End Source File
# Begin Source File

SOURCE=..\source\objects.cpp
# End Source File
# Begin Source File

SOURCE=..\source\resource\objloader.cpp
# End Source File
# Begin Source File

SOURCE=..\source\quadtree.cpp
# End Source File
# Begin Source File

SOURCE=..\source\renderer.cpp
# End Source File
# Begin Source File

SOURCE=..\source\resource\savegame.cpp
# End Source File
# Begin Source File

SOURCE=..\source\settings.cpp
# End Source File
# Begin Source File

SOURCE=..\source\resource\texloader.cpp
# End Source File
# Begin Source File

SOURCE=..\source\texture.cpp
# End Source File
# Begin Source File

SOURCE=..\source\resource\tgaimport.cpp
# End Source File
# Begin Source File

SOURCE=..\source\utils.cpp
# End Source File
# Begin Source File

SOURCE=..\source\resource\uw2dec.cpp
# End Source File
# Begin Source File

SOURCE=..\source\resource\model\wrllexer.cpp
# End Source File
# End Group
# Begin Group "uwadv header files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\cfgfile.hpp
# End Source File
# Begin Source File

SOURCE=..\source\conv\codevm.hpp
# End Source File
# Begin Source File

SOURCE=..\source\common.hpp
# End Source File
# Begin Source File

SOURCE=..\source\conv\convgraph.hpp
# End Source File
# Begin Source File

SOURCE=..\source\cutscene.hpp
# End Source File
# Begin Source File

SOURCE=..\source\resource\model\FlexLexer.h
# End Source File
# Begin Source File

SOURCE=..\source\resource\fread_endian.hpp
# End Source File
# Begin Source File

SOURCE=..\source\gamestrings.hpp
# End Source File
# Begin Source File

SOURCE=..\source\image.hpp
# End Source File
# Begin Source File

SOURCE=..\source\imgquad.hpp
# End Source File
# Begin Source File

SOURCE=..\source\level.hpp
# End Source File
# Begin Source File

SOURCE=..\source\models.hpp
# End Source File
# Begin Source File

SOURCE=..\source\objects.hpp
# End Source File
# Begin Source File

SOURCE=..\source\conv\opcodes.hpp
# End Source File
# Begin Source File

SOURCE=..\source\quadtree.hpp
# End Source File
# Begin Source File

SOURCE=..\source\renderer.hpp
# End Source File
# Begin Source File

SOURCE=..\source\savegame.hpp
# End Source File
# Begin Source File

SOURCE=..\source\settings.hpp
# End Source File
# Begin Source File

SOURCE=..\source\texture.hpp
# End Source File
# Begin Source File

SOURCE=..\source\uamath.hpp
# End Source File
# Begin Source File

SOURCE=..\source\uatypes.hpp
# End Source File
# Begin Source File

SOURCE=..\source\utils.hpp
# End Source File
# Begin Source File

SOURCE=..\source\resource\model\wrllexer.hpp
# End Source File
# End Group
# End Target
# End Project
