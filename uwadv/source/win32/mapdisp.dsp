# Microsoft Developer Studio Project File - Name="mapdisp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=mapdisp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mapdisp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mapdisp.mak" CFG="mapdisp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mapdisp - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "mapdisp - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mapdisp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../output/release/mapdisp"
# PROP Intermediate_Dir "../../output/release/mapdisp/intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".." /D "NDEBUG" /D "_STLP_USE_STATIC_LIB" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 sdl.lib sdlmain.lib opengl32.lib glu32.lib zlib.lib /nologo /subsystem:windows /machine:I386 /out:"../../output/release/mapdisp.exe"

!ELSEIF  "$(CFG)" == "mapdisp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../output/debug/mapdisp"
# PROP Intermediate_Dir "../../output/debug/mapdisp/intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 sdl.lib sdlmain.lib opengl32.lib glu32.lib zlib.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrt" /out:"../../output/debug/mapdisp.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "mapdisp - Win32 Release"
# Name "mapdisp - Win32 Debug"
# Begin Group "uwadv source files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\resource\cfgfile.cpp
# End Source File
# Begin Source File

SOURCE=..\image.cpp
# End Source File
# Begin Source File

SOURCE=..\resource\imageloader.cpp
# End Source File
# Begin Source File

SOURCE=..\level.cpp
# End Source File
# Begin Source File

SOURCE=..\resource\maploader.cpp
# End Source File
# Begin Source File

SOURCE=..\objects.cpp
# End Source File
# Begin Source File

SOURCE=..\resource\objloader.cpp
# End Source File
# Begin Source File

SOURCE=..\quadtree.cpp
# End Source File
# Begin Source File

SOURCE=..\renderer.cpp
# End Source File
# Begin Source File

SOURCE=..\resource\savegame.cpp
# End Source File
# Begin Source File

SOURCE=..\settings.cpp
# End Source File
# Begin Source File

SOURCE=..\resource\texloader.cpp
# End Source File
# Begin Source File

SOURCE=..\texture.cpp
# End Source File
# Begin Source File

SOURCE=..\utils.cpp
# End Source File
# End Group
# Begin Group "uwadv header files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common.hpp
# End Source File
# Begin Source File

SOURCE=..\resource\fread_endian.hpp
# End Source File
# Begin Source File

SOURCE=..\image.hpp
# End Source File
# Begin Source File

SOURCE=..\level.hpp
# End Source File
# Begin Source File

SOURCE=..\objects.hpp
# End Source File
# Begin Source File

SOURCE=..\quadtree.hpp
# End Source File
# Begin Source File

SOURCE=..\renderer.hpp
# End Source File
# Begin Source File

SOURCE=..\savegame.hpp
# End Source File
# Begin Source File

SOURCE=..\settings.hpp
# End Source File
# Begin Source File

SOURCE=..\texture.hpp
# End Source File
# Begin Source File

SOURCE=..\uamath.hpp
# End Source File
# Begin Source File

SOURCE=..\uatypes.hpp
# End Source File
# Begin Source File

SOURCE=..\utils.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\tools\mapdisp.cpp
# End Source File
# End Target
# End Project
