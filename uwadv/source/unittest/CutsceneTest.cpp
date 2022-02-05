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
/// \file CutsceneTest.cpp
/// \brief tests for cutscene class
//
#include "pch.hpp"
#include "ResourceManager.hpp"
#include "Cutscene.hpp"
#include "IndexedImage.hpp"
#include <ctime>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
   /// Tests Cutscene class.
   TEST_CLASS(CutsceneTest)
   {
      /// Tests loading Cutscene
      TEST_METHOD(TestCutsceneLoad)
      {
         Base::Settings& settings = GetTestSettings();

         settings.SetValue(Base::settingGamePrefix, std::string("uw1"));
         settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));

         Base::ResourceManager resourceManager{ settings };

         Cutscene cutscene;
         IndexedImage image;

         cutscene.Load(resourceManager, "cuts/cs000.n10", image);
         cutscene.GetFrame(image, 0);

         Assert::AreEqual<size_t>(320 * 200,
            image.GetPixels().size(),
            L"image must have been initialized");
      }

      /// test profiling cutscene loading, ctor
      TEST_METHOD(TestProfilingCutscene_Ctor)
      {
         time_t begin = time(0);
         const int max = 200000;
         for (int i = 0; i < max; i++)
         {
            Cutscene cuts;
         }

         time_t stop = time(0);

         double ctorTime = double(stop - begin) * 1000.0 / double(max);
         UaTrace("ctor : %1.2f ms\n", ctorTime);
      }

      /// test profiling cutscene loading, loading cutscene
      TEST_METHOD(TestProfilingCutscene_Load)
      {
         Base::Settings& settings = GetTestSettings();

         settings.SetValue(Base::settingGamePrefix, std::string("uw1"));
         settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));

         Base::ResourceManager resourceManager{ settings };

         time_t begin = time(0);
         const int max = 20;
         for (int i = 0; i < max; i++)
         {
            Cutscene cuts;
            IndexedImage image;
            cuts.Load(resourceManager, "cuts/cs000.n10", image);
         }

         time_t stop = time(0);

         double loadTime = double(stop - begin) * 1000.0 / double(max);
         UaTrace("Load : %3.1f ms\n", loadTime);
      }

      /// test profiling cutscene loading, extracting first frame
      TEST_METHOD(TestProfilingCutscene_GetFirstFrame)
      {
         Base::Settings& settings = GetTestSettings();

         settings.SetValue(Base::settingGamePrefix, std::string("uw1"));
         settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));

         Base::ResourceManager resourceManager{ settings };

         time_t begin = time(0);
         const int max = 100;
         for (int i = 0; i < max; i++)
         {
            Cutscene cuts;
            IndexedImage image;
            cuts.Load(resourceManager, "cuts/cs000.n10", image);
            cuts.GetFrame(image, 0);
         }

         time_t stop = time(0);

         double frame1Time = double(stop - begin) * 1000.0 / double(max);
         UaTrace("first frame : %2.1f ms\n", frame1Time);
      }

      /// benchmark extracting first frame 100 times
      TEST_METHOD(TestProfilingCutscene_GetFirstFrameMultipleTimes)
      {
         Base::Settings& settings = GetTestSettings();

         settings.SetValue(Base::settingGamePrefix, std::string("uw1"));
         settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));

         Base::ResourceManager resourceManager{ settings };

         Cutscene cuts;
         IndexedImage image;
         cuts.Load(resourceManager, "cuts/cs000.n10", image);
         cuts.GetFrame(image, 0);

         time_t begin = time(0);
         const int max = 100;
         for (int i = 0; i < max; i++)
         {
            cuts.GetFrame(image, 0);
         }

         time_t stop = time(0);

         double frame2Time = double(stop - begin) * 1000.0 / double(max);
         UaTrace("first frame, %u times : %2.1f ms\n", max, frame2Time);
      }

      /// benchmark extracting all frames of the cutscene
      TEST_METHOD(TestProfilingCutscene_GetAllFrames)
      {
         Base::Settings& settings = GetTestSettings();

         settings.SetValue(Base::settingGamePrefix, std::string("uw1"));
         settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));

         Base::ResourceManager resourceManager{ settings };

         Cutscene cuts;
         IndexedImage image;
         cuts.Load(resourceManager, "cuts/cs000.n10", image);

         time_t begin = time(0);
         const int max = 100;
         const int max2 = cuts.GetMaxFrames();

         for (int i = 0; i < max; i++)
         {
            for (int j = 0; j < max2; j++)
               cuts.GetFrame(image, j);
         }

         time_t stop = time(0);

         double frame3Time = double(stop - begin) * 1000.0 / double(max * max2);
         UaTrace("all frames : %2.1f ms\n", frame3Time);
      }
   };
} // namespace UnitTest
