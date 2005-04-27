# Microsoft Developer Studio Project File - Name="uastudio" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=uastudio - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "uastudio.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "uastudio.mak" CFG="uastudio - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "uastudio - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "uastudio - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "uastudio - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "uastudio___Win32_Release"
# PROP BASE Intermediate_Dir "uastudio___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\uastudio"
# PROP Intermediate_Dir "Release\uastudio\intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\..\source" /I "..\..\source\base" /I "..\..\source\conv" /I "..\..\source\import" /I "..\..\source\ui" /I "..\..\source\underw" /D "NDEBUG" /D "_CONSOLE" /D "WIN32" /D "_MBCS" /D "_STLP_USE_STATIC_LIB" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 base.lib conv.lib import.lib script.lib ui.lib underw.lib SDL.lib SDLmain.lib zlib.lib /nologo /subsystem:console /pdb:"Release\uastudio.pdb" /debug /machine:I386 /out:"Release\uastudio.exe" /libpath:"$(OutDir)\..\base" /libpath:"$(OutDir)\..\conv" /libpath:"$(OutDir)\..\import" /libpath:"$(OutDir)\..\script" /libpath:"$(OutDir)\..\ui" /libpath:"$(OutDir)\..\underw"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "uastudio - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "uastudio___Win32_Debug"
# PROP BASE Intermediate_Dir "uastudio___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug\uastudio"
# PROP Intermediate_Dir "Debug\uastudio\intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /I "..\..\source" /I "..\..\source\base" /I "..\..\source\conv" /I "..\..\source\import" /I "..\..\source\ui" /I "..\..\source\underw" /D "_CONSOLE" /D "_DEBUG" /D "HAVE_DEBUG" /D "WIN32" /D "_MBCS" /D "_STLP_USE_STATIC_LIB" /D "_STLP_DEBUG" /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 base.lib conv.lib import.lib script.lib ui.lib underw.lib SDL.lib SDLmain.lib zlib.lib /nologo /subsystem:console /pdb:"Debug\uastudio.pdb" /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /out:"Debug\uastudio.exe" /pdbtype:sept /libpath:"$(OutDir)\..\base" /libpath:"$(OutDir)\..\conv" /libpath:"$(OutDir)\..\import" /libpath:"$(OutDir)\..\script" /libpath:"$(OutDir)\..\ui" /libpath:"$(OutDir)\..\underw"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "uastudio - Win32 Release"
# Name "uastudio - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\source\debug.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\gamestrings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\uastudio\uastudio.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\source\win32\uastudio\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\source\win32\uastudio\uastudio_res.rc
# End Source File
# End Group
# End Target
# End Project
