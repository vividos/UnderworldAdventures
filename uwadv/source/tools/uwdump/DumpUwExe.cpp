//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2003,2019 Underworld Adventures Team
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
/// \file DumpUwExe.cpp
/// \brief builtin models dumping implementation
//
#include "common.hpp"
#include "Model3D.hpp"
#include "FileSystem.hpp"

class GameStrings;

extern bool DecodeBuiltInModels(const char* filename,
   std::vector<Model3DPtr>& allModels, bool dump = false, bool isUw2 = false);

extern void DumpExecutable(const std::string& filename, const GameStrings& gameStrings, bool isUw2);

void DumpUwExe(const std::string& filename, const GameStrings& gameStrings, bool isUw2)
{
   DumpExecutable(filename, gameStrings, isUw2);

   printf("3D models dumping\n");

   std::vector<Model3DPtr> allModels;
   DecodeBuiltInModels(filename.c_str(), allModels, true, isUw2);
}
