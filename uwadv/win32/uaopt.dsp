# Microsoft Developer Studio Project File - Name="uaopt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=uaopt - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "uaopt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "uaopt.mak" CFG="uaopt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "uaopt - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "uaopt - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "uaopt - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../output/release/uaopt"
# PROP Intermediate_Dir "../output/release/uaopt/intermediate"
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

!ELSEIF  "$(CFG)" == "uaopt - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../output/debug/uaopt"
# PROP Intermediate_Dir "../output/debug/uaopt/intermediate"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /O2 /Ob2 /I "..\source" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_DEBUG" /YX /FD /c
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

# Name "uaopt - Win32 Release"
# Name "uaopt - Win32 Debug"
# Begin Group "library files"

# PROP Default_Filter ""
# Begin Group "zziplib sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\base\zziplib\SDL_rwops_zzip.c
# ADD CPP /I "..\source\base\zziplib"
# End Source File
# Begin Source File

SOURCE="..\source\base\zziplib\zzip-dir.c"
# ADD CPP /I "..\source\base\zziplib"
# End Source File
# Begin Source File

SOURCE="..\source\base\zziplib\zzip-err.c"
# ADD CPP /I "..\source\base\zziplib"
# End Source File
# Begin Source File

SOURCE="..\source\base\zziplib\zzip-file.c"
# ADD CPP /I "..\source\base\zziplib"
# End Source File
# Begin Source File

SOURCE="..\source\base\zziplib\zzip-info.c"
# ADD CPP /I "..\source\base\zziplib"
# End Source File
# Begin Source File

SOURCE="..\source\base\zziplib\zzip-io.c"
# ADD CPP /I "..\source\base\zziplib"
# End Source File
# Begin Source File

SOURCE="..\source\base\zziplib\zzip-stat.c"
# ADD CPP /I "..\source\base\zziplib"
# End Source File
# Begin Source File

SOURCE="..\source\base\zziplib\zzip-zip.c"
# ADD CPP /I "..\source\base\zziplib"
# End Source File
# End Group
# Begin Group "zziplib headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\base\zziplib\SDL_rwops_zzip.h
# End Source File
# Begin Source File

SOURCE="..\source\base\zziplib\zzip-conf.h"
# End Source File
# Begin Source File

SOURCE="..\source\base\zziplib\zzip-file.h"
# End Source File
# Begin Source File

SOURCE="..\source\base\zziplib\zzip-io.h"
# End Source File
# Begin Source File

SOURCE="..\source\base\zziplib\zzip-msvc.h"
# End Source File
# Begin Source File

SOURCE="..\source\base\zziplib\zzip-stdint.h"
# End Source File
# Begin Source File

SOURCE=..\source\base\zziplib\zzip.h
# End Source File
# Begin Source File

SOURCE=..\source\base\zziplib\zzipformat.h
# End Source File
# Begin Source File

SOURCE=..\source\base\zziplib\zziplib.h
# End Source File
# End Group
# Begin Group "lua headers"

# PROP Default_Filter ""
# Begin Group "lua internal headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\script\lua\src\lapi.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lcode.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\ldebug.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\ldo.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lfunc.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lgc.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\llex.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\llimits.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lmem.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lobject.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lopcodes.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lparser.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lstate.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lstring.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\ltable.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\ltm.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lundump.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lvm.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lzio.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\source\script\lua\include\lauxlib.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\include\lua.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\include\luadebug.h
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\include\lualib.h
# End Source File
# End Group
# Begin Group "lua sources"

# PROP Default_Filter ""
# Begin Group "lualib sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\script\lua\src\lib\lauxlib.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lib\lbaselib.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lib\lmathlib.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lib\lstrlib.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# End Group
# Begin Source File

SOURCE=..\source\script\lua\src\lapi.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lcode.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\ldebug.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\ldo.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lfunc.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lgc.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\llex.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lmem.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lobject.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lparser.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lstate.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lstring.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\ltable.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\ltests.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\ltm.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lundump.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lvm.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# Begin Source File

SOURCE=..\source\script\lua\src\lzio.c
# ADD CPP /I "..\source\script\lua\include"
# End Source File
# End Group
# End Group
# End Target
# End Project
