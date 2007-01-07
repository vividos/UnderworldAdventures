@echo off
set WIXPATH=D:\devel\tools\wix

title Building .msi file...
echo Building .msi file...
echo.

del uwadv.msi 2> nul
del uwadv.wixobj 2> nul
del UserInterface.wixobj 2> nul
del ErrorTextList.wixobj 2> nul

%WIXPATH%\candle /nologo uwadv.wxs
%WIXPATH%\candle /nologo UserInterface.wxs
%WIXPATH%\candle /nologo ErrorTextList.wxs
%WIXPATH%\light /nologo uwadv.wixobj UserInterface.wixobj ErrorTextList.wixobj -out uwadv.msi

del UserInterface.wixobj
del uwadv.wixobj

pause
