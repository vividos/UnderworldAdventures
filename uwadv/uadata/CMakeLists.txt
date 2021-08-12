#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2021 Underworld Adventures Team
#
# CMakeList.txt: CMake project for uadata.
#
cmake_minimum_required(VERSION 3.8)

project(uadata)

install(FILES
	"Underworld Adventures Soundtrack.m3u"
	"README.Data.md"
	DESTINATION "uwadv/data")
