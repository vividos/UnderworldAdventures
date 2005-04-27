# Microsoft Developer Studio Project File - Name="ui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ui.mak" CFG="ui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ui - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ui - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ui - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\ui"
# PROP Intermediate_Dir "Release\ui\intermediate"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\..\source" /I "..\..\source\base" /I "..\..\source\import" /I "..\..\source\renderer" /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D "_STLP_USE_STATIC_LIB" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ui___Win32_Debug"
# PROP BASE Intermediate_Dir "ui___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug\ui"
# PROP Intermediate_Dir "Debug\ui\intermediate"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /I "..\..\source" /I "..\..\source\base" /I "..\..\source\import" /I "..\..\source\renderer" /D "_LIB" /D "_DEBUG" /D "HAVE_DEBUG" /D "WIN32" /D "_MBCS" /D "_STLP_USE_STATIC_LIB" /D "_STLP_DEBUG" /YX /FD /GZ /c
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

# Name "ui - Win32 Release"
# Name "ui - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\source\ui\cutscene.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ui\font.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ui\image.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ui\imgquad.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ui\mousecursor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ui\screen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ui\textedit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ui\textscroll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ui\window.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\source\ui\cutscene.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ui\fading.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ui\font.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ui\image.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ui\imgquad.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ui\mousecursor.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ui\screen.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ui\textedit.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ui\textscroll.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ui\window.hpp
# End Source File
# End Group
# End Target
# End Project
