//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2003,2021 Underworld Adventures Team
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
/// \file ImageManagerTest.cpp
/// \brief tests image manager and importing graphics
//
#include "pch.hpp"
#include "Settings.hpp"
#include "ResourceManager.hpp"
#include "ImageManager.hpp"
#include "IndexedImage.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
   /// Tests ImageManager class.
   TEST_CLASS(ImageManagerTest)
   {
      /// Tests pasting together uw1 object images
      TEST_METHOD(TestObjectImagesUw1)
      {
         Base::Settings& settings = GetTestSettings();

         settings.SetValue(Base::settingGamePrefix, std::string("uw1"));
         settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));

         Base::ResourceManager resourceManager{ settings };

         ImageManager mgr{ resourceManager };
         mgr.Init();

         std::vector<IndexedImage> imageList;
         mgr.LoadList(imageList, "objects", 0, 0, 1);

         IndexedImage part1, part2;
         part1.Create(256, 256);
         part2.Create(256, 256);

         for (Uint16 id = 0; id < 256; id++)
         {
            // objects 218 thru 223 have different size
            if (id < 218 || id >= 224)
               part1.PasteImage(imageList[id], (id & 0x0f) << 4, id & 0xf0);
         }

         for (Uint16 id = 256; id < 460; id++)
         {
            if (id != 302)
               part2.PasteImage(imageList[id], (id & 0x0f) << 4, id & 0xf0);
         }

         // TODO paste images 218 to 223 and 302 into remaining space

         //WritePngFile(part1, "uw1-objects.png");
         //WritePngFile(part2, "uw1-objects.png");
      }
   };
} // namespace UnitTest
