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
/*! \file filetest.cpp

   \brief File and TextFile test

*/

// needed includes
#include "unittest.hpp"
#include "file.hpp"
#include "textfile.hpp"
#include <SDL_rwops.h>
#include "SDL_rwops_gzfile.h"

namespace UnitTest
{

//! File and TextFile class test
/*! Tests reading and writing files using File and TextFile. Additionally
    reading from and writing to gzip-compressed streams is tested.
*/
class TestFile: public UnitTestCase
{
public:
   // generate test suite
   CPPUNIT_TEST_SUITE(TestFile)
      CPPUNIT_TEST(TestFileReadWrite)
      CPPUNIT_TEST(TestTextFileReadWrite)
      CPPUNIT_TEST(TestRwopsFileRead)
      CPPUNIT_TEST(TestGzipFileReadWrite)
   CPPUNIT_TEST_SUITE_END()

protected:
   void TestFileReadWrite();
   void TestTextFileReadWrite();
   void TestRwopsFileRead();
   void TestGzipFileReadWrite();
};

// register test suite
CPPUNIT_TEST_SUITE_REGISTRATION(TestFile)

} // namespace UnitTest

// methods

using namespace UnitTest;

/*! Tests writing and reading files via Base::File. */
void TestFile::TestFileReadWrite()
{
   TempFolder testFolder;
   std::string strFile = testFolder.GetPathName() + "/testfile.bin";

   Uint8 testData[] = { 0x00, 0x42, 0xab, 0x54, 0x12, 0x68, 0xff, 0xfe, 0x80 };

   // write test file
   {
      Base::File testFile(strFile, Base::modeWrite);
      CPPUNIT_ASSERT(true == testFile.IsOpen());

      CPPUNIT_ASSERT(0 == testFile.FileLength());
      CPPUNIT_ASSERT(0 == testFile.Tell());

      testFile.Write8(0x42);
      testFile.Write16(0xfffe);
      testFile.Write32(0x12345678);
      CPPUNIT_ASSERT(1+2+4 == testFile.Tell());

      testFile.WriteBuffer(testData, SDL_TABLESIZE(testData));
      CPPUNIT_ASSERT(1+2+4 + SDL_TABLESIZE(testData) == testFile.Tell());
   }

   // read test file
   {
      Base::File testFile(strFile, Base::modeRead);
      CPPUNIT_ASSERT(true == testFile.IsOpen());

      CPPUNIT_ASSERT(1+2+4 + SDL_TABLESIZE(testData) == testFile.FileLength());
      CPPUNIT_ASSERT(0 == testFile.Tell());

      CPPUNIT_ASSERT(0x42 == testFile.Read8());
      CPPUNIT_ASSERT(0xfffe == testFile.Read16());
      CPPUNIT_ASSERT(0x12345678 == testFile.Read32());
      CPPUNIT_ASSERT(1+2+4 == testFile.Tell());

      Uint8 testReadData[SDL_TABLESIZE(testData)];
      testFile.ReadBuffer(testReadData, SDL_TABLESIZE(testReadData));
      CPPUNIT_ASSERT(1+2+4 + SDL_TABLESIZE(testData) == testFile.Tell());

      CPPUNIT_ASSERT(0 == memcmp(testData, testReadData, SDL_TABLESIZE(testReadData)));
   }
}

/*! Tests writing and reading of text files via Base::TextFile. */
void TestFile::TestTextFileReadWrite()
{
   TempFolder testFolder;
   std::string strTextFile = testFolder.GetPathName() + "/testfile.txt";

   std::string c_strLine1("");
   std::string c_strLine2("");
   std::string c_strLine3("");

   // write test text file
   {
      Base::TextFile testFile(strTextFile, Base::modeWrite);
      CPPUNIT_ASSERT(true == testFile.IsOpen());

      testFile.WriteLine(c_strLine1);
      testFile.WriteLine(c_strLine2);
      testFile.WriteLine(c_strLine3);
   }

   // read test text file
   {
      Base::TextFile testFile(strTextFile, Base::modeRead);
      CPPUNIT_ASSERT(true == testFile.IsOpen());

      CPPUNIT_ASSERT(testFile.FileLength() > 0);
      CPPUNIT_ASSERT(0 == testFile.Tell());

      std::string strLine1, strLine2, strLine3;

      testFile.ReadLine(strLine1);
      CPPUNIT_ASSERT(strLine1 == c_strLine1);

      testFile.ReadLine(strLine2);
      CPPUNIT_ASSERT(strLine2 == c_strLine2);

      testFile.ReadLine(strLine3);
      CPPUNIT_ASSERT(strLine3 == c_strLine3);
   }
}

/*! Tests reading from SDL_RWops* constructed File class. */
void TestFile::TestRwopsFileRead()
{
   Uint8 testData[] =
   {
      0x42,
      0xfe, 0xff, // always stored in little-endian format
      0x78, 0x56, 0x34, 0x12, // always stored in little-endian format also
      0x00, 0x42, 0xab, 0x54, 0x12, 0x68, 0xff, 0xfe, 0x80
   };

   // read test file
   {
      Base::SDL_RWopsPtr rwops = Base::SDL_RWopsPtr(SDL_RWFromConstMem(testData, SDL_TABLESIZE(testData)));
      CPPUNIT_ASSERT(rwops.get() != NULL);

      Base::File testFile(rwops);
      CPPUNIT_ASSERT(true == testFile.IsOpen());

      CPPUNIT_ASSERT(SDL_TABLESIZE(testData) == testFile.FileLength());
      CPPUNIT_ASSERT(0 == testFile.Tell());

      CPPUNIT_ASSERT(0x42 == testFile.Read8());
      CPPUNIT_ASSERT(0xfffe == testFile.Read16());
      CPPUNIT_ASSERT(0x12345678 == testFile.Read32());
      CPPUNIT_ASSERT(1+2+4 == testFile.Tell());

      Uint8 testReadData[SDL_TABLESIZE(testData)];
      testFile.ReadBuffer(testReadData, SDL_TABLESIZE(testReadData));
      CPPUNIT_ASSERT(SDL_TABLESIZE(testData) == testFile.Tell());

      CPPUNIT_ASSERT(0 == memcmp(testData+1+2+4, testReadData, SDL_TABLESIZE(testReadData)));
   }
}

/*! Tests writing and reading gzip-compressed files via Base::File. */
void TestFile::TestGzipFileReadWrite()
{
   TempFolder testFolder;
   std::string strFile = testFolder.GetPathName() + "/testfile.gz";

   Uint8 testData[] = { 0x00, 0x42, 0xab, 0x54, 0x12, 0x68, 0xff, 0xfe, 0x80 };

   // write test file
   {
      Base::SDL_RWopsPtr rwops = Base::SDL_RWopsPtr(SDL_RWFromGzFile(strFile.c_str(), "wb9"));
      CPPUNIT_ASSERT(rwops.get() != NULL);

      Base::File testFile(rwops);
      CPPUNIT_ASSERT(true == testFile.IsOpen());

      CPPUNIT_ASSERT(-1 == testFile.FileLength());
      CPPUNIT_ASSERT(0 == testFile.Tell());

      testFile.Write8(0x42);
      testFile.Write16(0xfffe);
      testFile.Write32(0x12345678);
      CPPUNIT_ASSERT(1+2+4 == testFile.Tell());

      testFile.WriteBuffer(testData, SDL_TABLESIZE(testData));
      CPPUNIT_ASSERT(1+2+4 + SDL_TABLESIZE(testData) == testFile.Tell());
   }

   // read test file
   {
      Base::SDL_RWopsPtr rwops = Base::SDL_RWopsPtr(SDL_RWFromGzFile(strFile.c_str(), "rb"));
      CPPUNIT_ASSERT(rwops.get() != NULL);

      Base::File testFile(rwops);
      CPPUNIT_ASSERT(true == testFile.IsOpen());

      /// file length isn't known for gz-streams
      CPPUNIT_ASSERT(-1 == testFile.FileLength());
      CPPUNIT_ASSERT(0 == testFile.Tell());

      CPPUNIT_ASSERT(0x42 == testFile.Read8());
      CPPUNIT_ASSERT(0xfffe == testFile.Read16());
      CPPUNIT_ASSERT(0x12345678 == testFile.Read32());
      CPPUNIT_ASSERT(1+2+4 == testFile.Tell());

      Uint8 testReadData[SDL_TABLESIZE(testData)];
      testFile.ReadBuffer(testReadData, SDL_TABLESIZE(testReadData));
      CPPUNIT_ASSERT(1+2+4 + SDL_TABLESIZE(testData) == testFile.Tell());

      CPPUNIT_ASSERT(0 == memcmp(testData, testReadData, SDL_TABLESIZE(testReadData)));
   }
}
