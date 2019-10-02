REM
REM Underworld Adventures - an Ultima Underworld remake project
REM Copyright (c) 2019 Underworld Adventures Team
REM
REM Build script for uadata zip archive
REM

set OUTDIR=%1

echo Creating uadata zip archive in %OUTDIR%

mkdir %OUTDIR% 2> nul
mkdir %OUTDIR%data 2> nul

REM create zip archive
del %OUTDIR%data\uadata00.zip 2> nul

"c:\Program Files\7-Zip\7z.exe" a -r ^
    -x!*.user -x!*.cmd -x!*.lua -x!*.vcxproj* ^
    -x!*.user -x!Makefile* ^
    -x!README.Data.md ^
    -x!"Underworld Adventures Soundtrack.m3u" ^
    %OUTDIR%data\uadata00.zip *.*

copy "Underworld Adventures Soundtrack.m3u" %OUTDIR%data\
copy README.Data.md %OUTDIR%data\README.Data.txt
