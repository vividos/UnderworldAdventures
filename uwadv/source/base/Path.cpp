//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2022 Underworld Adventures Team
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
/// \file Path.cpp
/// \brief Path class
//
#include "pch.hpp"
#include "Path.hpp"
#include "FileSystem.hpp"

using Base::Path;

std::string Path::Directory(const std::string& path)
{
   size_t pos = path.find_last_of(
      Base::FileSystem::PathSeparator[0]);

   std::string filename = pos == std::string::npos
      ? std::string()
      : path.substr(0, pos + 1);

   return filename;
}

std::string Path::FilenameAndExt(const std::string& path)
{
   size_t pos = path.find_last_of(
      Base::FileSystem::PathSeparator[0]);

   std::string filename = pos == std::string::npos
      ? path
      : path.substr(pos + 1);

   return filename;
}

std::string Path::FilenameOnly(const std::string& path)
{
   std::string filename = Path::FilenameAndExt(path);

   size_t pos = filename.find_last_of('.');
   std::string filenameOnly = pos == std::string::npos
      ? filename
      : filename.substr(0, pos);

   return filenameOnly;
}

std::string Path::Extension(const std::string& path)
{
   size_t pos = path.find_last_of('.');
   std::string extension = pos == std::string::npos
      ? std::string()
      : path.substr(pos);

   return extension;
}

void Path::EnsureEndingSlash(std::string& path)
{
   if (path.find_last_of("\\/") != path.size() - 1)
      path.append(Base::FileSystem::PathSeparator);
}

std::string Path::Combine(const std::string& part1,
   const std::string& part2)
{
   std::string path = part1;

   if (!path.empty() && !part2.empty())
      EnsureEndingSlash(path);
   path.append(part2);

   return path;
}

std::string Path::Combine(const std::string& part1,
   const std::string& part2,
   const std::string& part3)
{
   std::string path = part1;

   if (!path.empty() && !part2.empty())
      EnsureEndingSlash(path);
   path.append(part2);

   if (!path.empty() && !part3.empty())
      EnsureEndingSlash(path);
   path.append(part3);

   return path;
}
