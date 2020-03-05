#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2020 Underworld Adventures Team
#
# Findzlib.cmake: CMake find project file for zlib.
#
set(ZLIB_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include")
set(ZLIB_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/zlibstat.lib")

string(STRIP "${ZLIB_LIBRARIES}" ZLIB_LIBRARIES)
