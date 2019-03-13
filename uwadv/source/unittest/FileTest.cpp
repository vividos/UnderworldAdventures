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
/// \file FileTest.cpp
/// \brief File and TextFile test
//
#include "UnitTest.hpp"
#include "File.hpp"
#include "TextFile.hpp"
#include <SDL_rwops.h>
#include "SDL_rwops_gzfile.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
   /// \brief File and TextFile class tests
   /// Tests reading and writing files using File and TextFile. Additionally
   /// reading from and writing to gzip-compressed streams is tested.
   TEST_CLASS(TestFile)
   {
      /// Tests writing and reading files via Base::File.
      TEST_METHOD(TestFileReadWrite)
      {
         TempFolder testFolder;
         std::string path = testFolder.GetPathName() + "/testfile.bin";

         Uint8 testData[] = { 0x00, 0x42, 0xab, 0x54, 0x12, 0x68, 0xff, 0xfe, 0x80 };

         // write test file
         {
            Base::File testFile(path, Base::modeWrite);
            Assert::IsTrue(true == testFile.IsOpen());

            Assert::IsTrue(0 == testFile.FileLength());
            Assert::IsTrue(0 == testFile.Tell());

            testFile.Write8(0x42);
            testFile.Write16(0xfffe);
            testFile.Write32(0x12345678);
            Assert::IsTrue(1 + 2 + 4 == testFile.Tell());

            testFile.WriteBuffer(testData, SDL_TABLESIZE(testData));
            Assert::IsTrue(1 + 2 + 4 + SDL_TABLESIZE(testData) == testFile.Tell());
         }

         // read test file
         {
            Base::File testFile(path, Base::modeRead);
            Assert::IsTrue(true == testFile.IsOpen());

            Assert::IsTrue(1 + 2 + 4 + SDL_TABLESIZE(testData) == testFile.FileLength());
            Assert::IsTrue(0 == testFile.Tell());

            Assert::IsTrue(0x42 == testFile.Read8());
            Assert::IsTrue(0xfffe == testFile.Read16());
            Assert::IsTrue(0x12345678 == testFile.Read32());
            Assert::IsTrue(1 + 2 + 4 == testFile.Tell());

            Uint8 testReadData[SDL_TABLESIZE(testData)];
            testFile.ReadBuffer(testReadData, SDL_TABLESIZE(testReadData));
            Assert::IsTrue(1 + 2 + 4 + SDL_TABLESIZE(testData) == testFile.Tell());

            Assert::IsTrue(0 == memcmp(testData, testReadData, SDL_TABLESIZE(testReadData)));
         }
      }

      /// Tests writing and reading of text files via Base::TextFile.
      TEST_METHOD(TestTextFileReadWrite)
      {
         TempFolder testFolder;
         std::string strTextFile = testFolder.GetPathName() + "/testfile.txt";

         std::string c_strLine1("");
         std::string c_strLine2("");
         std::string c_strLine3("");

         // write test text file
         {
            Base::TextFile testFile(strTextFile, Base::modeWrite);
            Assert::IsTrue(true == testFile.IsOpen());

            testFile.WriteLine(c_strLine1);
            testFile.WriteLine(c_strLine2);
            testFile.WriteLine(c_strLine3);
         }

         // read test text file
         {
            Base::TextFile testFile(strTextFile, Base::modeRead);
            Assert::IsTrue(true == testFile.IsOpen());

            Assert::IsTrue(testFile.FileLength() > 0);
            Assert::IsTrue(0 == testFile.Tell());

            std::string strLine1, strLine2, strLine3;

            testFile.ReadLine(strLine1);
            Assert::IsTrue(strLine1 == c_strLine1);

            testFile.ReadLine(strLine2);
            Assert::IsTrue(strLine2 == c_strLine2);

            testFile.ReadLine(strLine3);
            Assert::IsTrue(strLine3 == c_strLine3);
         }
      }

      /// Tests reading from SDL_RWops* constructed File class.
      TEST_METHOD(TestRwopsFileRead)
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
            Base::SDL_RWopsPtr rwops = Base::MakeRWopsPtr(SDL_RWFromConstMem(testData, SDL_TABLESIZE(testData)));
            Assert::IsTrue(rwops.get() != NULL);

            Base::File testFile(rwops);
            Assert::IsTrue(true == testFile.IsOpen());

            Assert::IsTrue(SDL_TABLESIZE(testData) == testFile.FileLength());
            Assert::IsTrue(0 == testFile.Tell());

            Assert::IsTrue(0x42 == testFile.Read8());
            Assert::IsTrue(0xfffe == testFile.Read16());
            Assert::IsTrue(0x12345678 == testFile.Read32());
            Assert::IsTrue(1 + 2 + 4 == testFile.Tell());

            Uint8 testReadData[SDL_TABLESIZE(testData)];
            testFile.ReadBuffer(testReadData, SDL_TABLESIZE(testReadData));
            Assert::IsTrue(SDL_TABLESIZE(testData) == testFile.Tell());

            Assert::IsTrue(0 == memcmp(testData + 1 + 2 + 4, testReadData, SDL_TABLESIZE(testReadData)));
         }
      }

      /// Tests writing and reading gzip-compressed files via Base::File.
      TEST_METHOD(TestGzipFileReadWrite)
      {
         TempFolder testFolder;
         std::string path = testFolder.GetPathName() + "/testfile.gz";

         Uint8 testData[] = { 0x00, 0x42, 0xab, 0x54, 0x12, 0x68, 0xff, 0xfe, 0x80 };

         // write test file
         {
            Base::SDL_RWopsPtr rwops = Base::MakeRWopsPtr(SDL_RWFromGzFile(path.c_str(), "wb9"));
            Assert::IsTrue(rwops.get() != NULL);

            Base::File testFile(rwops);
            Assert::IsTrue(true == testFile.IsOpen());

            Assert::IsTrue(-1 == testFile.FileLength());
            Assert::IsTrue(0 == testFile.Tell());

            testFile.Write8(0x42);
            testFile.Write16(0xfffe);
            testFile.Write32(0x12345678);
            Assert::IsTrue(1 + 2 + 4 == testFile.Tell());

            testFile.WriteBuffer(testData, SDL_TABLESIZE(testData));
            Assert::IsTrue(1 + 2 + 4 + SDL_TABLESIZE(testData) == testFile.Tell());
         }

         // read test file
         {
            Base::SDL_RWopsPtr rwops = Base::MakeRWopsPtr(SDL_RWFromGzFile(path.c_str(), "rb"));
            Assert::IsTrue(rwops.get() != NULL);

            Base::File testFile(rwops);
            Assert::IsTrue(true == testFile.IsOpen());

            /// file length isn't known for gz-streams
            Assert::IsTrue(-1 == testFile.FileLength());
            Assert::IsTrue(0 == testFile.Tell());

            Assert::IsTrue(0x42 == testFile.Read8());
            Assert::IsTrue(0xfffe == testFile.Read16());
            Assert::IsTrue(0x12345678 == testFile.Read32());
            Assert::IsTrue(1 + 2 + 4 == testFile.Tell());

            Uint8 testReadData[SDL_TABLESIZE(testData)];
            testFile.ReadBuffer(testReadData, SDL_TABLESIZE(testReadData));
            Assert::IsTrue(1 + 2 + 4 + SDL_TABLESIZE(testData) == testFile.Tell());

            Assert::IsTrue(0 == memcmp(testData, testReadData, SDL_TABLESIZE(testReadData)));
         }
      }
   };
} // namespace UnitTest
