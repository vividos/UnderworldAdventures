@echo off
REM
REM Underworld Adventures - an Ultima Underworld hacking project
REM Copyright (c) 2019-2021 Underworld Adventures Team
REM
REM Runs SonarCloud analysis build
REM

REM set this to your Visual Studio installation folder
set VSINSTALL=%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community

REM set this to your SonarQube tools folder
set SONARQUBE=C:\Projekte\Tools\SonarQube

REM and this to your OpenCppCoverage folder
set OPENCPPCOVERAGE=C:\Projekte\Tools\OpenCppCoverage\

REM
REM Preparations
REM
call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat"

if "%SONARLOGIN%" == "" echo "Environment variable SONARLOGIN is not set! Obtain a new token and set the environment variable!"
if "%SONARLOGIN%" == "" exit 1

set PATH=%PATH%;%SONARQUBE%\build-wrapper-win-x86;%SONARQUBE%\sonar-scanner-msbuild;%OPENCPPCOVERAGE%

REM
REM Build using SonarQube scanner for MSBuild
REM
rmdir .\.sonarqube /s /q 2> nul
rmdir .\bw-output /s /q 2> nul

SonarScanner.MSBuild.exe begin ^
    /k:"UnderworldAdventures" ^
    /v:"0.10" ^
    /d:"sonar.cfamily.build-wrapper-output=%CD%\bw-output" ^
    /d:"sonar.cfamily.threads=4" ^
    /d:"sonar.cfamily.cache.enabled=true" ^
    /d:"sonar.cfamily.cache.path=%CD%\.sonarqube\cache" ^
    /d:"sonar.coverageReportPaths=%CD%\CoverageReport-SonarQube.xml" ^
    /d:"sonar.host.url=https://sonarcloud.io" ^
    /o:"vividos-github" ^
    /d:"sonar.login=%SONARLOGIN%" ^
    /d:sonar.cs.vstest.reportsPaths="%CD%\TestResults\*.trx"
if errorlevel 1 goto end

REM
REM Rebuild Release|Win32
REM
build-wrapper-win-x86-64.exe --out-dir bw-output msbuild uwadv.sln /m /property:Configuration=SonarCloud,Platform=Win32 /target:Restore;Rebuild

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
   --export_type SonarQube:CoverageReport-SonarQube.xml ^
   --export_type html:CoverageReport ^
   --modules unittest.dll ^
   --excluded_modules vstest.console.exe ^
   -- "%VSINSTALL%\Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe" ^
   "output\bin\Release\unittest.dll" /Platform:x86 /InIsolation /logger:trx

SonarScanner.MSBuild.exe end /d:"sonar.login=%SONARLOGIN%"

:end

pause
