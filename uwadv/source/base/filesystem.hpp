//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Michael Fink
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
/// \file filesystem.hpp
/// \brief file system operations
//
#pragma once

#include <string>
#include <vector>

namespace Base
{
   /// \brief file system functions
   namespace FileSystem
   {
      /// returns a file list (full paths) from given search path with wildcards
      void FindFiles(const std::string& searchPath, std::vector<std::string>& fileList);

      /// removes a file from disk
      void RemoveFile(const std::string& filename);

      /// creates folder; creates necessary parent folders if needed
      void MakeFolder(const std::string& folderName);

      /// removes a folder; must be empty
      void RemoveFolder(const std::string& folderName);

      /// checks if given filename represents a file and exists
      bool FileExists(const std::string& filename);

      /// checks if given filename represents a folder and exists
      bool FolderExists(const std::string& folderName);

      /// recursively removes all files and subfolders of a folder
      void RecursiveRemoveFolder(const std::string& pathname);
   };

} // namespace Base
