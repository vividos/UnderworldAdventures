# Microsoft Developer Studio Project File - Name="underw" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=underw - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "underw.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "underw.mak" CFG="underw - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "underw - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "underw - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "underw - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\underw"
# PROP Intermediate_Dir "Release\underw\intermediate"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /Zi /O2 /I "..\..\source" /I "..\..\source\base" /I "..\..\source\conv" /I "..\..\source\script" /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D "_STLP_USE_STATIC_LIB" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "underw - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "underw___Win32_Debug"
# PROP BASE Intermediate_Dir "underw___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug\underw"
# PROP Intermediate_Dir "Debug\underw\intermediate"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /Gi /GX /ZI /Od /I "..\..\source" /I "..\..\source\base" /I "..\..\source\conv" /I "..\..\source\script" /D "_LIB" /D "_DEBUG" /D "HAVE_DEBUG" /D "WIN32" /D "_MBCS" /D "_STLP_USE_STATIC_LIB" /D "_STLP_DEBUG" /YX /FD /GZ /c
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

# Name "underw - Win32 Release"
# Name "underw - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\source\underw\geometry.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\underw\inventory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\underw\level.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\underw\mapnotes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\underw\objects.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\underw\pathfinder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\underw\physics.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\underw\player.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\underw\underworld.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\source\underw\geometry.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\underw\inventory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\underw\level.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\underw\mapnotes.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\underw\objects.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\underw\pathfinder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\underw\physics.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\underw\physicsbody.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\underw\player.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\underw\properties.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\underw\runes.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\underw\underworld.hpp
# End Source File
# End Group
# End Target
# End Project
