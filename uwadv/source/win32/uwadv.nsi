#
# Underworld Adventures install script
#

!define VERSION "04c-cheesecake"

# compiler utility commands
#
!packhdr temp.dat "upx -9 temp.dat"

# general installer config
#
OutFile "uwadv-${VERSION}-bin-win32.exe"
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
ComponentText "This will install Underworld Adventures version '${VERSION}' on your system."
InstType Typical

# directory page config
#
DirShow show
DirText "Please select a location to install Underworld Adventures version '${VERSION}' (or use the default)."

# install page config
#
AutoCloseWindow false
ShowInstDetails show

# uninstall config
#
UninstallText "This will uninstall Underworld Adventures version '${VERSION}' from your system."
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

ComponentText
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

SetOutPath $INSTDIR\uadata
File uadata\uadata00.uar

#CreateShortCut "$DESKTOP\Underworld Adventures.lnk" "$INSTDIR\uwadv.exe" "" "" "0"
#WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\uwadv" "DisplayName" "Underworld Adventures version '${VERSION}' (remove only)"
#WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\uwadv" "UninstallString" '"$INSTDIR\uninst-uwadv.exe"'

#WriteUninstaller $INSTDIR\uninst-uwadv.exe
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

Section Uninstall
Delete $INSTDIR\uwadv.exe
Delete $INSTDIR\uaconfig.exe
Delete $INSTDIR\SDL.dll
Delete $INSTDIR\SDL_mixer.dll
Delete $INSTDIR\uwadv.cfg
Delete $INSTDIR\Copying
Delete $INSTDIR\README.uwadv.txt
Delete $INSTDIR\uninst-uwadv.exe
Delete $INSTDIR\uadata\uadata00.uar

#Delete "$DESKTOP\Underworld Adventures.lnk"
#DeleteRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\uwadv"

RMDir $INSTDIR\uadata
RMDir $INSTDIR
SectionEnd
