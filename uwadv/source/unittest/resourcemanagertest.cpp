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
/// \file resourcemanagertest.cpp
/// \brief ResourceManager test
//
#include "unittest.hpp"
#include "ResourceManager.hpp"
#include "Settings.hpp"
#include "File.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
   /// \brief ResourceManager class tests
   /// Tests resource manager that loads resource files.
   /// \todo test search order of uadataXX.zip files
   TEST_CLASS(TestResourceManager)
   {
      /// Tests opening of resource files that may be zipped into a .zip file.
      /// Assumes that a valid "uwadv.cfg" file can be found where Underworld
      /// Adventures would expect the file.
      TEST_METHOD(TestResourceLoading)
      {
         Base::Settings& settings = GetTestSettings();

         Base::ResourceManager resManager(settings);
         Base::SDL_RWopsPtr rwops = resManager.GetResourceFile("uw1/keymap.cfg");
         Assert::IsNotNull(rwops.get());

         Base::File file(rwops);
         Assert::IsTrue(file.FileLength() > 0);
      }
   };
} // namespace UnitTest
