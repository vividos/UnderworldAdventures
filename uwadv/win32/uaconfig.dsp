# Microsoft Developer Studio Project File - Name="uaconfig" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=uaconfig - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "uaconfig.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "uaconfig.mak" CFG="uaconfig - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "uaconfig - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "uaconfig - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "uaconfig - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../output/release/uaconfig"
# PROP Intermediate_Dir "../output/release/uaconfig/intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\source" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib winmm.lib sdl.lib /nologo /subsystem:windows /machine:I386 /out:"../output/release/uaconfig.exe"

!ELSEIF  "$(CFG)" == "uaconfig - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../output/debug/uaconfig"
# PROP Intermediate_Dir "../output/debug/uaconfig/intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\source" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib winmm.lib sdl.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../output/debug/uaconfig.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "uaconfig - Win32 Release"
# Name "uaconfig - Win32 Debug"
# Begin Group "resource files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\source\win32\uaconfig\Corp.ico
# End Source File
# Begin Source File

SOURCE=..\source\win32\uaconfig\resource.h
# End Source File
# Begin Source File

SOURCE=..\source\win32\uaconfig\uaconfig_res.rc
# End Source File
# End Group
# Begin Group "uwadv files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\resource\cfgfile.cpp
# End Source File
# Begin Source File

SOURCE=..\source\cfgfile.hpp
# End Source File
# Begin Source File

SOURCE=..\source\settings.cpp
# End Source File
# Begin Source File

SOURCE=..\source\settings.hpp
# End Source File
# Begin Source File

SOURCE=..\source\utils.cpp
# End Source File
# Begin Source File

SOURCE=..\source\utils.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\source\win32\uaconfig\uaconfig.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=..\source\win32\uaconfig\uaconfig.hpp
# End Source File
# End Target
# End Project
