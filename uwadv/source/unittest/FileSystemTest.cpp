//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019,2021 Underworld Adventures Team
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
#include "File.hpp"

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
      /// Base::FileSystem::FindFiles(). Empty folder case.
      TEST_METHOD(TestListFiles_EmptyFolder)
      {
         // set up
         TempFolder testFolder;
         std::string path = testFolder.GetPathName();

         std::vector<std::string> fileList;

         // run
         try
         {
            Base::FileSystem::FindFiles(path + "/*.*", fileList, false);
         }
         catch (const Base::FileSystemException& ex)
         {
            ex;
            Assert::Fail();
         }

         // check
         // note: folder list must not contain "." or ".."
         Assert::IsTrue(fileList.empty());
      }

      /// Tests getting a list of files in a folder, using
      /// Base::FileSystem::FindFiles(). Single file in folder
      TEST_METHOD(TestListFiles_FileInFolder)
      {
         // set up
         TempFolder testFolder;
         std::string path = testFolder.GetPathName() + "/";

         Base::File testFile{ path + "testfile.bin", Base::modeWrite };
         Assert::IsTrue(true == testFile.IsOpen());

         testFile.Write8(0x42);
         testFile.Close();

         // run
         std::vector<std::string> fileList;

         try
         {
            Base::FileSystem::FindFiles(path + "*.bin", fileList, false);
         }
         catch (const Base::FileSystemException& ex)
         {
            std::wstring message{ L"exception: " };
            std::string text = ex.what();
            message += std::wstring(text.begin(), text.end());
            Assert::Fail(message.c_str());
         }

         // test list with files
         Assert::IsFalse(fileList.empty());
         Assert::IsTrue(fileList.front().find("testfile.bin") != -1);
      }
   };
} // namespace UnitTest
