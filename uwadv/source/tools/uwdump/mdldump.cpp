//
// Underworld Adventures - an Ultima Underworld hacking project
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
/// \file mdldump.cpp
/// \brief builtin models dumping implementation
//
#include "common.hpp"
#include "mdldump.hpp"
#include "models.hpp"
#include "filesystem.hpp"

extern bool ua_model_decode_builtins(const char* filename,
   std::vector<ua_model3d_ptr>& allmodels, bool dump = false);

void ua_dump_builtin_models::start(std::string& basepath, bool isUw2)
{
   std::string exe_name(basepath);
   exe_name.append(isUw2 ? "uw2.exe" : "uw.exe");

   if (!isUw2 && !Base::FileSystem::FileExists(exe_name.c_str()))
   {
      // check for uw_demo
      exe_name.assign(basepath);
      exe_name.append("uwdemo.exe");
   }

   // decode and dump
   std::vector<ua_model3d_ptr> allmodels;
   ua_model_decode_builtins(exe_name.c_str(), allmodels, true);
}
