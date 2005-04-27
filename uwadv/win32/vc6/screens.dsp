# Microsoft Developer Studio Project File - Name="screens" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=screens - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "screens.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "screens.mak" CFG="screens - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "screens - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "screens - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "screens - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\screens"
# PROP Intermediate_Dir "Release\screens\intermediate"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\..\source" /I "..\..\source\audio" /I "..\..\source\base" /I "..\..\source\conv" /I "..\..\source\import" /I "..\..\source\renderer" /I "..\..\source\script" /I "..\..\source\ui" /I "..\..\source\underw" /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D "_STLP_NO_IOSTREAMS" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "screens - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "screens___Win32_Debug"
# PROP BASE Intermediate_Dir "screens___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug\screens"
# PROP Intermediate_Dir "Debug\screens\intermediate"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /I "..\..\source" /I "..\..\source\audio" /I "..\..\source\base" /I "..\..\source\conv" /I "..\..\source\import" /I "..\..\source\renderer" /I "..\..\source\script" /I "..\..\source\ui" /I "..\..\source\underw" /D "_LIB" /D "_DEBUG" /D "HAVE_DEBUG" /D "WIN32" /D "_MBCS" /D "_STLP_NO_IOSTREAMS" /YX /FD /GZ /c
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

# Name "screens - Win32 Release"
# Name "screens - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\source\screens\acknowledgements.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\conversation.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\create_character.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\cutscene_view.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\ingame_ctrls.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\ingame_orig.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\map_view.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\panel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\save_game.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\start_menu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\start_splash.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\uwadv_menu.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\source\screens\acknowledgements.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\conversation.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\create_character.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\cutscene_view.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\ingame_ctrls.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\ingame_new.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\ingame_orig.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\map_view.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\panel.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\save_game.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\start_menu.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\start_splash.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\screens\uwadv_menu.hpp
# End Source File
# End Group
# End Target
# End Project
