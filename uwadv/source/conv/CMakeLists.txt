#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2021 Underworld Adventures Team
#
# CMakeList.txt: CMake project for conv.
#
cmake_minimum_required(VERSION 3.8)

project(conv)

add_library(${PROJECT_NAME} STATIC
	"pch.cpp" "pch.hpp"
	"CodeGraph.cpp" "CodeGraph.hpp"
	"CodeVM.cpp" "CodeVM.hpp"
	"Conversation.cpp" "Conversation.hpp"
	"ConversationDebugger.hpp"
	"ConvStack.hpp"
	"Opcodes.hpp")

target_include_directories(${PROJECT_NAME}
	PUBLIC "${PROJECT_SOURCE_DIR}"
	PRIVATE
		"${PROJECT_SOURCE_DIR}/../underworld")

target_link_libraries(${PROJECT_NAME} base underworld)
