#
# Underworld Adventures - an Ultima Underworld hacking project
# Copyright (c) 2002,2003,2004 Underworld Adventures Team
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# $Id$
#
# Underworld Adventures install script
# Note: process this with NSIS 2.0 as follows:
# makensis /DVERSION="<version-string>"
#

# compiler utility commands
#
!packhdr temp.dat "upx -9 temp.dat"

# compiler flags
#
SetCompressor lzma
SetOverwrite ifnewer
SetDateSave on
SetCompress auto
SetDatablockOptimize on
SetDateSave on


# general installer config
#
OutFile "uwadv-${VERSION}-win32.exe"
Name "Underworld Adventures '${VERSION}'"
CRCCheck on

# install directory config
#
#InstallDir "c:\uwadv\"
InstallDir "$PROGRAMFILES\Underworld Adventures"
InstallDirRegKey HKLM "Software\Underworld Adventures" "InstallPath"


# license page config
#
#Page license
LicenseText "You should read the following license before installing."
LicenseData "License.installer.txt"


# component page config
#
#Page components
#ComponentText "This will install Underworld Adventures '${VERSION}' on your system."
#InstType Typical


# directory page config
#
#Page directory
DirText "Please select a location to install Underworld Adventures '${VERSION}' (or use the default)."


# install page config
#
#Page instfiles
AutoCloseWindow false
ShowInstDetails hide


# uninstall config
#
#UninstPage uninstConfirm
#UninstPage instfiles
UninstallText "This will uninstall Underworld Adventures '${VERSION}' from your system."
ShowUninstDetails hide


#
# installation execution commands
#

#
# section: all needed uwadv files
#
Section "Underworld Adventures (required)"
SectionIn RO
SetOutPath $INSTDIR
File uwadv.exe
File uaconfig.exe
File SDL.dll
File SDL_mixer.dll
File uwadv.cfg
File keymap.cfg
File Copying
File README.uwadv.txt
File uwadv-manual.html
File uw1-keyboard.txt
File Changes.txt
File Authors.txt
CreateDirectory "$INSTDIR\uasave"

SetOutPath $INSTDIR\uadata
File uadata\uadata00.uar

SetOutPath $INSTDIR

WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Underworld Adventures" "DisplayName" "Underworld Adventures '${VERSION}'"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Underworld Adventures" "UninstallString" '"$INSTDIR\uninst-uwadv.exe"'
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Underworld Adventures" "NoModify" 1
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Underworld Adventures" "NoRepair" 1

WriteRegStr HKLM "Software\Underworld Adventures" "InstallPath" "$INSTDIR"

WriteUninstaller "$INSTDIR\uninst-uwadv.exe"

CreateShortCut "$DESKTOP\Underworld Adventures.lnk" "$INSTDIR\uwadv.exe" "" "" "0"
CreateDirectory "$SMPROGRAMS\Underworld Adventures"
CreateShortCut "$SMPROGRAMS\Underworld Adventures\Underworld Adventures.lnk" "$INSTDIR\uwadv.exe" "" "" "0"
CreateShortCut "$SMPROGRAMS\Underworld Adventures\Underworld Adventures Config.lnk" "$INSTDIR\uaconfig.exe" "" "" "0"
CreateShortCut "$SMPROGRAMS\Underworld Adventures\Underworld Adventures Readme.lnk" "$INSTDIR\README.uwadv.txt" "" "" "0"
CreateShortCut "$SMPROGRAMS\Underworld Adventures\Underworld Adventures Manual.lnk" "$INSTDIR\uwadv-manual.html" "" "" "0"
CreateShortCut "$SMPROGRAMS\Underworld Adventures\Ultima Underworld 1 Keyboard Commands.lnk" "$INSTDIR\uw1-keyboard.txt" "" "" "0"
CreateShortCut "$SMPROGRAMS\Underworld Adventures\Uninstall Underworld Adventures.lnk" "$INSTDIR\uninst-uwadv.exe" "" "" "0"

SectionEnd


Function .onInstSuccess
  MessageBox MB_YESNO|MB_ICONQUESTION \
    "Setup has completed. View readme file now?" IDNO NoReadme
    ExecShell open '$INSTDIR\README.uwadv.txt'
  NoReadme:
   MessageBox MB_OK "Please configure Underworld Adventures in the next dialog."
   Exec $INSTDIR\uaconfig.exe
FunctionEnd


#
# section: uninstall
#
Section "Uninstall"
Delete $INSTDIR\uwadv.exe
Delete $INSTDIR\uaconfig.exe
Delete $INSTDIR\SDL.dll
Delete $INSTDIR\SDL_mixer.dll
Delete $INSTDIR\uwadv.cfg
Delete $INSTDIR\uwadv.cfg.old
Delete $INSTDIR\keymap.cfg
Delete $INSTDIR\Copying
Delete $INSTDIR\README.uwadv.txt
Delete $INSTDIR\uwadv-manual.html
Delete $INSTDIR\uw1-keyboard.txt
Delete $INSTDIR\Authors.txt
Delete $INSTDIR\Changes.txt
Delete $INSTDIR\stdout.txt
Delete $INSTDIR\stderr.txt
Delete $INSTDIR\uninst-uwadv.exe
Delete $INSTDIR\uadata\uadata00.uar
RMDir $INSTDIR\uadata

ClearErrors
RMDir $INSTDIR\uasave

IfErrors HaveError HaveNoError
HaveError:
   MessageBox MB_OK "The Underworld Adventures folder could not be deleted. There may be stored savegames in the folder."
HaveNoError:

RMDir $INSTDIR

Delete "$DESKTOP\Underworld Adventures.lnk"
Delete "$SMPROGRAMS\Underworld Adventures\Underworld Adventures.lnk"
Delete "$SMPROGRAMS\Underworld Adventures\Underworld Adventures Config.lnk"
Delete "$SMPROGRAMS\Underworld Adventures\Underworld Adventures Readme.lnk"
Delete "$SMPROGRAMS\Underworld Adventures\Ultima Underworld 1 Keyboard Commands.lnk"
Delete "$SMPROGRAMS\Underworld Adventures\Uninstall Underworld Adventures.lnk"
RMDir "$SMPROGRAMS\Underworld Adventures"

DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Underworld Adventures"
DeleteRegKey HKLM "Software\Underworld Adventures"

SectionEnd
