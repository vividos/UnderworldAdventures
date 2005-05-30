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
# PROP BASE Output_Dir "uadebug___Win32_Release"
# PROP BASE Intermediate_Dir "uadebug___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\uadebug"
# PROP Intermediate_Dir "Release\uadebug\intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UADEBUG_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /Zi /O2 /I "..\..\source" /I "..\..\source\win32\uadebug\thirdparty\menuxp" /I "..\..\source\win32\uadebug\thirdparty\scintilla" /I "..\..\source\win32\uadebug\thirdparty\tabbingframework\include" /I "..\..\source\win32\uadebug\thirdparty\wtldockingwindows\include" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UADEBUG_EXPORTS" /D "WIN32" /D "_MBCS" /D "_STLP_USE_STATIC_LIB" /Yu"stdatl.hpp" /FD /Zm800 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib scintilla.lib /nologo /dll /pdb:"Release\uadebug.pdb" /debug /machine:I386 /out:"Release\uadebug.dll" /libpath:"..\..\source\win32\uadebug\thirdparty\scintilla\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "uadebug - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "uadebug___Win32_Debug"
# PROP BASE Intermediate_Dir "uadebug___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug\uadebug"
# PROP Intermediate_Dir "Debug\uadebug\intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UADEBUG_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /Gi /GX /ZI /Od /I "..\..\source" /I "..\..\source\win32\uadebug\thirdparty\menuxp" /I "..\..\source\win32\uadebug\thirdparty\scintilla" /I "..\..\source\win32\uadebug\thirdparty\tabbingframework\include" /I "..\..\source\win32\uadebug\thirdparty\wtldockingwindows\include" /D "_WINDOWS" /D "_USRDLL" /D "UADEBUG_EXPORTS" /D "_DEBUG" /D "HAVE_DEBUG" /D "WIN32" /D "_MBCS" /D "_STLP_USE_STATIC_LIB" /D "_STLP_DEBUG" /Yu"stdatl.hpp" /FD /GZ /Zm800 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib scintilla.lib /nologo /dll /pdb:"Debug\uadebug.pdb" /debug /machine:I386 /out:"Debug\uadebug.dll" /pdbtype:sept /libpath:"..\..\source\win32\uadebug\thirdparty\scintilla\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "uadebug - Win32 Release"
# Name "uadebug - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\source\win32\uadebug\DebugClient.cpp
# ADD CPP /Yu"stdatl.hpp"
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\EditListViewCtrl.cpp
# ADD CPP /Yu"stdatl.hpp"
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\GameStringsView.cpp
# ADD CPP /Yu"stdatl.hpp"
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\HotspotList.cpp
# ADD CPP /Yu"stdatl.hpp"
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\LuaSourceView.cpp
# ADD CPP /Yu"stdatl.hpp"
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\MainFrame.cpp
# ADD CPP /Yu"stdatl.hpp"
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\ObjectList.cpp
# ADD CPP /Yu"stdatl.hpp"
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\PlayerInfo.cpp
# ADD CPP /Yu"stdatl.hpp"
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\ProjectInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\ProjectManager.cpp
# ADD CPP /Yu"stdatl.hpp"
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\stdatl.cpp
# ADD CPP /Yc"stdatl.hpp"
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\TileMapView.cpp
# ADD CPP /Yu"stdatl.hpp"
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\TileMapViewCtrl.cpp
# ADD CPP /Yu"stdatl.hpp"
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\uadebug.cpp
# ADD CPP /Yu"stdatl.hpp"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\source\win32\uadebug\AboutDlg.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\DebugClient.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\EditListViewCtrl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\GameStringsView.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\HotspotList.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\LuaSourceView.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\MainFrame.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\ObjectList.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\PlayerInfo.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\ProjectInfo.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\ProjectManager.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\stdatl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\TileMapView.hpp
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\TileMapViewCtrl.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\source\win32\uadebug\res\cursor_tilemap.cur
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\res\Des.ico
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\res\game_strings.ico
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\res\Lua.ico
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\res\Sanct.ico
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\res\tb_game_strings.bmp
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\res\tb_lua_source.bmp
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\res\tb_mainframe.bmp
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\res\tb_tilemap.bmp
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\res\tilemap_view.ico
# End Source File
# Begin Source File

SOURCE=..\..\source\win32\uadebug\uadebug_res.rc
# End Source File
# End Group
# End Target
# End Project
