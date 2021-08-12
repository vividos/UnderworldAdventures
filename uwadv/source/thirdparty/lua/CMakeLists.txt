#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2021 Underworld Adventures Team
#
# CMakeList.txt: CMake project for lua.
#
cmake_minimum_required(VERSION 3.8)

project(lua)

add_library(${PROJECT_NAME} STATIC
	"src/lapi.c" "src/lapi.h"
	"src/lauxlib.c" "src/lauxlib.h"
	"src/lbaselib.c"
	"src/lcode.c" "src/lcode.h"
	"src/lcorolib.c"
	"src/lctype.c" "src/lctype.h"
	"src/ldblib.c"
	"src/ldebug.c" "src/ldebug.h"
	"src/ldo.c" "src/ldo.h"
	"src/ldump.c"
	"src/lfunc.c" "src/lfunc.h"
	"src/lgc.c" "src/lgc.h"
	"src/linit.c"
	"src/liolib.c"
	"src/ljumptab.h"
	"src/llex.c" "src/llex.h"
	"src/llimits.h"
	"src/lmathlib.c"
	"src/lmem.c" "src/lmem.h"
	"src/loadlib.c"
	"src/lobject.c" "src/lobject.h"
	"src/lopcodes.c" "src/lopcodes.h"
	"src/lopnames.h"
	"src/loslib.c"
	"src/lparser.c" "src/lparser.h"
	"src/lprefix.h"
	"src/lstate.c" "src/lstate.h"
	"src/lstring.c" "src/lstring.h"
	"src/lstrlib.c"
	"src/ltable.c" "src/ltable.h"
	"src/ltablib.c"
	"src/ltm.c" "src/ltm.h"
	"src/lua.c" "src/lua.h" "src/lua.hpp"
	"src/luaconf.h"
	"src/lualib.h"
	"src/lundump.c" "src/lundump.h"
	"src/lutf8lib.c"
	"src/lvm.c" "src/lvm.h"
	"src/lzio.c" "src/lzio.h")

target_include_directories(${PROJECT_NAME}
	PUBLIC "${PROJECT_SOURCE_DIR}/src")

# Also build luac in this project.
add_executable(luac "src/luac.c")

target_link_libraries(luac ${PROJECT_NAME})
