@echo off
REM
REM Underworld Adventures - an Ultima Underworld hacking project
REM Copyright (c) 2019 Underworld Adventures Team
REM
REM Runs Unit Tests and collects coverage information
REM

REM set this to your Visual Studio installation folder
set VSINSTALL=%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community

REM and this to your OpenCppCoverage folder
set OPENCPPCOVERAGE=D:\devel\tools\OpenCppCoverage\

REM
REM Preparations
REM
call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat"

set PATH=%PATH%;%OPENCPPCOVERAGE%

REM
REM Build Debug|Win32
REM
msbuild uwadv.sln /m /property:Configuration=Debug /property:Platform=Win32 /target:Build

REM
REM Run unit tests
REM
OpenCppCoverage.exe ^
   --continue_after_cpp_exception --cover_children ^
   --sources source\unittest ^
   --sources source\base ^
   --sources source\audio ^
   --sources source\import ^
   --sources source\physics ^
   --sources source\underw ^
   --excluded_sources packages ^
   --excluded_sources source\thirdparty ^
   --excluded_sources source\audio\midi ^
   --export_type html:CoverageReport ^
   --modules unittest.dll ^
   --excluded_modules vstest.console.exe ^
   -- "%VSINSTALL%\Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe" ^
   "output\bin\Debug\unittest.dll" /Platform:x86 /InIsolation /logger:trx

pause
