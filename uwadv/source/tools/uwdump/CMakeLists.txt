#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2021 Underworld Adventures Team
#
# CMakeList.txt: CMake project for uwdump.
#
cmake_minimum_required(VERSION 3.8)

project(uwdump)

add_executable(${PROJECT_NAME}
	"DumpArkArchive.cpp"
	"DumpConversationArchive.cpp"
	"DumpCombineData.cpp"
	"DumpExecutable.cpp"
	"DumpLevelArchive.cpp" "DumpLevelArchive.hpp"
	"DumpObjectProperties.cpp"
	"DumpUwExe.cpp"
	"UnderworldDumper.cpp" "UnderworldDumper.cpp"
	"uwdump.cpp")

target_include_directories(${PROJECT_NAME}
	PRIVATE
		"${PROJECT_SOURCE_DIR}/../import"
		"${PROJECT_SOURCE_DIR}/../renderer"
		"${PROJECT_SOURCE_DIR}/../underworld")

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} base import renderer underworld)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "uwadv/tools")
