# Microsoft Developer Studio Project File - Name="strpak" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=strpak - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "strpak.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "strpak.mak" CFG="strpak - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "strpak - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "strpak - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "strpak - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../output/release/strpak"
# PROP Intermediate_Dir "../../output/release/strpak/intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_STLP_USE_STATIC_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib sdl.lib /nologo /subsystem:console /machine:I386 /out:"../../output/release/strpak.exe"

!ELSEIF  "$(CFG)" == "strpak - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../output/debug/strpak"
# PROP Intermediate_Dir "../../output/debug/strpak/intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_STLP_DEBUG" /D "_STLP_USE_SYSTEM_ASSERT" /D "_STLP_USE_NEWALLOC" /D "_STLP_DEBUG_ALLOC" /D "_STLP_USE_STATIC_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib sdl.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../output/debug/strpak.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "strpak - Win32 Release"
# Name "strpak - Win32 Debug"
# Begin Group "uwadv source files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\resource\cfgfile.cpp
# End Source File
# Begin Source File

SOURCE=..\resource\gamestrings.cpp
# End Source File
# Begin Source File

SOURCE=..\settings.cpp
# End Source File
# Begin Source File

SOURCE=..\utils.cpp
# End Source File
# End Group
# Begin Group "uwadv header files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\cfgfile.hpp
# End Source File
# Begin Source File

SOURCE=..\common.hpp
# End Source File
# Begin Source File

SOURCE=..\resource\fread_endian.hpp
# End Source File
# Begin Source File

SOURCE=..\gamestrings.hpp
# End Source File
# Begin Source File

SOURCE=..\settings.hpp
# End Source File
# Begin Source File

SOURCE=..\uatypes.hpp
# End Source File
# Begin Source File

SOURCE=..\utils.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\tools\strpak.cpp
# End Source File
# End Target
# End Project
