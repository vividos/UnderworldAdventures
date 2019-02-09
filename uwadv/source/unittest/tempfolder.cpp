//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019 Michael Fink
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
/// \file tempfolder.cpp
/// \brief unit test temp folder implementation
//
#include "unittest.hpp"
#include "filesystem.hpp"
#include <sstream>
#include <iomanip>

using UnitTest::TempFolder;

/// temp base folder
const char* c_cstrTempFolder = ".";

TempFolder::TempFolder()
{
   std::string strCurrentFolder(c_cstrTempFolder);

   // find folder name
   for (int iNum = 0;; iNum++)
   {
      std::stringstream buffer;
      buffer << strCurrentFolder << "/temp-unittest/dir-" <<
         std::setfill('0') << std::setw(4) << iNum;

      m_strPathname = buffer.str();

      if (!Base::FileSystem::FolderExists(m_strPathname))
         break; // folder doesn't exist
   }

   Base::FileSystem::MakeFolder(m_strPathname);
}

TempFolder::~TempFolder()
{
   // remove folder contents and folder
   Base::FileSystem::RecursiveRemoveFolder(m_strPathname);

   // try to delete base folder, too
   try
   {
      std::string strTempFolder(c_cstrTempFolder);
      strTempFolder += "temp-unittest";

      Base::FileSystem::RemoveFolder(strTempFolder);
   }
   catch (const Base::FileSystemException& ex)
   {
      ex;
   }
}
