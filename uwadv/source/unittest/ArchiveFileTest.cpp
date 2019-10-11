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
/// \file ArchiveFileTest.cpp
/// \brief ArchiveFile test
//
#include "pch.hpp"
#include "ArchiveFile.hpp"
#include "Settings.hpp"
#include "ResourceManager.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{

   /// Tests reading .ark files in uw1 and uw2 using ArchiveFile.
   /// \todo check file lengths in uw1 mode, once file length is known (see
   ///       ArchiveFile todo's).
   /// \todo decode what scd.ark contains
   TEST_CLASS(ArchiveFileTest)
   {
      /// load uw1 levels
      TEST_METHOD(TestArchiveFileLoadLevelsUw1)
      {
         Base::Settings& settings = GetTestSettings();
         Base::ResourceManager resourceMgr(settings);

         Base::ArchiveFile file(resourceMgr.GetUnderworldFile(Base::resourceGameUw1, "data/lev.ark"));
         Assert::IsTrue(file.GetNumFiles() == 9 * 15); // 135 blocks

         // check if first 9x3 files are available
         for (unsigned int index = 0; index < 9 * 3; index++)
            Assert::IsTrue(true == file.IsAvailable(index));

         file.GetFile(1);
         // TODO: check file lengths
      }

      /// load uw1 conversations
      TEST_METHOD(TestArchiveFileLoadConversationsUw1)
      {
         Base::Settings& settings = GetTestSettings();
         Base::ResourceManager resourceMgr(settings);

         Base::ArchiveFile file(resourceMgr.GetUnderworldFile(Base::resourceGameUw1, "data/cnv.ark"));
         Assert::IsTrue(file.GetNumFiles() == 320); // 320 conversations

         // first conversation is unavailable, second isn't
         Assert::IsTrue(false == file.IsAvailable(0));
         Assert::IsTrue(true == file.IsAvailable(1));

         file.GetFile(1);
         // TODO: check file lengths
      }

      /// load uw2 bitmaps
      TEST_METHOD(TestArchiveFileLoadBitmapsUw2)
      {
         Base::Settings& settings = GetTestSettings();
         Base::ResourceManager resourceMgr(settings);

         Base::ArchiveFile file(resourceMgr.GetUnderworldFile(Base::resourceGameUw2, "data/byt.ark"), true);
         Assert::IsTrue(file.GetNumFiles() == 11); // 11 bitmaps

         // all files are exactly 64000 bytes long (320x200 bitmap)
         size_t maxFiles = file.GetNumFiles();
         for (size_t index = 0; index < maxFiles; index++)
            if (index != 3 && index != 10) // entries 3 and 10 are unused
            {
               Base::File f = file.GetFile(index);
               Assert::IsTrue(f.FileLength() == 64000);
            }
      }

      /// load uw2 conversations
      TEST_METHOD(TestArchiveFileLoadConversationsUw2)
      {
         Base::Settings& settings = GetTestSettings();
         Base::ResourceManager resourceMgr(settings);

         Base::ArchiveFile file(resourceMgr.GetUnderworldFile(Base::resourceGameUw2, "data/cnv.ark"), true);
         Assert::IsTrue(file.GetNumFiles() == 320); // 320 conversations

         Assert::IsTrue(false == file.IsAvailable(0));
         Assert::IsTrue(true == file.IsAvailable(1));

         // check some lengths of conversations
         Base::File f1 = file.GetFile(1);
         Assert::IsTrue(f1.FileLength() == 0x2a38);

         Base::File f2 = file.GetFile(2);
         Assert::IsTrue(f2.FileLength() == 0xe92);
      }

      /// load uw2 levels
      TEST_METHOD(TestArchiveFileLoadLevelsUw2)
      {
         Base::Settings& settings = GetTestSettings();
         Base::ResourceManager resourceMgr(settings);

         Base::ArchiveFile file(resourceMgr.GetUnderworldFile(Base::resourceGameUw2, "data/lev.ark"), true);
         Assert::IsTrue(file.GetNumFiles() == 80 * 4);

         // level tilemap and object list
         unsigned int index;
         for (index = 0; index < 80; index++)
         {
            // note: levels 73 to 79 are unavailable
            if (file.IsAvailable(index))
            {
               Base::File f = file.GetFile(index);
               Assert::IsTrue(f.FileLength() == 0x7e08);
            }
         }

         // texture mapping
         for (index = 80; index < 160; index++)
         {
            // map for levels 73 to 79 is unavailable
            if (file.IsAvailable(index))
            {
               Base::File f = file.GetFile(index);
               Assert::IsTrue(f.FileLength() == 0x86);
            }
         }

         // automap entries
         for (index = 160; index < 240; index++)
         {
            // automap block for most levels is not available
            if (file.IsAvailable(index))
            {
               Base::File f = file.GetFile(index);
               Assert::IsTrue(f.FileLength() == 0x1000);
            }
         }
      }

      /// load uw2 sdc.ark
      TEST_METHOD(TestArchiveFileLoadSdcArkUw2)
      {
         Base::Settings& settings = GetTestSettings();
         Base::ResourceManager resourceMgr(settings);

         Base::ArchiveFile file(resourceMgr.GetUnderworldFile(Base::resourceGameUw2, "data/scd.ark"), true);
         Assert::IsTrue(file.GetNumFiles() == 16);

         size_t maxFiles = file.GetNumFiles();
         for (size_t index = 0; index < maxFiles; index++)
            if (file.IsAvailable(index))
            {
               Base::File f = file.GetFile(index);
               UaTrace("scd entry %u is %u bytes long\n", index, f.FileLength());
            }
            else
               UaTrace("scd entry %u is not available\n", index);
      }
   };
} // namespace UnitTest
