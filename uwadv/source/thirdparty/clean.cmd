REM
REM Underworld Adventures - an Ultima Underworld hacking project
REM Copyright (c) 2019,2022 Michael Fink
REM
REM Clean script for thirdparty libraries
REM

set OUTDIR=%1

del %OUTDIR%libFLAC-8.dll 2> nul
del %OUTDIR%libmodplug-1.dll 2> nul
del %OUTDIR%libmpg123-0.dll 2> nul
del %OUTDIR%libogg-0.dll 2> nul
del %OUTDIR%libopus-0.dll 2> nul
del %OUTDIR%libopusfile-0.dll 2> nul
del %OUTDIR%libvorbis-0.dll 2> nul
del %OUTDIR%libvorbisfile-3.dll 2> nul
del %OUTDIR%SDL2_mixer.dll 2> nul

del %OUTDIR%\Scintilla.dll 2> nul
del %OUTDIR%\Lexilla.dll 2> nul
del %OUTDIR%\lua.properties 2> nul
del %OUTDIR%\README.Manual.txt 2> nul
del %OUTDIR%\tools\README.Tools.txt 2> nul
del %OUTDIR%\uw1-walkthrough.txt 2> nul

exit 0
