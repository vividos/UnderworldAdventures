#
# Underworld Adventures install script
#

!define VERSION "0.0.1"

# compiler utility commands
#
!packhdr temp.dat "tools\upx -9 temp.dat"

# general installer config
#
OutFile ..\..\output\uwadv-${VERSION}.exe
Name "Underworld Adventures ${VERSION}"
CRCCheck on

# install directory config
#
InstallDir "$PROGRAMFILES\Underworld Adventures"

# license page config
#
#LicenseText "You should read the following license before installing."
#LicenseData ..\..\docs\License.installer.txt

# component page config
#
ComponentText "This will install Underworld Adventures ${VERSION} on your system."
InstType Typical

# directory page config
#
DirShow show
DirText "Please select a location to install Underworld Adventures ${VERSION} (or use the default)."

# install page config
#
AutoCloseWindow false
ShowInstDetails show

# uninstall config
#
UninstallText "This will uninstall Underworld Adventures ${VERSION} from your system."
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
SectionIn 0
SetOutPath $INSTDIR
File ..\..\output\release\uwadv.exe
#File SDL.dll
File ..\..\Copying
#File ..\readme.txt
CreateShortCut "$DESKTOP\Underworld Adventures.lnk" "$INSTDIR\uwadv.exe" "" "" "0"
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\uwadv" "DisplayName" "Underworld Adventures ${VERSION} (remove only)"
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\uwadv" "UninstallString" '"$INSTDIR\uninst-uwadv.exe"'
WriteUninstaller $INSTDIR\uninst-uwadv.exe
SectionEnd

Section Uninstall
Delete $INSTDIR\uwadv.exe
Delete $INSTDIR\Copying
Delete $INSTDIR\SDL.dll
Delete $INSTDIR\uninst-uwadv.exe
Delete "$DESKTOP\Underworld Adventures.lnk"
DeleteRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\uwadv"
RMDir $INSTDIR
SectionEnd
