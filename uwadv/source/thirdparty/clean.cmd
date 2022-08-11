REM
REM Underworld Adventures - an Ultima Underworld hacking project
REM Copyright (c) 2019,2022 Michael Fink
REM
REM Clean script for thirdparty libraries
REM

set OUTDIR=%1

del %OUTDIR%\Scintilla.dll 2> nul
del %OUTDIR%\Lexilla.dll 2> nul
del %OUTDIR%\lua.properties 2> nul
del %OUTDIR%\README.Manual.txt 2> nul
del %OUTDIR%\tools\README.Tools.txt 2> nul
del %OUTDIR%\tools\luac.exe 2> nul
del %OUTDIR%\uw1-walkthrough.txt 2> nul

exit 0
