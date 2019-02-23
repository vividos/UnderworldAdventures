@echo off
REM
REM Underworld Adventures - an Ultima Underworld remake project
REM Copyright (c) 2019 Michael Fink
REM
REM Downloads zlib and compiles it
REM

REM set this to the filename of the zip archive to download
set ZIPNAME=zlib1211
REM set this to the folder name of the root folder in the zip archive
set PREFIX=zlib-1.2.11

REM set this to your Visual Studio installation folder
set VSINSTALL=%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Community

REM download package
set URL=http://zlib.net/%ZIPNAME%.zip

if not exist %ZIPNAME%.zip powershell -Command "& {Invoke-WebRequest -Uri %URL% -Out %ZIPNAME%.zip}"

REM unzip
rmdir /s /q %PREFIX%\
"c:\Program Files\7-Zip\7z.exe" x %ZIPNAME%.zip

REM set up Visual Studio
call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat"

pushd %PREFIX%\contrib\vstudio\vc14\

REM update Toolset to v141, in order to use VS2017
powershell -Command "& {(Get-Content zlibstat.vcxproj) -replace \"v140\",\"v141\" | out-file zlibstat.vcxproj}"

REM compile
msbuild zlibstat.vcxproj /m /p:Configuration=ReleaseWithoutAsm /p:Platform=Win32 /t:Rebuild

popd

REM copy artifacts
mkdir ..\%ZIPNAME% 2> nul
xcopy /y %PREFIX%\zlib.h ..\%ZIPNAME%\include\
xcopy /y %PREFIX%\zconf.h ..\%ZIPNAME%\include\

xcopy /y %PREFIX%\contrib\vstudio\vc14\x86\ZlibStatReleaseWithoutAsm\*.lib ..\%ZIPNAME%\lib\
xcopy /y %PREFIX%\contrib\vstudio\vc14\x86\ZlibStatReleaseWithoutAsm\*.pdb ..\%ZIPNAME%\lib\

pause
