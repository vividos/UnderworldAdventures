# Microsoft Developer Studio Project File - Name="uwdump" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=uwdump - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "uwdump.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "uwdump.mak" CFG="uwdump - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "uwdump - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "uwdump - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "uwdump - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "uwdump___Win32_Release"
# PROP BASE Intermediate_Dir "uwdump___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\uwdump"
# PROP Intermediate_Dir "Release\uwdump\intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\..\source" /I "..\..\source\base" /I "..\..\source\conv" /I "..\..\source\import" /I "..\..\source\renderer" /I "..\..\source\ui" /I "..\..\source\underw" /D "NDEBUG" /D "_CONSOLE" /D "WIN32" /D "_MBCS" /D "_STLP_NO_IOSTREAMS" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 base.lib conv.lib import.lib renderer.lib ui.lib SDL.lib SDLmain.lib opengl32.lib glu32.lib /nologo /subsystem:console /pdb:"Release\uwdump.pdb" /debug /machine:I386 /out:"Release\uwdump.exe" /libpath:"$(OutDir)\..\base" /libpath:"$(OutDir)\..\conv" /libpath:"$(OutDir)\..\import" /libpath:"$(OutDir)\..\renderer" /libpath:"$(OutDir)\..\ui"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "uwdump - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "uwdump___Win32_Debug"
# PROP BASE Intermediate_Dir "uwdump___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug\uwdump"
# PROP Intermediate_Dir "Debug\uwdump\intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /I "..\..\source" /I "..\..\source\base" /I "..\..\source\conv" /I "..\..\source\import" /I "..\..\source\renderer" /I "..\..\source\ui" /I "..\..\source\underw" /D "_CONSOLE" /D "_DEBUG" /D "HAVE_DEBUG" /D "WIN32" /D "_MBCS" /D "_STLP_NO_IOSTREAMS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 base.lib conv.lib import.lib renderer.lib ui.lib SDL.lib SDLmain.lib opengl32.lib glu32.lib /nologo /subsystem:console /pdb:"Debug\uwdump.pdb" /debug /machine:I386 /out:"Debug\uwdump.exe" /pdbtype:sept /libpath:"$(OutDir)\..\base" /libpath:"$(OutDir)\..\conv" /libpath:"$(OutDir)\..\import" /libpath:"$(OutDir)\..\renderer" /libpath:"$(OutDir)\..\ui"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "uwdump - Win32 Release"
# Name "uwdump - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\source\tools\uwdump\cnvdec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\gamestrings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\tools\uwdump\levark.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\tools\uwdump\mdldump.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\tools\uwdump\prop.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\tools\uwdump\uwdump.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\source\tools\uwdump\cnvdec.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\tools\uwdump\levark.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\tools\uwdump\mdldump.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\tools\uwdump\prop.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\tools\uwdump\uwdump.hpp
# End Source File
# End Group
# End Target
# End Project
