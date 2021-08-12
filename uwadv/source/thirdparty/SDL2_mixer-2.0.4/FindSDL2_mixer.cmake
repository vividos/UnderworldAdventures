#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2021 Underworld Adventures Team
#
# FindSDL2_mixer.cmake: CMake find project file for SDL2_mixer.
#
set(SDL2_MIXER_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include")

# Support both 32 and 64 bit builds
if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
	set(SDL2_MIXER_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2_mixer.lib")
	set(SDL2_MIXER_RUNTIME_FILES
		"${CMAKE_CURRENT_LIST_DIR}/lib/x64/libFLAC-8.dll"
		"${CMAKE_CURRENT_LIST_DIR}/lib/x64/libmodplug-1.dll"
		"${CMAKE_CURRENT_LIST_DIR}/lib/x64/libmpg123-0.dll"
		"${CMAKE_CURRENT_LIST_DIR}/lib/x64/libogg-0.dll"
		"${CMAKE_CURRENT_LIST_DIR}/lib/x64/libopus-0.dll"
		"${CMAKE_CURRENT_LIST_DIR}/lib/x64/libopusfile-0.dll"
		"${CMAKE_CURRENT_LIST_DIR}/lib/x64/libvorbis-0.dll"
		"${CMAKE_CURRENT_LIST_DIR}/lib/x64/libvorbisfile-3.dll"
		"${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2_mixer.dll")
else ()
	set(SDL2_MIXER_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2_mixer.lib")
	set(SDL2_MIXER_RUNTIME_FILES
		"${CMAKE_CURRENT_LIST_DIR}/lib/x86/libFLAC-8.dll"
		"${CMAKE_CURRENT_LIST_DIR}/lib/x86/libmodplug-1.dll"
		"${CMAKE_CURRENT_LIST_DIR}/lib/x86/libmpg123-0.dll"
		"${CMAKE_CURRENT_LIST_DIR}/lib/x86/libogg-0.dll"
		"${CMAKE_CURRENT_LIST_DIR}/lib/x86/libopus-0.dll"
		"${CMAKE_CURRENT_LIST_DIR}/lib/x86/libopusfile-0.dll"
		"${CMAKE_CURRENT_LIST_DIR}/lib/x86/libvorbis-0.dll"
		"${CMAKE_CURRENT_LIST_DIR}/lib/x86/libvorbisfile-3.dll"
		"${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2_mixer.dll")
endif ()

string(STRIP "${SDL2_MIXER_LIBRARIES}" SDL2_MIXER_LIBRARIES)
