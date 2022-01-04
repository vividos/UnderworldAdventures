//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2019-2022 Underworld Adventures Team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file version.hpp
/// \brief version number
//
#pragma once

#define MAJOR_VERSION 0
#define MINOR_VERSION 11
#define RELEASE_NUMBER 0
#define BUILD_NUMBER 0
#define BUILD_YEAR 2022
#define CODENAME ""

#define STRINGIFY(s) STRINGIFY2(s)
#define STRINGIFY2(s) #s

#define COPYRIGHT_TEXT "Copyright © 2002-" STRINGIFY(BUILD_YEAR) " Underworld Adventures Team"

#define VERSION_TEXT STRINGIFY(MAJOR_VERSION) "." STRINGIFY(MINOR_VERSION) "-" CODENAME
