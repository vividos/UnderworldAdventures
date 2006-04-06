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
/*! \file archivefiletest.cpp

   \brief ArchiveFile test

*/

// needed includes
#include "unittest.hpp"
#include "archivefile.hpp"
#include "settings.hpp"
#include "resourcemanager.hpp"

namespace UnitTest
{

//! ArchiveFile class test
/*! Tests reading .ark files in uw1 and uw2 using ArchiveFile.
*/
class TestArchiveFile: public UnitTestCase
{
public:
   // generate test suite
   CPPUNIT_TEST_SUITE(TestArchiveFile)
      CPPUNIT_TEST(TestArchiveFileLoad)
   CPPUNIT_TEST_SUITE_END()

protected:
   void TestArchiveFileLoad();
};

// register test suite
CPPUNIT_TEST_SUITE_REGISTRATION(TestArchiveFile)

} // namespace UnitTest

// methods

using namespace UnitTest;

/*! Tests reading in all known .ark files.
    \todo check file lengths in uw1 mode, once file length is known (see
          ArchiveFile todo's).
    \todo decode what scd.ark contains
*/
void TestArchiveFile::TestArchiveFileLoad()
{
   Base::Settings settings;
   Base::LoadSettings(settings);

   Base::ResourceManager resourceMgr(settings);

   // load uw1 levels
   {
      Base::ArchiveFile file(resourceMgr.GetUnderworldFile(Base::resourceGameUw1, "data/lev.ark"));
      CPPUNIT_ASSERT(file.GetNumFiles() == 9*15); // 135 blocks
      
      // check if first 9x3 files are available
      for(unsigned int ui=0; ui<9*3; ui++)
         CPPUNIT_ASSERT(true == file.IsAvailable(ui));

      file.GetFile(1);
      // TODO: check file lengths
   }

   // load uw1 conversations
   {
      Base::ArchiveFile file(resourceMgr.GetUnderworldFile(Base::resourceGameUw1, "data/cnv.ark"));
      CPPUNIT_ASSERT(file.GetNumFiles() == 320); // 320 conversations

      // first conversation is unavailable, second isn't
      CPPUNIT_ASSERT(false == file.IsAvailable(0));
      CPPUNIT_ASSERT(true == file.IsAvailable(1));

      file.GetFile(1);
      // TODO: check file lengths
   }

   // load uw2 bitmaps
   {
      Base::ArchiveFile file(resourceMgr.GetUnderworldFile(Base::resourceGameUw2, "data/byt.ark"), true);
      CPPUNIT_ASSERT(file.GetNumFiles() == 11); // 11 bitmaps

      // all files are exactly 64000 bytes long (320x200 bitmap)
      unsigned int uiMax = file.GetNumFiles();
      for(unsigned int ui=0; ui<uiMax; ui++)
      if (ui != 3 && ui != 10) // entries 3 and 10 are unused
      {
         Base::File f = file.GetFile(ui);
         CPPUNIT_ASSERT(f.FileLength() == 64000);
      }
   }

   // load uw2 conversations
   {
      Base::ArchiveFile file(resourceMgr.GetUnderworldFile(Base::resourceGameUw2, "data/cnv.ark"), true);
      CPPUNIT_ASSERT(file.GetNumFiles() == 320); // 320 conversations

      CPPUNIT_ASSERT(false == file.IsAvailable(0));
      CPPUNIT_ASSERT(true == file.IsAvailable(1));

      // check some lengths of conversations
      Base::File f1 = file.GetFile(1);
      CPPUNIT_ASSERT(f1.FileLength() == 0x2a38);

      Base::File f2 = file.GetFile(2);
      CPPUNIT_ASSERT(f2.FileLength() == 0xe92);
   }

   // load uw2 levels
   {
      Base::ArchiveFile file(resourceMgr.GetUnderworldFile(Base::resourceGameUw2, "data/lev.ark"), true);
      CPPUNIT_ASSERT(file.GetNumFiles() == 80*4);

      // level tilemap and object list
      unsigned int ui;
      for(ui=0; ui<80; ui++)
      {
         // note: levels 73 to 79 are unavailable
         if (file.IsAvailable(ui))
         {
            Base::File f = file.GetFile(ui);
            CPPUNIT_ASSERT(f.FileLength() == 0x7e08);
         }
      }

      // texture mapping
      for(ui=80; ui<160; ui++)
      {
         // map for levels 73 to 79 is unavailable
         if (file.IsAvailable(ui))
         {
            Base::File f = file.GetFile(ui);
            CPPUNIT_ASSERT(f.FileLength() == 0x86);
         }
      }

      // automap entries
      for(ui=160; ui<240; ui++)
      {
         // automap block for most levels is not available
         if (file.IsAvailable(ui))
         {
            Base::File f = file.GetFile(ui);
            CPPUNIT_ASSERT(f.FileLength() == 0x1000);
         }
      }
   }

   // load uw2 ???
   {
      Base::ArchiveFile file(resourceMgr.GetUnderworldFile(Base::resourceGameUw2, "data/scd.ark"), true);
      CPPUNIT_ASSERT(file.GetNumFiles() == 16);
/*
      unsigned int uiMax = file.GetNumFiles();
      for(unsigned int ui=0; ui<uiMax; ui++)
      if (file.IsAvailable(ui))
      {
         Base::File f = file.GetFile(ui);
         UaTrace("scd entry %u is %u bytes long\n", ui, f.FileLength());
      }
      else
         UaTrace("scd entry %u is not available\n", ui);
*/
   }
}
