#
# Underworld Adventures install script
#

!define VERSION "0.5-butterfinger"

# compiler utility commands
#
!packhdr temp.dat "upx -9 temp.dat"

# general installer config
#
OutFile "uwadv-bin-${VERSION}-win32.exe"
Name "Underworld Adventures '${VERSION}'"
CRCCheck on

# install directory config
#
InstallDir "c:\uwadv\"
#InstallDir "$PROGRAMFILES\Underworld Adventures"

# license page config
#
LicenseText "You should read the following license before installing."
LicenseData "License.installer.txt"

# component page config
#
ComponentText "This will install Underworld Adventures '${VERSION}' on your system."
InstType Typical

# directory page config
#
DirShow show
DirText "Please select a location to install Underworld Adventures '${VERSION}' (or use the default)."

# install page config
#
AutoCloseWindow false
ShowInstDetails show

# uninstall config
#
UninstallText "This will uninstall Underworld Adventures '${VERSION}' from your system."
ShowUninstDetails show

# compiler flags
#
SetOverwrite ifnewer
SetCompress auto
SetDatablockOptimize on
SetDateSave on

#
# installation execution commands
#

#
# section: all needed uwadv files
#
Section -
#SectionIn 0
SetOutPath $INSTDIR
File uwadv.exe
File uaconfig.exe
File SDL.dll
File SDL_mixer.dll
File uwadv.cfg
File Copying
File README.uwadv.txt
File uw1-keyboard.txt
CreateDirectory "$INSTDIR\uasave"

SetOutPath $INSTDIR\uadata
File uadata\uadata00.uar

SetOutPath $INSTDIR

WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\Underworld Adventures" "DisplayName" "Underworld Adventures '${VERSION}' (remove only)"
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\Underworld Adventures" "UninstallString" '"$INSTDIR\uninst-uwadv.exe"'

WriteUninstaller $INSTDIR\uninst-uwadv.exe
SectionEnd

#
# section: icons
#
Section "Start Menu and Desktop Icons"
SectionIn 1
SetShellVarContext current
CreateShortCut "$DESKTOP\Underworld Adventures.lnk" "$INSTDIR\uwadv.exe" "" "" "0"
CreateDirectory "$SMPROGRAMS\Underworld Adventures"
CreateShortCut "$SMPROGRAMS\Underworld Adventures\Underworld Adventures.lnk" "$INSTDIR\uwadv.exe" "" "" "0"
CreateShortCut "$SMPROGRAMS\Underworld Adventures\Underworld Adventures Config.lnk" "$INSTDIR\uaconfig.exe" "" "" "0"
CreateShortCut "$SMPROGRAMS\Underworld Adventures\Underworld Adventures Readme.lnk" "$INSTDIR\README.uwadv.txt" "" "" "0"
CreateShortCut "$SMPROGRAMS\Underworld Adventures\Ultima Underworld 1 Keyboard Commands.lnk" "$INSTDIR\uw1-keyboard.txt" "" "" "0"
SectionEnd

Function .onInstSuccess
  MessageBox MB_YESNO|MB_ICONQUESTION \
    "Setup has completed. View readme file now?" \
    IDNO NoReadme
    ExecShell open '$INSTDIR\README.uwadv.txt'
  NoReadme:
   MessageBox MB_OK "Please configure Underworld Adventures in the next dialog."
   Exec $INSTDIR\uaconfig.exe
FunctionEnd

#
# section: uninstall
#
Section Uninstall
Delete $INSTDIR\uwadv.exe
Delete $INSTDIR\uaconfig.exe
Delete $INSTDIR\SDL.dll
Delete $INSTDIR\SDL_mixer.dll
Delete $INSTDIR\uwadv.cfg
Delete $INSTDIR\uwadv.cfg.old
Delete $INSTDIR\Copying
Delete $INSTDIR\README.uwadv.txt
Delete $INSTDIR\uw1-keyboard.txt
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
RMDir "$SMPROGRAMS\Underworld Adventures"

DeleteRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\Underworld Adventures"

SectionEnd
