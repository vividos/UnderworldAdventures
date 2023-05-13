@echo off
REM
REM Underworld Adventures - an Ultima Underworld hacking project
REM Copyright (c) 2023 Underworld Adventures Team
REM
REM Cleans the project folder from temporary files
REM

echo Cleaning all files...

rmdir /s /q .vs 2> nul
rmdir /s /q .bw-output 2> nul
rmdir /s /q .sonar-cache 2> nul
rmdir /s /q .sonarqube 2> nul
rmdir /s /q output 2> nul
rmdir /s /q vcpkg_installed 2> nul
rmdir /s /q CoverageReport 2> nul
rmdir /s /q TestResults 2> nul
rmdir /s /q android\.idea 2> nul
rmdir /s /q android\.gradle 2> nul
rmdir /s /q android\app\.cxx 2> nul
rmdir /s /q android\app\build 2> nul
rmdir /s /q android\app\src\main\jniLibs 2> nul
rmdir /s /q win32\setup\obj 2> nul
rmdir /s /q win32\setup\bin 2> nul

del /S /Q *.lob 2> nul
del /S /Q *.user 2> nul
del /S /Q *.aps 2> nul
del /S /Q *.bml 2> nul
del /Q source\win32\uadebug\res\Ribbon.h 2> nul
del /Q source\win32\uadebug\res\Ribbon.rc 2> nul
del /Q LastCoverageResults.log 2> nul
del /Q CoverageReport-SonarQube.xml 2> nul
del /Q doxygen\doxygen-warnings.txt 2> nul
