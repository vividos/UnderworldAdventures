# Microsoft Developer Studio Project File - Name="crit_ai" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=crit_ai - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "crit_ai.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "crit_ai.mak" CFG="crit_ai - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "crit_ai - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "crit_ai - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "crit_ai - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release\intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\source" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 sdl.lib sdlmain.lib opengl32.lib glu32.lib toolslib.lib zlib.lib uaopt.lib /nologo /subsystem:windows /machine:I386 /libpath:"..\..\output\release\toolslib" /libpath:"..\..\output\release\uaopt"

!ELSEIF  "$(CFG)" == "crit_ai - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\source" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 sdl.lib sdlmain.lib opengl32.lib glu32.lib toolslib.lib zlib.lib uaopt.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrt" /pdbtype:sept /libpath:"..\..\output\debug\toolslib" /libpath:"..\..\output\debug\uaopt"

!ENDIF 

# Begin Target

# Name "crit_ai - Win32 Release"
# Name "crit_ai - Win32 Debug"
# Begin Group "uwadv extra files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\source\win32\debug.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\files.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\gamecfg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\inventory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\mapnotes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\objects.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\physics.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\physicsobject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\player.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\resource\playerimport.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\resource\proploader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\underworld.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\debug\uwaccess.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\uwscript.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\crit_ai.cpp
# End Source File
# End Target
# End Project
