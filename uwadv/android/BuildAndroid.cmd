@echo off
REM
REM Underworld Adventures - an Ultima Underworld hacking project
REM Copyright (c) 2022 Underworld Adventures Team
REM
REM Builds Android app
REM

REM set this to your installed Android NDK
set ANDROID_NDK_HOME=C:\Projekte\Tools\AndroidSdk\ndk\24.0.8215888

REM set this to your vcpkg git repository
set VCPKG_ROOT=C:\Projekte\Tools\vcpkg

pushd ..

cmake -B output\build\android-arm64-v8a-debug
cmake --build ^
  --preset "android-arm64-debug" ^
  --target "uwadv_android_apk"

popd
