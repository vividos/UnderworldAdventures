# Microsoft Developer Studio Project File - Name="script" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=script - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "script.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "script.mak" CFG="script - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "script - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "script - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "script - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\script"
# PROP Intermediate_Dir "Release\script\intermediate"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\..\source" /I "..\..\source\base" /I "..\..\source\conv" /I "..\..\source\import" /I "..\..\source\script\lua\include" /I "..\..\source\underw" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "script - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "script___Win32_Debug"
# PROP BASE Intermediate_Dir "script___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug\script"
# PROP Intermediate_Dir "Debug\script\intermediate"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\source" /I "..\..\source\base" /I "..\..\source\conv" /I "..\..\source\import" /I "..\..\source\script\lua\include" /I "..\..\source\underw" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
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

# Name "script - Win32 Release"
# Name "script - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\source\script\luascript.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\source\script\luascript.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\script\scripting.hpp
# End Source File
# End Group
# Begin Group "Lua Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\source\script\lua\src\lapi.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lib\lauxlib.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lib\lbaselib.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lcode.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lib\ldblib.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\ldebug.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\ldo.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lfunc.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lgc.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lib\liolib.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\llex.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lib\lmathlib.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lmem.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lobject.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lparser.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lstate.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lstring.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lib\lstrlib.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\ltable.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\ltests.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\ltm.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lundump.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lvm.c
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lzio.c
# End Source File
# End Group
# Begin Group "Lua Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\source\script\lua\src\lapi.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\include\lauxlib.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lcode.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\ldebug.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\ldo.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lfunc.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lgc.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\llex.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\llimits.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lmem.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lobject.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lopcodes.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lparser.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lstate.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lstring.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\ltable.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\ltm.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\include\lua.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\include\luadebug.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\include\lualib.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lundump.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lvm.h
# End Source File
# Begin Source File

SOURCE=..\..\source\script\lua\src\lzio.h
# End Source File
# End Group
# End Target
# End Project
