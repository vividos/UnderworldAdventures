/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2006,2019 Michael Fink

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
/*! \file cfgfiletest.cpp

   \brief ConfigFile class test

*/

// needed includes
#include "unittest.hpp"
#include "cfgfile.hpp"
#include "textfile.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{

//! ConfigFile test
/*! Tests loading and rewriting config files via ConfigFile. */
TEST_CLASS(TestConfigFile)
{
/*! Tests loading a config file. */
TEST_METHOD(TestLoadFile)
{
   TempFolder testFolder;
   std::string strPath = testFolder.GetPathName();

   std::string strFilename = strPath + "/test.cfg";
   {
      Base::TextFile file(strFilename, Base::modeWrite);
      file.WriteLine("# comment line");
      file.WriteLine("  # another comment  ");
      file.WriteLine("; alternative comment");
      file.WriteLine("   "); // empty line
      file.WriteLine("key1 value1");
      file.WriteLine("key2  \t  \t  value2");
      file.WriteLine("    key3 value 3    ");

      file.Close();
   }

   Base::ConfigFile cfgFile;
   cfgFile.Load(strFilename);

   Base::ConfigValueMap& cfgMap = cfgFile.GetValueMap();

   Assert::IsTrue(cfgMap["key1"] == "value1");
   Assert::IsTrue(cfgMap["key2"] == "value2");
   Assert::IsTrue(cfgMap["key3"] == "value 3");
}

/*! Tests saving a config file by rewriting a given config file.
    \todo check if lines are right-trimmed of spaces
*/
TEST_METHOD(TestRewriteFile)
{
   TempFolder testFolder;
   std::string strPath = testFolder.GetPathName();

   std::string strFilename = strPath + "/test.cfg";
   std::string strFilenameNew = strPath + "/test2.cfg";
   {
      Base::TextFile file(strFilename, Base::modeWrite);
      file.WriteLine("  # comment line  "); // comment lines are trimmed for space only
      file.WriteLine("key1 value1"); // normal pair
      file.WriteLine("   key2 \t  \t value2  "); // pair with whitespace in between

      file.Close();
   }

   // load, modify, save
   {
      Base::ConfigFile cfgFile;
      cfgFile.Load(strFilename);

      Base::ConfigValueMap& cfgMap = cfgFile.GetValueMap();
      cfgMap["key1"] = "uwadv1";
      cfgMap["key2"] = "uwadv2";

      cfgFile.Save(strFilename, strFilenameNew);
   }

   // check resulting text file
   {
      Base::TextFile textFile(strFilenameNew, Base::modeRead);

      Assert::IsTrue(true == textFile.IsOpen());

      std::string strLine;

      textFile.ReadLine(strLine);
// TODO right trimming isn't done yet      Assert::IsTrue(strLine == "# comment line");
      Assert::IsTrue(strLine == "# comment line  ");

      textFile.ReadLine(strLine);
      Assert::IsTrue(strLine == "key1 uwadv1");

      textFile.ReadLine(strLine);
      Assert::IsTrue(strLine == "key2 uwadv2");
   }
}
};
} // namespace UnitTest
