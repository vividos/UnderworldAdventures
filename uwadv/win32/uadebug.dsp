# Microsoft Developer Studio Project File - Name="uadebug" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=uadebug - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "uadebug.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "uadebug.mak" CFG="uadebug - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "uadebug - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "uadebug - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "uadebug - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../output/release/uadebug"
# PROP Intermediate_Dir "../output/release/uadebug/intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UADEBUG_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(wxwin)/include" /I "$(wxwin)/contrib/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UADEBUG_EXPORTS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxmsw.lib png.lib zlib.lib jpeg.lib tiff.lib regex.lib fl.lib /nologo /dll /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrtd.lib" /out:"../output/release/uadebug.dll" /libpath:"$(wxwin)/lib"

!ELSEIF  "$(CFG)" == "uadebug - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../output/debug/uadebug"
# PROP Intermediate_Dir "../output/debug/uadebug/intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UADEBUG_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(wxwin)/include" /I "$(wxwin)/contrib/include" /D "_DEBUG" /D "__WXDEBUG__" /D WXDEBUG=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UADEBUG_EXPORTS" /D "__WINDOWS__" /D "__WXMSW__" /D WINVER=0x0400 /D "STRICT" /YX"../debug/dbgcommon.hpp" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxmswd.lib pngd.lib zlibd.lib jpegd.lib tiffd.lib regexd.lib fld.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /out:"../output/debug/uadebug.dll" /pdbtype:sept /libpath:"$(wxwin)/lib"

!ENDIF 

# Begin Target

# Name "uadebug - Win32 Release"
# Name "uadebug - Win32 Debug"
# Begin Group "debugger source files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\source\debug\childframe.cpp
# ADD CPP /Yu"dbgcommon.hpp"
# End Source File
# Begin Source File

SOURCE=..\source\debug\dbgapp.cpp
# ADD CPP /Yu"dbgcommon.hpp"
# End Source File
# Begin Source File

SOURCE=..\source\debug\editlist.cpp
# ADD CPP /Yu"dbgcommon.hpp"
# End Source File
# Begin Source File

SOURCE=..\source\debug\mainframe.cpp
# ADD CPP /Yu"dbgcommon.hpp"
# End Source File
# Begin Source File

SOURCE=..\source\debug\objectlist.cpp
# ADD CPP /Yu"dbgcommon.hpp"
# End Source File
# Begin Source File

SOURCE=..\source\debug\pch.cpp
# ADD CPP /Yc"dbgcommon.hpp"
# End Source File
# Begin Source File

SOURCE=..\source\debug\playerinfo.cpp
# ADD CPP /Yu"dbgcommon.hpp"
# End Source File
# Begin Source File

SOURCE=.\uadebug.def
# End Source File
# End Group
# Begin Group "debugger header files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\source\debug\childframe.hpp
# End Source File
# Begin Source File

SOURCE=..\source\debug\dbgapp.hpp
# End Source File
# Begin Source File

SOURCE=..\source\debug\dbgcommon.hpp
# End Source File
# Begin Source File

SOURCE=..\source\debug\editlist.hpp
# End Source File
# Begin Source File

SOURCE=..\source\debug\mainframe.hpp
# End Source File
# Begin Source File

SOURCE=..\source\debug\objectlist.hpp
# End Source File
# Begin Source File

SOURCE=..\source\debug\playerinfo.hpp
# End Source File
# Begin Source File

SOURCE=..\source\debug\uwaccess.hpp
# End Source File
# End Group
# Begin Group "resource files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "misc. files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\debug\uwaccess.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Target
# End Project
