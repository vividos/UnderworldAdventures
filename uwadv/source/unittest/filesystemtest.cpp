/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2006 Michael Fink

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
/*! \file filesystemtest.cpp

   \brief FileSystem test

*/

// needed includes
#include "unittest.hpp"
#include "filesystem.hpp"

namespace UnitTest
{

//! FileSystem class test
/*! Tests file system functions provided by Base::FileSystem. */
class TestFileSystem: public UnitTestCase
{
public:
   // generate test suite
   CPPUNIT_TEST_SUITE(TestFileSystem)
      CPPUNIT_TEST(TestCreateRemoveFolder)
      CPPUNIT_TEST(TestListFiles)
   CPPUNIT_TEST_SUITE_END()

protected:
   void TestCreateRemoveFolder();
   void TestListFiles();
};

// register test suite
CPPUNIT_TEST_SUITE_REGISTRATION(TestFileSystem)


} // namespace UnitTest

// methods

using namespace UnitTest;

/*! Tests creating and removing folders. */
void TestFileSystem::TestCreateRemoveFolder()
{
   TempFolder testFolder;
   std::string strPath = testFolder.GetPathName();

   strPath += "/TestFolder";

   try
   {
      Base::FileSystem::MakeFolder(strPath);

      CPPUNIT_ASSERT(true == Base::FileSystem::FolderExists(strPath));
      CPPUNIT_ASSERT(false == Base::FileSystem::FileExists(strPath));

      Base::FileSystem::RemoveFolder(strPath);

      CPPUNIT_ASSERT(false == Base::FileSystem::FolderExists(strPath));
      CPPUNIT_ASSERT(false == Base::FileSystem::FileExists(strPath));
   }
   catch(Base::FileSystemException& e)
   {
      e;
      CPPUNIT_ASSERT_FALSE();
   }
}

/*! Tests getting a list of files in a folder, using
    Base::FileSystem::FindFiles.
    \todo complete test
*/
void TestFileSystem::TestListFiles()
{
   TempFolder testFolder;
   std::string strPath = testFolder.GetPathName();

   // test empty folder
   try
   {
      std::vector<std::string> vFileList;
      Base::FileSystem::FindFiles(strPath + "/*.*", vFileList);

      // note: folder list must not contain "." or ".."
      CPPUNIT_ASSERT(vFileList.size() == 0);
   }
   catch(Base::FileSystemException& e)
   {
      e;
      CPPUNIT_ASSERT_FALSE();
   }

   // test list with files
}
