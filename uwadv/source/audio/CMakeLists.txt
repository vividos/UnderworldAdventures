#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2021 Underworld Adventures Team
#
# CMakeList.txt: CMake project for audio.
#
cmake_minimum_required(VERSION 3.8)

project(audio)

# Confiure SDL2_mixer
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../thirdparty/SDL2_mixer-2.0.4)
find_package(SDL2_mixer REQUIRED)

add_library(${PROJECT_NAME} STATIC
	"pch.cpp" "pch.hpp"
	"Audio.cpp" "Audio.hpp"
	"IODataSource.hpp"
	"MidiPlayer.cpp" "MidiPlayer.hpp"
	"Playlist.cpp" "Playlist.hpp"
	"resamp.c" "resamp.h"
	"VoiceFile.cpp" "VoiceFile.hpp"
	"VoiceResample.cpp"
	"midi/array_size.h"
	"midi/common_types.h"
	"midi/CoreAudioMidiDriver.cpp" "midi/CoreAudioMidiDriver.h"
	"midi/IDataSource.h"
	"midi/LowLevelMidiDriver.cpp" "midi/LowLevelMidiDriver.h"
	"midi/MidiDriver.cpp" "midi/MidiDriver.h"
	"midi/ODataSource.h"
	"midi/pent_include.h"
	"midi/WindowsMidiDriver.cpp" "midi/WindowsMidiDriver.h"
	"midi/XMidiEvent.h"
	"midi/XMidiEventList.cpp" "midi/XMidiEventList.h"
	"midi/XMidiFile.cpp" "midi/XMidiFile.h"
	"midi/XMidiNoteStack.h"
	"midi/XMidiSequence.cpp" "midi/XMidiSequence.h"
	"midi/XMidiSequenceHandler.h")

target_include_directories(${PROJECT_NAME}
	PUBLIC "${PROJECT_SOURCE_DIR}"
	PRIVATE
		"${PROJECT_SOURCE_DIR}/../base"
		"${SDL2_MIXER_INCLUDE_DIRS}")

target_link_libraries(${PROJECT_NAME} base ${SDL2_MIXER_LIBRARIES})

if (WIN32)
	target_link_libraries(${PROJECT_NAME} winmm.lib)
endif(WIN32)

install(
	FILES ${SDL2_MIXER_RUNTIME_FILES}
	DESTINATION "uwadv")
