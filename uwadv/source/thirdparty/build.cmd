REM
REM Underworld Adventures - an Ultima Underworld hacking project
REM Copyright (c) 2019,2021,2022 Michael Fink
REM
REM Build script for thirdparty libraries
REM

set OUTDIR=%1

echo Copying thirdparty files and manuals to %OUTDIR%

mkdir %OUTDIR% 2> nul
mkdir %OUTDIR%\tools 2> nul
copy ..\win32\uadebug\thirdparty\scintilla\lib\Scintilla.dll %OUTDIR%
copy ..\win32\uadebug\thirdparty\scintilla\lib\Lexilla.dll %OUTDIR%
copy ..\win32\uadebug\thirdparty\scintilla\lib\lua.properties %OUTDIR%
copy ..\..\docs\README.Manual.md %OUTDIR%\README.Manual.txt
copy ..\..\docs\README.Tools.md %OUTDIR%\tools\README.Tools.txt
copy ..\..\docs\uw1-walkthrough.txt %OUTDIR%\uw1-walkthrough.txt
