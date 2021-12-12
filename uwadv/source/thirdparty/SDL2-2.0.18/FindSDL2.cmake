#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2020 Underworld Adventures Team
#
# FindSDL2.cmake: CMake find project file for SDL2.
#
set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include")

# Support both 32 and 64 bit builds
if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
	set(SDL2_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2main.lib")
	set(SDL2_RUNTIME_FILES "${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2.dll")
else ()
	set(SDL2_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2main.lib")
	set(SDL2_RUNTIME_FILES "${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2.dll")
endif ()

string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)
