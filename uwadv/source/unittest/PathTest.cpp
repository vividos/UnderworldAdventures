//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2022 Underworld Adventures Team
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
/// \file PathTest.cpp
/// \brief Path class test
//
#include "pch.hpp"
#include "Path.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
   /// \brief Path static functions tests
   /// Tests path functions provided by Base::Path.
   TEST_CLASS(PathTest)
   {
      /// Tests getting directory using Path::Directory()
      TEST_METHOD(TestPathDirectory)
      {
         Assert::AreEqual(std::string{ "" }, Base::Path::Directory(""));
         Assert::AreEqual(std::string{ "dir\\" }, Base::Path::Directory("dir\\filename.ext"));
         Assert::AreEqual(std::string{ "dir\\dir2\\" }, Base::Path::Directory("dir\\dir2\\filename.ext"));
         Assert::AreEqual(std::string{ "" }, Base::Path::Directory("filename.ext"));
      }

      /// Tests getting filename using Path::FilenameAndExt()
      TEST_METHOD(TestPathFilenameAndExt)
      {
         Assert::AreEqual(std::string{ "" }, Base::Path::FilenameAndExt(""));
         Assert::AreEqual(std::string{ "filename.ext" }, Base::Path::FilenameAndExt("dir\\filename.ext"));
         Assert::AreEqual(std::string{ "filename.ext" }, Base::Path::FilenameAndExt("dir\\dir2\\filename.ext"));
         Assert::AreEqual(std::string{ "filename.ext" }, Base::Path::FilenameAndExt("filename.ext"));
         Assert::AreEqual(std::string{ "filename" }, Base::Path::FilenameAndExt("filename"));
         Assert::AreEqual(std::string{ ".ext" }, Base::Path::FilenameAndExt(".ext"));
      }

      /// Tests getting filename without extension using Path::FilenameOnly()
      TEST_METHOD(TestPathFilenameOnly)
      {
         Assert::AreEqual(std::string{ "" }, Base::Path::FilenameOnly(""));
         Assert::AreEqual(std::string{ "filename" }, Base::Path::FilenameOnly("dir\\filename.ext"));
         Assert::AreEqual(std::string{ "filename" }, Base::Path::FilenameOnly("dir\\dir2\\filename.ext"));
         Assert::AreEqual(std::string{ "filename" }, Base::Path::FilenameOnly("filename.ext"));
         Assert::AreEqual(std::string{ "filename" }, Base::Path::FilenameOnly("filename.ext"));
         Assert::AreEqual(std::string{ "filename" }, Base::Path::FilenameOnly("filename"));
         Assert::AreEqual(std::string{ "" }, Base::Path::FilenameOnly(".ext"));
      }

      /// Tests getting extension using Path::Extension()
      TEST_METHOD(TestPathExtension)
      {
         Assert::AreEqual(std::string{ "" }, Base::Path::Extension(""));
         Assert::AreEqual(std::string{ ".ext" }, Base::Path::Extension("dir\\filename.ext"));
         Assert::AreEqual(std::string{ ".ext" }, Base::Path::Extension("dir\\dir2\\filename.ext"));
         Assert::AreEqual(std::string{ ".ext" }, Base::Path::Extension("filename.ext"));
         Assert::AreEqual(std::string{ "" }, Base::Path::Extension("filename"));
         Assert::AreEqual(std::string{ ".ext" }, Base::Path::Extension(".ext"));
      }

      /// Tests ensuring path ending with a slash using Path::EnsureEndingSlash()
      TEST_METHOD(TestPathEnsureEndingSlash)
      {
         std::string path1{ "dir\\dir2" };
         std::string path2{ "dir\\dir2\\" };

         Base::Path::EnsureEndingSlash(path1);
         Base::Path::EnsureEndingSlash(path2);

         Assert::AreEqual(std::string{ "dir\\dir2\\" }, path1);
         Assert::AreEqual(std::string{ "dir\\dir2\\" }, path2);
      }

      /// Tests combining paths using Path::Combine()
      TEST_METHOD(TestPathCombine)
      {
         std::string path1 = Base::Path::Combine("dir", "dir2");
         std::string path2 = Base::Path::Combine("dir", "dir2", "dir3");
         std::string path3 = Base::Path::Combine("", "");
         std::string path4 = Base::Path::Combine("dir", "");
         std::string path5 = Base::Path::Combine("", "dir");
         std::string path6 = Base::Path::Combine("dir", "", "");
         std::string path7 = Base::Path::Combine("", "dir", "");
         std::string path8 = Base::Path::Combine("", "", "dir");

         Assert::AreEqual(std::string{ "dir\\dir2" }, path1);
         Assert::AreEqual(std::string{ "dir\\dir2\\dir3" }, path2);
         Assert::AreEqual(std::string{ "" }, path3);
         Assert::AreEqual(std::string{ "dir" }, path4);
         Assert::AreEqual(std::string{ "dir" }, path5);
         Assert::AreEqual(std::string{ "dir" }, path6);
         Assert::AreEqual(std::string{ "dir" }, path7);
         Assert::AreEqual(std::string{ "dir" }, path8);
      }
   };
} // namespace UnitTest
