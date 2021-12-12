REM
REM Underworld Adventures - an Ultima Underworld remake project
REM Copyright (c) 2021 Underworld Adventures Team
REM
REM This cmd script generates .lib files from the Scintilla.def and Lexilla.def
REM files provided with binary SciTE zip files.

REM set this to your Visual Studio installation folder
set VSINSTALL=%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community

call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat"

lib /def:Scintilla.def /out:Scintilla.lib /machine:x86
lib /def:Lexilla.def /out:Lexilla.lib /machine:x86

pause
