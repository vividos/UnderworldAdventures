#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2022 Underworld Adventures Team
#
# arm64-android-dynamic.cmake: vcpkg custom triplet
#

set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CRT_LINKAGE static)
set(VCPKG_LIBRARY_LINKAGE dynamic)
set(VCPKG_CMAKE_SYSTEM_NAME Android)
