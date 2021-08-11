//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2021 Underworld Adventures Team
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
/// \file ScalerTest.cpp
/// \brief Scaler test
//
#include "pch.hpp"
#include "Scaler.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
   /// \brief Scaler class tests
   TEST_CLASS(ScalerTest)
   {
      /// Tests scaling by scale factor 2x
      TEST_METHOD(TestScaler2x)
      {
         std::vector<Uint32> source{ 0xff001122, 0xff115599, 0xff992211, 0xffff0080 };
         std::vector<Uint32> dest;

         size_t destSize = source.size() * 2 * 2;
         dest.resize(destSize + 1);
         dest[destSize] = 0xbaadf00d;

         Scaler::Scale2x(source.data(), dest.data(), 2, 2);

         Assert::AreEqual(0xbaadf00d, dest[destSize], L"guard value must not be overwritten");
      }

      /// Tests scaling by scale factor 3x
      TEST_METHOD(TestScaler3x)
      {
         std::vector<Uint32> source{ 0xff001122, 0xff115599, 0xff992211, 0xffff0080 };
         std::vector<Uint32> dest;

         size_t destSize = source.size() * 3 * 3;
         dest.resize(destSize + 1);
         dest[destSize] = 0xbaadf00d;

         Scaler::Scale3x(source.data(), dest.data(), 2, 2);

         Assert::AreEqual(0xbaadf00d, dest[destSize], L"guard value must not be overwritten");
      }

      /// Tests scaling by scale factor 4x
      TEST_METHOD(TestScaler4x)
      {
         std::vector<Uint32> source{ 0xff001122, 0xff115599, 0xff992211, 0xffff0080 };
         std::vector<Uint32> dest;

         size_t destSize = source.size() * 4 * 4;
         dest.resize(destSize + 1);
         dest[destSize] = 0xbaadf00d;

         Scaler::Scale4x(source.data(), dest.data(), 2, 2);

         Assert::AreEqual(0xbaadf00d, dest[destSize], L"guard value must not be overwritten");
      }
   };
} // namespace UnitTest
