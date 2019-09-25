//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019 Underworld Adventures Team
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
//
/// \file FileSystemTest.cpp
/// \brief FileSystem tests
//
#include "pch.hpp"
#include "FileSystem.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
   /// \brief FileSystem class tests
   /// Tests file system functions provided by Base::FileSystem.
   TEST_CLASS(FileSystemTest)
   {
      /// Tests creating and removing folders.
      TEST_METHOD(TestCreateRemoveFolder)
      {
         TempFolder testFolder;
         std::string path = testFolder.GetPathName();

         path += "/TestFolder";

         try
         {
            Base::FileSystem::MakeFolder(path);

            Assert::IsTrue(true == Base::FileSystem::FolderExists(path));
            Assert::IsTrue(false == Base::FileSystem::FileExists(path));

            Base::FileSystem::RemoveFolder(path);

            Assert::IsTrue(false == Base::FileSystem::FolderExists(path));
            Assert::IsTrue(false == Base::FileSystem::FileExists(path));
         }
         catch (const Base::FileSystemException& ex)
         {
            ex;
            Assert::Fail();
         }
      }

      /// Tests getting a list of files in a folder, using
      /// Base::FileSystem::FindFiles().
      /// \todo complete test
      TEST_METHOD(TestListFiles)
      {
         TempFolder testFolder;
         std::string path = testFolder.GetPathName();

         // test empty folder
         try
         {
            std::vector<std::string> vFileList;
            Base::FileSystem::FindFiles(path + "/*.*", vFileList);

            // note: folder list must not contain "." or ".."
            Assert::IsTrue(vFileList.size() == 0);
         }
         catch (const Base::FileSystemException& ex)
         {
            ex;
            Assert::Fail();
         }

         // test list with files
      }
   };
} // namespace UnitTest
