/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2002,2003,2004,2005,2006 Michael Fink

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file filesystem.hpp

   \brief file system operations

*/

// include guard
#ifndef uwadv_base_filesystem_hpp_
#define uwadv_base_filesystem_hpp_

#include <string>
#include <vector>

namespace Base
{

//! \brief file system functions \ingroup base
namespace FileSystem
{
   //! returns a file list (full paths) from given search path with wildcards
   void FindFiles(const std::string& strSearchPath, std::vector<std::string>& vecFileList);

   //! removes a file from disk
   void RemoveFile(const std::string& strFilename);

   //! creates folder; creates necessary parent folders if needed
   void MakeFolder(const std::string& strFolderName);
   //! removes a folder; must be empty
   void RemoveFolder(const std::string& strFolderName);

   //! checks if given filename represents a file and exists
   bool FileExists(const std::string& strFilename);
   //! checks if given filename represents a folder and exists
   bool FolderExists(const std::string& strFolderName);

   //! recursively removes all files and subfolders of a folder
   void RecursiveRemoveFolder(const std::string& strPathname);
};

} // namespace Base

#endif
