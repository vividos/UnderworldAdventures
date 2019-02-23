REM
REM Underworld Adventures - an Ultima Underworld hacking project
REM Copyright (c) 2019 Michael Fink
REM
REM Build script for thirdparty libraries
REM

set OUTDIR=%1

echo Copying thirdparty files to %OUTDIR%

mkdir %OUTDIR% 2> nul
copy SDL2-2.0.9\lib\x86\SDL2.dll %OUTDIR%
copy SDL2_mixer-2.0.4\lib\x86\*.dll %OUTDIR%
