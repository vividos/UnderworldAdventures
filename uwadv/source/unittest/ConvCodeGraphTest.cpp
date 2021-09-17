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
/// \file ConvCodeGraphTest.cpp
/// \brief Conv::ConvCodeGraph class test
//
#include "pch.hpp"
#include "Opcodes.hpp"
#include "CodeGraph.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Conv;

namespace UnitTest
{
   /// \brief tests for CodeGraph
   /// Tests decompiling conv code.
   TEST_CLASS(ConvCodeGraphTest)
   {
      /// test code graph class
      struct TestCodeGraph
      {
         std::vector<std::string> strings;
         std::vector<Uint16> code;
         std::map<Uint16, ImportedItem> mapImportedFunctions;
         std::map<Uint16, ImportedItem> mapImportedVariables;
         CodeGraph graph;

         TestCodeGraph(std::initializer_list<Uint16> codeBytes)
            :code(codeBytes),
            graph(code, 0, (Uint16)code.size(), strings, mapImportedFunctions, mapImportedVariables, 0)
         {
         }

         void Decompile()
         {
            graph.Decompile();
         }

         const CodeGraph::GraphList& GetGraph() const
         {
            return graph.GetGraph();
         }

         std::string GetCodeText() const
         {
            std::string codeText;

            for (const auto& item : graph.GetGraph())
            {
               if (item.m_type == typeStatement)
                  codeText += item.Format();
            }

            return codeText;
         }

         std::string GetOpcodeText() const
         {
            std::string opcodeText;

            for (const auto& item : graph.GetGraph())
            {
               if (item.m_type == typeOpcode)
                  opcodeText += item.FormatOpcode();
            }

            return opcodeText;
         }
      };

      /// tests decoding empty function
      TEST_METHOD(TestDecodeEmptyFunc)
      {
         // set up
         TestCodeGraph graph{
            op_PUSHBP, op_SPTOBP, op_PUSHI, 0, op_ADDSP, // func start
            op_BPTOSP, op_POPBP, op_RET,  // func end
         };

         // run
         graph.Decompile();

         // check
         Assert::IsFalse(graph.GetGraph().empty(), L"code graph must not be empty");

         std::string codeText = graph.GetOpcodeText();
         Assert::IsFalse(codeText.empty(), L"code text must not be empty");

         std::string opcodeText = graph.GetOpcodeText();
         Assert::IsFalse(opcodeText.empty(), L"opcode text must not be empty");
      }

      /// Tests if without else, with the following code:
      ///
      ///   {expression}
      ///   BEQ label1 -----+
      ///   {then-code}     |
      /// label1:   <-------+
      ///   ...
      TEST_METHOD(TestDecodeIf_WithoutElse)
      {
         // set up
         TestCodeGraph graph{
            op_PUSHBP, op_SPTOBP, op_PUSHI, 1, op_ADDSP, // func start, 1 local

            // expression local_1 == 0
            op_PUSHI_EFF, 0x0001,
            op_FETCHM,
            op_PUSHI, 0x0000,
            op_TSTEQ,

            // if statement
            op_BEQ, 3, // to endif-pos

            // then-code
            op_CALL, 0,

            // endif-pos

            op_BPTOSP, op_POPBP, op_RET,  // func end
         };

         // run
         graph.Decompile();

         // check
         Assert::IsFalse(graph.GetGraph().empty(), L"code graph must not be empty");

         std::string codeText = graph.GetCodeText();

         const char* expectedCode =
            "int local_1;"
            "if (local_1 == 0) {"
            "func_0000();"
            "} // end-if";
         Assert::IsTrue(codeText.find(expectedCode) != -1, L"expected code text must be found");
      }

      /// Tests if-else statement, with code:
      ///
      ///   {expression}
      ///   BEQ label1 -----+
      ///   {then-code}     |
      ///   BRA label2 --+  |
      /// label1:   <----|--+
      ///   {else-code}  |
      /// label2:   <----+
      ///   ...
      TEST_METHOD(TestDecodeIf_WithElse)
      {
         // set up
         TestCodeGraph graph{
            op_PUSHBP, op_SPTOBP, op_PUSHI, 1, op_ADDSP, // func start, 1 local

            // expression local_1 == 0
            op_PUSHI_EFF, 0x0001,
            op_FETCHM,
            op_PUSHI, 0x0000,
            op_TSTEQ,

            // if statement
            op_BEQ, 5, // to else-code

            // then-code
            op_CALL, 0,

            op_BRA, 3, // to the endif-pos

            // else-code
            op_CALL, 0,

            // endif-pos

            op_BPTOSP, op_POPBP, op_RET,  // func end
         };

         // run
         graph.Decompile();

         // check
         Assert::IsFalse(graph.GetGraph().empty(), L"code graph must not be empty");

         std::string codeText = graph.GetCodeText();

         const char* expectedCode =
            "int local_1;"
            "if (local_1 == 0) {"
            "func_0000();"
            "} else {"
            "func_0000();"
            "} // end-if";
         Assert::IsTrue(codeText.find(expectedCode) != -1, L"expected code text must be found");
      }

      /// Tests if-else statement variant, with code:
      ///
      ///   {expression}
      ///   BEQ label1 -----+
      ///   {then-code}     |
      ///   JMP label2 --+  |
      ///   BRA label2 --+  |
      /// label1:   <----|--+
      ///   {else-code}  |
      /// label2:   <----+
      ///   ...
      TEST_METHOD(TestDecodeIf_WithElse_JmpBraAtThenEnd)
      {
         // set up
         TestCodeGraph graph{
            op_PUSHBP, op_SPTOBP, op_PUSHI, 1, op_ADDSP, // func start, 1 local

            // expression local_1 == 0
            op_PUSHI_EFF, 0x0001,
            op_FETCHM,
            op_PUSHI, 0x0000,
            op_TSTEQ,

            // if statement
            op_BEQ, 7, // to else-code

            // then-code
            op_CALL, 0,

            op_JMP, 0x0015, // to the endif-pos
            op_BRA, 3, // to the endif-pos

            // else-code
            op_CALL, 0,

            // endif-pos

            op_BPTOSP, op_POPBP, op_RET,  // func end
         };

         // run
         graph.Decompile();

         // check
         Assert::IsFalse(graph.GetGraph().empty(), L"code graph must not be empty");

         std::string codeText = graph.GetCodeText();

         const char* expectedCode =
            "int local_1;"
            "if (local_1 == 0) {"
            "func_0000();"
            "} else {"
            "func_0000();"
            "} // end-if";
         Assert::IsTrue(codeText.find(expectedCode) != -1, L"expected code text must be found");
      }

      /// Tests if-else statement variant 2, with code:
      ///
      ///   {expression}
      ///   BEQ label1 -----+
      ///   {then-code}     |
      ///   JMP label2 --+  |
      ///   BRA label2 --+  |
      /// label1:   <----|--+
      ///   {else-code}  |
      ///   JMP label2 --+
      /// label2:   <----+
      ///   ...
      TEST_METHOD(TestDecodeIf_WithElse_JmpBraAtThenEnd_JmpAtElseEnd)
      {
         // set up
         TestCodeGraph graph{
            op_PUSHBP, op_SPTOBP, op_PUSHI, 1, op_ADDSP, // func start, 1 local

            // expression local_1 == 0
            op_PUSHI_EFF, 0x0001,
            op_FETCHM,
            op_PUSHI, 0x0000,
            op_TSTEQ,

            // if statement
            op_BEQ, 7, // to else-code

            // then-code
            op_CALL, 0,

            op_JMP, 0x0017, // to the endif-pos
            op_BRA, 5, // to the endif-pos

            // else-code
            op_CALL, 0,
            op_JMP, 0x0017, // to the endif-pos

            // endif-pos

            op_BPTOSP, op_POPBP, op_RET,  // func end
         };

         // run
         graph.Decompile();

         // check
         Assert::IsFalse(graph.GetGraph().empty(), L"code graph must not be empty");

         std::string codeText = graph.GetCodeText();

         const char* expectedCode =
            "int local_1;"
            "if (local_1 == 0) {"
            "func_0000();"
            "} else {"
            "func_0000();"
            "} // end-if";
         Assert::IsTrue(codeText.find(expectedCode) != -1, L"expected code text must be found");
      }

      /// Tests if-not (if with inverted expression), with code:
      ///
      ///   {expression}
      ///   BEQ label1 -----+
      ///   JMP label2 ---+ |
      /// label1:   <-----|-+
      ///   {else-code}   |
      /// label2:   <-----+
      ///   ...
      TEST_METHOD(TestDecodeIfNot_WithoutElse)
      {
         // set up
         TestCodeGraph graph{
            op_PUSHBP, op_SPTOBP, op_PUSHI, 1, op_ADDSP, // func start, 1 local

            // expression local_1 == 0
            op_PUSHI_EFF, 0x0001,
            op_FETCHM,
            op_PUSHI, 0x0000,
            op_TSTEQ,

            // if statement
            op_BEQ, 3, // to else-code

            // dummy-then-code, jumps to endif-pos
            op_JMP, 0x0011,

            // else-code
            op_CALL, 0,

            // endif-pos

            op_BPTOSP, op_POPBP, op_RET,  // func end
         };

         // run
         graph.Decompile();

         // check
         Assert::IsFalse(graph.GetGraph().empty(), L"code graph must not be empty");

         std::string codeText = graph.GetCodeText();

         const char* expectedCode =
            "int local_1;"
            "if (!(local_1 == 0)) {"
            "func_0000();"
            "} // end-if";
         Assert::IsTrue(codeText.find(expectedCode) != -1, L"expected code text must be found");
      }

      /// Tests if-not variant, containing nothing in the else-code part:
      ///
      ///   {expression}
      ///   BEQ label1 -----+
      ///   JMP label2 -----+
      /// label1:   <-------+
      ///   ...
      TEST_METHOD(TestDecodeIfNot_WithoutElse_NoElseCode)
      {
         // set up
         TestCodeGraph graph{
            op_PUSHBP, op_SPTOBP, op_PUSHI, 1, op_ADDSP, // func start, 1 local

            // expression local_1 == 0
            op_PUSHI_EFF, 0x0001,
            op_FETCHM,
            op_PUSHI, 0x0000,
            op_TSTEQ,

            // if statement
            op_BEQ, 3, // to else-code

            // dummy-then-code, jumps to endif-pos
            op_JMP, 0x000F,

            // empty else-code

            // endif-pos

            // some more code
            op_PUSHI, 42,
            op_PUSHI_EFF, 0x0001,
            op_SWAP,
            op_STO,

            op_BPTOSP, op_POPBP, op_RET,  // func end
         };

         // run
         graph.Decompile();

         // check
         Assert::IsFalse(graph.GetGraph().empty(), L"code graph must not be empty");

         std::string codeText = graph.GetCodeText();

         const char* expectedCode =
            "int local_1;"
            "if (!(local_1 == 0)) {"
            "} // end-if";
         Assert::IsTrue(codeText.find(expectedCode) != -1, L"expected code text must be found");
      }

      /// Tests if-not variant, with code:
      ///
      ///   {expression}
      ///   BEQ label1 -----+
      ///   JMP label2 ---+ |
      ///   BRA label2 ---+ |
      /// label1:   <-----|-+
      ///   {else-code}   |
      /// label2:   <-----+
      ///   ...
      TEST_METHOD(TestDecodeIfNot_WithoutElse_WithBraAfterJmp)
      {
         // set up
         TestCodeGraph graph{
            op_PUSHBP, op_SPTOBP, op_PUSHI, 1, op_ADDSP, // func start, 1 local

            // expression local_1 == 0
            op_PUSHI_EFF, 0x0001,
            op_FETCHM,
            op_PUSHI, 0x0000,
            op_TSTEQ,

            // if statement
            op_BEQ, 5, // to else-code

            // dummy-then-code, jumps to endif-pos
            op_JMP, 0x0013, // jumps to endif-pos
            op_BRA, 3, // jumps to endif-pos

            // else-code
            op_CALL, 0,

            // endif-pos

            op_BPTOSP, op_POPBP, op_RET,  // func end
         };

         // run
         graph.Decompile();

         // check
         Assert::IsFalse(graph.GetGraph().empty(), L"code graph must not be empty");

         std::string codeText = graph.GetCodeText();

         const char* expectedCode =
            "int local_1;"
            "if (!(local_1 == 0)) {"
            "func_0000();"
            "} // end-if";
         Assert::IsTrue(codeText.find(expectedCode) != -1, L"expected code text must be found");
      }

      /// Tests if-not with else, with code:
      ///
      ///   {expression}
      ///   BEQ label1 -----+
      ///   JMP label2 ---+ |
      ///   BRA label2 ---+ |
      /// label1:   <-----|-+
      ///   {else-code}   |
      ///   JMP label3 ---|-+
      /// label2:   <-----+ |
      ///   {then-code}     |
      /// label3:   <-------+
      ///   ...
      TEST_METHOD(TestDecodeIfNot_WithElse)
      {
         // set up
         TestCodeGraph graph{
            op_PUSHBP, op_SPTOBP, op_PUSHI, 1, op_ADDSP, // func start, 1 local

            // expression local_1 == 0
            op_PUSHI_EFF, 0x0001,
            op_FETCHM,
            op_PUSHI, 0x0000,
            op_TSTEQ,

            // if statement
            op_BEQ, 5, // to else-code

            // dummy-then-code
            op_JMP, 0x0015, // jumps to then-code
            op_BRA, 5, // also jumps to then-code

            // else-code
            op_CALL, 0,
            op_JMP, 0x0017, // jumps to endif-pos

            // then-code
            op_CALL, 0xFFFF,

            // endif-pos

            op_BPTOSP, op_POPBP, op_RET,  // func end
         };

         // run
         graph.Decompile();

         // check
         Assert::IsFalse(graph.GetGraph().empty(), L"code graph must not be empty");

         std::string codeText = graph.GetCodeText();

         const char* expectedCode =
            "int local_1;"
            "if (!(local_1 == 0)) {"
            "func_0000();"
            "} else {"
            "func_ffff();"
            "} // end-if";
         Assert::IsTrue(codeText.find(expectedCode) != -1, L"expected code text must be found");
      }

      /// Tests if-not variant, containing only a JMP in the else-code part:
      ///
      ///   {expression}
      ///   BEQ label1 -----+
      ///   JMP label3 -----|-+
      ///   BRA label2 ---+ | |
      /// label1:   <-----|-+ |
      ///   JMP label2 ---+   |
      /// label2:   <-----+   |
      ///   {then-code}       |
      /// label3:   <---------+
      ///   ...
      /// (found in Bragit's conversation)
      TEST_METHOD(TestDecodeIfNot_WithElse_JmpInElseCode)
      {
         // set up
         TestCodeGraph graph{
            op_PUSHBP, op_SPTOBP, op_PUSHI, 1, op_ADDSP, // func start, 1 local

            // expression local_1 == 0
            op_PUSHI_EFF, 0x0001,
            op_FETCHM,
            op_PUSHI, 0x0000,
            op_TSTEQ,

            // if statement
            op_BEQ, 5, // to else-code

            // dummy-then-code
            op_JMP, 0x0015, // jumps to endif-pos
            op_BRA, 3, // jumps to then-code

            // else-code
            op_JMP, 0x0013, // jumps to then-code

            // then-code
            op_CALL, 0,

            // endif-pos

            op_BPTOSP, op_POPBP, op_RET,  // func end
         };

         // run
         graph.Decompile();

         // check
         Assert::IsFalse(graph.GetGraph().empty(), L"code graph must not be empty");

         std::string codeText = graph.GetCodeText();
         std::string opcodeText = graph.GetOpcodeText();

         const char* expectedCode =
            "int local_1;"
            "if (!(local_1 == 0)) {"
            "func_0000();"
            "} // end-if";
         Assert::IsTrue(codeText.find(expectedCode) != -1, L"expected code text must be found");
      }
   };
} // namespace UnitTest
