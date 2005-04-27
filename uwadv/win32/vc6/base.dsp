# Microsoft Developer Studio Project File - Name="base" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=base - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "base.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "base.mak" CFG="base - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "base - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "base - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "base - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\base"
# PROP Intermediate_Dir "Release\base\intermediate"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\..\source" /I "..\..\source\base" /I "..\..\source\base\zziplib" /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D "_STLP_USE_STATIC_LIB" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "base - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "base___Win32_Debug"
# PROP BASE Intermediate_Dir "base___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug\base"
# PROP Intermediate_Dir "Debug\base\intermediate"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /I "..\..\source" /I "..\..\source\base" /I "..\..\source\base\zziplib" /D "_LIB" /D "_DEBUG" /D "HAVE_DEBUG" /D "WIN32" /D "_MBCS" /D "_STLP_USE_STATIC_LIB" /D "_STLP_DEBUG" /YX /FD /GZ /c
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

# Name "base - Win32 Release"
# Name "base - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\source\base\cfgfile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\base\files.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\base\keymap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\base\savegame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\base\settings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\base\utils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\source\base\cfgfile.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\base\files.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\base\keymap.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\base\savegame.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\base\settings.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\base\uamath.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\base\utils.hpp
# End Source File
# End Group
# Begin Group "zziplib Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\source\base\zziplib\SDL_rwops_zzip.c
# End Source File
# Begin Source File

SOURCE="..\..\source\base\zziplib\zzip-dir.c"
# End Source File
# Begin Source File

SOURCE="..\..\source\base\zziplib\zzip-err.c"
# End Source File
# Begin Source File

SOURCE="..\..\source\base\zziplib\zzip-file.c"
# End Source File
# Begin Source File

SOURCE="..\..\source\base\zziplib\zzip-info.c"
# End Source File
# Begin Source File

SOURCE="..\..\source\base\zziplib\zzip-io.c"
# End Source File
# Begin Source File

SOURCE="..\..\source\base\zziplib\zzip-stat.c"
# End Source File
# Begin Source File

SOURCE="..\..\source\base\zziplib\zzip-zip.c"
# End Source File
# End Group
# Begin Group "zziplib Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\source\base\zziplib\config.h
# End Source File
# Begin Source File

SOURCE=..\..\source\base\zziplib\SDL_rwops_zzip.h
# End Source File
# Begin Source File

SOURCE="..\..\source\base\zziplib\zzip-conf.h"
# End Source File
# Begin Source File

SOURCE="..\..\source\base\zziplib\zzip-config.h"
# End Source File
# Begin Source File

SOURCE="..\..\source\base\zziplib\zzip-file.h"
# End Source File
# Begin Source File

SOURCE="..\..\source\base\zziplib\zzip-io.h"
# End Source File
# Begin Source File

SOURCE="..\..\source\base\zziplib\zzip-msvc.h"
# End Source File
# Begin Source File

SOURCE="..\..\source\base\zziplib\zzip-stdint.h"
# End Source File
# Begin Source File

SOURCE=..\..\source\base\zziplib\zzip.h
# End Source File
# Begin Source File

SOURCE=..\..\source\base\zziplib\zzipformat.h
# End Source File
# Begin Source File

SOURCE=..\..\source\base\zziplib\zziplib.h
# End Source File
# End Group
# End Target
# End Project
