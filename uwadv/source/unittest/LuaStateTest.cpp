//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2019 Underworld Adventures Team
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
/// \file LuaStateTest.cpp
/// \brief LuaState test
//
#include "pch.hpp"
#include "LuaState.hpp"

extern "C"
{
#include "lua.h"
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
   /// \brief LuaState class tests
   /// Tests Lua state related functions
   TEST_CLASS(LuaStateTest)
   {
      /// Tests loading scripts
      TEST_METHOD(TestLoadScript)
      {
         // set up
         std::string luaSource ="-- comment\n"
            "text = \"Hello World!\";\n"
            "print(text);\n";

         Base::SDL_RWopsPtr rwops = Base::MakeRWopsPtr(
            SDL_RWFromMem(luaSource.data(), luaSource.size()));

         // run
         LuaState state;
         int ret = state.LoadScript(rwops, "chunk1");

         // check
         Assert::IsTrue(ret == LUA_OK, L"return value must be LUA_OK");
      }

      /// Tests calling functions using CheckedCall()
      TEST_METHOD(TestCheckedCall)
      {
         // set up
         std::string luaSource = "-- comment\n"
            "function printHelloWorld(num)\n"
            "text = \"Hello World!\" + num;\n"
            "print(text);\n"
            "end;\n";

         Base::SDL_RWopsPtr rwops = Base::MakeRWopsPtr(
            SDL_RWFromMem(luaSource.data(), luaSource.size()));

         LuaState state;
         state.LoadScript(rwops, "chunk1");

         // run
         lua_pushstring(state.GetLuaState(), "printHelloWorld");
         lua_pushinteger(state.GetLuaState(), 42);
         state.CheckedCall(1, 0);
      }

      /// Tests checking Lua syntax using CheckSyntax(); empty
      TEST_METHOD(TestCheckSyntaxEmptyCode)
      {
         // set up
         std::string luaSource;

         // run
         std::vector<std::string> errorMessages;
         bool result = LuaState::CheckSyntax(luaSource, errorMessages);

         // check
         Assert::IsTrue(result, L"empty code must be valid Lua code");
         Assert::IsTrue(errorMessages.empty(), L"error messages list must be empty");
      }

      /// Tests checking Lua syntax using CheckSyntax(); valid code
      TEST_METHOD(TestCheckSyntaxValidCode)
      {
         // set up
         std::string luaSource = "-- comment\n"
            "function printHelloWorld(num)\n"
            "text = \"Hello World!\" + num;\n"
            "print(text);\n"
            "end;\n";

         // run
         std::vector<std::string> errorMessages;
         bool result = LuaState::CheckSyntax(luaSource, errorMessages);

         // check
         Assert::IsTrue(result, L"must be valid Lua code");
         Assert::IsTrue(errorMessages.empty(), L"error messages list must be empty");
      }

      /// Tests checking Lua syntax using CheckSyntax(); invalid code
      TEST_METHOD(TestCheckSyntaxInvalidCode)
      {
         // set up
         std::string luaSource = "abc 123 -+*/";

         // run
         std::vector<std::string> errorMessages;
         bool result = LuaState::CheckSyntax(luaSource, errorMessages);

         // check
         Assert::IsFalse(result, L"must be invalid Lua code");
         Assert::IsFalse(errorMessages.empty(), L"error messages list must contain entries");
      }

      /// Tests getting active lines for empty Lua code
      TEST_METHOD(TestGetActiveLinesEmptyCode)
      {
         // set up
         std::string luaSource;

         // run
         std::vector<size_t> activeLinesList;
         LuaState::GetActiveLines(luaSource, activeLinesList);

         // check
         Assert::IsTrue(activeLinesList.empty(), L"list must be empty");
      }

      /// Tests getting active lines for whitespace Lua code
      TEST_METHOD(TestGetActiveLinesWhitespaceCode)
      {
         // set up
         std::string luaSource = "   \t\t\t\n\t\t\n";

         // run
         std::vector<size_t> activeLinesList;
         LuaState::GetActiveLines(luaSource, activeLinesList);

         // check
         Assert::IsFalse(activeLinesList.empty(), L"list must not be empty");
         Assert::AreEqual(1U, activeLinesList.size(), L"list must contain one entry");
      }

      /// Tests getting active lines for syntax error Lua code
      TEST_METHOD(TestGetActiveLinesSyntaxErrorCode)
      {
         // set up
         std::string luaSource = "abc 123 -+*/";

         // run
         std::vector<size_t> activeLinesList;
         LuaState::GetActiveLines(luaSource, activeLinesList);

         // check
         Assert::IsTrue(activeLinesList.empty(), L"list must be empty");
      }


      /// Tests getting active lines for simple Lua code
      TEST_METHOD(TestGetActiveLinesSimpleCode)
      {
         // set up
         std::string luaSource = "-- comment\n"
            "text = \"Hello World!\";\n"
            "print(text);\n";

         // run
         std::vector<size_t> activeLinesList;
         LuaState::GetActiveLines(luaSource, activeLinesList);

         // check
         Assert::IsFalse(activeLinesList.empty(), L"list must not be empty");
         Assert::AreEqual(2U, activeLinesList.size(), L"list must contain exactly 2 entries");

         Assert::AreEqual(2U, activeLinesList[0], L"first active line is line 2");
         Assert::AreEqual(3U, activeLinesList[1], L"active lines are 1-based");
      }
   };

} // namespace UnitTest
