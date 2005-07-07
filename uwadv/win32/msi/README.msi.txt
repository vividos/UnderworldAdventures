README for Underworld Adventures Windows Installer

Prerequisites
-------------

The following uwadv files need to be available to build the installer:

..\vc71\Release\:
luac.exe
SDL.dll
SDL_mixer.dll
strpak.exe
uaconfig.exe
uadebug.dll
uastudio.exe
uwadv.exe
uwdump.exe

..\..\source\win32\uadebug\thirdparty\scintilla\:
SciLexer.dll (automatically installed by "uadebug-thirdparty-?.zip"

..\..\docs\docbook\:
uwadv-manual.html
uastudio-manual.html

..\..\uadata\:
uadata00.uar

.\:
VC_User_CRT71_RTL_X86_---.msm
VC_User_STL71_RTL_X86_---.msm


Building
--------

Start building the .msi file with this command line:

<WIXPATH>\candle uwadv.wxs
<WIXPATH>\candle UserInterface.wxs
<WIXPATH>\light uwadv.wixobj UserInterface.wixobj -out uwadv.msi

<WIXPATH> is the path to the Windows Installer XML tools.
