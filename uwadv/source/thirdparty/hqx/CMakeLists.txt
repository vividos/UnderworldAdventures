#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2021 Underworld Adventures Team
#
# CMakeList.txt: CMake project for hqx.
#
cmake_minimum_required(VERSION 3.8)

project(hqx)

add_library(${PROJECT_NAME} STATIC
	"src/common.h"
	"src/hq2x.c"
	"src/hq3x.c"
	"src/hq4x.c"
	"src/hqx.h"
	"src/init.c")

# Compiler options to enable static linking
target_compile_options(${PROJECT_NAME}
	PUBLIC "/U_DLL"
	PRIVATE "/D_WIN32"
	PRIVATE "/D_LIB"
)

target_include_directories(${PROJECT_NAME}
	PUBLIC "${PROJECT_SOURCE_DIR}/src")
